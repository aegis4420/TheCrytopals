#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

vector <uint8_t> base64ToBytes(const string& base64) {
    vector<uint8_t> bytes;
    vector<uint8_t> T(256, 0);
    for (size_t i = 0; i < base64_chars.size(); i++) {
        T[static_cast<unsigned char>(base64_chars[i])] = i;
    }

    size_t val = 0;
    int valb = -8;
    for (unsigned char c : base64) {
        if (T[c] == 0 && c != 'A') break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            bytes.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return bytes;
    
}


string bytesToBase64(const vector <uint8_t>& rawBytes) {
    stringstream ss;
    ss << base64_chars;

    for (char c : rawBytes) {
        
    }

    return ss.str();

}

string byteToHex(const string& target) {
    stringstream ss;
    ss << hex;

    for (char c : target) {
        ss << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }

    return ss.str();
}

int main() {
    
    vector<uint8_t> bytes;
    vector<uint8_t> T(256, 0);
    for (size_t i = 0; i < base64_chars.size(); i++) {
        T[static_cast<unsigned char>(base64_chars[i])] = i;
    }

    for (int i = 0; i < T.size(); i++) {
        cout << "The char is: " << static_cast<int>(T[i]) << " Index: " << i << endl;
    }
    return 0;
}