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
    int n, k; // num, len
    cin >> n >> k;
    cout << n << endl;
    for(int i = 0; i < n; i++) {
        int l1 = rand()%k, l2 = rand()%(k/2); 
        print_num(l1);
        cout << ' ';
        print_num(l2);
        cout << endl;
    }
}