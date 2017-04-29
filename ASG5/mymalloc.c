//
//  mymalloc.c
//  Name: Michael Roitman
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<unistd.h>

#define MAX_ORDER 12
#define PAGE_SIZE 0x1000

typedef unsigned long long uint64_t;
typedef unsigned uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

// function prototypes
void print_ra(void *);
void print_ran(void *);
void print(uint32_t *);
void printAll();
void *grow_heap();
void *my_malloc(uint32_t);
void my_free(void *);


uint32_t *free_list_start[MAX_ORDER+1]; // the free list pointers for different orders
void *start_brk;			// the heap start address (set in main())

//////////////////////////////
/* Print address relative to heap start (start_brk)*/
void print_ra(void *a) {
	a = (void *)((uint32_t)a-(uint32_t)start_brk);
	printf("0x%x",(uint32_t)a);
}

//////////////////////////////
/* Print address relative to heap start (start_brk) with newline*/
void print_ran(void *a) {
	print_ra(a);
	printf("\n");
} 

//////////////////////////////
/* Print a free list*/
void print(uint32_t *pointer) {
    if (pointer == NULL) 
        printf("Empty list.\n");
    else {
        print_ra(pointer);
        for (pointer=(uint32_t *)(*pointer); pointer!=NULL; pointer=(uint32_t *)(*pointer)) {
            printf("\t");
            print_ra(pointer);			
        }
        printf("\n");
    }
}    

//////////////////////////////
/* Print all free lists*/
void printAll() {
    int i;
    
    for (i=0; i<MAX_ORDER+1; i++) {
        printf("%d:\t",i);
        print(free_list_start[i]);
    }
}

//////////////////////////////
/* Increment heap top (brk) by one page 
   Output: NULL if failure; otherwise the old heap top 
   Meaning: if the function returns a non-NULL value, then 
         all logical addresses from <return value> to 
         (<return value> + PAGE_SIZE - 1) are now usable
*/
void *grow_heap() {
    return sbrk(PAGE_SIZE); // tell Linux to increment heap top
}

///////////////////////////////////////////////////////////
/* Allocate memory using Buddy algorithm 
   Input: amount of requested memory 
   Output: the beginning address of requested memory, or
         NULL if not able to allocate (heap cannot grow anymore)
*/

void *my_malloc(uint32_t size) {
    if (size == 0) return NULL;
    
    int n = (int)ceil(log2(size+4)); // order; 4 more bytes to store order

    if (n > MAX_ORDER) return NULL;

    // TODO: implement as per requirements

    //check for free block
    if(free_list_start[n] != NULL)
    {
        uint32_t* temp = (uint32_t*) free_list_start[n];

        free_list_start[n] = (uint32_t*) *temp;

        //set free_list_start[n] based on order of block loc
        *temp = n;

        return temp + 1;
        
    }

    else
    {
        while(free_list_start[n] == NULL)
        {
            n++;
            if (n > MAX_ORDER) //bounds check
            {
                //grow the heap
                uint32_t* memory = (uint32_t*) grow_heap();

                //add allocated memory to free list at MAX_ORDER
                free_list_start[MAX_ORDER] = memory;

                //set the next link of memory to NULL
                *memory = NULL;

                //recursive call
                return my_malloc(size);
            }
        }

        //start address for block
        uint32_t* block = free_list_start[n];

        //remove block from linked list of order n
        free_list_start[n] = (uint32_t*) *block;

        //first element of linked list at n-1 set to block
        free_list_start[n - 1] = block;

        //first half of link at n gets second half of itself
        *block = (uint32_t) (block + ((uint32_t) pow(2, n - 1) / 4));

        //set block pointer to start address of 2nd half of link at n
        block = (uint32_t*) *block;

        //next link of 2nd half gets NULL
        *block = NULL;

        //recursive call
        return my_malloc(size);
    }


}


///////////////////////////////////////////////////////////
/* Deallocate memory using Buddy algorithm 
   Input: pointer received from my_malloc 
*/

void my_free(void *ptr) {
    //first 4 blocks of input are order
    //account for that shift:
    uint32_t* del_mem = ((uint32_t*) ptr) -1;

    //order is next of del_mem
    uint32_t order = *del_mem;

    //add new block based on order
    if(free_list_start[order] == NULL)
    {
        //set first element of list
        free_list_start[order] = del_mem; 

        //set next link to NULL
        *del_mem = NULL;

        return;
    }

    //if start of the new memory is less than first element
    if(((uint32_t) del_mem) < ((uint32_t) free_list_start[order]))
    {
        //insert at begining
        uint32_t* temp = free_list_start[order];
        free_list_start[order] = del_mem;

        *del_mem = temp;
    }

    // grab the element before the current one
    uint32_t* temp = free_list_start[order]; 

    // if the start of given memory block is less than the first element 
    if (((uint32_t) del_mem) < ((uint32_t) temp)) 
    { 

        // insert at the head 
        free_list_start[order] = del_mem; 

        // set next of mem to be freed link to the previous head 
        *del_mem = (uint32_t) temp; 
    }

    else 
    { 
        // grab current memory
        uint32_t* current = (uint32_t*) *temp; 
        while (current != NULL) 
        { 
            // check if this is the place to insert
            if (((uint32_t) del_mem) < ((uint32_t) current)) 
            {
                break; 
            }
            else 
            { 
                temp = current; 
                current = (uint32_t*) *current;
            }
        }
        // set del_mem's next link to current (larger address)
        *del_mem = (uint32_t) current; 

        *temp = (uint32_t) del_mem; 
    }
    
    //try to merge
    if (order < MAX_ORDER) 
    {
        if (del_mem != free_list_start[order] && temp == (del_mem - ((uint32_t) pow(2, order) / 4))) 
        { 
            // if the smaller of the addresses is a multiple of 2n+1
            if (((uint32_t) temp) % ((uint32_t) pow(2, order + 1)) == 0) 
            { 
                if (free_list_start[order] == temp) 
                { 
                    free_list_start[order] = *del_mem; // set the first element to next of mem to be removed
                }

                else 
                { 
                    // current will be element behind previou element
                    uint32_t* current = free_list_start[order]; 
                    while (*current != ((uint32_t) temp))
                    {
                         current = (uint32_t*) *current; 
                    }

                    // element behind previous element gets element after mem that was freed
                    *current = *del_mem; 
                }

                // set order (first 4 bytes)
                *temp = order + 1; 

                // offset
                my_free((void*) (temp + 1)); 
                return;
            }
        }

        else if(*del_mem != NULL && *del_mem == (((uint32_t) del_mem) + ((uint32_t) pow(2, order)))) { 

            if (((uint32_t) del_mem) % ((uint32_t) pow(2, order + 1)) == 0) { 
                uint32_t* second = (uint32_t*) *del_mem; 

                if (free_list_start[order] == del_mem) { 
                    free_list_start[order] = (uint32_t*) *second; 
                }
                else { 
                    // set link before mem to free to second
                    *temp = *second; 
                }
                // set order
                *del_mem = order + 1; 

                // offset
                my_free((void*) (del_mem + 1)); 
            }
        }
    }


    
}


///////////////////////////////////////////////////////////
/* Change this function only for testing. No functionality 
   should reside in this function; the GTA will use his own
   main function. 
*/

int main(int argc, char *argv[])
{   
    int i;
	
    start_brk = sbrk(0); // get heap start address

    // initialize all free lists to NULL
    for (i=0; i<MAX_ORDER+1; i++) free_list_start[i]=NULL;

    void *ptr1 = my_malloc(8); print_ran(ptr1); 
    void *ptr2 = my_malloc(8); print_ran(ptr2); 
    void *ptr3 = my_malloc(8); print_ran(ptr3); 
    void *ptr4 = my_malloc(8); print_ran(ptr4); 
    void *ptr5 = my_malloc(8); print_ran(ptr5); 
    void *ptr6 = my_malloc(8); print_ran(ptr6); 
    void *ptr7 = my_malloc(8); print_ran(ptr7); 
    void *ptr8 = my_malloc(8); print_ran(ptr8); 	

    my_free(ptr3); 
    my_free(ptr5); 
    my_free(ptr2); 
    my_free(ptr1); 
    my_free(ptr4); 

    void *ptr9 = my_malloc(48); print_ran(ptr9); 
    void *ptr10 = my_malloc(8); print_ran(ptr10); 
	
    return 0;
}
