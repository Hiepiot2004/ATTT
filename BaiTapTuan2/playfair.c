#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 5

char matrix[SIZE][SIZE];

void prepareKey(char *key) {
    int used[26] = {0};
    int i, j, k = 0;
    for (i = 0; key[i]; i++) {
        if (key[i] == 'J') key[i] = 'I';
        if (!used[key[i] - 'A']) {
            matrix[k / SIZE][k % SIZE] = key[i];
            used[key[i] - 'A'] = 1;
            k++;
        }
    }
    for (i = 0; i < 26; i++) {
        if (i + 'A' == 'J') continue;
        if (!used[i]) {
            matrix[k / SIZE][k % SIZE] = i + 'A';
            k++;
        }
    }
}

void findPosition(char ch, int *row, int *col) {
    if (ch == 'J') ch = 'I';
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (matrix[i][j] == ch) {
                *row = i;
                *col = j;
                return;
            }
        }
    }
}

void playfairEncrypt(char *plaintext, char *ciphertext) {
    int len = strlen(plaintext), i;
    char preparedText[100] = {0};
    int j = 0;
    for (i = 0; i < len; i++) {
        if (plaintext[i] == ' ') continue;
        preparedText[j++] = toupper(plaintext[i]);
    }
    for (i = 0; i < j; i += 2) {
        if (preparedText[i] == preparedText[i + 1]) {
            memmove(&preparedText[i + 1], &preparedText[i], j - i);
            preparedText[i + 1] = 'X';
            j++;
        }
    }
    if (j % 2 != 0) preparedText[j++] = 'X';
    preparedText[j] = '\0';
    for (i = 0; i < j; i += 2) {
        int r1, c1, r2, c2;
        findPosition(preparedText[i], &r1, &c1);
        findPosition(preparedText[i + 1], &r2, &c2);
        if (r1 == r2) {
            ciphertext[i] = matrix[r1][(c1 + 1) % SIZE];
            ciphertext[i + 1] = matrix[r2][(c2 + 1) % SIZE];
        } else if (c1 == c2) {
            ciphertext[i] = matrix[(r1 + 1) % SIZE][c1];
            ciphertext[i + 1] = matrix[(r2 + 1) % SIZE][c2];
        } else {
            ciphertext[i] = matrix[r1][c2];
            ciphertext[i + 1] = matrix[r2][c1];
        }
    }
    ciphertext[j] = '\0';
}

int main() {
    char key[100], plaintext[100], ciphertext[100];
    printf("Nhap khoa: ");
    gets(key);
    printf("Nhap ban ro: ");
    gets(plaintext);
    for (int i = 0; key[i]; i++) key[i] = toupper(key[i]);
    for (int i = 0; plaintext[i]; i++) plaintext[i] = toupper(plaintext[i]);
    prepareKey(key);
    playfairEncrypt(plaintext, ciphertext);
    printf("Ban ma: %s\n", ciphertext);
    return 0;
}
