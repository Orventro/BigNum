#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    bn *a = bn_new(), *b = bn_new();
    bn_init_string(a, "33232930569601");
    bn_init_string(b, "678223072849");
    printf("%d %d\n", (*a).size, (*b).size);
    bn_mul_to(b, a);
    printf("%s\n", bn_to_string(b, 10));
    //free(s);
}
