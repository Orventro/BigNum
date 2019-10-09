#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    bn *a = bn_new();
    bn_init_int(a, 1);
    bn_init_string(a, "6557470319842");

    bn_mul_to(a, a);
    char *c = bn_to_string(a, 10);
    printf("%s\n", c);
    free(c);
    delete_bn(a);
    //free(s);
}
