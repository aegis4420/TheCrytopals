#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

static const char BASE64_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

vector<uint8_t> hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

string base64Encode (vector <uint8_t>& rawBytes) {
    string base64;
    size_t padding = (3 - (rawBytes.size() % 3)) % 3;
    vector<uint8_t> paddedBytes(rawBytes);
    
    paddedBytes.insert(paddedBytes.end(), padding, 0);

    for (size_t i = 0; i < paddedBytes.size(); i += 3) {
        uint32_t triple = (paddedBytes[i] << 16) | (paddedBytes[i + 1] << 8) | paddedBytes[i + 2];
        base64 += BASE64_ALPHABET[(triple >> 18) & 0x3F];
        base64 += BASE64_ALPHABET[(triple >> 12) & 0x3F];
        base64 += BASE64_ALPHABET[(triple >> 6) & 0x3F];
        base64 += BASE64_ALPHABET[triple & 0x3F];
    }

    if (padding > 0) {
        base64.erase(base64.end() - padding, base64.end());
        base64.append(padding, '=');
    }

    return base64;
}



int main() {
    string target = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    vector<uint8_t> rawBytes = hexToBytes(target);

    string base64String = base64Encode(rawBytes);

    cout << "Base64 Encoded: " << base64String << endl;

    return 0;
}