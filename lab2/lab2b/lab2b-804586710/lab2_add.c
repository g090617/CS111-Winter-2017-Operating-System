#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void add(long long *pointer, long long value) 
{
  long long sum = *pointer + value;
  *pointer = sum;
}


int opt_yield;
void add2(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    pthread_yield();
  *pointer = sum;
}

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
  long long *pointer;
  //long long value;
  int iteration;
  int yield;
  char sync;
};

void* mission(void * arg)
{
  struct _add_arg* info = (struct _add_arg *)arg;
  long long* pointer = info ->pointer;
  int iteration = info -> iteration;
  int yield = info -> yield;
  int sync = info -> sync;

  int i = 0;

  

  for( i = 0; i < iteration; i++)
    {
      if(sync == 'm')
	pthread_mutex_lock(&lock);
      else if(sync == 's')
	slock(&spin_lock);
      else if(sync == 'c')
	cslock(&spin_lock);
      if(yield)
	add2(pointer, 1);
      else
	add(pointer, 1);
      if(sync == 'm')
	pthread_mutex_unlock(&lock);
      else if(sync == 's')
	unlock(&spin_lock);
      else if(sync == 'c')
	csunlock(&spin_lock);
    }

  for( i = 0; i < iteration; i++)
    {
      if(sync == 'm')
	pthread_mutex_lock(&lock);
      else if(sync == 's')
	slock(&spin_lock);
      else if(sync == 'c')
        cslock(&spin_lock);
      if(yield)
	add2(pointer, -1);
      else
	add(pointer, -1);
      if(sync == 'm')
	pthread_mutex_unlock(&lock);
      else if(sync == 's')
	unlock(&spin_lock);
      else if(sync == 'c')
        csunlock(&spin_lock);
    }
}


int main( int argc, char **argv)
{
  int opt = 0;
  int option_index = 0;
  int iteration = 0;
  int num_threads = 1;

  char sync_val = '\0';
  struct timespec start, end;
  
  while(1)
    {
      static struct option long_options[] = 
	{
	  {"iterations", required_argument, NULL, 'i'},
	  {"threads",    required_argument, NULL, 't'},
	  {"yield",      no_argument,       NULL, 'y'},
	  {"sync",       required_argument, NULL, 's'},
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
      opt_yield = 1;
      break;
    case 's':
      sync_val = *optarg;
      break;
    case '?':
      break;
    default:
      break;
    }
    }

  int i = 0;
  int status;
  long long counter = 0;
  pthread_t threadid[num_threads];
  int temp = 1;
  long long nanosecond = 0;
  int operation = 0;
  long throughput = 0;

  struct _add_arg info;
  info.pointer = &counter;
  info.iteration = iteration;
  info.sync = sync_val;
  info.yield = opt_yield;

  clock_gettime(CLOCK_MONOTONIC, &start);
  
  init(&spin_lock);
	
  for( i = 0; i < num_threads; i ++)
    {
      pthread_create(&threadid[i], NULL, &mission, &info);
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
  operation = num_threads * iteration * 2;
  throughput = nanosecond/(long)operation;

  

  printf("add");
  if(opt_yield)
    printf("-yield");
  if(sync_val == 'm')
    printf("-m");
  else if(sync_val == 'c')
    printf("-c");
  else if(sync_val == 's')
    printf("-s");
  if(sync_val == '\0' && opt_yield == 0)
    printf("-none");

  printf(",%d,%d,%d,%lld,%ld,%lld\n", 
	 num_threads, 
	 iteration,
	 operation,
	 nanosecond,
	 throughput,
	 counter);
  // printf("end = %lld \n",(long long) end.tv_sec*1000000000 + (long long) end.tv_nsec);
  // printf("start = %lld \n",(long long) start.tv_sec*1000000000 + (long long)start.tv_nsec);
  return 0;
}
