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

/*
In C++, characters in a string are already in rawBytes
*/

string repeatingXOR(const string& target, const string& key) {
    string result = "";
    int targetIndex = 0;
    int keyIndex = 0;

    while (targetIndex < target.size()) {
        if (keyIndex == 3)
            keyIndex = 0;

        result += target[targetIndex] ^ key[keyIndex];
        
        keyIndex++;
        targetIndex++;
    }

    return result;
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
    string target = "Burning 'em, if you ain't quick and nimble\n"
    "I go crazy when I hear a cymbal";
    string key = "ICE";


    string maskedResult = repeatingXOR(target, key);
    string result = byteToHex(maskedResult);

    cout << result << endl;

     return 0;
}