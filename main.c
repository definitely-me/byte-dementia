//
// Created by agony on 05.03.2026.
//
#include "common.h"

void help() {
    printf("\nbyteDementia v.0.5. Add-Rotate-Xor cryptography CLI application.\n\n");
    printf("%-5s    %s\n\n", "Flags", "Description");
    printf("%-5s -> %s\n", "-i", "Input path");
    printf("%-5s -> %s\n", "-o", "Output path (leave unset to use input directory)");
    printf("%-5s -> %s\n", "-k", "Your ASCII key");
    printf("%-5s -> %s\n", "-c", "Enable 'corrupt' mode (encrypt)");
    printf("%-5s -> %s\n", "-r", "Enable 'revive' mode (decrypt)");
    printf("%-5s -> %s\n\n", "-h", "Print this help message");
    printf("Examples:\n");
    printf("  Encrypt: ./byteDementia05 -i secret.txt -k mypass123 -c\n");
    printf("  Decrypt: ./byteDementia05 -i secret.txt.dmta -k mypass123 -r -o result.txt\n\n");
    printf("Note: Files are processed in-memory. Max file size depends on available RAM.\n\n");
}

int main(int argCount, char *argValues[]) {
    printf("|_  _|_ _ \n|_)\\/| (/_\n   /      \n");
    printf("           _| _  _ _  _  _ _|_. _ \nv. 0.5    (_|(/_| | |(/_| | | |(_|\n\n");

    struct Opt opt = {NULL, 0, NULL, NULL, NULL};

    for (int i = 0; i < argCount; i++) {
        if (!strcmp(argValues[i], "-i")) {
            opt.inputPath = argValues[i + 1];
            i++;
        } else if (!strcmp(argValues[i], "-o")) {
            opt.outputPath = argValues[i + 1];
            i++;
        } else if (!strcmp(argValues[i], "-h")) {
            help();
            return 0;
        } else if (!strcmp(argValues[i], "-k")) {
            opt.key = argValues[i + 1];
            opt.keyLength = strlen(argValues[i + 1]);
            i++;
        } else if (!strcmp(argValues[i], "-c")) {
            opt.mode = argValues[i];

            if (opt.outputPath == NULL) {
                if (opt.inputPath == NULL) {
                    printf("[i] Set an -i flag first.\n");
                    return 1;
                }
                opt.outputPath = opt.inputPath;
            }

            char *outputPath = malloc(strlen(opt.outputPath) + 6);
            strcpy(outputPath, opt.outputPath);
            strcat(outputPath, ".dmta");
            opt.outputPath = outputPath;

        } else if (!strcmp(argValues[i], "-r")) {
            if (opt.outputPath == NULL) {
                if (opt.inputPath == NULL) {
                    printf("[i] Set an -i flag first.\n");
                    return 1;
                }

                char *outputPath = malloc(strlen(opt.inputPath));
                strcpy(outputPath, opt.inputPath);
                outputPath[strlen(opt.inputPath) - 5] = '\0';
                opt.outputPath = outputPath;
            }
            opt.mode = argValues[i];
        }
    }

    if (opt.inputPath == NULL || opt.key == NULL || opt.mode == NULL) {
        printf("%-5s", "[!] Use -h flag to print help\n");
        printf("[!] Make sure you use all the necessary flags\n");
        return 1;
    }

    byte *healthyBytes;
    struct Header header;

    if (readFile(&healthyBytes, &header, &opt)) {
        return 1;
    }

    memcpy(header.identity, "DMTIA", 5);
    memcpy(header.version, "0.5", 3);

    printf("[*] Target: %s ", opt.inputPath);
    printf("(%ld bytes)\n", header.size);

    if (!strcmp("-c", opt.mode)) {
        printf("[*] Mode:   Encrypt (Add-Rotate-Xor)\n");
        if (corrupt(healthyBytes, &opt, &header)) {
            return 1;
        }
    }

    if (!strcmp("-r", opt.mode)) {
        printf("[*] Mode:   Decrypt (Add-Rotate-Xor)\n");
        if (revive(healthyBytes, &opt, &header)) {
            return 1;
        }
    }
    writeFile(healthyBytes, &opt, &header);
    free(healthyBytes);
    return 0;
}