#ifndef _CRYPTO_H
#define _CRYPTO_H

#include <stdint.h>
#include <atomic>
#include "support/configCosmos.h"
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace Cosmos
{
    namespace Support
    {
        class Crypto
        {
        public:
            Crypto();
            ~Crypto();
            int32_t setKey(string key);
            int32_t setIV(vector<uint8_t> iv);
            vector<uint8_t> getRandIV(uint8_t size);
            vector<uint8_t> encrypt(vector<uint8_t> plaintext_str);
            vector<uint8_t> decrypt(vector<uint8_t> ciphertext_str);

        private:
            EVP_CIPHER_CTX *ctx;
            string key = "secretkey";
            vector<uint8_t> iv;

            static std::atomic<uint16_t> sequence; // TODO
        };
    }

}

#endif // _CRYPTO_H