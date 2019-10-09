#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bn.c"

int main() {
    int n;
    scanf("%d", &n);
    bn *s = bn_new(), *c = bn_new();
    char buff[64], op, sp;
    scanf("%s", buff);
    bn_init_string(s, buff);
    for(int i = 0; i < n; i++) {
        sp = getchar();
        op = getchar();
        sp = getchar();
        scanf("%s", buff);
        // printf("%d ", i);
        printf("%s", bn_to_string(s, 10));
        //printf(" %d", (*s).sign);
        bn_init_string(c, buff);
        printf(" %c %s", op, buff);
        // printf("ok\n");
        // bn_print(s);
        // bn_print_expr(s);
        //printf("print('%c', end = ' ')\n", op);
        // bn_print_expr(c);
        // printf("print('=', end=' ')\n");
        if     (op == '+') bn_add_to(s, c);
        else if(op == '-') bn_sub_to(s, c);
        else if(op == '*') bn_mul_to(s, c);
        else if(op == '%') bn_mod_to(s, c);
        else if(op == '/') bn_div_to(s, c);
        // printf("%d %lld = ", (*s).size, (*s).data[0]);
        // bn_print(s);
        printf(" = %s\n", bn_to_string(s, 10));
        // printf("print('%c')\n", op);
        // bn_print(s);
        // printf("print()\n");
    }
    // bn_print_expr(s);
    // bn_print(s);
    // printf("\n");
    delete_bn(s);
    delete_bn(c);
}
