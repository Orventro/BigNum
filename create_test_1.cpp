#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;

void print_num(int len) {
    for(int i = 0; i < len; i++)
        cout << (char)(rand()%10+'0');
}

int main() {
    int n;
    cin >> n;
    printf("%d\n", n);
    print_num(1000);
    printf("\n");
    for(int i = 0; i < n; i++) {
        string ops = "+-*%/";
        printf("%c ", ops[rand()%5]);
        print_num(rand()%1000+10);
        printf("\n");
    }
}