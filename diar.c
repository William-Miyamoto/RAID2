// Hamming code project by William Miyamoto (CS444-01, 2024) //
// Part 2: RAID 2 decoding method supporting 1 bit error correction //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

int numBytes;
char *file;
unsigned char hfix(unsigned char hamming);
int hbuild(char *file);
int debug;

int main (int argc, char *argv[]) { // driver code with getopt switch case
    int c;
	file = NULL;
	while ((c = getopt(argc, argv, "f:s:d")) != -1) { // command line getopt behaviors
		switch (c) {
			case 'd':
				debug = 1;
				break;
			case 'f': // filename
				file = (char*) malloc(sizeof(char) * (strlen(optarg) + 1));
				strcpy(file, optarg);
				break;
			case 's': // filesize
				numBytes = atoi(optarg);
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
	printf("%s: %d B\n", file, numBytes);
    hbuild(file);
	
}

int hbuild(char *file) { // build file method
    FILE *fmain; // file to be reconstructed
    FILE *parts[7]; // partfile array
	unsigned char buf[8]; // buffers in the opposite direction; each buffer holds a hamming code of a nibble and must be constructed by bit
	int cb = 0; // current byte count
	for (int i = 0; i < 8; i++) buf[i] = 0;

	char **partfiles = malloc(7 * sizeof(char*));
	for (int i = 0; i < 7; i++) { // this sets up an array of partfiles using the provided syntax and sprintf with a double pointer array
        partfiles[i] = (char*) malloc(sizeof(char) * (strlen(file) + 7));
        sprintf(partfiles[i], "%s.part%d", file, i);
        parts[i] = fopen(partfiles[i], "rb");
    }
	file = strcat(file, ".2");
	fmain = fopen(file, "wb");
	unsigned char temp;

	while (cb < numBytes) { // while not eof for last partfile
		for (int i = 0; i < 7; i++) { // fill the buffers with hamming codes from 1 byte of each part file
			if (fread(&temp, sizeof(char), 1, parts[i]) != 1) break; 
			for (int i = 0; i < 8; i++) {
				buf[i] = buf[i] << 1;
				buf[i] = buf[i] | ((temp >> (7 - i)) & 1);
			}
		} // at this point, each of the buffer values should hold one hamming code for each of the 8 nibbles
		for (int i = 0; i < 8; i += 2) {
			buf[i] = hfix(buf[i]); // fix 1 bit errors; return original nibble
			buf[i + 1] = hfix(buf[i + 1]);
			unsigned char tempbyte = ((buf[i] << 4) | buf[i + 1]);
			fwrite(&tempbyte, sizeof(unsigned char), 1, fmain);
		}
		cb += 4; // increment bytes processed by 4 (we have processed 8 nibbles, or 4 bytes)
	}
	return 0;
}

unsigned char hfix(unsigned char hamming) {
	int afo = 0; // address failure offset
	unsigned char P1c, P2c, P3c; // CORRECT hamming code bits recalculated via xor
	
	unsigned char D1 = (hamming >> 4) & 1; // data bits into single bit chars; d1 is 4 bits over this time
    unsigned char D2 = (hamming >> 2) & 1;
    unsigned char D3 = (hamming >> 1) & 1;
    unsigned char D4 = hamming & 1;

	unsigned char P1 = (hamming >> 6) & 1;
	unsigned char P2 = (hamming >> 5) & 1;
	unsigned char P3 = (hamming >> 3) & 1;

	P1c = D1 ^ D2 ^ D4; // recalculate correct parity bits
    P2c = D1 ^ D3 ^ D4;
    P3c = D2 ^ D3 ^ D4;

	if ((P1c ^ P1) == 1) afo += 1; // calculate failure offset
	if ((P2c ^ P2) == 1) afo += 2;
	if ((P3c ^ P3) == 1) afo += 4;

	if (afo > 0) { // if there was an address failure...
		hamming = hamming ^ (0x01 << (7 - afo));
	}

	D1 = (hamming >> 4) & 1; // data bits into single bit chars; d1 is 4 bits over this time
    D2 = (hamming >> 2) & 1;
    D3 = (hamming >> 1) & 1;
    D4 = hamming & 1;

	return  (D1 << 3) | // we return the original nibble
			(D2 << 2) |
            (D3 << 1) |
            (D4);
}