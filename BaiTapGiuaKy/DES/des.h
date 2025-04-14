#ifndef DES_H
#define DES_H

#include <stdint.h>
#include <stddef.h>

// Ham ma hoa 1 block 8 byte (64 bit) voi DES
// in: mang 8 byte (plaintext block)
// out: mang 8 byte (ciphertext block)
// key: mang 8 byte (key DES 64 bit, thuc te la 56 bit hieu dung)
void des_encrypt_block(const uint8_t in[8], uint8_t out[8], const uint8_t key[8]);

// Ham giai ma 1 block 8 byte voi DES
// in: mang 8 byte (ciphertext block)
// out: mang 8 byte (plaintext block)
// key: mang 8 byte (key DES 64 bit)
void des_decrypt_block(const uint8_t in[8], uint8_t out[8], const uint8_t key[8]);

#endif
