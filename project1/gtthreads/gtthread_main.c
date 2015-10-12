#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"
#include <assert.h>
#include <string.h>
 
/* Tests creation.
   Should print "Hello World!" */
 
void test_main_thread_get_self(void);
void test_hello_world(void);
void test_interleaving(void);
void test_thread_accepts_start_arguments(void);
void test_thread_cancelation(void);
void test_thread_join(void);
void test_thread_return_value(void);
void test_thread_return_value_via_gtthread_exit(void);
void test_multilevel_join(void);
void test_recursive_thread_join(void);
void test_join_with_self_fails(void);
void test_join_with_nonExistentThread_fails(void);
void test_join_with_alreadyJoinQueueThread_fails(void);
 
void test_gtthread_mutex_init(void);
void test_single_thread_lock_unlock(void);
void test_locking_on_uninitialized_mutex_fails(void);
void test_unlocking_on_uninitialized_mutex_fails(void);
void test_unlocking_unheld_mutex_fails(void);
void test_destroying_held_mutex_fails(void);
void test_mutex_acquisition_order(void);
 
void test_simple_preemption(void);
void test_preemption(void);
 
void test_mutex_locking_with_preemption(void);
 
void *thr1(void *in) {
  printf("Hello World!\n");
  fflush(stdout);
  return NULL;
}
 
void *thr2(void* in) {
        char* toPrint = (char*)in;
        printf(toPrint);
        return NULL;
}
 
int main() {
 
  gtthread_init(1000);
 
  int i = 10000000;
  while(i-- > 0)
          gtthread_yield();
 
  test_main_thread_get_self();
 
  test_hello_world();
 
  test_thread_accepts_start_arguments();
 
  test_interleaving();
 
  test_thread_cancelation();
 
  test_thread_join();
 
  test_thread_return_value();
 
  test_thread_return_value_via_gtthread_exit();
 
  test_multilevel_join();
 
  test_recursive_thread_join();
 
  test_join_with_self_fails();
 
  test_join_with_nonExistentThread_fails();
 
  test_join_with_alreadyJoinQueueThread_fails();
 
////////////
 
  printf("***BEGIN MUTEX TESTS***\n");
 
  test_gtthread_mutex_init();
 
  test_locking_on_uninitialized_mutex_fails();
 
  test_unlocking_on_uninitialized_mutex_fails();
 
  test_unlocking_unheld_mutex_fails();
 
  test_single_thread_lock_unlock();
 
  test_destroying_held_mutex_fails();
 
  test_mutex_acquisition_order();
 
  printf("***BEGIN PRE-EMPTION TESTS***\n");
 
  //test_simple_preemption();
  test_preemption();
 
  test_mutex_locking_with_preemption();
 
  return EXIT_SUCCESS;
}
 
void test_thread_accepts_start_arguments(void) {
        printf("***Testing that thread accepts start argument***\n");
 
        gtthread_t t1;
 
        char* toPrint = "Thread accepted start argument!\n";
 
        gtthread_create( &t1, thr2, toPrint);
 
        gtthread_yield();
}
 
void test_main_thread_get_self(void) {
        printf("***Testing main thread get self***\n");
        gtthread_t self = gtthread_self();
        assert(self == 1L);
}
 
void test_hello_world(void) {
        printf("***Testing hello world***\n");
 
        gtthread_t t1;
 
        gtthread_create( &t1, thr1, NULL);
 
        gtthread_yield();
}
 
void *interleaving_thread(void *in) {
        steque_t* steque = ((steque_t*)in);
 
        printf("interleaving thread putting first 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"interleaving_thread_1");
        steque_enqueue(steque, (steque_item)"interleaving_thread_2");
        steque_enqueue(steque, (steque_item)"interleaving_thread_3");
        steque_enqueue(steque, (steque_item)"interleaving_thread_4");
        steque_enqueue(steque, (steque_item)"interleaving_thread_5");
 
        gtthread_yield();
 
        printf("interleaving thread putting last 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"interleaving_thread_6");
        steque_enqueue(steque, (steque_item)"interleaving_thread_7");
        steque_enqueue(steque, (steque_item)"interleaving_thread_8");
        steque_enqueue(steque, (steque_item)"interleaving_thread_9");
        steque_enqueue(steque, (steque_item)"interleaving_thread_10");
        return NULL;
}
 
void test_interleaving(void) {
        printf("***Test for interleaving***\n");
 
        gtthread_t t1;
        steque_t* steque = (steque_t*)malloc(sizeof(steque_t));
        steque_init(steque);
 
        gtthread_create( &t1, interleaving_thread, (void*)steque);
 
        printf("main thread putting first 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"main_thread_1");
        steque_enqueue(steque, (steque_item)"main_thread_2");
        steque_enqueue(steque, (steque_item)"main_thread_3");
        steque_enqueue(steque, (steque_item)"main_thread_4");
        steque_enqueue(steque, (steque_item)"main_thread_5");
 
        gtthread_yield();
 
        printf("main thread putting last 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"main_thread_6");
        steque_enqueue(steque, (steque_item)"main_thread_7");
        steque_enqueue(steque, (steque_item)"main_thread_8");
        steque_enqueue(steque, (steque_item)"main_thread_9");
        steque_enqueue(steque, (steque_item)"main_thread_10");
 
        gtthread_yield();
 
        assert(steque_size(steque) == 20);
        char* to_compare;
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_1") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_2") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_3") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_4") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_5") == 0);
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_1") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_2") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_3") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_4") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_5") == 0);
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_6") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_7") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_8") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_9") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_10") == 0);
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_6") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_7") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_8") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_9") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_10") == 0);
 
        free(steque);
}
 
void test_thread_cancelation(void) {
        printf("***Test for thread cancelation***\n");
 
        gtthread_t t1;
        steque_t* steque = (steque_t*)malloc(sizeof(steque_t));
        steque_init(steque);
 
        gtthread_create( &t1, interleaving_thread, (void*)steque);
 
        printf("main thread putting first 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"main_thread_1");
        steque_enqueue(steque, (steque_item)"main_thread_2");
        steque_enqueue(steque, (steque_item)"main_thread_3");
        steque_enqueue(steque, (steque_item)"main_thread_4");
        steque_enqueue(steque, (steque_item)"main_thread_5");
 
        gtthread_yield();
 
        printf("main thread putting last 5 elements\n");
 
        steque_enqueue(steque, (steque_item)"main_thread_6");
        steque_enqueue(steque, (steque_item)"main_thread_7");
        steque_enqueue(steque, (steque_item)"main_thread_8");
        steque_enqueue(steque, (steque_item)"main_thread_9");
        steque_enqueue(steque, (steque_item)"main_thread_10");
 
        //cancel the interleaving thread
        gtthread_cancel(t1);
 
        gtthread_yield();
 
        assert(steque_size(steque) == 15);
        char* to_compare;
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_1") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_2") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_3") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_4") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_5") == 0);
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_1") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_2") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_3") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_4") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_5") == 0);
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_6") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_7") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_8") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_9") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "main_thread_10") == 0);
 
        free(steque);
 
}
 
 
 
void test_thread_join(void) {
        printf("***Test for thread joining***\n");
 
        gtthread_t t1;
        steque_t* steque = (steque_t*)malloc(sizeof(steque_t));
        steque_init(steque);
 
        gtthread_create( &t1, interleaving_thread, (void*)steque);
 
        printf("main thread waiting to join other thread\n");
 
        gtthread_join(t1, NULL);
 
        printf("main thread joined with other thread\n");
 
        assert(steque_size(steque) == 10);
        char* to_compare;
 
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_1") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_2") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_3") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_4") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_5") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_6") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_7") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_8") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_9") == 0);
        to_compare = (char*)steque_pop(steque);
        assert(strcmp(to_compare, "interleaving_thread_10") == 0);
 
        free(steque);
}
 
void * thread_returning_value(void* in) {
        printf("Inside thread about to return a value normally...\n");
        return (void*)"thread_return_value";
}
 
void test_thread_return_value(void) {
        printf("***Test for thread returning value through normal exit***\n");
 
        gtthread_t t1;
 
        gtthread_create( &t1, thread_returning_value, NULL);
 
        void** status = (void**)malloc(sizeof(void*));
        gtthread_join(t1, status);
 
        assert(status != NULL);
 
        assert(strcmp((char*)(*status), "thread_return_value") == 0);
 
        //free(*status);
        free(status);
}
 
void * thread_returning_value_via_gtthread(void* in) {
        printf("Inside thread about to return a value via gtthread...\n");
        gtthread_exit((void*)"thread_return_value_new");
        return NULL;
}
 
void test_thread_return_value_via_gtthread_exit(void) {
        printf("***Test for thread returning value through gtthread***\n");
 
        gtthread_t t1;
 
        gtthread_create( &t1, thread_returning_value_via_gtthread, NULL);
 
        void** status = (void**)malloc(sizeof(void*));
        gtthread_join(t1, status);
 
        assert(status != NULL);
 
        assert(strcmp((char*)(*status), "thread_return_value_new") == 0);
 
        //free(*status);
        free(status);
}
 
void* multi_level_thread_test_level3(void* in) {
        printf("Inside level 3 thread\n");
 
        int* toReturn = (int*)malloc(sizeof(int));
        *toReturn = 3;
        return toReturn;
}
 
void* multi_level_thread_test_level2(void* in) {
        printf("Inside level 2 thread\n");
        gtthread_t t1;
 
        gtthread_create( &t1, multi_level_thread_test_level3, NULL);
 
        void** status = (void**)malloc(sizeof(void*));
        gtthread_join(t1, status);
        printf("Back to level 2 thread!\n");
 
        int retVal = (int)(*((int*)*status));
        assert(retVal == 3);
 
        //free(*status);
        free(status);
        int* toReturn = (int*)malloc(sizeof(int));
        *toReturn = 2;
        return toReturn;
}
 
void* multi_level_thread_test_level1(void* in) {
        printf("Inside level 1 thread\n");
        gtthread_t t1;
 
        gtthread_create( &t1, multi_level_thread_test_level2, NULL);
 
        void** status = (void**)malloc(sizeof(void*));
        gtthread_join(t1, status);
        printf("Back to level 1 thread!\n");
 
        int retVal = (int)(*((int*)*status));
        assert(retVal == 2);
 
        //free(*status);
        free(status);
 
        int* toReturn = (int*)malloc(sizeof(int));
        *toReturn = 1;
        return toReturn;
}
 
 
void test_multilevel_join(void) {
        printf("***Test for multi level join***\n");
        gtthread_t t1;
 
        gtthread_create( &t1, multi_level_thread_test_level1, NULL);
 
        void** status = (void**)malloc(sizeof(void*));
        printf("Main thread waiting on level 1 thread...\n");
        gtthread_join(t1, status);
 
        int retVal = (int)(*((int*)*status));
        assert(retVal == 1);
 
        printf("Back to main thread!\n");
 
        //free(*status);
        free(status);
}
 
void* multilevel_recursive_join(void* recursionLevel) {
        printf("Inside thread at recursion level %d\n", *((int*)recursionLevel));
        int recLevel = *((int*)recursionLevel);
        if (recLevel < 10) {
                gtthread_t t1;
                int* toPass = (int*)malloc(sizeof(int));
                *toPass = recLevel + 1;
                gtthread_create( &t1, multilevel_recursive_join, (void*)toPass);
                void** status = (void**)malloc(sizeof(void*));
                printf("Waiting to join thread at recursion level %d...\n", recLevel + 1);
                gtthread_join(t1, status);
 
                int retVal = (int)(*((int*)*status));
                assert(retVal == (recLevel + 1));
 
                //free(*status);
                free(status);
                free(toPass);
 
        }
 
        int* toReturn = (int*)malloc(sizeof(int));
        *toReturn = recLevel;
        printf("Returning from thread at recursion level %d\n", *((int*)recursionLevel));
        return toReturn;
 
}
 
 
void test_recursive_thread_join(void) {
        printf("***Test for multi level recursive join***\n");
        gtthread_t t1;
 
        int* toPass = (int*)malloc(sizeof(int));
        *toPass = 1;
        gtthread_create( &t1, multilevel_recursive_join, (void*)toPass);
 
        void** status = (void**)malloc(sizeof(void*));
        printf("Main thread waiting on level 1 thread...\n");
        gtthread_join(t1, status);
 
        int retVal = (int)(*((int*)*status));
        assert(retVal == 1);
 
        printf("Back to main thread!\n");
 
        //free(*status);
        free(status);
        free(toPass);
 
}
 
void test_join_with_self_fails(void) {
        printf("***Test that join with self fails***\n");
        assert(gtthread_join(gtthread_self(), NULL) == -1);
}
 
void test_join_with_nonExistentThread_fails(void) {
        printf("***Test that joining with non-existent thread fails***\n");
        assert(gtthread_join(1000000, NULL) == -1);
}
 
void test_join_with_alreadyJoinQueueThread_fails(void) {
        //TODO flesh this out
}
 
////////////////////////////////////
 
void test_locking_on_uninitialized_mutex_fails(void) {
        printf("***Test that lock un-initialized mutex fails***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_lock(&mut_t) == -1);
}
 
void test_unlocking_on_uninitialized_mutex_fails(void) {
        printf("***Test that unlocking un-initialized mutex fails***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_unlock(&mut_t) == -1);
}
 
void test_unlocking_unheld_mutex_fails(void) {
        printf("***Test that unlocking un-held mutex fails***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_init(&mut_t) == 0);
        assert(gtthread_mutex_unlock(&mut_t) == -1);
        assert(gtthread_mutex_destroy(&mut_t) == 0);
}
 
void test_gtthread_mutex_init(void) {
        printf("***Testing gtthread_mutex_init***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_init(&mut_t) == 0);
        assert(gtthread_mutex_destroy(&mut_t) == 0);
}
 
void test_destroying_held_mutex_fails(void) {
        printf("***Test that destroying a held mutex fails***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_init(&mut_t) == 0);
        assert(gtthread_mutex_lock(&mut_t) == 0);
        assert(gtthread_mutex_destroy(&mut_t) == -1);
        assert(gtthread_mutex_unlock(&mut_t) == 0);
        assert(gtthread_mutex_destroy(&mut_t) == 0);
}
 
void test_single_thread_lock_unlock(void) {
        printf("***Testing single thread lock and unlock***\n");
        gtthread_mutex_t mut_t;
        assert(gtthread_mutex_init(&mut_t) == 0);
        assert(gtthread_mutex_lock(&mut_t) == 0);
        printf("Inside critical section!\n");
        assert(gtthread_mutex_unlock(&mut_t) == 0);
        assert(gtthread_mutex_destroy(&mut_t) == 0);
 
}
 
typedef struct mutex_with_args {
        gtthread_mutex_t mut_t;
        void* arg;
        void* retVal;
} mut_args;
 
void* mutex_acquisition_order_thread(void* in) {
        mut_args* margs = (mut_args*)in;
 
        gtthread_mutex_lock(&(margs->mut_t));
 
                printf("Thread %d got the mutex\n", *((int*)margs->arg));
 
                if (*((int*)(margs->retVal)) == -1)
                        *((int*)(margs->retVal)) = *((int*)(margs->arg));
 
                //let the other thread if any try  to acquire the lock and set the value
                int i = 5;
                while (i-- > 0)
                        gtthread_yield();
 
        gtthread_mutex_unlock(&(margs->mut_t));
 
        return NULL;
}
 
 
void test_mutex_acquisition_order(void) {
        printf("***Testing mutex acquisition order***\n");
        mut_args margs1;
        mut_args margs2;
        gtthread_mutex_t mut;
        assert(gtthread_mutex_init(&mut) == 0);
 
        assert(mut > 0);
 
        margs1.mut_t = mut;
        margs2.mut_t = mut;
 
        int t1 = 10;
        int t2 = 20;
        int retVal = -1;
 
        margs1.arg = &t1;
        margs1.retVal = &retVal;
 
        margs2.arg = &t2;
        margs2.retVal = &retVal;
 
        gtthread_t first, second;
        assert(gtthread_create(&first, mutex_acquisition_order_thread, (void*)&margs1) == 0);
        assert(gtthread_create(&second, mutex_acquisition_order_thread, (void*)&margs2) == 0);
 
        //spend some time looping
        int numLoops = 10;
        while(numLoops-- > 0) {
                gtthread_yield();
        }
 
        assert(gtthread_join(first, NULL) == 0);
        assert(gtthread_join(second, NULL) == 0);
 
        assert(retVal == t1); //the first thread should have gotten the chance to set retVal
 
        assert(gtthread_mutex_destroy(&mut) == 0);
}
 
///////////////
 
void* preEmption1(void* in) {
        printf("In pre-emption thread %d\n", (int)gtthread_self());
 
        unsigned long i = 10000000;
        while(i-- > 0) {
 
                if (i == 9000000) {
                        printf("The pre-emption thread is yielding!\n");
                        gtthread_yield();
                        printf("Pre-emption thread is back from yielding!\n");
                }
        }
 
        printf("Pre-emption thread %d finished!\n", (int)gtthread_self());
 
        return NULL;
}
 
void* preEmption2(void* in) {
        printf("In pre-emption thread %d\n", (int)gtthread_self());
 
        unsigned long i = 10000000;
        while(i-- > 0) {
 
        }
 
        printf("Pre-emption thread %d finished!\n", (int)gtthread_self());
 
        return NULL;
}
 
 
void test_simple_preemption(void) {
        printf("***Testing simple pre-emption***\n");
 
 
        gtthread_t first, second;
        gtthread_create(&first, preEmption1, NULL);
        gtthread_create(&second, preEmption2, NULL);
 
        printf("Main thread waiting to join with pre-emption thread 1\n");
        gtthread_join(first, NULL);
        printf("Main thread waiting to join with pre-emption thread 2\n");
        gtthread_join(second, NULL);
 
}
 
int test_preemption_flag = 0;
void* t1(void* in) {
        printf("Got inside thread 1\n");
 
        while(test_preemption_flag == 0);
 
        return NULL;
}
 
void* t2(void* in) {
        printf("Got inside thread 2\n");
        gtthread_yield();
        printf("Thread 2 got back from yielding\n");
        test_preemption_flag = 1;
        return  NULL;
}
 
void test_preemption(void) {
        printf("***Testing for pre-emption - second test***\n");
 
        gtthread_t one, two;
        gtthread_create(&one, t1, NULL);
        gtthread_create(&two, t2, NULL);
 
        gtthread_yield();
        gtthread_join(one, NULL);
        gtthread_join(two, NULL);
 
        printf("Joined both threads!\n");
}
 
////////////
static gtthread_mutex_t lockForPreEmptionTest;
 
void* theMethod(void* in) {
 
        printf("Inside the other thread method!!\n");
 
        assert(gtthread_mutex_lock(&lockForPreEmptionTest) == 0);
        int i = 100000000;
        printf("Other thread beginning to spin...\n");
        while(i-- != 0);
        printf("Other thread now yielding...\n");
        gtthread_yield();
        printf("Other thread back from yielding! Spinning again...\n");
        i = 100000000;
        while(i-- != 0);
        printf("Other thread back from final spin and now releasing lock!!\n");
        int* toSet = (int*)in;
        *toSet = 56; //this will get checked in the test method
        assert(gtthread_mutex_unlock(&lockForPreEmptionTest) == 0);
 
        return NULL;
}
 
 
void test_mutex_locking_with_preemption(void) {
        printf("***Testing for pre-emption when locking a mutex***\n");
 
        gtthread_t t1;
        int test = 5;
        gtthread_create(&t1, theMethod, &test);
        gtthread_mutex_init(&lockForPreEmptionTest);
 
        //let the other thread go
        printf("Main thread now yielding so the other thread gets a chance...\n");
        gtthread_yield();
        assert(gtthread_mutex_lock(&lockForPreEmptionTest) == 0);
        printf("Main thread finally got the lock!\n");
        assert(test == 56);
        assert(gtthread_mutex_unlock(&lockForPreEmptionTest) == 0);
        assert(gtthread_mutex_destroy(&lockForPreEmptionTest) == 0);
 
 
 
}