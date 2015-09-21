/**********************************************************************
gtthread_mutex.c.  

This file contains the implementation of the mutex subset of the
gtthreads library.  The locks can be implemented with a simple queue.
 **********************************************************************/

/*
  Include as needed
*/

#include <stdio.h>
#include <stdlib.h>

#include "gtthread.h"

/*
  The gtthread_mutex_init() function is analogous to
  pthread_mutex_init with the default parameters enforced.
  There is no need to create a static initializer analogous to
  PTHREAD_MUTEX_INITIALIZER.
 */
int gtthread_mutex_init(gtthread_mutex_t* mutex){
  steque_init(&mutex->queue);
  mutex->locked = false;
  mutex->locked_thread_id = -1;
  return 0;
}

/*
  The gtthread_mutex_lock() is analogous to pthread_mutex_lock.
  Returns zero on success.
 */
int gtthread_mutex_lock(gtthread_mutex_t* mutex){
  if (mutex->locked == false) {
    mutex->locked = true;
    mutex->locked_thread_id = current_thread->thread_id;    
    return 0;
  }
  
  /*queue the thread on the wait queue*/
   steque_enqueue(&mutex->queue, &current_thread->thread_id);
   
   /* spin while locked*/
   while(1){
      while (mutex->locked == true) {
      gtthread_yield();
    }
	
	/* get next thread in the queue to be run*/
	int * next_thread_id = (int *)steque_front(&mutex->queue);	
    if (*next_thread_id == current_thread->thread_id) {
      steque_pop(&mutex->queue);
      mutex->locked = true;
      mutex->locked_thread_id = current_thread->thread_id;      
      return 0;
    } else {      
      gtthread_yield();
    }
	
   }
   
   return -1;
}

/*
  The gtthread_mutex_unlock() is analogous to pthread_mutex_unlock.
  Returns zero on success.
 */
int gtthread_mutex_unlock(gtthread_mutex_t *mutex){
   mutex->locked_thread_id = -1;
   mutex->locked = false;
   return 0;
}

/*
  The gtthread_mutex_destroy() function is analogous to
  pthread_mutex_destroy and frees any resourcs associated with the mutex.
*/
int gtthread_mutex_destroy(gtthread_mutex_t *mutex){
  steque_destroy(&mutex->queue);
  return 0;
}
