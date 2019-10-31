#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned long long int uint64;
typedef long long int int64;
typedef unsigned int uint32;

const int64 quot_32_bits = (1ll<<32)-1;

typedef struct bn_s{
    int64 *data;
    int size, actual_size, sign;
} bn;

typedef struct bn_pair{
    bn *quot, *rem;
} bn_pair;

typedef struct bn_and_uint{
    bn *quot;
    uint64 rem;
} bn_and_uint;

int *bn_init_int(bn *t, int val) {
    (*t).size = 1;
    (*t).data[0] = abs(val);
    (*t).sign = val < 0;
    return 0;
}

bn *bn_new() {
    bn *x = malloc(sizeof(bn));
    (*x).size = (*x).actual_size = 1;
    (*x).data = calloc(1, 8);
    (*x).sign = 0;
    return x;
}

int delete_bn(bn *x) {
    free((*x).data);
    free(x);
    return 0;
}

bn *init(bn const *orig) {
    bn *x = calloc(1, sizeof(bn));
    (*x).size = (*orig).size;
    (*x).sign = (*orig).sign;
    (*x).actual_size = 1;
    while((*x).actual_size < (*x).size) (*x).actual_size *= 2;
    (*x).data = calloc((*x).actual_size, 8);
    for(int i = 0; i < (*x).size; i++)
        (*x).data[i] = (*orig).data[i];
    return x;
}

int bn_move_into(bn *dest, bn *orig) {
    free((*dest).data);
    *dest = *orig;
    free(orig);
}

int bn_copy_into(bn *dest, bn *orig) {
    bn_move_into(dest, init(orig));
}

bn_pair *bn_pair_new() {
    bn_pair *p = malloc(sizeof(bn_pair));
    (*p).quot = bn_new();
    (*p).rem = bn_new();
    return p;
}

bn_and_uint *bn_and_uint_new() {
    bn_and_uint *b = malloc(sizeof(bn_and_uint));
    (*b).quot = bn_new();
    (*b).rem = 0;
    return b;
}

int is_zero(bn const *x) {
    return (*x).size == 1 && (*x).data[0] == 0;
}

// returns true for 0
// ignors sign
int is_pow2(bn const *x) {
    for(int i = 0; i < x->size-1; i++) 
        if(x->data[i])
            return 0;
    int64 a = x->data[x->size-1];
    while(a > 1) {
        if(a % 2) 
            return 0;
        a /= 2;
    }
    return 1;
}

void bn_shrink(bn *x) {
    int i = (*x).size-1;
    while((*x).data[i] == 0)
        i--;
    (*x).size = i+1;
}

void bn_print_data(bn const *x) {
    for(int i = (*x).size-1; i >= 0; i--) {
        printf("%llu", (*x).data[i]);
        if(i)
            printf(" * 2^%d + ", i*32);
    }
    printf("\n");
}

void bn_print(bn const *x) {
    if((*x).size == 1) {
        long long int t = (*x).data[0];
        if((*x).sign) t *= -1;
        printf("%lld", t);
        return;
    }
    char *buff = calloc(10000, 1), *expr = malloc(50000), *exprc = expr;
    exprc += sprintf(exprc, "echo '");
    for(int i = (*x).size-1; i >= 0; i--) {
        exprc += sprintf(exprc, "%llu", (*x).data[i]);
        if(i)
            exprc += sprintf(exprc, " * 2^%d + ", i*32);
    }
    exprc += sprintf(exprc, "' | BC_LINE_LENGTH=0 bc");
    FILE *f = popen(expr, "r");
    fgets(buff, 200, f);
    if((*x).sign) printf("-");
    printf("%s", buff);
    pclose(f);
    free(buff);
    free(expr);
}

void bn_report(bn const *x) {
    printf("\n");
    printf("Size: %d\n", (*x).size);
    printf("Hex:\n");
    for(int i = 0; i < (*x).size; i++) 
        printf("%d\t%llx\n", i, (*x).data[i]);
    printf("Decimal: ");
    bn_print(x);
    printf("\n\n");
}

int *bn_neg(bn *t) {
    (*t).sign ^= 1;
}

void bn_resize(bn *t, int newsize) {
    if(newsize > (*t).actual_size) {
        while(newsize > (*t).actual_size) 
            (*t).actual_size *= 2;
        (*t).data = realloc((*t).data, (*t).actual_size*8);
        for(int i = (*t).size; i < newsize; i++)
            (*t).data[i] = 0;
    } else if (newsize > (*t).size) {
        for(int i = (*t).size; i < newsize; i++)
            (*t).data[i] = 0;
    }
    (*t).size = newsize;
}

int bn_shift(bn *a, int shift) {
    if(is_zero(a)) return 0;
    if(shift > 0) {
        const int oldsize = (*a).size, sd32 = shift/32, sm32 = shift%32; 
        if(sm32) {
            bn_resize(a, (*a).size + sd32 + 1);
            int64 m1 = ((1ull<<32)-1) ^ ((1ull<<sm32)-1);
            for(int i = (*a).size-1; i >= sd32+1; i--) 
                (*a).data[i] = ((*a).data[i-sd32]<<sm32 & m1) | 
                                (*a).data[i-sd32-1]>>(32-sm32);
            (*a).data[sd32] = ((*a).data[0]<<sm32) & m1;
            for(int i = sd32-1; i >= 0; i--) 
                (*a).data[i] = 0;
            if((*a).data[(*a).size-1] == 0)
                bn_resize(a, (*a).size - 1);
        } else {
            bn_resize(a, (*a).size + sd32);
            for(int i = (*a).size-1; i >= sd32; i--)
                (*a).data[i] = (*a).data[i-sd32];
            for(int i = sd32-1; i >= 0; i--)
                (*a).data[i] = 0;
        }
    } else if(shift < 0) {
        shift = -shift;
        const int sd32 = shift/32, sm32 = shift%32, newsize = (*a).size-sd32; 
        if(newsize == 0) {
            bn_resize(a, 1);
            (*a).data[0] = 0;
        } else if(sm32) {
            int64 m1 = (1<<sm32)-1;
            for(int i = 0; i < newsize-1; i++) {
                (*a).data[i] = ((*a).data[i+sd32]>>sm32 | 
                                (*a).data[i+sd32+1]<<(32-sm32) ) & quot_32_bits;
            }
            (*a).data[newsize-1] = (*a).data[(*a).size-1]>>sm32;
            if((*a).data[newsize-1])
                bn_resize(a, newsize);
            else if(newsize > 1)
                bn_resize(a, newsize-1);
            else
                bn_resize(a, 1);
        } else {
            for(int i = 0; i < newsize; i++)
                (*a).data[i] = (*a).data[i+sd32];
            bn_resize(a, (*a).size - sd32);
        }
    }
}

// positive if a > b
// ignore sign
int64 bn_cmp_abs(bn const *a, bn const *b) {
    if((*a).size == (*b).size) {
        int i = (*a).size-1;
        while(i > 0 && (*a).data[i] == (*b).data[i]) i--;
        return (*a).data[i] - (*b).data[i];
    } else return (*a).size - (*b).size; 
}

// positive if a > b
int64 bn_cmp(bn const *a, bn const *b) {
    if((*a).sign == (*b).sign) {
        int s = (*a).sign ? -1 : 1;
        return bn_cmp_abs(a, b) * s;
    } else{
        if((*a).size == 1 && (*b).size == 1 && (*a).data[0] == 0 && (*b).data[0] == 0)
            return 0;
        if((*a).sign) return -1;
        else          return  1;
    }
}

int bn_add_abs(bn *t, bn const *right) {
    if((*t).size < (*right).size) bn_resize(t, (*right).size);
    int64 nv = 0;
    for(int i = 0; i < (*right).size; i++) {
        nv += (*t).data[i] + (*right).data[i];
        (*t).data[i] = nv & quot_32_bits;
        nv >>= 32;
    }
    if((*t).size > (*right).size) {
        int i = (*right).size;
        while(nv && i < (*t).size){
            nv += (*t).data[i];
            (*t).data[i++] = nv & quot_32_bits;
            nv >>= 32;
        }
    }
    if(nv) {
        bn_resize(t, (*t).size+1);
        (*t).data[(*t).size-1] = nv;
    }
    return 0;
}

int bn_sub_abs(bn *t, bn const *right) {
    for(int i = 0; i < (*right).size; i++) {
        (*t).data[i] -= (*right).data[i];
        if((*t).data[i] < 0) {
            (*t).data[i] += 1ll<<32;
            (*t).data[i+1]--;
        }
    }
    if((*t).size > (*right).size) {
        int i = (*right).size;
        while((*t).data[i] < 0) {
            (*t).data[i] += 1ll<<32;
            (*t).data[++i]--;
        }
    }
    int i = (*t).size-1;
    while((*t).data[i] == 0 && i > 0) i--;
    bn_resize(t, i+1);
}

int bn_add_int_abs(bn *t, uint32 right) {
    (*t).data[0] += right;
    int i = 0, s = (*t).size;
    while((*t).data[i] > quot_32_bits && i < s) {
        (*t).data[i+1] ++;
        (*t).data[i] &= quot_32_bits;
    }
    if((*t).data[s-1] > quot_32_bits) {
        bn_resize(t, s+1);
        (*t).data[s] = (*t).data[s-1] >> 32;
        (*t).data[s-1] &= quot_32_bits;
    }
}

int bn_sub_int_abs(bn *t, uint32 right) {
    (*t).data[0] -= right;
    if((*t).data[0] < 0) {
        (*t).data[1] --;
        (*t).data[0] += (1ll<<32);
    }
    if((*t).data[1] == 0)
        bn_resize(t, 1);
}

int bn_add_to(bn *t, bn const *right) {
    char s = (*t).sign ^ (*right).sign;
    if(s) {
        if(bn_cmp_abs(t, right) >= 0) {
            bn_sub_abs(t, right);
        } else {
            bn *_right = init(right);
            // bn_print
            bn_sub_abs(_right, t);
            (*_right).sign = !(*t).sign;
            bn_move_into(t, _right);
        }
    } else {
        bn_add_abs(t, right);
    }
}

bn *bn_add(bn const *left, bn const *right) {
    bn *t = init(left);
    bn_add_to(t, right);
    return t;
}

int bn_sub_to(bn *t, bn const *right) {
    (*t).sign ^= 1;
    bn_add_to(t, right);
    (*t).sign ^= 1;
}

bn *bn_sub(bn const *left, bn const *right) {
    bn *t = init(left);
    bn_sub_to(t, right);
    return t;
}

int bn_mul_to_int(bn *t, int mul) {
    int ls = (*t).size;
    (*t).sign ^= mul < 0;
    mul = abs(mul);
    if(is_zero(t) || mul == 0) {
        bn_init_int(t, 0);
        return 0;
    }
    int64 vn=0;
    for(int i = 0; i < ls; i++) {
        vn += (*t).data[i] * mul;
        (*t).data[i] = vn & quot_32_bits;
        vn >>= 32;
    }
    if(vn) {
        bn_resize(t, (*t).size+1);
        (*t).data[(*t).size-1] = vn;
    }
}

bn* bn_mul(bn const *left, bn const *right) {
    int ls = (*left).size, rs = (*right).size;
    bn *a = bn_new();
    if(is_zero(left) || is_zero(right)) return a;
    bn_resize(a, ls + rs);
    for(int i = 0; i < ls; i++) {
        for(int j = 0; j < rs; j++) {
            uint64 t = ((uint64)(*left).data[i]) * ((uint64)(*right).data[j]);
            (*a).data[i+j] += t&quot_32_bits;
            (*a).data[i+j+1] += t>>32;
        }
    }
    for(int i = 0; i < ls+rs-1; i++) {
        if((*a).data[i] > quot_32_bits) {
            (*a).data[i+1] += (*a).data[i]>>32;
            (*a).data[i] &= quot_32_bits;
        }
    }
    if((*a).data[ls + rs - 1] == 0)
        bn_resize(a, ls + rs - 1);
    (*a).sign = (*left).sign ^ (*right).sign;
    return a;
}

int bn_mul_to(bn *t, bn const *right) {
    // if((*right).size == 1) {
    //     bn_mul_to_int(t, (*right).data[0]);
    // } else if((*t).size == 1) {
    //     bn *_right = init(right);
    //     bn_mul_to_int(_right, (*t).data[0]);
    //     bn_move_into(t, _right);
    // } else {
        bn *a = bn_mul(t, right);
        bn_move_into(t, a);
    // }
}

// actual division
bn_pair *bn_full_division_abs(bn const *left, bn const *right) { // left / right
    bn_pair *ans = bn_pair_new(); // (quotioent ; remainder)
    int64 cmp = bn_cmp_abs(left, right);
    if(cmp <= 0) {
        if(cmp == 0) {
            bn_init_int((*ans).quot, 1);
            bn_init_int((*ans).rem, 0);
        } else {
            bn_init_int((*ans).quot, 0);
            (*ans).rem = init(left);
        }
        return ans;
    }
    bn *m[32];
    m[0] = init(right);
    (*m[0]).sign = 0;
    for(int i = 1; i < 32; i++) 
        m[i] = bn_add(m[i-1], m[i-1]);
    int i;
    // printf("1\n");
    for(i = (*left).size-1; i >= 0; i--) {
        bn_shift((*ans).rem, 32);
        (*(*ans).rem).data[0] = (*left).data[i];
        if(bn_cmp((*ans).rem, m[0]) >= 0) {
            bn_shift((*ans).rem, -32);
            // printf("2\n");
            break;
        }
    }
    for(; i >= 0; i--) {
        bn_shift((*ans).rem, 32);
        (*(*ans).rem).data[0] = (*left).data[i];
        int j = 31;
        while (bn_cmp((*ans).rem, m[0]) >= 0) {
            for(; j >= 0; j--)
                if(bn_cmp((*ans).rem, m[j]) >= 0)
                    break;
            (*(*ans).quot).data[0] += 1ll<<j;
            bn_sub_to((*ans).rem, m[j]);
        }
        bn_shift((*ans).quot, 32);
    }
    bn_shift((*ans).quot, -32);
    for(int i = 0; i < 32; i++) delete_bn(m[i]);
    return ans;
}

bn_and_uint *bn_full_division_int(bn const *left, int64 right) { // left / right
    bn_and_uint *ans = bn_and_uint_new(); // (quotioent ; remainder)
    ans->quot = init(left);
    int sign = (right < 0) ^ left->sign;
    ans->quot->sign = 0;
    right = abs(right);
    for(int i = (*left).size-1; i > 0; i--) {
        ans->quot->data[i-1] += (ans->quot->data[i]%right)<<32;
        ans->quot->data[i] /= right;
    }
    ans->rem = ans->quot->data[0] % right;
    ans->quot->data[0] /= right;
    ans->quot->sign = sign;
    if((ans->quot->size > 1) && (ans->quot->data[ans->quot->size-1] == 0))
        ans->quot->size--;
    return ans;
}

//dealing with signs
bn_pair *bn_full_division(bn const *left, bn const *right) {
    bn_pair *ans = bn_full_division_abs(left, right); // quotient; remainder
    if((*left).sign ^ (*right).sign) {
        (*(*ans).quot).sign = 1;
        if(!is_zero((*ans).rem)) {
            (*(*ans).rem).sign = (*left).sign;
            // rem = (abs(right) - abs(rem)) * sgn(right) * sgn(left) 
            // or just rem = -rem + right
            bn_add_to((*ans).rem, right); 
            bn_add_int_abs((*ans).quot, 1); // subtract 1
        }
    }
    return ans;
}

int bn_div_to(bn *t, bn const *right) {
    bn_pair *d = bn_full_division(t, right);
    bn_move_into(t, (*d).quot);
    delete_bn((*d).rem);
    free(d);
}

int bn_div_to_int(bn *t, int right) {
    bn_and_uint *d = bn_full_division_int(t, right);
    bn_move_into(t, (*d).quot);
    free(d);
}

int bn_mod_to(bn *t, bn const *right) {
    // printf("m1\n");
    bn_pair *d = bn_full_division(t, right);
    bn_move_into(t, (*d).rem);
    delete_bn((*d).quot);
    free(d);
    // printf("m2\n");
}

bn *bn_div(bn const *left, bn const *right) {
    //printf("d1\n");
    bn_pair *d = bn_full_division(left, right);
    bn *ans = (*d).quot;
    delete_bn((*d).rem);
    free(d);
    //printf("d2\n");
    return ans;
}

bn *bn_mod(bn const *left, bn const *right) {
    bn_pair *d = bn_full_division(left, right);
    bn *ans = (*d).rem;
    delete_bn((*d).quot);
    free(d);
    return ans;
}


int bn_init_string(bn *t, const char *str) {
    bn_init_int(t, 0);
    bn *c = bn_new();
    char sign = 0;
    if(*str == '-') {
        sign = 1;
        str++;
    }
    while('0' <= *str && *str <= '9') {
        // bn_print(t);
        // printf(" ");
        // bn_print(c);
        // printf(" ");
        bn_mul_to_int(t, 10);
        bn_add_int_abs(t, *str-'0');
        // bn_print(t);
        // printf("\n");
        str++;
    }
    (*t).sign = sign;
    delete_bn(c);
}

const char *bn_to_string(bn const *t, uint32 radix) {
    if(is_zero(t)) {
        char *s = malloc(2);
        s[0] = '0';
        s[1] = 0;
        return s;
    }
    int pw = 0;
    int64 n = 1;
    while(n*radix < (1ll<<32)) {
        n *= radix;
        pw++;
    }
    uint32 radix_max = n;
    bn *a = init(t);
    int digits = (*t).size*(pw+1) + 2;
    char *buff = calloc(digits, 1), *ret;
    int i = digits-2;
    const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    bn_and_uint *bau;
    while((*a).size > 1) {
        bau = bn_full_division_int(a, radix_max);  // (quotient ; remainder)
        for(int j = 0; j < pw; j++) {
            buff[i--] = alphabet[(*bau).rem%radix];
            (*bau).rem /= radix;
        }
        bn_move_into(a, (*bau).quot);
        free(bau);
    }
    while((*a).data[0]) {
        buff[i--] = alphabet[(*a).data[0] % radix];
        (*a).data[0] /= radix;
    }
    delete_bn(a);
    if(!is_zero(t) && (*t).sign) buff[i] = '-';
    else i++;
    ret = calloc(digits + 3 - i, 1);
    strcpy(ret, buff+i);
    free(buff);
    return ret;
}

int bn_pow_to(bn *t, int deg) {
    bn *cur_pow = init(t);
    if(deg%2 == 0)
        bn_init_int(t, 1);
    deg >>= 1;
    while(deg) {
        bn_mul_to(cur_pow, cur_pow);
        if(deg & 1)
            bn_mul_to(t, cur_pow);
        deg >>= 1;
    }
    delete_bn(cur_pow);
}

// const char* bn_to_string_no_divs(bn const *t, uint32 radix){

// }

//no overwriting
int bn_to_string_rec_fast(bn const *t, uint32 radix, int pw, char *c){
    if(is_zero(t)) {
        *c = '0';
        return 1;
    }
    if((*t).size > 30000) {
        int digits = (*t).size*pw + 2;
        bn *a = bn_new();
        bn_init_int(a, radix);
        bn_pow_to(a, digits/2);
        (*a).sign = (*t).sign;
        bn_pair *bnp = bn_full_division(t, a);
        (*(*bnp).rem).sign = 0;
        delete_bn(a);
        int bl2 = bn_to_string_rec_fast((*bnp).rem, radix, pw, c);
        for(int i = -bl2; i > -(digits/2); i--) c[i] = '0';
        int bl1 = bn_to_string_rec_fast((*bnp).quot, radix, pw, c-(digits/2));
        delete_bn((*bnp).rem);
        delete_bn((*bnp).quot);
        free(bnp);
        return bl1 + digits/2;
    } else {
        // uint64 a = (*t).data[0];
        // if((*t).size > 1)
        //     a += ((uint64)(*t).data[1])<<32ull;
        // if(a == 0) {
        //     *c = '0';
        //     return 1;
        // }
        // const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        // int l = 0;
        // while(a > 0) {
        //     (*c) = alphabet[a % radix];
        //     a /= radix;
        //     c--;
        //     l++;
        // }
        // return l;
        char *s = (char*)bn_to_string(t, radix);
        int l = strlen(s);
        c -= l-1;
        for(int i = 0; s[i]; i++) c[i] = s[i];
        return l;
    }
}

const char* bn_to_string_fast(bn const *t, uint32 radix){
    int pw;
    uint64 n = 1;
    while(n < 1ll<<32) {
        n *= radix;
        pw++;
    }
    int len1 = (*t).size*pw;
    char *buff = malloc(len1);
    buff[len1-1] = 0;
    int len2 = bn_to_string_rec_fast(t, radix, pw, buff+len1-2);
    char *ret = malloc(len2);
    if((*t).sign) ret[0] = '-';
    strcpy(ret + ((*t).sign != 0), buff+len1-len2-1);
    return ret;
}

int bn_root_to_newton(bn *t, int reciprocal) {
    bn *s = init(t);
    bn_shift(t, (*t).size / -2);
    for(int i = 0; i < 100; i++){
        bn *a = init(t);
        bn_pow_to(t, reciprocal-1);
        bn *b = bn_mul(t, a);
        bn_sub_to(b, s);
        bn_mul_to_int(t, reciprocal);
        bn_pair *pair = bn_full_division(b, t);
        bn_move_into(t, bn_sub(a, (*pair).quot));
        delete_bn(a);
        delete_bn(b);
        if((*(*pair).quot).size == 1 && (*(*pair).quot).data[0] == 0) {
            delete_bn((*pair).quot);
            delete_bn((*pair).rem);
            break;
        }
        delete_bn((*pair).quot);
        delete_bn((*pair).rem);
    }
    delete_bn(s);
    bn *tp = init(t);
    bn_pow_to(tp, reciprocal);
    if(bn_cmp(tp, s) >= 0) {
        bn_sub_int_abs(t, 1);
    }
}

int bn_root_to_bin(bn *t, int rep) {
    if(rep == 1) return 0;
    int sign = 0;
    if((*t).sign) {
        sign = 1;
        (*t).sign = 0;
    }
    bn *s = init(t);
    bn *mn = bn_new(), *mx = init(t); // min max
    bn_shift(mx, -((*mx).size*(rep-1)/rep));
    bn_init_int(mn, 1);
    int i = 0;
    while(bn_cmp(mn, mx) < 0) {
        i++;
        bn_move_into(t, bn_add(mn, mx));
        bn_add_int_abs(t, 1);
        bn_div_to_int(t, 2);
        bn *c = init(t);
        bn_pow_to(c, rep);
        int64 cmp = bn_cmp(c, s);
        if(cmp == 0) {
            delete_bn(s);
            delete_bn(mn);
            delete_bn(mx);
            (*t).sign = sign;
            return 0;
        } else if(cmp > 0) {
            bn_copy_into(mx, t);
            bn_sub_int_abs(mx, 1);
        } else {
            bn_copy_into(mn, t);
        }
    }
    bn_move_into(t, mx);
    delete_bn(s);
    delete_bn(mn);
    (*t).sign = sign;
    return 0;
}

// умножение методом Карацубы
bn *bn_karat_mul(bn const *a, bn const *b) {
    int s=1, ms = max((*a).size, (*b).size);
    if(min((*a).size, (*b).size) < 1000) {
        // printf("small numbers\n");
        return bn_mul(a, b);
    } // else printf("big numbers\n");
    while(s < ms) s *= 2;
    bn  *a0 = bn_new(), *a1 = bn_new(), *b0 = bn_new(), *b1 = bn_new();
    bn_resize(a0, min(s/2, (*a).size)); 
    bn_resize(a1, max(0,   (*a).size-s/2)); 
    bn_resize(b0, min(s/2, (*b).size)); 
    bn_resize(b1, max(0,   (*b).size-s/2)); 
    for(int i = 0; i < (*a0).size; i++) (*a0).data[i] = (*a).data[i];
    for(int i = 0; i < (*a1).size; i++) (*a1).data[i] = (*a).data[i+s/2];
    for(int i = 0; i < (*b0).size; i++) (*b0).data[i] = (*b).data[i];
    for(int i = 0; i < (*b1).size; i++) (*b1).data[i] = (*b).data[i+s/2];
    if((*a1).size == 0) bn_resize(a1, 1);
    if((*b1).size == 0) bn_resize(b1, 1);
    // printf("%s %s %s %s\n", bn_to_string(a0, 10), bn_to_string(a1, 10), bn_to_string(b0, 10), bn_to_string(b1, 10));
    bn  *a0b0 = bn_karat_mul(a0, b0), 
        *a1b1 = bn_karat_mul(a1, b1), 
        *a0a1 = bn_add(a0, a1), 
        *b0b1 = bn_add(b0, b1), 
        *msum = bn_karat_mul(a0a1, b0b1);
    // printf("%s %s %s %s %s\n", bn_to_string(a0b0, 10), bn_to_string(a1b1, 10), bn_to_string(a0a1, 10), bn_to_string(b0b1, 10), bn_to_string(msum, 10));
    delete_bn(a0a1);
    delete_bn(b0b1);
    delete_bn(a0);
    delete_bn(a1);
    delete_bn(b0);
    delete_bn(b1);
    bn_sub_to(msum, a0b0);
    bn_sub_to(msum, a1b1);
    bn_shift(msum, s*32/2);
    bn_shift(a1b1, s*32);
    bn_add_to(a1b1, msum);
    bn_add_to(a1b1, a0b0);
    delete_bn(a0b0);
    delete_bn(msum);
    (*a1b1).sign = (*a).sign ^ (*b).sign;
    return a1b1;
}

// newton-rapson method
bn *bn_inverse(bn const *a, int digits) {
    bn *b = bn_new();
    bn_init_int(b, 1);
    int m = 1+(a->size-1)*32;
    int64 adata = a->data[a->size-1];
    while(adata /= 2) 
        m++;
    if(is_pow2(a)) { // not works on powers of two, apparently
        if(is_zero(a)){
            //error
            return bn_new();
        } else {
            bn_shift(b, digits-m+1);
            return b;
        }
    }
    // inv = 140/33 + a(a*256/99 - 64/11) - implement later
    // inv = (48 - 32a) / 34              - currently
    bn_shift(b, digits);
    bn *b2 = init(b);
    bn_shift(b2, 1);
    bn *inv  = bn_new();
    bn_init_int(inv, 1);
    bn_shift(inv, m);
    bn_mul_to_int(inv, 48);
    bn *a_copy = init(a);
    bn_mul_to_int(a_copy, 32);
    bn_sub_to(inv, a);
    bn_shift(inv, digits-2*m);
    bn_div_to_int(inv, 34);
    // printf("%s\n", bn_to_string(inv, 10));
    int i;
    for(i = 0;; i++){
        // inv = inv*(2*b - a*inv) / b
        bn *t = init(a);
        bn_mul_to(t, inv);
        bn_sub_to(t, b2);
        t->sign = 0;
        bn_mul_to(inv, t);
        bn_shift(inv, -digits);
        free(t);
        // 0 < b - a*inv < a
        t = bn_mul(a, inv);
        bn_sub_to(t, b);
        t->sign ^= 1;
        if(t->sign == 0 && bn_cmp(t, a) < 0)
            break;
    }
    // printf("%d\n", i);
    free(b);
    free(b2);
    return inv;
}

bn_pair *bn_full_division_fast_abs(bn const *a, bn const *b) { // a / b
    bn_pair *ans = bn_pair_new();
    int64 cmp = bn_cmp_abs(a, b);
    if(cmp <= 0) {
        if(cmp < 0) 
            ans->rem = init(a);
        else 
            bn_init_int(ans->quot, 1); 
        return ans;
    }
    int digits = a->size * 32;
    bn *t = bn_inverse(b, digits);
    ans->quot = bn_mul(t, a);
    bn_shift(ans->quot, -digits);
    free(t);
    t = bn_mul(ans->quot, b);
    ans->rem = bn_sub(a, t);
    free(t);
    if(bn_cmp_abs(ans->rem, b) >= 0) {
        bn_sub_to(ans->rem, b);
        bn_add_int_abs(ans->quot, 1);
    }
    return ans;
}