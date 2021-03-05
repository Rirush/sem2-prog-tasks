#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<long long> primes;

bool check_if_prime(long long n) {
    for(auto prime : primes) {
        if(n % prime == 0) {
            return false;
        }
    }
    return true;
}

void populate_primes(long long border) {
    for(long long i = 2; i <= border; i++) {
        if(check_if_prime(i)) {
            primes.push_back(i);
        }
    }
}

int main() {
    populate_primes(100000);
    string e;
    ifstream in("e2mil.txt");
    char c;
    while(in >> c) {
        if(c >= '0' && c <= '9') {
            e.push_back(c);
        }
    }
    for(int i = 0; i < e.size() - 10; i++) {
        string s = e.substr(i, 10);
        long long v = stoll(s);
        if(check_if_prime(v)) {
            cout << v;
            break;
        }
    }
    return 0;
}
