#include <stdio.h>
#include <stdlib.h>
//new line

//Submission Code:--------------------------------------------------------------

//split the array (inp) into words, returning the number of words
//a sequence of non-blank characters ending in one or more blank spaces
//is a word
//argc should be set to the number of words
//argv should contain the words
int parse_command(char* inp, int* argc, char* argv[])
{
    //we will incriment argc as we read
    //...so init to zero
    *argc = 0;

    //to start, we recall that argv is pre-allocated
    //start by looping through inp
    for(char* ch = inp; *ch; ch++)
    {
        //if we are looking at a real character...
        if(*ch != ' ')
        {
            //read the word
            for(int i = 0; *ch != ' ' && *ch; i++)
            {
                argv[*argc][i] = *ch;
                ch++;
            }

            //incriment argc
            *argc = *argc + 1;
        }
    }
}


//convert value into a number given by specified base
//store conversion in str
//if base is 10, and value is negative, include a negative sign in str
//all other bases should be treated as unsigned
//return a pointer to the value of str
char* itoa(int value, char *str, int base)
{
}

//write a function called printany that can print any primitive type
//may use up to two args, no global variables
void printany()
{
}

//write a function that takes 3 ints (base, flag, limit) and a DESC pointer to g
//populate memory pointed to by g as per stuct in assignment
//void populate_desc(int base, int limit, int flag, DESC *g)
//{
//}

//END Submission Code:-----------------------------------------------------------
//
//

int main()
{

    char* test = "string 3 words";

    int* num = malloc(sizeof(int));
    
    char** res = malloc(3 * (sizeof(char*)));
    res[0] = (char*) malloc(20 * sizeof(char));
    res[1] = (char*) malloc(20 * sizeof(char));
    res[2] = (char*) malloc(20 * sizeof(char));
    
    parse_command(test, num, res);

    printf("%s\n", res[0]);
    printf("%s\n", res[1]);
    printf("%s\n", res[2]);


    return 0;
}

