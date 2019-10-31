#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    char *buff1 = malloc(100000), *buff2 = malloc(100000);

    scanf("%s %s", buff1, buff2);
    bn *a = bn_new(), *b = bn_new();
    bn_init_string(a, buff1);
    bn_init_string(b, buff2);

    free(buff1);
    free(buff2);

    bn_pair *bnp = bn_full_division_fast(a, b);
    buff1 = (char*)bn_to_string(bnp->quot, 10);
    buff2 = (char*)bn_to_string(bnp->rem,  10);
    printf("%s\n%s\n", buff1, buff2);
    
    free(buff1);
    free(buff2);
    delete_bn(a);
    delete_bn(b);
    delete_bn(bnp->quot);
    delete_bn(bnp->rem);
    free(bnp);
}
