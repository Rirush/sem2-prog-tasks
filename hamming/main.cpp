#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

uint8_t next_bit(uint8_t &v) {
    uint8_t r = v & 1;
    v >>= 1;
    return r;
}

uint8_t next_byte(uint16_t &v) {
    uint8_t r = v & 0b11111111;
    v >>= 8;
    return r;
}

void push_bit(uint8_t &v, uint8_t bit) {
    v <<= 1;
    v |= bit;
}

void push_byte(uint16_t &v, uint8_t byte) {
    v <<= 8;
    v |= byte;
}

uint8_t encode(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
    uint8_t p1 = d1 ^ d2 ^ d4;
    uint8_t p2 = d1 ^ d3 ^ d4;
    uint8_t p3 = d2 ^ d3 ^ d4;
    uint8_t p0 = p1 ^ p2 ^ p3 ^ d1 ^ d2 ^ d3 ^ d4;
    uint8_t result = 0;
    push_bit(result, d4);
    push_bit(result, d3);
    push_bit(result, d2);
    push_bit(result, p3);
    push_bit(result, d1);
    push_bit(result, p2);
    push_bit(result, p1);
    push_bit(result, p0);
    return result;
}

uint16_t encode(uint8_t value) {
    uint8_t d[8] = {next_bit(value), next_bit(value), next_bit(value), next_bit(value), next_bit(value), next_bit(value), next_bit(value), next_bit(value)};
    uint8_t parts[2] = {encode(d[0], d[1], d[2], d[3]), encode(d[4], d[5], d[6], d[7])};
    uint16_t result = 0;
    push_byte(result, parts[0]);
    push_byte(result, parts[1]);
    return result;
}

uint8_t decode(uint8_t part) {
    uint8_t part_copy = part;
    uint8_t p0 = next_bit(part);
    uint8_t p1 = next_bit(part);
    uint8_t p2 = next_bit(part);
    uint8_t d1 = next_bit(part);
    uint8_t p3 = next_bit(part);
    uint8_t d2 = next_bit(part);
    uint8_t d3 = next_bit(part);
    uint8_t d4 = next_bit(part);
    uint8_t error = 0;
    push_bit(error, p3 ^ d2 ^ d3 ^ d4);
    push_bit(error, p2 ^ d1 ^ d3 ^ d4);
    push_bit(error, p1 ^ d1 ^ d2 ^ d4);
    p0 = p0 ^ p1 ^ p2 ^ p3 ^ d1 ^ d2 ^ d3 ^ d4;
    if(p0 == 0 && error != 0) {
        cout << "unrecoverable error" << endl;
    }
    part_copy ^= 1 << error;
    uint8_t result = 0;
    push_bit(result, (part_copy >> 7) & 1);
    push_bit(result, (part_copy >> 6) & 1);
    push_bit(result, (part_copy >> 5) & 1);
    push_bit(result, (part_copy >> 3) & 1);
    return result;
}

uint8_t decode(uint16_t encoded) {
    uint8_t parts[2] = {next_byte(encoded), next_byte(encoded)};
    uint8_t result = 0;
    result |= decode(parts[0]);
    result <<= 4;
    result |= decode(parts[1]);
    return result;
}

int main(int argc, char **argv) {
    if(argc != 4) {
        cout << "Usage: " << argv[0] << " [encode|decode] fileIn fileOut" << endl;
        return 1;
    }
    bool encoding = true;
    if(strcmp(argv[1], "encode") == 0) {
        encoding = true;
    } else if(strcmp(argv[1], "decode") == 0) {
        encoding = false;
    } else {
        cout << "Usage: " << argv[0] << " [encode|decode] fileIn fileOut" << endl;
        return 1;
    }
    auto file_in = fopen(argv[2], "rb");
    auto file_out = fopen(argv[3], "wb");
    if(file_in == nullptr) {
        cout << "Failed to open " << argv[2] << endl;
        return 2;
    }
    if(file_out == nullptr) {
        cout << "Failed to open " << argv[3] << endl;
        return 2;
    }

    char* buf = new char[128]();
    while(true) {
        size_t read = fread(buf, 1, 128, file_in);
        if(read == 0) {
            break;
        } else if (errno != 0) {
            cout << "Failed to read data " << strerror(errno) << endl;
            return 3;
        }
        for(size_t i = 0; i < read; i++) {
            if(encoding) {
                uint16_t out = encode(buf[i]);
                fwrite(&out, 2, 1, file_out);
                if(errno != 0) {
                    cout << "Failed to write data " << strerror(errno) << endl;
                    return 4;
                }
            } else {
                uint16_t block = 0;
                push_byte(block, buf[i+1]);
                push_byte(block, buf[i]);
                i++;
                uint8_t out = decode(block);
                fwrite(&out, 1, 1, file_out);
                if(errno != 0) {
                    cout << "Failed to write data " << strerror(errno) << endl;
                    return 4;
                }
            }
        }
    }
    fclose(file_in);
    fclose(file_out);
}
