#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

char *file;
char ** partfiles;
int debug;
int filesize;
unsigned char hamming (unsigned char nibble);
int hwrite(char* file);

int main (int argc, char *argv[]) { // driver code with getopt switch case
    int c;
	file = NULL;
	while ((c = getopt(argc, argv, "f:d")) != -1) { // command line getopt behaviors
		switch (c) {
			case 'd': // debug flag
				debug = 1;
				break;
			case 'f': // filename
				file = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(file, optarg);
				break;
			default:
				break;
		}
	}
	if (file == NULL) { // default input file behavior
        printf("No file specified, default file is 'completeShakespeare.txt'\n");
		file = (char*) malloc(sizeof(char) * (strlen("completeShakespeare.txt") + 1));
		strcpy(file,"completeShakespeare.txt");
	}
    hwrite(file);
    printf("%s: %d B\n", file, filesize);
}

int hwrite (char* file) { // read/write/buffer management method
    FILE *fmain;
    FILE *parts[7];
    unsigned char temp;
    unsigned char buf[7]; // buffers!
    for (int i = 0; i < 7; i++) buf[i] = 0; // buffer init to 0

    char **partfiles = malloc(7 * sizeof(char*));
    for (int i = 0; i < 7; i++) { // this sets up an array of partfiles using the provided syntax and sprintf with a double pointer array
        partfiles[i] = (char*) malloc(sizeof(char) * (strlen(file) + 7));
        sprintf(partfiles[i], "%s.part%d", file, i);
        parts[i] = fopen(partfiles[i], "wb");
    }
    
    fmain = fopen(file, "rb");
    int bitcount = 0;
    while (!feof(fmain)) { // while not end of input file
    
        if (fread(&temp, sizeof(char), 1, fmain) != 1) { // read one byte unless eof, then don't read it
        break; }

        unsigned char rnibble = (temp & 0x0F); // 0x0F is 00001111 in binary, separates right nibble
        unsigned char lnibble = (temp >> 4); // shift the right nibble off, now we have the left nibble
        lnibble = hamming(lnibble); // retrieve hamming codes of nibbles
        rnibble = hamming(rnibble); 

        for (int i = 0; i < 7; i++) {
            buf[i] = (buf[i] << 1); // left nibble management
            buf[i] = buf[i] | ((lnibble >> (6 - i)) & 1);

            buf[i] = (buf[i] << 1); // right nibble management
            buf[i] = buf[i] | ((rnibble >> (6 - i)) & 1);
        }
        bitcount += 2; // we're adding 2 bits to the buffers on each iteration, one for each nibble
        if (bitcount == 8) { // once we have a full byte in the buffer, write to the corresponding part file and reset it
            for (int i = 0; i < 7; i++) {
                fwrite(&buf[i], sizeof(unsigned char), 1, parts[i]);
            }
            bitcount = 0;
            filesize += 4;
        }
    }
    return 0;
}

unsigned char hamming (unsigned char nibble) { //method to convert nibbles into their hamming code equivalents
    unsigned char hamming = 0;

    unsigned char D1 = (nibble >> 3) & 1; // data bits into single bit chars
    unsigned char D2 = (nibble >> 2) & 1;
    unsigned char D3 = (nibble >> 1) & 1;
    unsigned char D4 = nibble & 1;

    unsigned char P1 = D1 ^ D2 ^ D4; // parity/error detection bits into single bit chars
    unsigned char P2 = D1 ^ D3 ^ D4;
    unsigned char P3 = D2 ^ D3 ^ D4;

    hamming = (P1 << 6) | // extended or process to move each bit into the new code
              (P2 << 5) |
              (D1 << 4) |
              (P3 << 3) |
              (D2 << 2) |
              (D3 << 1) |
              (D4); // awesome redundant parenthesis

    return hamming;
}
