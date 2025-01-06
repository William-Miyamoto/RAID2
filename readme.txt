TO RUN RAID 2 ENCODER:
Compile using make. Run raid using syntax "./raid -f {filename}", where {filename} is the complete name
of the file to be encoded.

TO RUN RAID 2 DECODER (diar):
Compile using make (if not already made). Run decoder using syntax "./diar -f {filename} -s {size}", where 
{filename} is the ORIGINAL name of the file to be built, and {size} is the size of the original file in bytes.

EXAMPLES:
./raid -f test.txt
./diar -f test.txt -s 4
./raid -f completeShakespeare.txt
./diar -f completeShakespeare.txt -s 5694072

TO ENABLE DEBUGGING:
Simply add -d as a flag anywhere in the command line input (though not between another flag and subsequent input).

EXAMPLES:
./raid -f test.txt -d
./raid -d -f test.txt
./diar -d -f test.txt -s 4

INVALID EXAMPLES:
./raid -f -d test.txt
./diar -f test.txt -s -d 4

####################################################################################################

Raid.c encodes text files into several part files using hamming (7, 4) codes. It supports a debugging
flag and a filename input, both specified on command line input.

Diar.c assembles 7 part files using hamming (7, 4) codes, rebuilding the original input file from which
they are derived. It supports a debugging flag, a filename input, and a file size input. It is important
to note that without pre-existing part files the program will (obviously) fail, and the size of the original
file in bytes MUST BE SPECIFIED. 
It also supports 1 bit error detection and correction, utilising parity bits of individual hamming codes to
flip an incorrect bit via bitwise operations – this can be either a data bit or a parity bit. It is important
to note, though, that while traditional RAID 2 can detect 2 bit errors, this program does not have the capability
to do so. Any more than 1 corrupt part file will disable the program.

