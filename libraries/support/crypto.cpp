#include "crypto.h"

namespace Cosmos
{
    namespace Support
    {
        Crypto::Crypto()
        {
            ctx = EVP_CIPHER_CTX_new();
            EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
        }

        Crypto::~Crypto()
        {
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);
        }

        int32_t Crypto::setKey(string key)
        {
            this->key = key;
            return 0;
        }

        int32_t Crypto::setIV(vector<uint8_t> iv)
        {
            this->iv = iv;
            return 0;
        }

        vector<uint8_t> Crypto::getRandIV(uint8_t size)
        {
            vector<uint8_t> iv;
            iv.resize(size);
            RAND_bytes(iv.data(), size);

            return iv;
        }

        vector<uint8_t> Crypto::encrypt(vector<uint8_t> plaintext_str)
        {
            int ciphertext_len;
            vector<uint8_t> buf(plaintext_str.size());
            vector<uint8_t> ciphertext_str;

            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL);
            EVP_EncryptInit_ex(ctx, NULL, NULL, (unsigned char *)key.c_str(), iv.data());
            EVP_EncryptUpdate(ctx, buf.data(), &ciphertext_len, plaintext_str.data(), plaintext_str.size());

            for (int i = 0; i < ciphertext_len; i++)
            {
                ciphertext_str.push_back(buf[i]);
            }
            return ciphertext_str;
        }

        vector<uint8_t> Crypto::decrypt(vector<uint8_t> ciphertext_str)
        {
            int plaintext_len;
            vector<uint8_t> buf(ciphertext_str.size());
            vector<uint8_t> plaintext_str;

            for (size_t i = 0; i < ciphertext_str.size() - iv.size(); i++)
            {
                plaintext_str.push_back(ciphertext_str[i]);
            }

            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL);
            EVP_DecryptInit_ex(ctx, NULL, NULL, (unsigned char *)key.c_str(), iv.data());
            EVP_DecryptUpdate(ctx, buf.data(), &plaintext_len, plaintext_str.data(), plaintext_str.size());

            plaintext_str.clear();
            for (int i = 0; i < plaintext_len; i++)
            {
                plaintext_str.push_back(buf[i]);
            }

            return plaintext_str;
        }
    }

}