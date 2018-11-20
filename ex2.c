//312349103  Eldar Shalev
#include <stdio.h>
#include <memory.h>
//define
#define SIZE 2 // for utf-16
#define BULK 1

// Enum Definition
enum Encoding {
    Unix, Mac, Win, Illegal
};

/**
 * getEndOfLineFormat function.
 * @param os - operation system type.
 * @return - the end line char according to the format.
 */
char getEndOfLineFormat(enum Encoding os) {
    char endLine;
    if (os == Unix) {
        endLine = '\n';
    } else if (os == Mac) {
        endLine = '\r';
    }
    return endLine;

}


/**
 * isBigEndian function.
 * @param buffer - the reader char.
 * @return 1 if big endiann, 0 othewise.
 */
int isBigEndian(char *buffer) {
    int isBigEndian;
    unsigned short int r = (unsigned short int) buffer[0];
    if ((r == (unsigned short int) 0xfffe)) {
        isBigEndian = 1;
    } else if ((r = (unsigned short int) buffer[1]) == (unsigned short int) 0xfffe) {
        isBigEndian = 0;
    }
    return isBigEndian;

}

/**
 * validationOfInput function.
 * @param nameOfDstFile - Destination of file name.
 * @return 1 if there is a point in file name, 0 otherwise.
 */
int validationOfInput(char *nameOfDstFile) {
    int len = strlen(nameOfDstFile);
    int i = 0;
    for (; i < len; i++) {
        if (nameOfDstFile[i] == '.') {
            return 1;
        }
    }
    return 0;
}

/**
 * makeDstFile function - the function makes a destination file from copy of source file.
 * @param src - source file.
 * @param dst - destination file.
 * @param isSwap - if need to change endiannes
 */
void makeDstFile(char *src, char *dst, int isSwap) {
    FILE *source = fopen(src, "rb");
    //check if we can read from files.
    if (source == NULL) {
        return;
    }
    FILE *target = fopen(dst, "wb");
    if (target == NULL) {
        return;
    }
    // A buffer to start reading from
    char buffer[SIZE];
    //read each char by buffer
    while ((fread(buffer, sizeof(buffer), BULK, source) != 0)) {
        if (!isSwap) {
            fwrite(buffer, sizeof(buffer), BULK, target);
        } else {
            //swap in case there was a flag.
            char temp = buffer[0];
            buffer[0] = buffer[1];
            buffer[1] = temp;
            fwrite(buffer, sizeof(buffer), BULK, target);
        }
    }
    //close the files
    fclose(target);
    fclose(source);
}

/**
 * getEncodingFormat function.
 * @param strType - os type.
 * @return enum encoding type.
 */
enum Encoding getEncodingFormat(char *strType) {
    enum Encoding type;
    //make compare between inpute to our enum
    if (strcmp(strType, "-win") == 0) {
        type = Win;

    } else if (strcmp(strType, "-mac") == 0) {
        type = Mac;
    } else if (strcmp(strType, "-unix") == 0) {
        type = Unix;
    } else {
        //illegal param
        type = Illegal;
    }
    return type;
}

/**
 * encodingFromUnixToMac function.
 * @param srcFile - source file.
 * @param dstFile - destination file.
 * @param srcEncoding - source operation system.
 * @param dstEncoding - destination operation system.
 * @param isSwap - if need to change file's endiannes.
 */
void
encodingFromUnixToMac(char *srcFile, char *dstFile, enum Encoding srcEncoding, enum Encoding dstEncoding, int isSwap) {

    FILE *source = fopen(srcFile, "rb");
    //check if there is a source and destination files.
    if (source == NULL) {
        return;
    }
    FILE *destination = fopen(dstFile, "wb");
    if (destination == NULL) {
        return;
    }
    // Getting format
    char endLineSymbolSrc = getEndOfLineFormat(srcEncoding);
    char endLineSymboltrg = getEndOfLineFormat(dstEncoding);
    char buffer[SIZE];
    int count = 0;
    int isBig = 0;
    // as long as we can read bytes- read it to buffer
    while ((fread(buffer, sizeof(buffer), BULK, source) != 0)) {
        if (count == 0) {
            isBig = isBigEndian(buffer);
            count++;
        }
        if (!isSwap) {
            if (buffer[isBig] == endLineSymbolSrc && buffer[!isBig] == '\0') {
                buffer[isBig] = endLineSymboltrg;
                buffer[!isBig] = '\0';

            }
        } else {
            if (buffer[isBig] == endLineSymbolSrc && buffer[!isBig] == '\0') {
                buffer[!isBig] = endLineSymboltrg;
                buffer[isBig] = '\0';


            } else {
                //swap other chars of file
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1] = temp;
            }
        }
        //write the buffer to destination file
        fwrite(buffer, sizeof(buffer), BULK, destination);

    }
    // Close files.
    fclose(destination);
    fclose(source);

}

/**
 * encodingFromWin function.
 * @param srcFile - source file.
 * @param dstFile - target file.
 * @param srcEncoding - source os.
 * @param dstEncoding - target os.
 */
void encodingFromWin(char *srcFile, char *dstFile, enum Encoding srcEncoding, enum Encoding dstEncoding) {
    FILE *source = fopen(srcFile, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE *destination = fopen(dstFile, "wb");
    if (destination == NULL) {
        return;
    }
    char endLineSymbolSrc = getEndOfLineFormat(srcEncoding);
    char buffer[SIZE];
    int count = 0;
    int isBig = 0;
    // Read as long as it's not 0
    while ((fread(buffer, sizeof(buffer), BULK, source) != 0)) {
        if (count == 0) {
            isBig = isBigEndian(buffer);
            count++;
        }
        //unix or mac to win
        if (srcEncoding != Win && dstEncoding == Win) {
            //mac to win
            if (buffer[isBig] == endLineSymbolSrc) {
                buffer[isBig] = '\n';
                buffer[!isBig] = '\0';
                char tmpBuffer[SIZE];
                tmpBuffer[isBig] = '\r';
                tmpBuffer[!isBig] = '\0';
                fwrite(tmpBuffer, sizeof(tmpBuffer), BULK, destination);
            }
            fwrite(buffer, sizeof(buffer), BULK, destination);
        } else if (srcEncoding == Win && dstEncoding == Unix) {
            //win to unix
            if (buffer[isBig] == '\n' || buffer[isBig] != '\r') {
                fwrite(buffer, sizeof(buffer), BULK, destination);
            }
        } else if (srcEncoding == Win && dstEncoding == Mac) {
            //win to mac
            if (buffer[isBig] != '\n' || buffer[isBig] == '\r') {
                fwrite(buffer, sizeof(buffer), BULK, destination);
            }
        }
    }
    //close the files
    fclose(destination);
    fclose(source);
}

/**
 * isSwapBytes function.
 * @param flag - input flag keep\swap
 * @return 1 for swap, 0 to keep.
 */
int isSwapBytes(char *flag) {
    if (strcmp(flag, "-swap") == 0) {
        return 1;
    } else if (strcmp(flag, "-keep") == 0) {
        return 0;
    } else {
        return -1;
    }
}


/**
 * encodingWinSwap function.
 * @param srcFile - source file.
 * @param dstFile - target file.
 * @param srcEncoding - source os.
 * @param dstEncoding - target os
 */
void encodingWinSwap(char *srcFile, char *dstFile, enum Encoding srcEncoding, enum Encoding dstEncoding) {
    FILE *source = fopen(srcFile, "rb");
    //check if files are fine
    if (source == NULL) {
        return;
    }
    FILE *destination = fopen(dstFile, "wb");
    if (destination == NULL) {
        return;
    }
    char endLineSymbolSrc = getEndOfLineFormat(srcEncoding);
    char buffer[SIZE];
    int count = 0;
    int isBig = 0;
    while ((fread(buffer, sizeof(buffer), BULK, source) != 0)) {
        if (count == 0) {
            isBig = isBigEndian(buffer);
            count++;
        }
        //unix\mac to win
        if (srcEncoding != Win && dstEncoding == Win) {
            //mac/unix to win
            if (buffer[isBig] == endLineSymbolSrc) {
                buffer[!isBig] = '\n';
                buffer[isBig] = '\0';
                char tmpBuffer[SIZE];
                tmpBuffer[!isBig] = '\r';
                tmpBuffer[isBig] = '\0';
                fwrite(tmpBuffer, sizeof(tmpBuffer), BULK, destination);
            } else {
                //swap other chars
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1] = temp;
            }
            fwrite(buffer, sizeof(buffer), BULK, destination);
        } else if (srcEncoding == Win && dstEncoding == Unix) {
            //win to unix
            if (buffer[isBig] == '\n' || buffer[isBig] != '\r') {
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1] = temp;
                fwrite(buffer, sizeof(buffer), BULK, destination);

            }
        } else if (srcEncoding == Win && dstEncoding == Mac) {
            //win to mac
            if (buffer[isBig] != '\n' || buffer[isBig] == '\r') {
                char temp = buffer[0];
                buffer[0] = buffer[1];
                buffer[1] = temp;
                fwrite(buffer, sizeof(buffer), BULK, destination);

            }
        }
    }
    //close the files
    fclose(destination);
    fclose(source);
}

/**
 *
 * @param argc the number of arguments .
 * @param argv the given arguments, src, dst and optional flags.
 * @return there is no return value but it creates a txt file according to given arguments.
 */
int main(int argc, char *argv[]) {

    switch (argc) {
        // Case without flags at all.
        case 3:
            // Checking validation of input
            if (validationOfInput(argv[1]) == 1 && validationOfInput(argv[2]) == 1) {
                makeDstFile(argv[1], argv[2], 0);
            }
            break;
            // Case we have to convert from one os to another os
        case 5: {
            // Get the encoding os.
            enum Encoding srcEncoding = getEncodingFormat(argv[3]);
            enum Encoding dstEncoding = getEncodingFormat(argv[4]);
            // Check for validation
            if (srcEncoding == Illegal || dstEncoding == Illegal) {
                return 0;
            }
            // If both encoding are the same use regular function
            if (srcEncoding == dstEncoding) {
                makeDstFile(argv[1], argv[2], 0);
                return 0;
            }
            // If we deal with unix and mac combination
            if (srcEncoding != Win && dstEncoding != Win) {
                encodingFromUnixToMac(argv[1], argv[2], srcEncoding, dstEncoding, 0);
                // If we deal with win and (unix/mac)
            } else if (srcEncoding == Win || dstEncoding == Win) {
                encodingFromWin(argv[1], argv[2], srcEncoding, dstEncoding);
            }
            break;
        }
            // Same as case 5 but with flag swap/keep
        case 6: {
            enum Encoding srcEncoding = getEncodingFormat(argv[3]);
            enum Encoding dstEncoding = getEncodingFormat(argv[4]);
            if (srcEncoding == Illegal || dstEncoding == Illegal) {
                return 0;
            }
            // To check if we got swap or keep
            int isSwap = isSwapBytes(argv[5]);
            if (isSwap == -1) {
                return 0;
            }
            // Send the flag also.
            if (srcEncoding == dstEncoding) {
                makeDstFile(argv[1], argv[2], isSwap);
                return 0;
            }
            // Dealing with mac/unix
            if (srcEncoding != Win && dstEncoding != Win) {
                encodingFromUnixToMac(argv[1], argv[2], srcEncoding, dstEncoding, isSwap);

                // If we deal with win and (unix/mac)
            } else if (srcEncoding == Win || dstEncoding == Win) {
                if (isSwap) {
                    encodingWinSwap(argv[1], argv[2], srcEncoding, dstEncoding);

                } else {
                    encodingFromWin(argv[1], argv[2], srcEncoding, dstEncoding);

                }
            }
            break;
        }
    }
    return 0;
}