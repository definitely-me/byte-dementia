//
// Created by agony on 05.03.2026.
//
#include "common.h"
#include "randombytes.h"

int corrupt(byte *healthyBytes, const struct Opt *opt, struct Header *header) {
    int64_t fileSize = header->size;
    int64_t checksumA = 0;
    int64_t checksumB = 0;

    if (fileSize == 0) {
        printf("[!] File is empty.\n");
        return 1;
    }

    char *saltyKey = malloc(sizeof(char) * SALTY_KEY_SIZE);
    processKey(opt, header, &saltyKey);
    int keyCounter = 0;
    byte currentByte = 0;

    for (int i = 0; i < fileSize; i++) {

        checksumA += healthyBytes[i];
        checksumB += checksumA + healthyBytes[i];

        currentByte = healthyBytes[i] ^ saltyKey[keyCounter];
        if (i != 0) {
            currentByte = currentByte + healthyBytes[i - 1];
        }

        healthyBytes[i] = currentByte;
        keyCounter++;

        if (keyCounter >= SALTY_KEY_SIZE) {
            keyCounter = 0;
        }
    }

    free(saltyKey);
    header->checksum = checksumA + checksumB;
    return 0;
}

int revive(byte *healthyBytes, const struct Opt *opt, const struct Header *header) {
    char usrInput;
    int64_t fileSize = header->size;
    struct Header origHeader = *(struct Header *) healthyBytes;
    int64_t decryptedSize = origHeader.size;
    int64_t checksumA = 0;
    int64_t checksumB = 0;

    char *saltyKey = malloc(sizeof(char) * SALTY_KEY_SIZE);
    processKey(opt, &origHeader, &saltyKey);

    if (memcmp(origHeader.identity, header->identity, 5) != 0 ||
        memcmp(origHeader.version, header->version, 3) != 0) {
        printf("[!] File is not compatible/header was corrupted.\n");
        return 1;
    }
    printf("[*] Original file size: %ld bytes.\n", decryptedSize);

    int keyCounter = (fileSize - 1) % SALTY_KEY_SIZE;
    byte currentByte = 0;

    for (int i = fileSize + sizeof(*header) - 1; i >= sizeof(*header); i--) {
        if (i == sizeof(*header)) {
            currentByte = healthyBytes[i] ^ saltyKey[keyCounter];
        } else {
            currentByte = (healthyBytes[i] - healthyBytes[i - 1]) ^ saltyKey[keyCounter];
        }

        healthyBytes[i] = currentByte;
        keyCounter--;

        if (keyCounter == -1) {
            keyCounter = SALTY_KEY_SIZE - 1;
        }
    }

    for (int i = sizeof(*header); i < fileSize + sizeof(*header); i++) {
        checksumA += healthyBytes[i];
        checksumB += checksumA + healthyBytes[i];
    }

    if (checksumA + checksumB != origHeader.checksum) {
        printf("[!] Checksum is invalid due to wrong key or data loss.\n");
        printf("[?] Proceed anyway? y/n:");
        scanf("%c", &usrInput);
        if (usrInput != 'y') {
            return 1;
        }
    }
    else {
        printf("[+] Checksum is valid\n");
    }

    free(saltyKey);
    return 0;
}

int processKey(const struct Opt *opt, struct Header *header, char **saltyKey) {
    char *mode = opt->mode;
    char *key = opt->key;
    int keyLength = opt->keyLength;
    int saltCounter = 0;
    int keyCounter = 0;
    byte salt[8];

    if (!strcmp(mode, "-c")) {
        printf("[*] Generating random seed...\n");
        if (randombytes(salt, 8)) {
            printf("[!] randombytes.c error\n");
            return 1;
        }
        memcpy(header->salt, salt, 8);
    }

    if (!strcmp(mode, "-r")) {
        memcpy(salt, header->salt, 8);
    }

    printf("[*] Salt seed: ");
    for (int i = 0; i < 8; i++) {
        printf("%02x", salt[i]);
    }
    printf("\n");
    printf("[*] Generating 1024-byte key (500,000 iterations)...\n");

    for (int i = 0; i < SALTY_KEY_SIZE; i++) {
        if (i % 2 == 0) {
            (*saltyKey)[i] = salt[saltCounter];
            saltCounter++;
        }
        else {
            (*saltyKey)[i] = key[keyCounter];
            keyCounter++;
        }
        if (keyCounter == keyLength) {keyCounter = 0;}
        if (saltCounter == 8) {saltCounter = 0;}
    }
    for (int64_t i = 0; i < SALTY_KEY_SIZE * 500000; i++) {
        (*saltyKey)[i % SALTY_KEY_SIZE] = (*saltyKey)[(i * i) % SALTY_KEY_SIZE] ^ (*saltyKey)[(i + 124 * 12) % SALTY_KEY_SIZE];
        (*saltyKey)[i % SALTY_KEY_SIZE] = (*saltyKey)[(i + i) % SALTY_KEY_SIZE] ^ (*saltyKey)[(i + 'a' + 'g' + 'o' + 'n' + 'y') % SALTY_KEY_SIZE];
    }

    printf("[+] Key generated successfully.\n");

    return 0;
}
