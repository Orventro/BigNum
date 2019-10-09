#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    bn *a = bn_new();
    bn_init_string(a, "7");
    int p;
    scanf("%d", &p);
    bn_pow_to(a, p);
    const char *s = bn_to_string(a, 10);
    printf("%s\n", s);
    //free(s);
}
