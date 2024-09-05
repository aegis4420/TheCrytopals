#include <iostream>
#include <fstream>
#include <sstream>
#include <vector> 
#include <cstdint> // for uint8_t
using namespace std;

const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

// AES S-Box for SubWord operation
static const uint8_t sBox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// AES Round Constants (Rcon) used in key expansion
static const uint8_t rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Rotate Word rotates a 4-byte word (shift left by 1 byte)
vector <uint8_t> rotWord (const vector <uint8_t>& word) {
    return {word[1], word[2], word[3], word[0]};
}

// Substitute Words substitutes each byte in a word using the S-Box
vector <uint8_t> subWord (const vector <uint8_t>& word) {
    return {sBox[word[0]], sBox[word[1]], sBox[word[2]], sBox[word[3]]};
}

void keyExpansion(const vector <uint8_t>& key, vector <uint8_t>& expandedKeys) {
    // AES-128 uses a 16-byte key, and we need 44 words (176 bytes) for the expanded key
    const int numRounds = 10;
    const int keySize = 16; // 16 bytes = 4 words
    const int expandedKeySize = 176;  // 44 words * 4 bytes

    expandedKeys.resize(expandedKeySize);

    // Copy the original key into the first part of the exapnded key
    for (int i = 0; i < keySize; i++) {
        expandedKeys[i] = key[i];
    }

    // The number of 4-byte words in the key
    int currentPos = keySize; // Start after the initial key (16 bytes)

    int rconIndex = 0;

    // Perform key expansion to fill up exapndedKeys

    while (currentPos < expandedKeySize) {
        // Get he last word (4 bytes) from the previous round key
        vector<uint8_t> temp(4);
        temp[0] = expandedKeys[currentPos - 4];
        temp[1] = expandedKeys[currentPos - 3];
        temp[2] = expandedKeys[currentPos - 2];
        temp[3] = expandedKeys[currentPos - 1];

        // Every 16 bytes (4 words), perform the key schedule core
        if (currentPos % keySize == 0) {
            // Step 1: Rotate the word
            temp = rotWord(temp);
            // Step 2: Substitue bytes using the S-Box
            temp = subWord(temp);
            // Step 3: XOR with Rcon
            temp[0] ^= rcon[rconIndex++];
        }

        // XOR with the word 16 bytes earlier (4 words back)
        for (int i = 0; i < 4; i++) {
            expandedKeys[currentPos] = expandedKeys[currentPos - keySize] ^ temp[i];
            currentPos++;
        }
    }
}




string removeNewlines(const string& base64) {
    string cleaned;
    for (char c : base64) {
        if (c != '\n' && c != '\r') {
            cleaned += c;
        }
    }
    return cleaned;
}

vector<uint8_t> base64ToBytes(const string& base64) {
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


vector <uint8_t> aes128ToBytes (vector <uint8_t>& rawBytes, vector <uint8_t>& key) {
    vector <uint8_t> decryptedText;

    // Ensure ciphertext length is a multiple of AES block size
    if (rawBytes.size() % 16 != 0) {
        cerr << "Ciphertext langth is not a multiple of AES block size (16 bytes)" << endl;
        return decryptedText;
    }

    // Expand the key for AES-128 (already implemented)
    vector <uint8_t> expandedKeys(176);
    keyExpansion(key, expandedKeys);

    // Process each 16-byte block
    for (size_t i = 0; i < rawBytes.size(); i += 16) {
        vector <uint8_t> block (rawBytes.begin() + i, rawBytes.begin() + i + 16);

        // Decrypt the block (implement AES decryption for a single block)
        // Apply the decryption rounds here

        // Append the decrypted block to the result
        decryptedText.insert(decryptedText.end(), block.begin(), block.end());
    }

    return decryptedText;
}

// Implement AES decryption operations (AddRoundKey, InvSubBytes, InvShiftRows, InvMixColumns)
// and block processing functions here


int main() {
    ifstream file("challenge7.txt");

    if (!file) {
        cerr << "Unable to open file challenge7.txt";
        return 1;
    }

    stringstream ss;
    ss << file.rdbuf();

    string key_string = "YELLOW SUBMARINE";
    vector <uint8_t> key(key_string.begin(), key_string.end());

    string cleanText = removeNewlines(ss.str());

    vector <uint8_t> rawBytes = base64ToBytes(cleanText); 


    file.close();
}