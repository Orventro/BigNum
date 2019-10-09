#include <stdio.h>
#include <stdlib.h>
#include "bn.c"

int main()
{
    bn *a = bn_new(), *b, *c = bn_new(), *d = bn_new();
    bn_init_int(a, 5);
    bn_init_int(c, 7);
    bn_init_int(d, 3);
    bn_shift(a, 32);
    bn_print(a);
    printf("\n");
    bn_sub_to(a, c);
    bn_report(a);
    bn_report(d);
    for(int i = 0; i < 7; i++) {
        // printf("%d ", i);
        // bn_print(a);
        // printf(" * ");
        // bn_print(a);
        b = bn_mul(a, a);
        delete_bn(a);
        a = init(b);
        delete_bn(b);
        // printf(" = ");
        // printf("\n");
        printf("%d ", 2<<i);
        bn_print(a);
        printf("\n");
    }
    bn_print(a);
    printf("\n");
    delete_bn(a);
    delete_bn(c);
    delete_bn(d);
}
