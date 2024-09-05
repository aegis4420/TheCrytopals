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

}
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