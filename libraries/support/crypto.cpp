#include "crypto.h"

namespace Cosmos
{
    namespace Support
    {
        std::atomic<uint32_t> Crypto::prev_sequence{0};

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

        int32_t Crypto::encrypt(vector<uint8_t> plaintext_str, vector<uint8_t> &ciphertext_str)
        {
            int ciphertext_len;
            vector<uint8_t> buf(plaintext_str.size() + 4);
            vector<uint8_t> plaintext;

            plaintext.resize(4);
            uint32to(decisec(), plaintext.data());
            plaintext.insert(plaintext.end(), plaintext_str.begin(), plaintext_str.end());

            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL);
            EVP_EncryptInit_ex(ctx, NULL, NULL, (unsigned char *)key.c_str(), iv.data());
            EVP_EncryptUpdate(ctx, buf.data(), &ciphertext_len, plaintext.data(), plaintext.size());

            ciphertext_str.clear();
            for (int i = 0; i < ciphertext_len; i++)
            {
                ciphertext_str.push_back(buf[i]);
            }

            ciphertext_str.insert(ciphertext_str.end(), iv.begin(), iv.end());

            return 0;
        }

        int32_t Crypto::decrypt(vector<uint8_t> ciphertext_str, vector<uint8_t> &plaintext_str)
        {
            int plaintext_len;
            vector<uint8_t> buf(ciphertext_str.size() + 4);
            vector<uint8_t> plaintext;

            plaintext.insert(plaintext.begin(), ciphertext_str.begin(), ciphertext_str.end() - iv.size());

            EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL);
            EVP_DecryptInit_ex(ctx, NULL, NULL, (unsigned char *)key.c_str(), iv.data());
            EVP_DecryptUpdate(ctx, buf.data(), &plaintext_len, plaintext.data(), plaintext.size());

            vector<uint8_t> sequence(4);
            memcpy(sequence.data(), &buf[0], 4);

            uint32_t curr_sequence = uint32from(sequence.data());
            if (curr_sequence <= prev_sequence)
            {
                return COSMOS_GENERAL_ERROR_BAD_ACK;
            }

            plaintext_str.clear();
            for (int i = 4; i < plaintext_len; i++)
            {
                plaintext_str.push_back(buf[i]);
            }

            prev_sequence = curr_sequence;
            return 0;
        }
    }

}