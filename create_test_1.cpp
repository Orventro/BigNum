#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;

int main() {
    int n;
    cin >> n;
    for(int i = 0; i < n; i++)
        cout << (char)(rand()%10+'0');
    // cout << "\n+\n";
    // for(int i = 0; i < n; i++)
    //     cout << (char)(rand()%10+'0');
    cout << endl;
}