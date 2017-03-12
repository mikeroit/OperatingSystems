///////////////////////////////////////////////////////
// Semaphore implementation
// This implementation provides SEM_MAXNUMBER semaphores
// for use by user processes; the semaphore number is specified 
// using an 8-bit number (called key), which restricts us to 
// have up to 256 semaphore variables
// DO NOT use a semaphore object that has not been created; the functions
// always return in such cases
// TODO: Allow user process to create own semaphore object

#include "kernel_only.h"

SEMAPHORE sem[SEM_MAXNUMBER];	// the semaphore locks; maximum 256 of them 

/*** Initialize all semaphorees ***/
void init_semaphores() {
	int i;
	for (i=0; i<SEM_MAXNUMBER; i++) {
		init_queue(&(sem[i].waitq));
		sem[i].value = 0;
		sem[i].available = TRUE;
	}
	sem[0].available = FALSE;
}

/*** Create a semaphore object ***/
// At least one of the cooperating processes (typically
// the main process) should create the semaphore before use.
// The function returns 0 if no semaphore object is
// available; otherwise the semaphore object number is returned 
// init_value is the start value of the semaphore
sem_t semaphore_create(uint8_t init_value, PCB *p) {
    int i = 1; //index at 1 for same reason as in mutex.c
    for(i; i < SEM_MAXNUMBER; i++)
    {
        if(sem[i].available == TRUE)
        {
            sem[i].available = FALSE;
            sem[i].creator = p->pid;
            p->semaphore.wait_on = -1;
            sem[i].value = init_value;
            return i;
        }
    }

    //keep this return statement for if there is not a semaphore available
	return 0;
}

/*** Destroy a semaphore with a given key ***/
// This should be called by the process who created the semaphore
// using semaphore_create; the function makes the semaphore key available
// for use by other creators
// Semaphore is automatically destroyed if creator process dies; creator
// process should always destroy a semaphore when no other process is
// using it; otherwise the behavior is undefined
void semaphore_destroy(sem_t key, PCB *p) {
    sem[key].available = TRUE;

    //loop until all procs are dequeued
    while(TRUE)
    {
        PCB* x = dequeue(&sem[key].waitq);
        if(x == NULL)
        {
            break;
        }
    }

}

/*** DOWN operation on a semaphore ***/
// Returns TRUE if process p is able to obtain semaphore
// number <key>; otherwise the process is queued and FALSE is
// returned.
bool semaphore_down(sem_t key, PCB *p) {
    if(sem[key].available == FALSE)
    {
        if(sem[key].value > 0)
        {
            sem[key].value -= 1;
            p->mutex.wait_on = -1;
            return TRUE;
        }

        else
        {
            p->semaphore.queue_index = enqueue(&sem[key].waitq, p);
            p->semaphore.wait_on = key;
        }
    }

    //change this to return FALSE if we hit the else block above
	return FALSE;
}

/*** UP operation on a sempahore ***/
void semaphore_up(sem_t key, PCB *p) {
    if(sem[key].available == FALSE)
    {
        sem[key].value += 1;
        PCB* temp = dequeue(&sem[key].waitq);
        if(temp == NULL)
        {
            temp->state = READY;
            sem[key].value -= 1;
            temp->semaphore.wait_on = -1;
        }
    }
}

/*** Cleanup semaphorees for a process ***/
void free_semaphores(PCB *p) {
	int i;

	for (i=1; i<SEM_MAXNUMBER; i++) {
		// see if process is creator of the semaphore
		if (p->pid == sem[i].creator) semaphore_destroy((sem_t)i,p);
	}

	// remove from wait queue, if any
	if (p->semaphore.wait_on != -1) 
		remove_queue_item(&sem[p->semaphore.wait_on].waitq, p->semaphore.queue_index);
	
}



