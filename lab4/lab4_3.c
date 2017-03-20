#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <mraa/aio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#define SERVER_PORT 17000

int sockfd;
long sleep_time = 3;
int stop_flag = 0;
mraa_aio_context temp_a0;
int B = 4275;
int scale_flag = 0;

SSL_CTX *sslContext;
SSL *sslClient;
FILE* logfd;


void* read_from_server(void* arg)
{

  //uint16_t temp_value = 0;
  //FILE *fd = fopen("log.txt","w+");                                               
  //time_t t;
  //temp_a0 = mraa_aio_init(0);

  int buf_length = 150;
  char buf[buf_length];

  if(temp_a0 == NULL)
    exit(1);


  char read_buffer[256];
  memset(read_buffer, 0, 256);

  while(SSL_read(sslClient, read_buffer, 255) > 0)
    {
      fprintf(stdout, "buffer command: %s \n", read_buffer);
      if(strcmp(read_buffer, "START") == 0)
	{
	  stop_flag = 0;
	  printf("start\n");
	  fprintf(logfd,"START\n");
	}
      else if(strcmp(read_buffer, "STOP") == 0)
	{
	  stop_flag = 1;
	  printf("stop\n");
	  fprintf(logfd,"STOP\n");
	}
      else if(strcmp(read_buffer,"OFF") == 0)
	{
	  printf("off\n");
	  fprintf(logfd,"OFF\n");
	  exit(0);
	}
      else if(strcmp(read_buffer,"SCALE=F") == 0)
	{
	  printf("scale=f\n");
	  scale_flag = 0;
	  fprintf(logfd,"SCALE=F\n");
	}
      else if(strcmp(read_buffer,"SCALE=C") == 0)
	{
	  scale_flag = 1;
	  printf("scale=c\n");
	  fprintf(logfd,"SCALE=C\n");
	}
      else if(read_buffer[0] == 'P' && read_buffer[1] == 'E' &&
	      read_buffer[2] == 'R' && read_buffer[3] == 'I' &&
	      read_buffer[4] == 'O' && read_buffer[5] == 'D')
	{
	  char* endptr;
	  sleep_time = strtol(&read_buffer[7],&endptr,10);
	  printf("period change to %ld s\n", sleep_time);
	  printf("period\n");
	  fprintf(logfd,"%s\n",read_buffer);
	}
      else if(strcmp(read_buffer,"DISP N") == 0)
	{
	  printf("DISP N\n");
	  fprintf(logfd,"DISP N\n");
	}
      else if(strcmp(read_buffer,"DISP Y") == 0)
	{
	  printf("DISP Y\n");
          fprintf(logfd,"DISP Y\n");
        }
      else
	{
	  printf("%s I\n",read_buffer);
	  fprintf(logfd,"%s I\n",read_buffer);
	}
      memset(read_buffer, 0, 256);
    }
  return NULL;
}


SSL_CTX *ssl_init(void)
{
  SSL_CTX *newContext;
  
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  newContext = SSL_CTX_new(TLSv1_client_method());
  if(newContext == NULL)
    error("ERROR can't get SSL context\n");

  return newContext;
}


int main()
{
  //SSL_CTX *sslContext;
  //SSL *sslClient;
  printf("Here\n");
  int sslErr;

  SSL_library_init();
  sslContext = ssl_init();
  sslClient = SSL_new(sslContext);
  if(sslClient == NULL)
    error("ERROR can't get SSL structure\n");

  printf("Here2\n");
  uint16_t temp_value = 0;
  //FILE *fd = fopen("log.txt","w+");                                                     
  //time_t t;
  temp_a0 = mraa_aio_init(0);

  temp_value = mraa_aio_read(temp_a0);
  temp_value = mraa_aio_read(temp_a0);
  temp_value = mraa_aio_read(temp_a0);

  struct sockaddr_in serv_addr;
  struct hostent *server;

  logfd = fopen("lab4_3.log","w+");

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    {
      fprintf(stderr,"INVALID SOCKET\n");
      exit(1);
    }
  printf("Here3\n");
  /*
  if(!SSL_set_fd(sslClient, sockfd))
    {
      printf("here5\n");
      error("ERROR can't connect SSL to fd\n");
    }
  sslErr = SSL_connect(sslClient);
  printf("Here4\n");
  if(sslErr == 1)
    {
      #if DEBUG
      printf("successful SSL connection\n");
      #endif
    }
  else
    {
      printf("Server rejected connectionL ");
      //sslErrMsgs(sslClient,sslErr);
      exit(0);
    }
  */

  printf("Here1\n");

  server = gethostbyname("r01.cs.ucla.edu");
  if(server == NULL)
    {
      fprintf(stderr, "INVALID HOST NAME\n");
      exit(1);
    }

  memset((void *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
  serv_addr.sin_port = htons(SERVER_PORT);
  
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
      fprintf(stderr,"ERROR connecting");
      exit(1);
    }
  printf("connect successful\n");


  if(!SSL_set_fd(sslClient, sockfd))
    {
      printf("here5\n");
      error("ERROR can't connect SSL to fd\n");
    }
  sslErr = SSL_connect(sslClient);
  printf("Here4\n");
  if(sslErr == 1)
    {
      #if DEBUG
      printf("successful SSL connection\n");
      #endif
    }
  else
    {
      printf("Server rejected connectionL ");
      //sslErrMsgs(sslClient,sslErr);                                                     
      exit(0);
    }





  pthread_t read_thread;
  pthread_create(&read_thread, NULL, read_from_server, NULL);

  char buffer[64];
  memset(buffer, 0, 64);

  while(1)
    {
      if(stop_flag != 1)
	{

	  float R = (1023.0/(float)temp_value) - 1.0;
	  temp_value = mraa_aio_read(temp_a0);
	  R = 100000.0 * R;

	  float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;
	  if(scale_flag ==0)
	    temperature = temperature * (9.0/5.0) + 32.0;

	  //int temp = 0;
	  //char *tempchar;
	  //long val = 0;
	  sprintf(buffer, "804586710 TEMP=%.1f",temperature);
	  //val = strtol(buffer, &tempchar, 10);
	  //printf("%ld\n",val);
	  printf("buffer is %s\n", buffer);
	  fprintf(logfd,"%s\n",buffer);
	  SSL_write(sslClient, buffer, strlen(buffer) + 1);
	  sleep(sleep_time);
	}
    }
  return 0;
}
