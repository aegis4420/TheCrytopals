#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

vector<uint8_t> hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
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


/*
Character frequency analysis is effective because the patterns of letter usage in English are distinctive and well-documented. 
By comparing the frequencies of characters in a text to those expected in English, you can often determine if the text is likely written in English, 
even if the text is encrypted or obfuscated.
*/
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

int main() {
    string target = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    vector<uint8_t> rawBytes = hexToBytes(target);

    // Vector to store scores and results
    vector<pair<double, string>> results;

    // Try all possible keys
    uint8_t key = 0;
    for (size_t i = 0; i < 256; i++) {
        string result = xorWithKey(rawBytes, key);
        double score = scoreEnglishText(result);

        // Store the score and result
        results.emplace_back(score, result);
        key++;
    }

    // Sort results by score in descending order
    sort(results.begin(), results.end(), [](const pair<double, string>& a, const pair<double, string>& b) {
        return a.first > b.first;
    });

    // Output the top 5 results
    cout << "Top 5 results:" << endl;
    for (int i = 0; i < 6 && i < results.size(); i++) {
        cout << "Rank " << (i + 1) << ": Key: " << static_cast<int>(i) << ", Score: " << results[i].first << ", Result: " << results[i].second << endl;
    }

    return 0;
}