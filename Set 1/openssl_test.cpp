#include <iostream>
#include <openssl/evp.h>

using namespace std;

int main() {
    std::cout << "OpenSSL version: " << OpenSSL_version(OPENSSL_VERSION) << std::endl;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx) {
        std::cout << "EVP_CIPHER_CTX created successfully" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
    } else {
        std::cout << "Failed to create EVP_CIPHER_CTX" << std::endl;
    }

    return 0;
}