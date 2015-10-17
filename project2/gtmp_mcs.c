#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "gtmp.h"

/*
    From the MCS Paper: A scalable, distributed tree-based barrier with only local spinning.

    type treenode = record
        parentsense : Boolean
	parentpointer : ^Boolean
	childpointers : array [0..1] of ^Boolean
	havechild : array [0..3] of Boolean
	childnotready : array [0..3] of Boolean
	dummy : Boolean //pseudo-data

    shared nodes : array [0..P-1] of treenode
        // nodes[vpid] is allocated in shared memory
        // locally accessible to processor vpid
    processor private vpid : integer // a unique virtual processor index
    processor private sense : Boolean

    // on processor i, sense is initially true
    // in nodes[i]:
    //    havechild[j] = true if 4 * i + j + 1 < P; otherwise false
    //    parentpointer = &nodes[floor((i-1)/4].childnotready[(i-1) mod 4],
    //        or dummy if i = 0
    //    childpointers[0] = &nodes[2*i+1].parentsense, or &dummy if 2*i+1 >= P
    //    childpointers[1] = &nodes[2*i+2].parentsense, or &dummy if 2*i+2 >= P
    //    initially childnotready = havechild and parentsense = false
	
    procedure tree_barrier
        with nodes[vpid] do
	    repeat until childnotready = {false, false, false, false}
	    childnotready := havechild //prepare for next barrier
	    parentpointer^ := false //let parent know I'm ready
	    // if not root, wait until my parent signals wakeup
	    if vpid != 0
	        repeat until parentsense = sense
	    // signal children in wakeup tree
	    childpointers[0]^ := sense
	    childpointers[1]^ := sense
	    sense := not sense
*/
typedef struct _treenode_t{
  int parentsense;
  int sense;
  int * parentpointer;
  int * childpointers[2];
  int havechild[4];
  int childnotready[4];  
  
}treenode_t;

static treenode_t *nodes;
static int dummy;

void gtmp_init(int num_threads){
  int i;
  int j;  

  /* Setting up the tree */
  nodes = (treenode_t*) malloc(num_threads * sizeof(treenode_t));
  
  dummy = -1;

  for(i = 0; i < num_threads; i++) {
    for(j = 0; j < 4; j++) {
     if(4 * i + j + 1 < num_threads) {
       nodes[i].havechild[j] = 1;
     } else {
       nodes[i].havechild[j] = 0;
     }
     nodes[i].childnotready[j] = nodes[i].havechild[j];
	}
  
	  nodes[i].parentpointer = (i != 0) ? &nodes[(int)((i-1)/4)].childnotready[(i-1) % 4] : &dummy;
	  nodes[i].childpointers[0] = (2*i+1 < num_threads) ? &nodes[2*i+1].parentsense : &dummy;
	  nodes[i].childpointers[1] = (2*i+2 < num_threads) ? &nodes[2*i+2].parentsense : &dummy;
	  nodes[i].parentsense = 0;
	  nodes[i].sense = 1;
	  
	

  }
 
}

void gtmp_barrier(){
   int i;
   int vpid = omp_get_thread_num();
   
   
   for(i = 0; i < 4; i++){
	while(nodes[vpid].childnotready[i] != 0){
        }
   }

   int j;

   for(j = 0; j < 4; j++){
	nodes[vpid].childnotready[j] = nodes[vpid].havechild[j];
   }

   *nodes[vpid].parentpointer = 0;

   if(vpid != 0){
          while(nodes[vpid].parentsense != nodes[vpid].sense){
          }
   }
   *nodes[vpid].childpointers[0] = nodes[vpid].sense;
   *nodes[vpid].childpointers[1] = nodes[vpid].sense;
   nodes[vpid].sense = !nodes[vpid].sense;

}

void gtmp_finalize(){
     if(nodes != NULL) {		
		free(nodes);
	}
}
