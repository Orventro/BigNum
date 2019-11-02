#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bn_s;
typedef struct bn_s bn;
bn *bn_new(); 
bn *bn_init(bn const *orig);
int bn_init_string(bn *t, const char *init_string);
int bn_init_string_radix(bn *t, const char *init_string, int radix);
int bn_init_int(bn *t, int init_int);
int bn_delete(bn *t);
int bn_add_to(bn *t, bn const *right);
int bn_sub_to(bn *t, bn const *right);
int bn_mul_to(bn *t, bn const *right);
int bn_div_to(bn *t, bn const *right);
int bn_mod_to(bn *t, bn const *right);
int bn_pow_to(bn *t, int degree);
int bn_root_to(bn *t, int reciprocal);
bn* bn_add(bn const *left, bn const *right);
bn* bn_sub(bn const *left, bn const *right);
bn* bn_mul(bn const *left, bn const *right);
bn* bn_div(bn const *left, bn const *right);
bn* bn_mod(bn const *left, bn const *right);
const char *bn_to_string(bn const *t, int radix);
int bn_cmp(bn const *left, bn const *right);
int bn_neg(bn *t); 
int bn_abs(bn *t); 
int bn_sign(bn const *t); 

// bn *bn_full_division_abs(bn const *left, bn const *right);