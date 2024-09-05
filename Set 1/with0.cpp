#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <tuple> 

using namespace std;

const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


vector<uint8_t> base64ToBytes(const string& base64) {
    vector<uint8_t> bytes;
    vector<uint8_t> T(256, 0);
    for (size_t i = 0; i < base64_chars.size(); i++) {
        T[static_cast<unsigned char>(base64_chars[i])] = i;
    }

    for (int i = 0; i < T.size(); i++) {
        cout << "Index: " << i << " char: " << static_cast<int>(T[i]) << endl;
    }

    size_t val = 0;
    int valb = -8;
    int count = 0;
    for (unsigned char c : base64) {
        // cout << "Current char: " << c << endl;
        if (T[c] == 0 && T[c] != 'A') {
            // cout << "time to break" << endl;
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            bytes.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
        count++;
    }

    // cout << count << endl;

    return bytes;
    
}


int main() {
    

    vector<uint8_t> rawBytes = base64ToBytes("SGVsbG8sIFdvcmxkIQ==");

    // for (int i = 0; i < rawBytes.size(); i++) {
    //     cout << rawBytes[i];
    // }
    return 0;
}