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

string removeNewlines(const string& base64) {
    string cleaned;
    for (char c : base64) {
        if (c != '\n' && c != '\r') {
            cleaned += c;
        }
    }
    return cleaned;
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

vector <pair<double,size_t>> findBestKeySize (const vector <uint8_t>& rawBytes) {
    vector <pair<double, size_t>> result;
    
    for (size_t key_size = 2; key_size < 41; key_size++) {
        pair <double, size_t> temp (normalizeHD(rawBytes, key_size), key_size);
        result.push_back(temp);
    }
    sort(result.begin(), result.end());

    return result;
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

string xorWithKey(const vector<uint8_t>& rawBytes, uint8_t key) {
    string result;
    for (uint8_t byte : rawBytes) {
        result += static_cast<char>(byte ^ key);
    }
    return result;
}

double scoreEnglishText(const string& text) {
    static const unordered_map<char, double> freq = {        
        {'E', 12.02}, {'T', 9.10}, {'A', 8.12}, {'O', 7.68}, {'I', 7.31},
        {'N', 6.95}, {'S', 6.28}, {'R', 6.02}, {'H', 5.92}, {'D', 4.32},
        {'L', 3.98}, {'U', 2.88}, {'C', 2.71}, {'M', 2.61}, {'F', 2.30},
        {'Y', 2.11}, {'W', 2.09}, {'G', 2.03}, {'P', 1.82}, {'B', 1.49},
        {'V', 1.11}, {'K', 0.69}, {'X', 0.17}, {'Q', 0.11}, {'J', 0.10},
        {'Z', 0.07}};

    double score = 0;
    for (char c : text) {
        if (isalpha(c)) {
            auto it = freq.find(toupper(c));
            if (it != freq.end()) {
                double letterScore = it->second;
                if (islower(c)) {
                    letterScore *= 1.2;  // Boost lowercase letters
                } else if (isupper(c)) {
                    letterScore *= 0.9;  // Slightly reduce score for uppercase letters
                }
                score += letterScore;
            }
        } else if (isspace(c) || ispunct(c)) {
            score += 0.1;  // Small positive score for spaces or common punctuation
        } else if (!isprint(c)) {
            score -= 100;  // Heavy penalty for non-printable characters
        } else {
            score -= 50;  // Penalty for nonsensical characters
        }
    }
    return score;
}

vector <vector <uint8_t>> transposeBlock (const vector <uint8_t>& target, size_t key_size) {
    vector <vector <uint8_t>> result(key_size);

    for (size_t i = 0; i < target.size(); i++) {
        size_t columnIndex = ((i + key_size) % key_size);
        result[columnIndex].push_back(target[i]);
    }

    return result;
}

vector <uint8_t> findBestKey (const vector <vector <uint8_t>>& transposedBlocks) {
    vector <uint8_t> result;

    for (size_t i = 0; i < transposedBlocks.size(); i++) {
        vector<tuple<char, double, string>> results;
        uint8_t key = 0;
        for (size_t k = 0; k < 256; k++) {
            string result = xorWithKey(transposedBlocks[i], key);
            double score = scoreEnglishText(result);

            // Store the score and result
            results.emplace_back(key, score, result);
            key++;
        }

        sort(results.begin(), results.end(), [](const tuple<char, double, string>& a, const tuple<char, double, string>& b) {
            return get<1>(a) > get<1>(b);
        });

        // for (int i = 0; i < 1 && i < results.size(); i++) {
        //     cout << "Rank " << (i + 1) << ": Key: " << get<0>(results[i]) << ", Score: " << get<1>(results[i]) << ", Result: " << get<2>(results[i]) << endl;
        // }

        result.push_back(get<0>(results[0]));
    }

    return result;
}

vector <uint8_t> repeatingXOR(const vector<uint8_t>& target, const vector<uint8_t>& key) {
    vector <uint8_t> result;
    int targetIndex = 0;
    int keyIndex = 0;

    while (targetIndex < target.size()) {
        if (keyIndex == key.size())
            keyIndex = 0;

        result.push_back(target[targetIndex] ^ key[keyIndex]);
        
        keyIndex++;
        targetIndex++;
    }

    return result;
}

int main() {
    
    ifstream file("challenge6.txt");

    if (!file) {
        cerr << "Unable to open file challenge6.txt";
        return 1;
    }

    stringstream ss;
    ss << file.rdbuf();

    vector<uint8_t> rawBytes = base64ToBytes(removeNewlines(ss.str()));

    for (int i = 0; i < rawBytes.size(); i++) {
        cout << rawBytes[i];
    }

    // take number of key_size bytes twice and compare the Hummington Difference
    // vector <pair<double, size_t>> resultKeySize = findBestKeySize(rawBytes);
    
    // vector <vector<uint8_t>> transposedBlocks = transposeBlock(rawBytes, resultKeySize[0].second);

    // vector <uint8_t> resultKeys = findBestKey(transposedBlocks);

    // vector <uint8_t> dicypheredText = repeatingXOR(rawBytes, resultKeys);


    // cout << "Best key: ";
    // for (int i = 0; i < resultKeys.size(); i++) {
    //     cout << resultKeys[i];
    // }
    // cout << endl;

    // cout << "Plain text: ";
    // for (int i = 0; i < dicypheredText.size(); i++) {
    //     cout << dicypheredText[i];
    // }

    file.close();
    return 0;
}


// With 2 blocks
// 2 or 6