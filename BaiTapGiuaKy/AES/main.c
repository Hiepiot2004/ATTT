#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"

/*
 * Ham doc file vao buffer, tra ve con tro du lieu va do dai doc duoc.
 */
unsigned char* read_file(const char *filename, size_t *length) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Khong the mo file %s de doc.\n", filename);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    rewind(f);
    
    unsigned char *buffer = (unsigned char*)malloc(*length);
    if (!buffer) {
        fprintf(stderr, "Khong cap phat duoc bo nho.\n");
        exit(1);
    }
    fread(buffer, 1, *length, f);
    fclose(f);
    return buffer;
}

/*
 * Ham ghi du lieu ra file.
 */
void write_file(const char *filename, const unsigned char *data, size_t length) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Khong the mo file %s de ghi.\n", filename);
        exit(1);
    }
    fwrite(data, 1, length, f);
    fclose(f);
}

/*
 * Ham chuyen mang byte sang chuoi hex (moi byte thanh 2 ky tu)
 */
void bytes_to_hex(const uint8_t *bytes, size_t length, char *hex_str) {
    for (size_t i = 0; i < length; i++) {
        sprintf(hex_str + i * 2, "%02X", bytes[i]);
    }
    hex_str[length * 2] = '\0';
}

/*
 * Ham chuyen chuoi hex (2 ky tu) thanh mang byte.
 */
void hex_to_bytes(const char *hex_str, uint8_t *out) {
    size_t len = strlen(hex_str);
    for (size_t i = 0; i < len / 2; i++) {
        unsigned int val;
        sscanf(hex_str + 2 * i, "%2x", &val);
        out[i] = (uint8_t)val;
    }
}

int main() {
    // 1. Doc file input.txt (plaintext) va key_aes.txt (key dang hex)
    size_t input_len;
    unsigned char *input_data = read_file("input.txt", &input_len);
    
    size_t key_len;
    unsigned char *key_hex = read_file("key_aes.txt", &key_len);
    // Loai bo ky tu xuong dong, khoang trang neu co
    for (size_t i = 0; i < key_len; i++) {
        if (key_hex[i] == '\n' || key_hex[i] == '\r' || key_hex[i] == ' ')
            key_hex[i] = '\0';
    }
    // Chuyen chuoi hex sang mang 16 byte cho key AES
    uint8_t aes_key[16] = {0};
    hex_to_bytes((char*)key_hex, aes_key);
    
    // 2. Kiem tra do dai input: AES xu ly block 16 byte
    if (input_len % 16 != 0) {
        fprintf(stderr, "Canh bao: Do dai input khong phai boi so cua 16. Can padding hoac dieu chinh.\n");
        // Tiep tuc chay, nhung du lieu con thieu se bi cat
    }
    size_t num_blocks = input_len / 16;
    unsigned char *encrypted_data = (unsigned char*)malloc(input_len);
    unsigned char *decrypted_data = (unsigned char*)malloc(input_len);
    if (!encrypted_data || !decrypted_data) {
        fprintf(stderr, "Khong cap phat duoc bo nho cho encrypt/giai ma.\n");
        exit(1);
    }
    
    // Ma hoa tung block 16 byte
    for (size_t i = 0; i < num_blocks; i++) {
        aes_encrypt_block(input_data + i * 16, encrypted_data + i * 16, aes_key);
    }
    
    // Chuyen ciphertext sang chuoi hex de ghi ra file
    size_t hex_output_len = input_len * 2;
    char *hex_ciphertext = (char*)malloc(hex_output_len + 1);
    if (!hex_ciphertext) {
        fprintf(stderr, "Khong cap phat duoc bo nho cho hex_ciphertext.\n");
        exit(1);
    }
    bytes_to_hex(encrypted_data, input_len, hex_ciphertext);
    write_file("aes_encrypted.txt", (unsigned char*)hex_ciphertext, hex_output_len);
    
    // 3. Doc file ciphertext dang hex, chuyen ve mang byte de giai ma
    free(hex_ciphertext);
    size_t enc_file_len;
    unsigned char *enc_file_data = read_file("aes_encrypted.txt", &enc_file_len);
    size_t cipher_binary_len = enc_file_len / 2;
    unsigned char *cipher_binary = (unsigned char*)malloc(cipher_binary_len);
    if (!cipher_binary) {
        fprintf(stderr, "Khong cap phat duoc bo nho cho cipher_binary.\n");
        exit(1);
    }
    hex_to_bytes((char*)enc_file_data, cipher_binary);
    
    // Giai ma tung block
    for (size_t i = 0; i < num_blocks; i++) {
        aes_decrypt_block(cipher_binary + i * 16, decrypted_data + i * 16, aes_key);
    }
    write_file("aes_decrypted.txt", decrypted_data, input_len);
    
    // Giai phong bo nho
    free(input_data);
    free(key_hex);
    free(encrypted_data);
    free(decrypted_data);
    free(enc_file_data);
    free(cipher_binary);
    
    printf("Qua trinh ma hoa/giai ma AES hoan tat.\n");
    return 0;
}
