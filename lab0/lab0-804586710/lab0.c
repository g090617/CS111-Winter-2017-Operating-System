#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>


void sig_handler(int sig)
{
  if(sig == SIGSEGV)
    {
      fprintf(stderr,"Catch Segmentation Fault\n");
      exit(3);
    }
}

int main(int argc, char **argv)
{
  int c = 0;
  char *fd0 = NULL;
  char *fd1 = NULL;
  char *seg = NULL;
  int is_seg = 0;

  while (1)
    {
      static struct option long_options[] = 
	{
	  {"input", required_argument, NULL, 'i'},
	  {"output", required_argument, NULL, 'o'},
	  {"segfault", no_argument, NULL, 's'},
	  {"catch", no_argument, NULL, 'c'},
	  {0, 0, 0, 0}
	};
       int option_index = 0;

       c = getopt_long(argc, argv, "iosc", long_options, &option_index);

       if (c == -1)
	 break;

       switch (c)
	 {
	 case 'i':
	   fd0 = optarg;
	   //printf("input argument is %s \n", fd0);
	   break;
	 case 'o':
	   fd1 = optarg;
	   //printf("input argument is %s \n", fd1);
	   break;
	 case 's':
	   is_seg = 1;
	   break;
	 case 'c':
	   signal(SIGSEGV, sig_handler);
	   break;
	 case '?':
	   break;
	 default:
	   abort();
	 }
    }
  
  if( is_seg == 1)
    printf("%c",seg[0]);

  int in_file_descriptor = STDIN_FILENO;
  if(fd0 != NULL)
    in_file_descriptor = open( fd0, 0);

  if(in_file_descriptor < 0)
    {
      fprintf(stderr, "input file doesn't exist by stderr\n");
      perror("input file doesn't exist by perror\n");
      exit(1);
    }
  else if(in_file_descriptor != STDIN_FILENO)
    {
      close(STDIN_FILENO);
      dup(in_file_descriptor);
      close(in_file_descriptor);
    }
  
  int out_file_descriptor = STDOUT_FILENO;
  if(fd1 != NULL)
    out_file_descriptor = creat(fd1,0666);

  if(out_file_descriptor < 0)
    {
      fprintf(stderr, "output file doesn't exist by stderr\n");
      perror("output file doesn't exist by perror\n");
      exit(2);
    }
  else  if(out_file_descriptor != STDOUT_FILENO)
    {
      close(STDOUT_FILENO);
      dup(out_file_descriptor);
      close(out_file_descriptor);
    }

  char  temp;
  while(read(STDIN_FILENO, &temp, sizeof(char)))
    {
//printf("temp is %c \n", temp);
      write(STDOUT_FILENO, &temp, sizeof(char));
    }

  exit(0);
}
