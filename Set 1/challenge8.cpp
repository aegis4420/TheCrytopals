#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <array>

using namespace std;

string removeNewlines(const string& base64) {
    string cleaned;
    for (char c : base64) {
        if (c != '\n' && c != '\r') {
            cleaned += c;
        }
    }
    return cleaned;
}

vector<uint8_t> hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

bool detectECB(const vector<uint8_t>& rawBytes) {
    const int blockSize = 16;
    set<array<uint8_t, blockSize>> seenBlocks;

    // Loop through rawBytes in chunks of 16 bytes
    for (size_t i = 0; i + blockSize <= rawBytes.size(); i += blockSize) {
        // Extract the 16-byte block
        array<uint8_t, blockSize> block;
        copy(rawBytes.begin() + i, rawBytes.begin() + i + blockSize, block.begin());

        // Debug: Print each block for visual inspection
        cout << "Block " << i / blockSize << ": ";
        for (size_t i = 0; i < block.size(); i++) {
            cout << block[i];
        }
        cout << endl;

        // If the block is already in the set, it's a strong indication of ECB
        if (!seenBlocks.insert(block).second) {
            return true;
        }
    }

    // No repeated blocks found, unlikely to be ECB
    return false;
}


/*
We don't necessarily need to convert the hex encoded text into raw bytes as long as we make sure to seperate the right amount of bytes into a chunk.
Since one hex char is 4 bits, so 5 hex chars are 1 byte.
That means we want 32 hex chars to have 16 raw bytes.
*/
bool detectECBHex(const string& hexString) {
    const int blockSize = 32;  // 16 raw bytes = 32 hex characters
    unordered_map<string, int> blockCounts;

    // Loop through hexString in chunks of 32 hex characters
    for (size_t i = 0; i + blockSize <= hexString.size(); i += blockSize) {
        // Extract the 32-character block (representing 16 raw bytes)
        string block = hexString.substr(i, blockSize);

        // Debug: Print each block for visual inspection
        cout << "Block " << i / blockSize << ": " << block << endl;

        // Check if the block is already seen
        if (blockCounts.find(block) != blockCounts.end()) {
            cout << "Repeated block found at position " << i / blockSize << endl;
            return true;  // ECB detected
        } else {
            blockCounts[block] = 1;
        }
    }

    // No repeated blocks found
    return false;
}


int main() {
    ifstream file("challenge8.txt");

    if (!file) {
        cerr << "Unable to open file challenge8.txt" << endl;
        return 1;
    }

    stringstream ss;
    ss << file.rdbuf();

    file.close();

    
    vector<uint8_t> rawBytes = hexToBytes(removeNewlines(ss.str()));

    if (detectECB(rawBytes)) {
        cout << "ECB encryption detected!" << endl;
    } else {
        cout << "No ECB encryption detected!" << endl;
    }

    return 0;
}