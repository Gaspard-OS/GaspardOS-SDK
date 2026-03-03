#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
 struct GaspardARCHIVE {
    wchar_t filename[100];
    uint32_t size;
} __attribute__((packed));


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s output_file input_files...\n", argv[0]);
        return 1;
    }

    FILE *output = fopen(argv[1], "ab");
    if (!output) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 2; i < argc; ++i) {
        FILE *input = fopen(argv[i], "rb");
        if (!input) {
            perror("Error opening input file");
            continue;
        }

        struct GaspardARCHIVE header;

        setlocale(LC_ALL, ""); // Set the locale to use the system's default character set

    size_t numConverted = mbstowcs(header.filename, argv[i], strlen(argv[i]));
    if (numConverted == (size_t)-1) {
        perror("mbstowcs");
        return -1;
    }
    numConverted++;
    header.filename[numConverted] = L'\0'; // Null-terminate the wide character string

        wprintf(L"Nom fichier %ls \n",header.filename);
        fseek(input, 0L, SEEK_END);
        header.size = ftell(input);
        fseek(input, 0L, SEEK_SET);

        if (fwrite(&header, sizeof(header), 1, output) != 1) {
            perror("Error writing to output file");
            continue;
        }

        char buffer[4096];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), input)) > 0) {
            if (fwrite(buffer, 1, bytesRead, output) != bytesRead) {
                perror("Error writing to output file");
                break;
            }
        }

        fclose(input);
    }

    fclose(output);
    return 0;
}