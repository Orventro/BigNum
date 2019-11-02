#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bn.h"

int main() {
    char *buff1 = malloc(100000), *buff2;
    bn *a = bn_new();
    scanf("%s", buff1);
    bn_init_string(a, buff1);
    bn_root_to(a, 2);
    buff2 = (char*)bn_to_string(a, 10);
    printf("%s\n", buff2);
    bn_delete(a);
    free(buff1);
    free(buff2);
}
