#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <tuple>
#include <limits>

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

// Function to compute Hamming distance between two byte vectors
int computeHD(const vector<uint8_t>& block1, const vector<uint8_t>& block2) {
    int distance = 0;

    for (size_t i = 0; i < block1.size(); i++) {
        int xorResult = block1[i] ^ block2[i];
        while (xorResult) {
            distance += xorResult & 1;
            xorResult >>= 1;
        }
    }

    return distance;
}

// Function to normalize the Hamming distance for a given key size
double normalizeHD(const vector<uint8_t>& target, size_t key_size) {
    if (target.size() < 4 * key_size) {
        return -1.0; // Not enough data to compare
    }

    double totalDistance = 0.0;
    size_t count = 0;

    // Compare the first four blocks of size key_size
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = i + 1; j < 4; ++j) {
            vector<uint8_t> block1(target.begin() + i * key_size, target.begin() + (i + 1) * key_size);
            vector<uint8_t> block2(target.begin() + j * key_size, target.begin() + (j + 1) * key_size);

            int hammingDistance = computeHD(block1, block2);
            totalDistance += static_cast<double>(hammingDistance) / key_size;
            ++count;
        }
    }

    return count > 0 ? totalDistance / count : -1.0;
}

int main() {
    
    ifstream file("challenge6.txt");

    if (!file) {
        cerr << "Unable to open file challenge6.txt";
        return 1;
    }

    
    stringstream ss;
    ss << file.rdbuf();



    vector<uint8_t> rawBytes = base64ToBytes(ss.str());

    vector <pair<double, size_t>> result;
    for (size_t key_size = 2; key_size < 41; key_size++) {
        pair <double, size_t> temp (normalizeHD(rawBytes, key_size), key_size);
        result.push_back(temp);
    }

    sort(result.begin(), result.end());

    for (int i = 0; i < result.size(); i++) {
        cout << "Score: " << result[i].first << endl << "key_size: " << result[i].second << endl << endl;
    }


    return 0;
}