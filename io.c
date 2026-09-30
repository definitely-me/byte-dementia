//
// Created by agony on 05.03.2026.
//
#include "common.h"

int readFile(byte **healthyBytes, struct Header *header, const struct Opt *opt) {
    int64_t fileSize = 0;

    FILE *healthyFile = fopen(opt->inputPath, "rb");

    if (healthyFile == NULL) {
        printf("[!] Your input is null\n");
        return 1;
    }

    fseek(healthyFile, 0, SEEK_END);

    if (!strcmp(opt->mode, "-r")) {
        fileSize = ftell(healthyFile) - sizeof(*header);
    } else {
        fileSize = ftell(healthyFile);
    }

    *healthyBytes = malloc(sizeof(byte) * fileSize + sizeof(*header));
    fseek(healthyFile, 0, SEEK_SET);

    if (!strcmp(opt->mode, "-r")) {
        fread(*healthyBytes, sizeof(byte), fileSize + sizeof(*header), healthyFile);
    } else {
        fread(*healthyBytes, sizeof(byte), fileSize, healthyFile);
    }

    fclose(healthyFile);
    header->size = fileSize;
    return 0;
}

void writeFile(const byte *healthyBytes, const struct Opt *opt, const struct Header *header) {
    FILE *corruptedFile = fopen(opt->outputPath, "wb");

    if (!(strcmp(opt->mode, "-c"))) {
        fwrite(header, sizeof(*header), 1, corruptedFile);
        fwrite(healthyBytes, sizeof(byte), header->size, corruptedFile);
        printf("[+] Target corrupted & saved successfully.\n");
    }

    if (!(strcmp(opt->mode, "-r"))) {
        fwrite(healthyBytes + sizeof(*header), sizeof(byte), header->size, corruptedFile);
        printf("[+] Target revived & saved successfully.\n");
    }

    fclose(corruptedFile);
}