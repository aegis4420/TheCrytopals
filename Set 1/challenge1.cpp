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
                score += it->second;
            }
        }
    }
    return score;
}

int main() {
    using namespace std;

    ifstream file("challenge4.txt");

    if (!file) {
        cerr << "Unable to open file challenge4.txt";
        return 1;
    }

    vector<tuple<double, string, int, char>> results; // Store score, result, line number, key
    string line;
    int lineNumber = 0;

    // Process each line separately
    while (getline(file, line)) {
        ++lineNumber;  // Increment line number
        vector<uint8_t> rawBytes = hexToBytes(line);

        for (int key = 0; key < 256; ++key) {
            string result = xorWithKey(rawBytes, static_cast<uint8_t>(key));
            double score = scoreEnglishText(result);
            results.emplace_back(score, result, lineNumber, key);
        }
    }

    // Sort results by score in descending order
    sort(results.begin(), results.end(), [](const tuple<double, string, int, int>& a, const tuple<double, string, int, int>& b) {
        return get<0>(a) > get<0>(b);
    });

    // Output the top 5 results
    cout << "Top 5 results:" << endl;
    for (int j = 0; j < 5 && j < results.size(); ++j) {
        cout << "Rank " << (j + 1) 
             << ": Line: " << get<2>(results[j]) 
             << ", Key: " << get<3>(results[j]) 
             << ", Score: " << get<0>(results[j]) 
             << ", Result: " << get<1>(results[j]) << endl;
    }

    file.close();
    return 0;
}