#include "stdio.h"

int sum(int a, int b, int c)
{
    return a+b+c;
}

typedef enum{
    a = 1,
    b,
    c,
    d,
    e,
} numbercheck_t;
typedef struct
{
    int a;
}hung_t;

int main()
{
    numbercheck_t number;
    number = sum(1,0,0);
    if(1 == number)
        printf("RenesasVietNam\n");
    // int a = sum(2,3,4);
    // printf("Tong: %d\n", a);
    printf("Number: %d\n", number);
    
}