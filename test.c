#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bn.c"

int main() {
    int n;
    scanf("%d", &n);
    bn *s = bn_new(), *c = bn_new();
    char *buff = malloc(100000), op, sp;
    scanf("%s", buff);
    bn_init_string(s, buff);
    for(int i = 0; i < n; i++) {
        sp = getchar();
        op = getchar();
        sp = getchar();
        scanf("%s", buff);
        bn_init_string(c, buff);
        if     (op == '+') bn_add_to(s, c);
        else if(op == '-') bn_sub_to(s, c);
        else if(op == '*') bn_mul_to(s, c);
        else if(op == '%') bn_mod_to(s, c);
        else if(op == '/') bn_div_to(s, c);
        char *temp = (char*)bn_to_string(s, 10);
        printf("%s\n", temp);
        free(temp);
    }
    delete_bn(s);
    delete_bn(c);
    free(buff);
}
