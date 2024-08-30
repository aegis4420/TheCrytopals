#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;


vector<uint8_t> hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

/*
Using rawBytes2 (key) to mask it (XOR) rawBytes1 to get the secret message.
*/
string xorCombination(const vector<uint8_t>& rawBytes1, const vector<uint8_t>& rawBytes2) {
    string result;

    int len = min(rawBytes1.size(), rawBytes2.size());
    for (size_t i = 0; i < len; i++) {
        result += static_cast<char>(rawBytes1[i] ^ rawBytes2[i]);
    }
  
    return result;
}



int main() {
    string target1 = "1c0111001f010100061a024b53535009181c";
    string target2 = "686974207468652062756c6c277320657965";

    vector<uint8_t> rawBytes1 = hexToBytes(target1);
    vector<uint8_t> rawBytes2 = hexToBytes(target2);

    cout << xorCombination(rawBytes1, rawBytes2) << endl;

    return 0;
}