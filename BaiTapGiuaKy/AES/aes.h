#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stddef.h>

// Ham ma hoa 1 block 16 byte (128 bit) voi AES-128
// in: mang 16 byte (plaintext block)
// out: mang 16 byte (ciphertext block)
// key: mang 16 byte (key AES-128)
void aes_encrypt_block(const uint8_t in[16], uint8_t out[16], const uint8_t key[16]);

// Ham giai ma 1 block 16 byte voi AES-128
// in: mang 16 byte (ciphertext block)
// out: mang 16 byte (plaintext block)
// key: mang 16 byte (key AES-128)
void aes_decrypt_block(const uint8_t in[16], uint8_t out[16], const uint8_t key[16]);

#endif
