//
// Created by agony on 05.03.2026.
//
#pragma once
#ifndef BYTEDEMENTIA_COMMON_H
#define BYTEDEMENTIA_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

#define SALTY_KEY_SIZE 1024

struct Opt {
    char *key;
    int keyLength;
    char *inputPath;
    char *outputPath;
    char *mode;
};

struct Header {
    char identity[5];
    char version[3];
    int64_t size;
    byte salt[8];
    int64_t checksum;
};

int corrupt(byte *healthyBytes, const struct Opt *opt, struct Header *header);
int revive(byte *healthyBytes, const struct Opt *opt,  const struct Header *header);
void writeFile(const byte *healthyBytes, const struct Opt *opt, const struct Header *header);
int readFile(byte **healthyBytes, struct Header *header, const struct Opt *opt);
int processKey(const struct Opt *opt, struct Header *header, char** saltyKey);

#endif //BYTEDEMENTIA_COMMON_H