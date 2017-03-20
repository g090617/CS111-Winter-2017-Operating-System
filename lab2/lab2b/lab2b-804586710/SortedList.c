#include "SortedList.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  //printf("in insert\n");
  //printf("key is %s\n",element -> key);
  SortedListElement_t *temp = list -> next;
  if( temp == NULL)//empty list                                                 
    {                                                                                   
      if(opt_yield == 1 || opt_yield == 5)                                               
        pthread_yield();                                                                 
      //printf("before first element\n");                                               
      list -> next = element;                                                           
      element -> prev = list;                                                            
      element -> next = NULL;
      return;
      //printf("inserted first element");                                                
    }

  while(temp != NULL)
    {
      //printf("key is %s\n",element -> key);
      if(strcmp(element -> key, temp -> key) > 0)
	{
	  //printf("element is greater\n");
	  if(temp -> next == NULL) //last element in list                             
	    {                                                                         
	      if(opt_yield == 1 || opt_yield == 5)                                    
		pthread_yield();                                                      
	      temp -> next = element;                                                 
	      element -> next = NULL;                                                 
	      element -> prev = temp;                                                 
	      return;                                                                 
	    }                                                                         
	  else                                                                        
	    temp = temp -> next;
	  //getchar();
	}
      else
	{
	  //printf("element is smaller or equal\n");
	  if(opt_yield == 1 || opt_yield == 5)                                        
	    pthread_yield();                                                          
	  element -> next = temp;                                                     
	  element -> prev = temp -> prev;                                             
	  temp -> prev ->next = element;                                              
	  temp -> prev = element;                                                     
	  return;  
	}
    }
  /*
  printf("in sorted list insert\n");

  if( list -> next == NULL)//empty list
    {
      if(opt_yield == 1 || opt_yield == 5)
	pthread_yield();
      //printf("before first element\n");
      list -> next = element;
      element -> prev = list;
      element -> next = NULL;
      //printf("inserted first element");
    }
  else
    {
      //printf("Not first element\n");
      int i = 0;
      SortedListElement_t *temp;
      temp = list -> next;
      while(temp != NULL)
	{
	  printf("keep insert\n");
	  if( strcmp(element -> key, temp -> key) >= 0)
	    {
	      if(temp -> next == NULL) //last element in list
		{
		  if(opt_yield == 1 || opt_yield == 5)
		    pthread_yield();
		  temp -> next = element;
		  element -> next = NULL;
		  element -> prev = temp;
		  return;
		}
	      else
		temp = temp -> next;
	    }
	  else
	    {
	      if(opt_yield == 1 || opt_yield == 5)
		pthread_yield();
	      element -> next = temp;
	      element -> prev = temp -> prev;
	      temp -> prev ->next = element;
	      temp -> prev = element;
	      return;
	    }
	}
    }
  */
}

int SortedList_delete(SortedListElement_t *element)
{
  //printf("in delete\n");
  //printf("key is %s\n",element -> key);
  if(element -> prev == NULL )
    {
      printf("element -> prev -> next == NULL\n");
      getchar();
    }
  if (element -> prev -> next == element && 
      element -> next == NULL)//last element in list
    {
      //printf("last element in the list\n");
      if(opt_yield == 2 || opt_yield == 6)
	pthread_yield();
      //printf("1\n");
      element -> prev -> next = NULL;
      //printf("2\n");
      element -> prev = NULL;
      //printf("3\n");
      element -> next = NULL;
      //printf("4\n");
      element -> key = NULL;
      //printf("5\n");
      element = NULL;
      //printf("5\n");
      return 0;
    }
  else if( element -> prev -> next == element &&
	   element -> next -> prev == element)
    {
      //printf("not last\n");
      if(opt_yield == 2 || opt_yield == 6)
        pthread_yield();
      element -> prev -> next = element -> next;
      //printf("1\n");
      element -> next -> prev = element -> prev;
      //printf("2\n");
      element -> prev =NULL;
      //printf("3\n");
      element -> next =NULL;
      //printf("4\n");
      //element -> key = NULL;
      element = NULL;
      return 0;
    }
  return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  //printf("in lookup\n");
  if(key == NULL)
    {
      //  printf("key is null\n");
      getchar();
    }
  SortedListElement_t *temp = list -> next;
  if(temp -> key == NULL)
    {
      //printf("temp is NULL\n");
      getchar();
    }
  while(temp != NULL)
    {
      if(strcmp(temp -> key,key) == 0)
	{
	  //printf("find key\n");
	  return temp;
	}
      if(opt_yield == 5 || opt_yield == 6)
        pthread_yield();
      temp = temp -> next;
    }
  //printf("look for %s\n",key);
  //printf("nothing found");
  return NULL;
}

int SortedList_length(SortedList_t *list)
{
  //printf("in length\n");
  int count = 0;
  SortedListElement_t *temp = list -> next;
  while(temp != NULL)
    {
      count++;
      if(opt_yield == 5 || opt_yield == 6)
        pthread_yield();
      temp = temp -> next;
    }
  return count;
}
