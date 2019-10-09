#include "bn.c"
#include <stdio.h>

typedef struct mat22{
    bn *a, *b, *c, *d;
} mat22;

mat22* new_mat(int a, int b, int c, int d) {
    mat22 *m = malloc(sizeof(mat22));
    (*m).a = bn_new();
    (*m).b = bn_new();
    (*m).c = bn_new();
    (*m).d = bn_new();
    bn_init_int((*m).a, a);
    bn_init_int((*m).b, b);
    bn_init_int((*m).c, c);
    bn_init_int((*m).d, d);
    return m;
}

mat22* mat_mul(mat22 *x, mat22 *y) {
    mat22 *r = new_mat(0, 0, 0, 0);
    bn *xaya = bn_karat_mul((*x).a, (*y).a), *xbyc = bn_karat_mul((*x).b, (*y).c),
       *xayb = bn_karat_mul((*x).a, (*y).b), *xbyd = bn_karat_mul((*x).b, (*y).d),
       *xcya = bn_karat_mul((*x).c, (*y).a), *xdyc = bn_karat_mul((*x).d, (*y).c),
       *xcyb = bn_karat_mul((*x).c, (*y).b), *xdyd = bn_karat_mul((*x).d, (*y).d);
    bn_move_into((*r).a, bn_add(xaya, xbyc));
    bn_move_into((*r).b, bn_add(xayb, xbyd));
    bn_move_into((*r).c, bn_add(xcya, xdyc));
    bn_move_into((*r).d, bn_add(xcyb, xdyd));
    delete_bn(xaya); delete_bn(xayb); delete_bn(xcya); delete_bn(xcyb); 
    delete_bn(xbyc); delete_bn(xbyd); delete_bn(xdyc); delete_bn(xdyd); 
    return r;
}

void mat_sq(mat22 *m) {
    bn *aa = bn_karat_mul((*m).a, (*m).a), *dd = bn_karat_mul((*m).d, (*m).d), 
       *bc = bn_karat_mul((*m).b, (*m).c), *ad = bn_add((*m).a, (*m).d);
    // printf("%s %s\n%s %s\n", bn_to_string(aa, 10), bn_to_string(bc, 10), bn_to_string(ad, 10), bn_to_string(dd, 10));
    bn_move_into((*m).a, bn_add(aa, bc));
    bn_move_into((*m).b, bn_karat_mul(ad, (*m).b));
    bn_move_into((*m).c, bn_karat_mul(ad, (*m).c));
    bn_move_into((*m).d, bn_add(dd, bc));
    delete_bn(aa);
    delete_bn(dd);
    delete_bn(bc);
    delete_bn(ad);
}

void delete_mat(mat22 *m) {
    delete_bn((*m).a);
    delete_bn((*m).b);
    delete_bn((*m).c);
    delete_bn((*m).d);
    free(m);
}

// void mat_print(mat22 *m){
//     printf("%s %s\n%s %s\n", bn_to_string((*m).a, 10), bn_to_string((*m).b, 10), bn_to_string((*m).c, 10), bn_to_string((*m).d, 10));
// }

int main() {
    int n;
    scanf("%d", &n);
    mat22 *fin = new_mat(1, 0, 0, 1), *cur = new_mat(0, 1, 1, 1);
    if(n%2) {
        mat22 *t = mat_mul(fin, cur);
        delete_mat(fin);
        fin = t;
    }
    n /= 2;
    while(n) {
        mat_sq(cur);
        // printf("cur:\n");
        // mat_print(cur);
        // printf("\n");
        if(n%2) {
            mat22 *t = mat_mul(fin, cur);
            delete_mat(fin);
            fin = t;
        }
        n /= 2;
    }
    // mat_print(fin);
    char *c = bn_to_string((*fin).b, 10);
    printf("%s\n", c);
    free(c);
    delete_mat(fin);
    delete_mat(cur);
}