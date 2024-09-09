#include <iostream>
#include <fstream>
#include <sstream>
#include <vector> 
#include <cstdint> // for uint8_t
#include <openssl/aes.h>
#include <openssl/evp.h>

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

vector <uint8_t> ase128Decrypt (const vector <uint8_t>& rawBytes, const vector <uint8_t>& key) {

    // The algorithm requires the key size of 128 bits (16 bytes)
    if (key.size() != 16) {
        throw runtime_error("Key must be 16 bytes long.");
    }

    // CTX is the structure used to store info about the cypher and information
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw runtime_error("Failed to create EVP_CIPHER_CTX.");
    }

    // Intialize the copher context for decryption, EVP_aes_128_ecb() specifies ECB mode
    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Failed to initialize decryption.");
    }

    // EVP_DecryptUpdate decrypts the rawBytes into blocks, writing the result to plaintext
    // len contains the number of bytes written to the plaintext buffer
    vector<uint8_t> plaintext(rawBytes.size());
    int len;
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, rawBytes.data(), rawBytes.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Decryption failed.");
    }
    
    int plaintext_len = len;

    // It is used to finalize the decryption process and handly any remaining bytes that may not have been processed in the full blocks
    // If this step fails, usually indicates the padding is invalid
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("Final decryption step failed.");
    }

    // The total length of the plaintext is updated to include the additional bytes processed in the final step. 
    // The plaintext vector is resized to match the exact decrypted size.
    plaintext_len += len;
    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}


int main() {
    ifstream file("challenge7.txt");

    if (!file) {
        cerr << "Unable to open file challenge7.txt";
        return 1;
    }

    stringstream ss;
    ss << file.rdbuf();

    file.close();

    string key_string = "YELLOW SUBMARINE";
    vector <uint8_t> key(key_string.begin(), key_string.end());

    string cleanText = removeNewlines(ss.str());

    vector <uint8_t> rawBytes = base64ToBytes(cleanText); 

    // Decrypt the bytes
    vector <uint8_t> decryptedBytes = ase128Decrypt(rawBytes, key);

    // Convert decrypted bytes to string and print
    string decryptedText(decryptedBytes.begin(), decryptedBytes.end());
    cout << decryptedText << endl;



}