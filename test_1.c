#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    // int n;
    // scanf("%d", n);
    srand(time(0));
    char s[100];
    for(int i = 0; i < 99; i++) 
        s[i] = (char)(rand()%10)+'0';
    s[99] = 0;
    printf("%s\n", s);
    bn *b = bn_new();
    bn_init_string(b, s);
    // uint32 t;
    // scanf("%ud", &t);
    // bn_and_uint *p = bn_full_division_abs_int(b, t);
    for(int i = 0; i < 1000; i++)
        printf("%s\n", bn_to_string(b, 10));
}
