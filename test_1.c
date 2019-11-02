#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void v(){
    char *a = calloc(1, 1);
    printf("0x%llx\n", (unsigned long long)a);
}

int main() {
    v();
    int a;
    scanf("%d", &a);
}
