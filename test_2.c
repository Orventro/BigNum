#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    int n;
    scanf("%d", &n);
    char *buff1 = malloc(100000), *buff2 = malloc(100000), *buff3, *buff4;
    for(int i = 0; i < n; i++) {
        scanf("%s %s", buff1, buff2);
        bn *a = bn_new(), *b = bn_new();
        bn_init_string(a, buff1);
        bn_init_string(b, buff2);

        bn_pair *bnp = bn_full_division(a, b);
        buff3 = (char*)bn_to_string(bnp->quot, 10);
        buff4 = (char*)bn_to_string(bnp->rem,  10);
        printf("%s %s\n", buff3, buff4);
        
        free(buff3);
        free(buff4);
        delete_bn(a);
        delete_bn(b);
        delete_bn(bnp->quot);
        delete_bn(bnp->rem);
        free(bnp);
    }
    free(buff1);
    free(buff2);
}
