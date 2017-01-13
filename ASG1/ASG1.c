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
            //we need to keep the iterator in this scope so we can
            //add the null character
            int i = 0;
            for(i; *ch != ' ' && *ch; i++)
            {
                argv[*argc][i] = *ch;
                ch++;
            }

            //append the null character
            argv[*argc][i] = '\0';


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
    
    //handle negative input
    int setNeg = 0;
    if(value < 0)
    {
        value *= -1;
        if(base == 10)
        {
            setNeg = 1;
        }
    }
    if(setNeg == 1)
    {
        str[0] = '-';
    }

    //our OS can't handle anything bigger than 0x7FFFFFFF
    printf("%d\n", value);
    value = value & 0x7FFFFFFF;

    //handle value=0 as a special case
    if(value == 0)
    {
       *str = '0';
       return str;
    }

    

    
    //find out how many bits we need
    int numBits = 1;
    int tempCounter = 1;

    while((tempCounter < 0x7FFFFFFF / base) && tempCounter*base < value)
    {
        tempCounter *= base;
        numBits++;
    }

    //temp char to hold result
    char temp[numBits];

    //we need a mutable copy of value
    int v = value;

    //declare an indexer in this scope 
    //so that we can append a null char after the loop
    int i = setNeg;
    while(v > 0)
    {
        char charToSet;
        if((v % base) < 10)
        {
            charToSet = (char) (v % base) + 48;
        }
        else
        {
            charToSet = (char) (v % base) + 55;
        }

        //set bit
        temp[i] = charToSet;

        i++;
        v = v / base;
    }
    //append the null character
    temp[i] = '\0';

    //write to str in reverse order
    i = 0;
    for(int j = numBits - 1; j >= 0; j--)
    {
        str[i] = temp[j];
        i++;
    }
    //append the null character
    str[i] = '\0';
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


//Testing:-----------------------------------------------------------------------
int test_parse_command()
{
    char* test = "    this is   a test  123 23 bye ";

    int* num = malloc(sizeof(int));
    
    char** res = malloc(7 * (sizeof(char*)));
    res[0] = (char*) malloc(20 * sizeof(char));
    res[1] = (char*) malloc(20 * sizeof(char));
    res[2] = (char*) malloc(20 * sizeof(char));
    res[3] = (char*) malloc(20 * sizeof(char));
    res[4] = (char*) malloc(20 * sizeof(char));
    res[5] = (char*) malloc(20 * sizeof(char));
    res[6] = (char*) malloc(20 * sizeof(char));
    
    parse_command(test, num, res);

    printf("%s\n", res[0]);
    printf("%s\n", res[1]);
    printf("%s\n", res[2]);
    printf("%s\n", res[3]);
    printf("%s\n", res[4]);
    printf("%s\n", res[5]);
    printf("%s\n", res[6]);

    printf("%d\n", *num);

    return 1;
}

int test_itoa()
{
    char* temp = (char*) malloc(64 * sizeof(char));

    itoa(1, temp, 10);
    //itoa(0x7FFFFFFF, temp, 10);
    //itoa(0x7FFFFFFF, temp, 8);
    //itoa(0x7FFFFFFF, temp, 2);

    printf("%s\n", temp);

    return 1;
}

//END Testing:-------------------------------------------------------------------
int main()
{
    test_itoa();

    return 0;
}



