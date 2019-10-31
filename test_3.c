#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    int n = 0;
    scanf("%d\n", &n);
    char *buff1 = malloc(100000), *buff2 = malloc(100000), *buff3, *buff4;
    for(int i = 0; i < n; i++) {
        scanf("%s %s\n", buff1, buff2);
        bn *a = bn_new(), *b = bn_new();
        bn_init_string(a, buff1);
        bn_init_string(b, buff2);

        bn *m = bn_mul(a, b);
        buff3 = (char*)bn_to_string(m, 10);
        printf("%s\n", buff3);
        
        free(buff3);
        delete_bn(a);
        delete_bn(b);
        delete_bn(m);
    }
    free(buff1);
    free(buff2);
}
