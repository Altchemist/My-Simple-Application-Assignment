#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int *ptr;

    int x;
    x = 5;

    ptr = &x;

    printf("The value of the pointer is : %d\n", *ptr);

    return 0;

    
}
