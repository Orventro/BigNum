#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    bn *a = bn_new(), *b = bn_new();
    bn_init_int(a, 1);
    bn_init_string(a, "4807526976");
    bn_init_string(b, "251728825683549488150424261");
    printf("%d %d\n", (*a).size, (*b).size);
    bn_move_into(a, bn_karat_mul(a, b));
    char *c = (char*)bn_to_string(a, 10);
    printf("%s\n", c);
    free(c);
    delete_bn(a);
    delete_bn(b);
    //free(s);
}
