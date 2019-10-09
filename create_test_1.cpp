#include <iostream>
#include <random>
#include <vector>
#include <string>

using namespace std;

int main() {
    int n;
    cin >> n;
    vector<string> ops;
    for(;;) {
        string s;
        cin >> s;
        if(s != "0") {
            ops.push_back(s);
        } else break;
    }
    cout << n << endl;
    cout << rand() << endl;
    for(int i = 0; i < n; i++) {
        cout << ops[rand()%ops.size()] << ' ' << rand() << endl;
    }
}