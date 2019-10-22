#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    char *buff = malloc(100000);
    scanf("%s", buff);
    bn *a = bn_new();
    bn_init_string(a, buff);
    free(buff);
    buff = (char*) bn_to_string_fast(a, 10);
    printf("%s\n", buff);
    free(buff);
    delete_bn(a);
}
