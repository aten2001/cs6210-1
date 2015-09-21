/**********************************************************************
gtthread_sched.c.  

This file contains the implementation of the scheduling subset of the
gtthreads library.  A simple round-robin queue should be used.
 **********************************************************************/
/*
  Include as needed
*/
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "gtthread.h"
/* 
   Students should define global variables and helper functions as
   they see fit.
 */


/* Some global constants. */
#define MAX_THREADS 100
#define THREAD_STACK_SIZE 16384

/* global variables */
int thread_id_counter = 0;
steque_t queue;
static sigset_t vtalrm;

gtthread_t* threads[MAX_THREADS];
gtthread_t main_thread;
gtthread_t* current_thread;

/* helper functions */
void scheduler(int sig){
  ucontext_t * current_context;
  /* put finished thread on the back of the queue */
  sigprocmask(SIG_BLOCK, &vtalrm, NULL);
  /* if there are no mre threads to run*/
  
  if (steque_isempty(&queue)){
    /* if the current thread is finished end program*/
	if(current_thread->finished || current_thread->cancelled){
	  /* swap context to main*/
	  //printf("current thread id: %d\n",current_thread->thread_id);
	  current_context = &current_thread->thread_context;
	  current_thread = &main_thread;
	  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
	  swapcontext(current_context, &main_thread.thread_context);
	  return;
	}
	
	/* keep running the current thread if nothing is queued*/
	sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
	return;
  }
    //printf("current thread id: %d\n",current_thread->thread_id);
    //printf("I am in scheduler\n");
  
  /* get the thread at the head of the queue*/ 
  gtthread_t* next_thread = steque_pop(&queue);
  /* make sure the thread is not cancelled or finished*/ 
  if(!current_thread->finished && !current_thread->cancelled){
    steque_enqueue(&queue, current_thread);
  }
  
  if (current_thread != NULL) {
    current_context = &current_thread->thread_context;
  } else {
    current_context = &main_thread.thread_context;
  }
  /* swap context*/  
  current_thread = next_thread;
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
  swapcontext(current_context, &next_thread->thread_context);
  
}

void run_thread(void *(*start_routine)(void *), void* args)
{
  void * return_value = start_routine(args);
  //printf("return value: %d\n", return_value);
  //printf("in run_thread id: %d\n",main_thread.thread_id);
  if (gtthread_equal(*current_thread, main_thread)) {
    int * t = (int *)return_value;
	 exit(*t);
  } else {
    gtthread_exit(return_value);
  }
}

/*
  The gtthread_init() function does not have a corresponding pthread equivalent.
  It must be called from the main thread before any other GTThreads
  functions are called. It allows the caller to specify the scheduling
  period (quantum in micro second), and may also perform any other
  necessary initialization.  If period is zero, then thread switching should
  occur only on calls to gtthread_yield().

  Recall that the initial thread of the program (i.e. the one running
  main() ) is a thread like any other. It should have a
  gtthread_t that clients can retrieve by calling gtthread_self()
  from the initial thread, and they should be able to specify it as an
  argument to other GTThreads functions. The only difference in the
  initial thread is how it behaves when it executes a return
  instruction. You can find details on this difference in the man page
  for pthread_create.
 */
void gtthread_init(long period){
	/* Initialize the threads queue. */
	steque_init(&queue);
	 
	/* Setting up the signal mask */
	sigemptyset(&vtalrm);
	sigaddset(&vtalrm, SIGVTALRM);
	
	/* Set up a signal handler. */
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_handler = &scheduler;
	if (sigaction(SIGVTALRM, &act, NULL) < 0) {
	  perror ("sigaction");
      exit(1);
    }
	/* Set up the timer */
	if (period > 0) {
      struct itimerval* T = (struct itimerval*) malloc(sizeof(struct itimerval));
      T->it_value.tv_sec = T->it_interval.tv_sec = 0;
      T->it_value.tv_usec = T->it_interval.tv_usec = period;
      setitimer(ITIMER_VIRTUAL, T, NULL);
    }

	/* Set up the context for main and inialize it. */
	main_thread.thread_id = -100;
	main_thread.joined_thread_id = -1;
	if (getcontext(&main_thread.thread_context)  == -1) {
      perror("Unable to getcontext for main");
      exit(EXIT_FAILURE);
    }
	current_thread = &main_thread;
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread,
		    void *(*start_routine)(void *),
		    void *arg){
  thread->finished = false;
  thread->cancelled = false;
  thread->thread_id = thread_id_counter++;
  thread->joined_thread_id = -1;
  /* check number of threads*/
  if (thread->thread_id >= MAX_THREADS) {
    perror("Max thread count reached!");
    exit(EXIT_FAILURE);
  }

  /* get thread context*/
  if (getcontext(&thread->thread_context) < 0) {
    perror("Unable to getcontext");
    exit(EXIT_FAILURE);
  }
  
  
  /* Allocate a thread stack */
  
  thread->thread_context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
  thread->thread_context.uc_stack.ss_size = SIGSTKSZ;
  thread->thread_context.uc_link = &main_thread.thread_context;
  
  makecontext(&thread->thread_context, run_thread, 2, start_routine, arg);
  
  threads[thread->thread_id] = thread;
  sigprocmask(SIG_BLOCK, &vtalrm, NULL);
  steque_enqueue(&queue, thread);
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
  
  //printf("created thread id: %d\n", thread->thread_id);
  return 0;
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){
  gtthread_t* joined_thread;
  /* cant join itself*/  
  if (gtthread_equal(thread, gtthread_self())) {
    return -1;
  }
  
  /*get the thread requested*/
  /* check first if it is main thread*/
  
   if(thread.thread_id == -100){
     joined_thread = &main_thread;
   }else{
     joined_thread = threads[thread.thread_id];
   }
   
  //gtthread_t* joined_thread = threads[thread.thread_id];
  
 
  //printf("I am here\n");
  //printf("trying to join thread id: %d \n", joined_thread->thread_id);
  /*check for deadlock scenario*/
   //printf("current_thread->joined_thread_id: %d\n", current_thread->joined_thread_id);
   //printf("joined_thread->thread_id): %d\n", joined_thread->joined_thread_id);
  if (current_thread->joined_thread_id == joined_thread->thread_id) {
     return -2;
  }
  
  joined_thread->joined_thread_id = current_thread->thread_id;
  
  /* block until the joined thread is finshed or cancelled*/
  while (!joined_thread->finished && !joined_thread->cancelled) {
    gtthread_yield();
  }
  
  /* set the status of the joined thread to the */
  if (status != NULL) {
    *status = joined_thread->return_value;
  }
  
  return 0;
}

/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){
  current_thread->return_value = retval;
  current_thread->finished = true;
  //printf("exiting thread id: %d\n", current_thread->thread_id);
  raise(SIGVTALRM);
}


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
void gtthread_yield(void){
  raise(SIGVTALRM);
}

/*
  The gtthread_equal() function is analogous to pthread_equal,
  returning non-zero if the threads are the same and zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){
  return t1.thread_id == t2.thread_id;
}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread){
  threads[thread.thread_id]->cancelled = true;
  return 0;
}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
  return *current_thread;
}
