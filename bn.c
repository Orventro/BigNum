#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned long long int uint64;
typedef long long int int64;
typedef unsigned int uint32;

const int64 quot_32_bits = (1ll<<32)-1;

typedef struct bn{
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

int bn_move_into(bn *dest, bn *orig) {
    free((*dest).data);
    *dest = *orig;
    free(orig);
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

void bn_print_expr(bn const *x) {
    printf("print(");
    for(int i = (*x).size-1; i >= 0; i--) {
        printf("%llu", (*x).data[i]);
        if(i)
            printf(" * 2**%d + ", i*32);
    }
    printf(", end=' ')\n");
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
    int len = strlen(buff);
    buff[len-1] = 0;
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

bn *init(bn const *orig) {
    bn *x = calloc(1, sizeof(bn));
    (*x).size = (*orig).size;
    (*x).sign = (*orig).sign;
    (*x).actual_size = (*orig).actual_size;
    (*x).data = calloc((*x).actual_size, 8);
    for(int i = 0; i < (*x).size; i++)
        (*x).data[i] = (*orig).data[i];
    return x;
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
            printf("%d %d\n", newsize, (*a).size);
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

int64 bn_cmp_abs(bn const *a, bn const *b) {
    if((*a).size == (*b).size) {
        int i = (*a).size-1;
        while(i > 0 && (*a).data[i] == (*b).data[i]) i--;
        return (*a).data[i] - (*b).data[i];
    } else return (*a).size - (*b).size; 
}

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
        (*t).data[i+1] += (*t).data[i] >> 32;
        (*t).data[i] &= quot_32_bits;
    }
    if((*t).data[s-1] > quot_32_bits) {
        bn_resize(t, s+1);
        (*t).data[s] = (*t).data[s-1] >> 32;
        (*t).data[s-1] &= quot_32_bits;
    }
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
            int64 t = (*left).data[i] * (*right).data[j];
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
    return ans;
}

bn_and_uint *bn_full_division_abs_int(bn const *left, uint32 right) { // left / right
    bn_and_uint *ans = bn_and_uint_new(); // (quotioent ; remainder)
    uint64 m[32];
    m[0] = right;
    for(int i = 1; i < 32; i++) 
        m[i] = m[i-1] + m[i-1];
    int i = (*left).size-1;
    if((*left).data[i] < right)
        (*ans).rem = (*left).data[i--];
    for(; i >= 0; i--) {
        (*ans).rem <<= 32;
        (*ans).rem |= (*left).data[i];
        int j = 31;
        while ((*ans).rem >= m[0]) {
            for(; j >= 0; j--)
                if((*ans).rem >= m[j])
                    break;
            (*(*ans).quot).data[0] += 1ll<<j;
            (*ans).rem -= m[j];
        }
        bn_shift((*ans).quot, 32);
    }
    bn_shift((*ans).quot, -32);
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
        bn_init_int(c, *str-'0');
        // bn_print(c);
        // printf(" ");
        bn_mul_to_int(t, 10);
        bn_add_to(t, c);
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
    while(n < (1ll<<32)) {
        n *= radix;
        pw++;
    }
    bn *a = init(t);
    char *buff = calloc(pw * (*t).size + 1, 1);
    int i = pw * (*t).size;
    const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    while(!is_zero(a)) {
        // printf("0\n");
        bn_and_uint *bau = bn_full_division_abs_int(a, radix);  // (quotient ; remainder)
        buff[i--] = alphabet[(*bau).rem];
        bn_move_into(a, (*bau).quot);
        free(bau);
    }
    if(!is_zero(t) && (*t).sign) buff[i] = '-';
    else i++;
    return buff+i;
}

int bn_pow_to(bn *t, int deg) {
    bn *cur_pow = init(t);
    if(deg%2 == 0)
        bn_init_int(t, 1);
    deg >>= 1;
    while(deg) {
        printf("%s**2 = ", bn_to_string(cur_pow, 10));
        bn_mul_to(cur_pow, cur_pow);
        printf("%s\n", bn_to_string(cur_pow, 10));
        printf("%s*%s = ", bn_to_string(cur_pow, 10), bn_to_string(t, 10));
        if(deg & 1)
            bn_mul_to(t, cur_pow);
        printf("%s\n", bn_to_string(t, 10));
        deg >>= 1;
    }
    delete_bn(cur_pow);
}
