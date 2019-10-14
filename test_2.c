#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bn.c"

int main() {
    bn *a = bn_new(), *b = bn_new();
    char *buff = malloc(100000);
    scanf("%s", buff);
    bn_init_string(a, buff);
    scanf("%s", buff);
    bn_init_string(b, buff);
    free(buff);
    bn_pair *bau = bn_full_division(b, a);
    buff = (char*)bn_to_string((*bau).quot, 10);
    printf("%s\n", buff);
    free(buff);
    buff = (char*)bn_to_string((*bau).rem, 10);
    printf("%s\n", buff);
    free(buff);
    delete_bn(a);
    delete_bn(b);
    //free(s);
}
