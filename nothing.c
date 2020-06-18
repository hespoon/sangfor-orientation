#include <stdio.h>
#include <string.h>
int main()
{
    int a = 10;
    switch (a)
    {
    case 1:;
        int b = 10;
        printf("1b: %d\n", b);
        break;
    case 10:;
        int b = 20;
        printf("2b: %d\n", b);
        break;
    }
    return 0;
}