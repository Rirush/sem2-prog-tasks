#include <iostream>

using namespace std;

unsigned int factorial(unsigned int n) {
    if(n == 0) {
        return 1;
    }
    return n * factorial(n - 1);
}

unsigned int sum_numbers(unsigned int m) {
    if(m < 10) {
        return m;
    }
    return sum_numbers(m/10) + m % 10;
}

double fast_pow(double x, unsigned int n) {
    if(n == 0) {
        return 1;
    }
    if(n % 2 == 0) {
        return fast_pow(x * x, n / 2);
    } else {
        return x * fast_pow(x, n - 1);
    }
}

unsigned int base2_size(unsigned int n) {
    if(n < 2) {
        return 1;
    }
    return 1+base2_size(n/2);
}

void to_base2(unsigned int n, char* buf, unsigned short stage) {
    if(n == 0) {
        return;
    } else if(n == 1) {
        buf[stage] = '1';
        return;
    }
    buf[stage] = (n % 2) + '0';
    return to_base2(n / 2, buf, stage-1);
}

char* to_base2(unsigned int n) {
    unsigned short s = base2_size(n);
    char* output = new char[s+1]();
    to_base2(n, output, s-1);
    return output;
}

//void hanoi_tower(unsigned int n) {
//
//}

//unsigned int longest_segment(char** field, unsigned int h, unsigned int w) {
//
//}

int main() {
    cout << factorial(4) << endl;
    cout << sum_numbers(1234) << endl;
    cout << fast_pow(3, 3) << endl;
    cout << to_base2(5) << endl;
    return 0;
}
