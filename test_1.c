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
    int rep;
    scanf("%d", &rep);
    bn_root_to_bin(a, rep);
    // buff = (char*) bn_to_string(a, 10);
    // printf("%s\n", buff);
    // free(buff);
    delete_bn(a);
}
