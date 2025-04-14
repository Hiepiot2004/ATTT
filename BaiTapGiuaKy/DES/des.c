#include "des.h"
#include <string.h>

/* Bang hoan vi ban dau (Initial Permutation - IP) */
static const int IP[64] = {
    58,50,42,34,26,18,10,2,
    60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,
    64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,
    59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,
    63,55,47,39,31,23,15,7
};

/* Bang hoan vi nguoc (Final Permutation - FP) */
static const int FP[64] = {
    40,8,48,16,56,24,64,32,
    39,7,47,15,55,23,63,31,
    38,6,46,14,54,22,62,30,
    37,5,45,13,53,21,61,29,
    36,4,44,12,52,20,60,28,
    35,3,43,11,51,19,59,27,
    34,2,42,10,50,18,58,26,
    33,1,41,9,49,17,57,25
};

/* Bang mo rong E (32 -> 48 bit) */
static const int E[48] = {
    32,1,2,3,4,5,
    4,5,6,7,8,9,
    8,9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
};

/* Bang hoan vi P cua ham F (32-bit) */
static const int P[32] = {
    16,7,20,21,
    29,12,28,17,
    1,15,23,26,
    5,18,31,10,
    2,8,24,14,
    32,27,3,9,
    19,13,30,6,
    22,11,4,25
};

/* Bang chon key PC-1 (chon 56 bit tu 64 bit, loai parity bit) */
static const int PC1[56] = {
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
};

/* Bang chon key PC-2 (chon 48 bit tu 56 bit) */
static const int PC2[48] = {
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
};

/* Bang S: 8 hop, moi hop co 4x16 */
static const int S[8][4][16] = {
    {   /* S1 */
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {   /* S2 */
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {   /* S3 */
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {   /* S4 */
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {   /* S5 */
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {   /* S6 */
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {   /* S7 */
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {   /* S8 */
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
}
};

/* Bang dich vong (16 round) */
static const int SHIFTS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 1
};

/* Ham chuyen 1 block 8 byte thanh mang 64 bit (moi bit la 0 hoac 1) */
static void block_to_bits(const uint8_t block[8], int bits[64])
{
    for (int i = 0; i < 8; i++) {
        for (int j = 7; j >= 0; j--) {
            bits[i * 8 + (7 - j)] = (block[i] >> j) & 0x01;
        }
    }
}

/* Ham chuyen mang 64 bit ve 1 block 8 byte */
static void bits_to_block(const int bits[64], uint8_t block[8])
{
    memset(block, 0, 8);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            block[i] |= bits[i*8 + j] << (7 - j);
        }
    }
}

/* Ham hoan vi: out[i] = in[table[i]-1] */
static void permute(const int *in, int *out, const int *table, int n)
{
    for (int i = 0; i < n; i++) {
        out[i] = in[ table[i] - 1 ];
    }
}

/* Ham sinh 16 subkeys (moi subkey 48 bit) */
static void generate_subkeys(const int key_bits[64], int round_keys[16][48])
{
    int permuted_key[56];
    // PC-1: chuyen key_bits (64 bit) -> permuted_key (56 bit)
    permute(key_bits, permuted_key, PC1, 56);

    int C[28], D[28];
    for (int i = 0; i < 28; i++) {
        C[i] = permuted_key[i];
        D[i] = permuted_key[i+28];
    }

    for (int round = 0; round < 16; round++) {
        int shifts = SHIFTS[round];

        int temp[28];
        memcpy(temp, C, 28 * sizeof(int));
        for (int i = 0; i < 28; i++) {
            C[i] = temp[(i + shifts) % 28];
        }
        memcpy(temp, D, 28 * sizeof(int));
        for (int i = 0; i < 28; i++) {
            D[i] = temp[(i + shifts) % 28];
        }

        int CD[56];
        for (int i = 0; i < 28; i++) {
            CD[i] = C[i];
            CD[i+28] = D[i];
        }
        permute(CD, round_keys[round], PC2, 48);
    }
}

/* Ham Feistel F: dau vao 32 bit, subkey 48 bit, ra 32 bit */
static void feistel(const int R[32], const int subkey[48], int out[32])
{
    int expanded_R[48];
    permute(R, expanded_R, E, 48);
    for (int i = 0; i < 48; i++) {
        expanded_R[i] ^= subkey[i];
    }

    int sbox_output[32];
    for (int i = 0; i < 8; i++) {
        int offset = i * 6;
        int row = (expanded_R[offset] << 1) | expanded_R[offset+5];
        int col = (expanded_R[offset+1] << 3) | (expanded_R[offset+2] << 2) |
                  (expanded_R[offset+3] << 1) | expanded_R[offset+4];
        int s_val = S[i][row][col];
        for (int j = 0; j < 4; j++) {
            sbox_output[i*4 + (3-j)] = (s_val >> j) & 0x01;
        }
    }
    permute(sbox_output, out, P, 32);
}

/* Ham xu ly block 64 bit, du dung cho ma hoa/giai ma
   encrypt = 1 de ma hoa, encrypt = 0 de giai ma */
static void des_process_block(const uint8_t in[8], uint8_t out[8], const uint8_t key[8], int encrypt)
{
    int block[64], permuted_block[64];
    block_to_bits(in, block);

    permute(block, permuted_block, IP, 64);

    int L[32], R[32];
    for (int i = 0; i < 32; i++) {
        L[i] = permuted_block[i];
        R[i] = permuted_block[i+32];
    }

    int key_bits[64];
    block_to_bits(key, key_bits);
    int round_keys[16][48];
    generate_subkeys(key_bits, round_keys);

    for (int round = 0; round < 16; round++) {
        int tempR[32];
        memcpy(tempR, R, 32 * sizeof(int));

        int f_out[32];
        if (encrypt)
            feistel(R, round_keys[round], f_out);
        else
            feistel(R, round_keys[15-round], f_out);

        for (int i = 0; i < 32; i++) {
            R[i] = L[i] ^ f_out[i];
        }
        memcpy(L, tempR, 32 * sizeof(int));
    }

    int preoutput[64];
    for (int i = 0; i < 32; i++) {
        preoutput[i] = R[i];
        preoutput[i+32] = L[i];
    }

    int final_block[64];
    permute(preoutput, final_block, FP, 64);

    bits_to_block(final_block, out);
}

void des_encrypt_block(const uint8_t in[8], uint8_t out[8], const uint8_t key[8])
{
    des_process_block(in, out, key, 1);
}

void des_decrypt_block(const uint8_t in[8], uint8_t out[8], const uint8_t key[8])
{
    des_process_block(in, out, key, 0);
}
