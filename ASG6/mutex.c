///////////////////////////////////////////////////////
// Mutex implementation
// This implementation provides MUTEX_MAXNUMBER mutex locks
// for use by user processes; the lock number is specified 
// using an 8-bit number (called key), which restricts us to 
// have up to 256 mutex variables
// DO NOT use a mutex object that has not been created; the functions
// always return in such cases
// TODO: Allow user process to create own mutex object

#include "kernel_only.h"

MUTEX mx[MUTEX_MAXNUMBER];	// the mutex locks; maximum 256 of them

/*** Initialize all mutex objects ***/
void init_mutexes() {
	int i;
	for (i=0; i<MUTEX_MAXNUMBER; i++) {
		mx[i].available = TRUE; // the mutex is available for use
		mx[i].lock_with = NULL;
		init_queue(&(mx[i].waitq));
	}
	mx[0].available = FALSE;
}

/*** Create a mutex object ***/
// At least one of the cooperating processes (typically
// the main process) should create the mutex before use
// The function returns 0 if no mutex objects are available;
// otherwise the mutex object number is returned
mutex_t mutex_create(PCB *p) {
    //initialize iterator at 1, not 0
    int i = 1;
    for(i; i< MUTEX_MAXNUMBER; i++)
    {
        if(mx[i].available == TRUE)
        {
            mx[i].available = FALSE;

            mx[i].creator = p->pid;

            mx[i].lock_with = NULL;

            p->mutex.wait_on = -1;

            return i;
        }
    }

    //need to keep this return statement
    //incase there was no mutex
	return 0;
}

/*** Destroy a mutex with a given key ***/
// This should be called by the process who created the mutex
// using mutex_create; the function makes the mutex key available
// for use by other creators.
// Mutex is automatically destroyed if creator process dies; creator
// process should always destroy a mutex when no other process is
// holding a lock on it; otherwise the behavior is undefined
void mutex_destroy(mutex_t key, PCB *p) {
    mx[key].available = TRUE;

    //get rid of all procs that are waiting
    while(TRUE)
    {
        PCB* x = dequeue(&mx[key].waitq);

        //terminate when we find a NULL proc
        if(x == NULL) break;
    }


}

/*** Obtain lock on mutex ***/
// Return true if process p is able to obtain a lock on mutex
// number <key>; otherwise the process is queued and FALSE is
// returned.
// Non-recursive: if the process holding the lock tries
// to obtain the lock again, it will cause a deadlock
bool mutex_lock(mutex_t key, PCB *p) {

    if(mx[key].available == FALSE)
    {
        if(mx[key].lock_with == NULL)
        {
            mx[key].lock_with = p;
            p->mutex.wait_on = -1;
            return TRUE;
        }

        else
        {
            p->mutex.queue_index = enqueue(&mx[key].waitq, p);
            p->mutex.wait_on = key;
        }
    }
	return FALSE;
}

/*** Release a previously obtained lock ***/
// Returns FALSE if lock is not owned by process p;
// otherwise the lock is given to a waiting process and TRUE
// is returned
bool mutex_unlock(mutex_t key, PCB *p) {
    if(mx[key].available == FALSE && mx[key].lock_with == p)
    {
        mx[key].lock_with = NULL;
        PCB* temp = dequeue(&mx[key].waitq);

        //if we have a valid entry at begining of queue
        if(temp != NULL)
        {
            temp->state = READY;
            temp->mutex.wait_on = -1;
            mx[key].lock_with = temp;
        }
        return TRUE;
    }
	return FALSE;
}

/*** Cleanup mutexes for a process ***/
void free_mutex_locks(PCB *p) {
	int i;

	for (i=1; i<MUTEX_MAXNUMBER; i++) {
		// see if process is creator of the mutex
		if (p->pid == mx[i].creator) mutex_destroy((mutex_t)i,p);
	}

	// remove from wait queue, if any
	if (p->mutex.wait_on != -1) 
		remove_queue_item(&mx[p->mutex.wait_on].waitq, p->mutex.queue_index);	
}



