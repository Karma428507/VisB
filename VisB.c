#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef enum {
    BIN_ZERO,
    BIN_ONE
} BINARY;

static FILE* vBinFile;      // Input
static FILE* compiledFile;  // Output

static BINARY binary[0x1400];
static int binaryCounter = 0;

static char compiledBinary[0x200];
static int compiledCounter = 0;

static char *fileNameOutput;
static char *fileName;

void parse();
void compile();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("VisB can only take two statements, type in $h for help.");
        return 1;
    }

    if (strcmp(argv[1], "$h") == 0) {
        printf("Put in your file name after typing in VisB that contains visual binary.");
        return 0;
    }

    vBinFile = fopen(argv[1], "r");

    if (vBinFile == NULL) {
        printf("The file '%s' doesn't exist retype the file or type in $h for help.", argv[1]);
        return 1;
    }

    fileName = argv[1];

    parse();
    compile();
}

void parse() {
    char * line = NULL;
    size_t len = 0;
    int read;

    int i;

    while ((read = getline(&line, &len, vBinFile)) != -1) {
        char *noEnter = strtok(line, "\n");
        char *spacedWords = strtok(noEnter, " ");

        while (spacedWords != NULL)
        {
            if (strcmp(spacedWords, "one") == 0) {
                binary[binaryCounter] = BIN_ONE;
                binaryCounter++;
            }

            if (strcmp(spacedWords, "zero") == 0) {
                binary[binaryCounter] = BIN_ZERO;
                binaryCounter++;
            }

            spacedWords = strtok(NULL, " ");
        }

        //binaryCounter++;
    }

    fclose(vBinFile);

    if (binaryCounter == 0) {
        printf("File '%d' has no binary syntax.", fileName);
        exit(1);
    }

    if (binaryCounter % 8 != 0) {
        printf("File '%s' is missing %d bits.", fileName, binaryCounter % 8);
        exit(1);
    }
}

int binaryToInt(int index) {
    if (binary[index] == BIN_ONE) {
        return 1;
    }

    return 0;
}

void compile() {
    BINARY canSend = BIN_ZERO;
    char current = 0;
    int i;

    for (i = 0; i <= binaryCounter; i++) {
        switch (i % 8) {
            case 0:
                // Send last char into index
                if (canSend == BIN_ONE) {
                    if (current == 0) current = '\0';
                    compiledBinary[compiledCounter] = current;
                    current = 0;
                    compiledCounter++;
                }

                current |= 128 * binaryToInt(i);
                break;
            case 1:
                current |= 64 * binaryToInt(i);
                break;
            case 2:
                current |= 32 * binaryToInt(i);
                break;
            case 3:
                current |= 16 * binaryToInt(i);
                break;
            case 4:
                current |= 8 * binaryToInt(i);
                break;
            case 5:
                current |= 4 * binaryToInt(i);
                break;
            case 6:
                current |= 2 * binaryToInt(i);
                break;
            case 7:
                current |= binaryToInt(i);
                canSend = BIN_ONE;
                break;
        }
    }

    fileNameOutput = strtok(fileName, ".");
    strcat(fileNameOutput, ".bin");
    compiledFile = fopen(fileNameOutput, "w");
    fwrite(compiledBinary, sizeof(compiledBinary), 1, compiledFile);
    fclose(compiledFile);
}