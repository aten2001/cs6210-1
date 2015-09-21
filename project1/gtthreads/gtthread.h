#ifndef GTTHREAD_H
#define GTTHREAD_H

#include "steque.h"
#include <ucontext.h>

#define bool int
#define true 1
#define false 0

/* Define gtthread_t and gtthread_mutex_t types here */

/* structure for the details of a gtthread_t*/
typedef struct gtthread_id{
  int thread_id;
  ucontext_t thread_context;
  void* return_value;
  bool cancelled;
  bool finished;
  int joined_thread_id;
}gtthread_t;

/* structure for the details of a gtthread_mutex_t */
typedef struct gtthread_mutex_t {
  steque_t queue; 
  bool locked;
  int locked_thread_id;
} gtthread_mutex_t;

extern gtthread_t * current_thread;

void gtthread_init(long period);
int  gtthread_create(gtthread_t *thread,
                     void *(*start_routine)(void *),
                     void *arg);
int  gtthread_join(gtthread_t thread, void **status);
void gtthread_exit(void *retval);
void gtthread_yield(void);
int  gtthread_equal(gtthread_t t1, gtthread_t t2);
int  gtthread_cancel(gtthread_t thread);
gtthread_t gtthread_self(void);


int  gtthread_mutex_init(gtthread_mutex_t *mutex);
int  gtthread_mutex_lock(gtthread_mutex_t *mutex);
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex);
int  gtthread_mutex_destroy(gtthread_mutex_t *mutex);
#endif
