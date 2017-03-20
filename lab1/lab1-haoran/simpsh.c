#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

void print_rusage(struct timeval ustart, 
		  struct timeval sstart, 
		  struct rusage ru,
		  int who)
{
  struct timeval uend, send;
  getrusage(who, &ru);
  uend = ru.ru_utime;
  send = ru.ru_stime;
  //printf("end utime is %f, end stime is %f\n",
  //	 (double)uend.tv_sec + (double)uend.tv_usec/1000000,
  //	 (double)send.tv_sec + (double)send.tv_usec/1000000);
  printf("user time is %f, system time is %f s\n",
	 (double)uend.tv_sec - (double)ustart.tv_sec +
	 ((double)uend.tv_usec - (double)ustart.tv_usec)/1000000,
	 (double)send.tv_sec - (double)sstart.tv_sec +
	 ((double)send.tv_usec - (double)sstart.tv_usec)/1000000);
}


void sig_handler(int sig)
{
  fprintf(stderr,"Catch non exist file error %d\n",sig);
  exit(sig);
}

struct pid_cmd
{
  int process_id;
  int cmd_size;
  char* cmd[100];
};

int main(int argc, char **argv)
{
  int opt = 0;
  int descriptor_index = 3; 
  //0 for stdin 
  //1 for stdout
  //2 for stderr
  int ifd = STDIN_FILENO;
  int ofd = STDOUT_FILENO;
  int efd;
  int o_flag = 0;
  int command_index = 0;
  int command_flag = 0;
  int command_arg_index = 0;
  int num_of_rdonly = 0;
  int index_of_rdonly[100];
  int verbose_flag = 0;

  int fd_array[100];
  int fd_array_index = 0;

  int pipe_fd[2];
  int pipe_array[100][2];
  int pipe_flag[100];
 
  int temppid;
  int status;
  long command_index_num;

  int num_wait;

  int profile_flag = 0;

  long long start_utime = 0;
  long long start_stime = 0;

  char* opt_descriptor[100];
  char temp;
  char* command_arg[100];

  char* optarg_ptr;

  char* ans = "sleep";

  struct pid_cmd command_info[100];

  struct rusage ru;
  struct timeval ustart, uend,sstart, send, cu_start, cs_start, cu_end, cs_end;

  int ii;
  for( ii =0; ii< 100;ii++)
    pipe_flag[ii] = -1;
  //error num
  //1: non existent file
  //2: cannot create file
  //3: invalid file descriptor
  

  while(1)
    {
      static struct option long_options[] =
	{
	  {"rdonly",    required_argument, NULL, 'r'},
	  {"wronly",    required_argument, NULL, 'w'},
	  {"rdwr",      required_argument, NULL, 'z'},
	  {"command",   required_argument, NULL, 'c'},
	  {"close",     required_argument, NULL, 'x'},
	  {"catch",     required_argument, NULL, 'e'},
	  {"ignore",    required_argument, NULL, 'i'},
	  {"default",   required_argument, NULL, 'd'},
	  {"verbose",   no_argument,       NULL, 'v'},
	  {"pipe",      no_argument,       NULL, 'p'},
	  {"abort",     no_argument,       NULL, 'a'},
	  {"pause",     no_argument,       NULL, 's'},
	  {"wait",      no_argument,       NULL, 'y'},
	  {"profile",   no_argument,       NULL, 'P'},
	  {"append",    no_argument,       NULL, O_APPEND},
	  {"cloexec",   no_argument,       NULL, O_CLOEXEC},
	  {"creat",     no_argument,       NULL, O_CREAT},
	  {"directory", no_argument,       NULL, O_DIRECTORY},
	  {"dsync",     no_argument,       NULL, O_DSYNC},
	  {"excl",      no_argument,       NULL, O_EXCL},
	  {"nofollow",  no_argument,       NULL, O_NOFOLLOW},
	  {"nonblock",  no_argument,       NULL, O_NONBLOCK},
	  {"rsync",     no_argument,       NULL, O_RSYNC},
	  {"sync",      no_argument,       NULL, O_SYNC},
	  {"trunc",     no_argument,       NULL, O_TRUNC},
	  {0,0,0,0}
	};
      
      int option_index = 0;
      
      pid_t pid;
      int status;

      getrusage(RUSAGE_SELF, &ru);
      ustart = ru.ru_utime;
      sstart = ru.ru_stime;

      opt = getopt_long(argc,argv, "-rwgeasxcvp", long_options, &option_index);
      
      if(command_flag == 0 && opt == -1)//no option
	{
	  //printf("break out\n");
	  break;
	}
      else if((command_flag == 1 && opt != 1)/* || (command_flag == 1 && optarg[0] == '>')*/)
	{
	  if(verbose_flag == 1)
	    printf("\n");

	  //printf("%s wait num is %d\n\n\n",command_arg[0],num_wait);
	  //printf("command_arg index is %d\n",command_arg_index);
	  int i = 0;
	  char* tempp;
	  //printf("cao! before for loop\n");
	  for(i == 0; i < command_arg_index; i++)
	  {
	    //printf("command_arg i is %s\n\n\n", command_arg[i]);
	    //printf("%s\n",command_arg[i]);
	    if( i == 0)
	      {
		command_info[num_wait].cmd[i]  = command_arg[i];
		//printf("After assign %s\n",command_info[num_wait].cmd[i] );
	      }
	    else
	      {
		//printf("command_arg i is %s", command_arg[i]);
		command_info[num_wait].cmd[i] = command_arg[i];
	    //command_info[num_wait].cmd = "aaa";
		//printf("command info is %s\n",command_info[num_wait].cmd );
	      }
	  }
	  
	  command_info[num_wait].cmd_size = command_arg_index;
	  getrusage(RUSAGE_CHILDREN, &ru);
	  cu_start = ustart;
	  cs_start = sstart;

	  if((pid = fork()) < 0)
	    {
	      fprintf(stderr,"Fork fail\n");
	      exit(1);
	    }
	  else if(pid == 0)
	    {
	      command_arg[command_arg_index] = '\0';
	      
	      if(pipe_flag[ifd] == 0)
		close(pipe_flag[ifd + 1]);
	      else if(pipe_flag[ifd] == 1)
		close(pipe_flag[ifd - 1]);

	      
	      if(pipe_flag[ofd]== 0)
		close(pipe_flag[ofd + 1]);
	      else if(pipe_flag[ofd] == 1)
		close(pipe_flag[ofd - 1]);

	      
	      if(pipe_flag[efd]== 0)
		close(pipe_flag[efd + 1]);
	      else if(pipe_flag[efd] == 1)
		close(pipe_flag[efd - 1]);
	      

	      close(0);
	      dup(ifd);
	      close(ifd);
	      close(1);
	      dup(ofd);
	      close(ofd);
	      close(2);
	      dup(efd);
	      close(efd);
	      if(execvp(*command_arg,command_arg) == -1)
		{
		  int temperr = errno;
		  fprintf(stderr,"exevp() fail in here\n");
		  exit(1);
		}
	    }
	  else
	    {	     
	      command_info[num_wait].process_id = pid;
	      num_wait++;
	      if(pipe_flag[ifd] != -1)
		close(ifd);
		
	      if(pipe_flag[ofd] != -1)
                close(ofd);

	      if(pipe_flag[efd] != -1)
                close(efd);
	      
	    }
	  command_arg_index = 0;
	  command_index = 0;
	  command_flag =0;
	}
      

      switch(opt)
	{
	case O_APPEND:
	case O_CLOEXEC:
	case O_DSYNC:
	case O_SYNC:
	  //case O_RSYNC:
	case O_TRUNC:
	case O_DIRECTORY:
	case O_CREAT:
	case O_NOFOLLOW:
	case O_EXCL:
	case O_NONBLOCK:
	  if(verbose_flag == 1)
	    printf("--%s ", optarg);
	  o_flag = o_flag | (int)opt;
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'r':
	  if(verbose_flag == 1)
	    printf("--rdonly %s\n", optarg);
	  ifd = open(optarg,o_flag | O_RDONLY);
	  if(ifd >= 0 && ifd != descriptor_index)//not the same file descriptor
	    {
	      fd_array[fd_array_index] = ifd;
	      fd_array_index++;
	    }
	  else if(ifd >= 0 && ifd == descriptor_index)
	    {
	      fd_array[fd_array_index] = ifd;
	      fd_array_index++;
	    }
	  else if(ifd < 0)//error
	    {
	      fprintf(stderr,"file does not exist\n");
	      exit(1);
	    }
	  int i;
	  //for(i = 0; i < fd_array_index; i++)
	  //printf("rdonly fd array contains %d\n",fd_array[i]);
	  descriptor_index++;

	  if(profile_flag == 1)
	    {
	      //printf("here\n");
	      print_rusage( ustart, sstart, ru, RUSAGE_SELF); 
	    }
	  break;
	case 'w':
	  if(verbose_flag == 1)
            printf("--rwonly %s\n", optarg);
	  ofd = open(optarg,o_flag | O_WRONLY);
          if(ofd >= 0 && ofd != descriptor_index)
            {
	      fd_array[fd_array_index] = ofd;
	      fd_array_index++;
            }
          else if(ofd >= 0 && ofd == descriptor_index)
            {
              fd_array[fd_array_index] = ofd;
              fd_array_index++;
            }
          else if(ofd < 0)
            {
              fprintf(stderr,"Cannot create file\n");
              exit(1);
            }
	  //for(i = 0; i < fd_array_index; i++)
	  //printf("wronly fd array contains %d\n",fd_array[i]);
	  descriptor_index++;
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'z':
	  if(verbose_flag == 1)
            printf("--rdwr %s\n", optarg);
          ofd = open(optarg,o_flag | O_WRONLY |O_RDONLY);
          if(ofd >= 0 && ofd != descriptor_index)
            {
              fd_array[fd_array_index] = ofd;
              fd_array_index++;
            }
	  else if(ofd >= 0 && ofd == descriptor_index)
	    {
	      fd_array[fd_array_index] = ofd;
	      fd_array_index++;
	    }
          else if(ofd < 0)
            {
              fprintf(stderr,"Cannot create file\n");
              exit(1);
            }
          descriptor_index++;
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'p':
	  if(verbose_flag == 1)
	    printf("--pipe\n");
	  if(pipe(pipe_fd) == -1)
	    {
	      fprintf(stderr, "Cannot create pipe\n");
	      exit(1);
	    }

	  fd_array[fd_array_index] = pipe_fd[0];
	  pipe_flag[pipe_fd[0]] = 0;
	  //pipe_array[pipe_fd[0]][0] = pipe_fd[0];
	  //pipe_array[pipe_fd[0]][1] = pipe_fd[1];
	  fd_array_index++;
	  fd_array[fd_array_index] = pipe_fd[1];
	  pipe_flag[pipe_fd[1]] = 1;
	  fd_array_index++;
	  //for(i = 0; i < fd_array_index; i++)
	  //printf("pipe pipe array contains %d %d\n",pipe_fd[0],pipe_fd[1]);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'c':
	  if(verbose_flag == 1)
	  {
	    printf("--command %s ",optarg);
	  }

	  command_flag = 1;
	  //command_index_num = strtol(optarg, &optarg_ptr, 10) + 3;
	  fd_array_index = strtol(optarg, &optarg_ptr, 10);
	  //command_index_num = fd_array[fd_array_index];
	  if(strlen(optarg_ptr) != 0)//it is not a file descriptor, report error
            {
              printf("string length is %d \n",strlen(optarg_ptr));
              printf("string part is %s \n",optarg_ptr);
              printf("%c %d\n", optarg_ptr[0],optarg_ptr[0]);
            }
          else
	    {// change to the corresponding file descriptor
	      command_index_num = fd_array[fd_array_index];
	      //printf("ifd is %d\n",command_index_num);
	      ifd = command_index_num;
	      if (fcntl(ifd, F_GETFL) < 0 && errno == EBADF) {
		fprintf(stderr,"invalid file descriptor\n");
		exit(1);
	      }
	      command_index++;
	    }
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'v':
	  verbose_flag = 1;
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 1:
	  if(verbose_flag == 1 && optarg != NULL)
	  {
	    printf("%s ",optarg);
	  }
	  fd_array_index = strtol(optarg, &optarg_ptr, 10);
	  //printf("fd array index is %d\n",fd_array_index);

	  //command_index_num = strtol(optarg, &optarg_ptr, 10) + 3;
	  if(strlen(optarg_ptr) != 0)
	    {//a string may be invalid options or command depends on the command flag
	      if(command_flag == 1 && command_index == 3
		 && strlen(optarg) == strlen(optarg_ptr))
		{//command with full descriptors and no numbers ahead
		  command_arg[command_arg_index] = optarg_ptr;
		  command_arg_index++;
		}
	      else if(command_flag == 1 && command_index == 3
		      &&strlen(optarg) != strlen(optarg_ptr))
		{//command with full descriptors with numbers ahead
		  command_arg[command_arg_index] = optarg;
		  command_arg_index++;
		}
	      else
		{
		  fprintf(stderr, "invalid option\n");
		  exit(1);
		}
	    }
	  else
	    {
	      if(command_flag == 0 )
		{//an integer without setting the command flag does not has meaning
		  fprintf(stderr," descriptor without a command is nothing \n");
		  exit(1);
		}
	      else if(command_flag == 1 && command_index == 3)
		{
		  command_arg[command_arg_index] = optarg;
                  command_arg_index++;
		}
	      else if(command_flag == 1 && command_index > 3)
		{
		  command_arg[command_arg_index] = optarg;
		  command_arg_index++;
		}
	      else
		{
		  if (fcntl(fd_array[fd_array_index],F_GETFL) < 0 && errno == EBADF) 
		    {
		      fprintf(stderr,"invalid file descriptor\n");
		      exit(1);
		    }
		  int valid_flag = 0;
	     
		  command_index_num = fd_array[fd_array_index];
		  if(command_index == 1)
		    {
		      ofd = command_index_num;
		      //printf("ofd is %d\n",ofd);
		    }
		  else
		    efd = command_index_num;
		  command_index++;
		}
	    }
	  //if(profile_flag == 1)
	  //{
	  //  print_rusage( ustart, sstart, ru);
	  // }
	  break;
	case 'x':
	  if(verbose_flag == 1)
	    printf("--close %s\n",optarg);
	  //printf("close\n\n\n");
	  if (fcntl(atoi(optarg), F_GETFL) < 0 && errno == EBADF) 
	    {
	      fprintf(stderr,"invalid file descriptor\n");
	      exit(1);
	    }
	  close(fd_array[atoi(optarg)]);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'e':
	  if(verbose_flag == 1)
	    printf("--catch %s\n",optarg);
	  printf("in catch\n");
	  signal(atoi(optarg),sig_handler);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'a':
	  if(verbose_flag == 1)
	    printf("--abort\n");
	  raise(SIGSEGV);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'y':
	  if(verbose_flag == 1)
	    printf("--wait\n");
	  
	  for(i = 0; i < num_wait; i++)
	  {
	  //  printf("num wait is %d\n",num_wait);
	    int exit_pid = waitpid(-1, &status, 0);
	      //printf("exit status is %d\n",status);
	    //if(profile_flag == 1)
	    //{
	    //	print_rusage( cu_start, cs_start, ru);
	    //}
	    int k,m;
	    for(k = 0; k < num_wait; k++)
	      {
		if(exit_pid == command_info[k].process_id)
		  {
		      printf("%d ",status);
		    for(m = 0; m <command_info[k].cmd_size; m++)
		      {
			//printf("k is %d m is %d\n",k,m);
			printf("%s ", command_info[k].cmd[m]);
		      }
		    printf("\n");
		  }
	      }
	    if(profile_flag == 1)
	      {
		printf("Children\n");
		print_rusage( cu_start, cs_start, ru, RUSAGE_CHILDREN);
	      }
	  }
	  if(profile_flag == 1)
            {
              print_rusage( cu_start, cs_start, ru, RUSAGE_SELF);
            }
	  //wait(NULL);
	  break;
	case 's'://pause
	  if(verbose_flag == 1)
	    printf("--pause\n");
	  pause();
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'i'://ignore
	  if(verbose_flag == 1)
	    printf("--ignore %s",optarg);
	  signal(atoi(optarg),SIG_IGN);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
	  break;
	case 'd'://default
          if(verbose_flag == 1)
            printf("--default %s",optarg);
          signal(atoi(optarg),SIG_DFL);
	  if(profile_flag == 1)
            {
              print_rusage( ustart, sstart, ru, RUSAGE_SELF);
            }
          break;
	case 'P':
	  if(verbose_flag == 1)
	    printf("--profile\n");
	  profile_flag = 1;
	 
	  break;
	case '?':
	  //printf("In switch ? optarg is %s \n", optarg);
	  break;
	default:
	  //printf("In switch default optarg is %s \n", optarg);
	  break;
	}
    }

  
  if(ofd == STDOUT_FILENO)
    {
      int i;
      for(i = 0;i <num_of_rdonly;i++)
	{
	  while(read(index_of_rdonly[i],&temp,sizeof(char)))
	    {
	      write(STDOUT_FILENO,&temp,sizeof(char));
	    }
	}
    }
  
  return 0;
}
