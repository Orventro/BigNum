#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bn.h"

int main() {
    char *buff1 = malloc(100000), *buff2;
    bn *a = bn_new(), *b = bn_new();
    scanf("%s", buff1);
    bn_init_string(a, buff1);
    scanf("%s", buff1);
    bn_init_string(b, buff1);
    bn_div_to(a, b);
    buff2 = (char*)bn_to_string(a, 8);
    printf("%s\n", buff2);
    bn_delete(a);
    bn_delete(b);
    free(buff1);
    free(buff2);
}
