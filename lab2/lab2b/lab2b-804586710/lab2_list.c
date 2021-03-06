#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "SortedList.h"

long long total_m_lock = 0;
int opt_yield = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct timespec start, end, l_start, l_end;


typedef struct lock_t
{
  int flag;
}lock_t;

lock_t spin_lock;

void init(lock_t *lock)
{
  lock -> flag = 0;
}

void slock(lock_t *lock)
{
  while( __sync_lock_test_and_set(&lock ->flag, 1) == 1);
}

void unlock(lock_t *lock)
{
  __sync_lock_release(&lock -> flag);
  //lock -> flag = 0;                                                                     
}

void cslock(lock_t *lock)
{
  while(__sync_val_compare_and_swap(&lock -> flag, 0, 1) == 1);
}

void csunlock(lock_t *lock)
{
  lock -> flag = 0;
}


struct _add_arg
{
  SortedList_t *pointer;
  SortedListElement_t *work;
  int iteration;
  int yield;
  int num_list;
  char sync;
};

void* mission(void * arg)
{
  struct _add_arg* info = (struct _add_arg *)arg;
  SortedList_t* pointer = info ->pointer;
  int iteration = info -> iteration;
  int yield = info -> yield;
  int list_num = info -> num_list;
  int remainder = 0;
  char sync = info -> sync;
  SortedListElement_t *work = info -> work;
  struct timespec temp_start, temp_end;

  //printf("work is %s\n",work[0].key);
  int i = 0;
  //printf("Before insert\n");
  for(i = 0; i < iteration; i++)
    {
      //printf("before call insert\n");
      clock_gettime(CLOCK_MONOTONIC, &temp_start);

      if(sync == 'm')
	{
	  pthread_mutex_lock(&lock);
	  //printf("mutex\n");
	}
      else if(sync == 's')
        slock(&spin_lock);
      else if(sync == 'c')
        cslock(&spin_lock);

      clock_gettime(CLOCK_MONOTONIC, &temp_end);

      total_m_lock = total_m_lock +
	(long long) temp_end.tv_sec*1000000000 +
	(long long) temp_end.tv_nsec -
	(long long) temp_start.tv_sec*1000000000 -
	(long long) temp_start.tv_nsec;
      //printf("work is %s\n", work[i].key);
      //printf("before insert list num is %d\n",list_num);
      remainder = i % list_num;
      //printf("remainder is %d\n");
      SortedList_insert(&pointer[remainder], &work[i]);
      if(sync == 'm')
        pthread_mutex_unlock(&lock);
      else if(sync == 's')
        unlock(&spin_lock);
      else if(sync == 'c')
        csunlock(&spin_lock);
    }
  
  for(i= 0; i < iteration; i++)
    {
      clock_gettime(CLOCK_MONOTONIC, &temp_start);

      if(sync == 'm')
      pthread_mutex_lock(&lock);
      else if(sync == 's')
      slock(&spin_lock);
      else if(sync == 'c')
      cslock(&spin_lock);

      clock_gettime(CLOCK_MONOTONIC, &temp_end);

      total_m_lock = total_m_lock +
	(long long) temp_end.tv_sec*1000000000 +
	(long long) temp_end.tv_nsec -
	(long long) temp_start.tv_sec*1000000000 -
	(long long) temp_start.tv_nsec;
      // printf("before look up\n");
      //printf("i is %d\n",i);
      //printf("work[i] is %s\n",work[i].key);
      SortedListElement_t *temp = SortedList_lookup(&pointer[i % list_num], work[i].key);
      //  printf("after look up\n");
      if(temp == NULL)
        {
          fprintf(stderr,"look up not found\n");
          exit(1);
        }
      //printf("before delete\n");
      //if(sync == 'm')
      //pthread_mutex_lock(&lock);
      //else if(sync == 's')
      //slock(&spin_lock);
      //else if(sync == 'c')
      //cslock(&spin_lock);
      int delete_status = SortedList_delete(temp) ;
      if(delete_status == 1)
	{
	  fprintf(stderr,"corrupted list\n");
	  exit(1);
	}
      if(sync == 'm')
        pthread_mutex_unlock(&lock);
      else if(sync == 's')
        unlock(&spin_lock);
      else if(sync == 'c')
        csunlock(&spin_lock);
    }   
  //printf("finishi delete ==========\n");
  //printf("length is %d\n",SortedList_length(pointer));  
}

int main( int argc, char **argv)
{
  int opt = 0;
  int option_index = 0;
  int iteration = 0;
  int num_threads = 1;
  int list_num = 1;

  char sync_val = '\0';
  char *yield_val = NULL;

  char *stringset = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890";
  int stringset_len = strlen(stringset);

  
  
  while(1)
    {
      static struct option long_options[] =
        {
          {"iterations", required_argument, NULL, 'i'},
          {"threads",    required_argument, NULL, 't'},
          {"yield",      required_argument, NULL, 'y'},
          {"sync",       required_argument, NULL, 's'},
	  {"lists",      required_argument, NULL, 'l'},
          {0,            0,                 0,     0 }
        };
      
      
      opt = getopt_long(argc,argv, "itsy", long_options, &option_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case 'i':
	  //printf("iterations %s\n",optarg);                                    
	  iteration = atoi(optarg);
	  //printf("iteration is %d\n", iteration);                                    
	  break;
	case 't':
	  //printf("threads %s\n",optarg);                                              
	  num_threads = atoi(optarg);
	  //printf("num threads is %d\n", num_threads);                                
	  break;
	case 'y':
	  yield_val = optarg;
	  break;
	case 's':
	  sync_val = *optarg;
	  break;
	case 'l':
	  list_num = atoi(optarg);
	  break;
	case '?':
	  break;
	default:
	  break;
	}
    }

  SortedListElement_t elements[num_threads][iteration];
  int i, j,k, randlen;
  char *randkey;

  srand(time(NULL));

  for(i = 0; i < num_threads; i++)
    {
      for(j = 0; j < iteration; j++)
	{
	  randlen = rand() % 10 + 1;
	  randkey = malloc(sizeof(char)*11);

	  for(k = 0; k < randlen; k++)
	    {
	      //printf("randkey[k] is %c\n",stringset[rand() % stringset_len]);
	      randkey[k] = stringset[rand() % stringset_len];
	      //printf("randkey is %s\n",randkey);
	    }
	  randkey[randlen] = '\0';
	  elements[i][j].key = randkey;
	  //printf("element[i][j] is %s\n",elements[i][j].key);
	}
    }
  
  SortedList_t list[list_num];
  for(i = 0; i < list_num; i++)
    list[i].next = NULL;

  int status;
  long long counter = 0;
  pthread_t threadid[num_threads];
  int temp = 1;
  long long nanosecond = 0;
  int operation = 0;
  long throughput = 0;

  if(yield_val == NULL)
    {
      opt_yield = 0;
    }
  else if(strcmp(yield_val, "i"))
    {
      opt_yield = 1;
    }
  else if(strcmp(yield_val, "d"))
    {
      opt_yield = 2;
    }
  else if(strcmp(yield_val, "il"))
    {
      opt_yield = 5;
    }
  else if(strcmp(yield_val, "dl"))
    {
      opt_yield = 6;
    }
  
  struct _add_arg info[num_threads];
  
  init(&spin_lock);
  
  for( i = 0; i < num_threads; i ++)
    {
      info[i].pointer = &list[0];
      info[i].iteration = iteration;
      info[i].yield = opt_yield;
      info[i].num_list = list_num;
      info[i].sync = sync_val;
      info[i].work = elements[i];
    }
  
  clock_gettime(CLOCK_MONOTONIC, &start);

  for( i = 0; i < num_threads; i ++)
    {
      pthread_create(&threadid[i], NULL, &mission, &info[i]);
    }

  for( i = 0; i < num_threads; i ++)
    {
      pthread_join(threadid[i], NULL);
    }

  clock_gettime(CLOCK_MONOTONIC, &end);

  nanosecond =
    (long long) end.tv_sec*1000000000 +
    (long long) end.tv_nsec -
    (long long) start.tv_sec*1000000000 -
    (long long)start.tv_nsec;

  operation = num_threads * iteration * 3;
  throughput = nanosecond/(long)operation;

  printf("list");
  if(yield_val != NULL)
    printf("-%s",yield_val);
  else
    printf("-none");
  if(sync_val == 'm')
    printf("-m");
  else if(sync_val == 'c')
    printf("-c");
  else if(sync_val == 's')
    printf("-s");
  if(sync_val == '\0')
    printf("-none");

  int num_lock = 2*iteration*num_threads;
  printf(",%d,%d,%d,%d,%lld,%ld,%lld\n",
         num_threads,
         iteration,
	 list_num,
         operation,
         nanosecond,
         throughput,
	 total_m_lock/num_lock);
  //printf("length is %d\n",SortedList_length(&list));    
  return 0;
}
