#include <iostream>
#include <getopt.h>

// args
// --samplerate - in hz
// --samplesize - in bytes
// --freq - in hz, what note to aim for. a440 by default

int main (int argc, char * const argv[]) {
    int samplerate = 44100;
    int samplesize = 2;
    int freq = 440;
    
    option options[] = { {"samplerate", required_argument, NULL, 'r' },
        {"samplesize", required_argument, NULL, 's' },
        {"freq", required_argument, NULL, 'f' },
        {NULL, 0, NULL, 0 } };
    char ch;
    int longindex;
    while (ch = getopt_long_only(argc, argv, "rsf:", options, &longindex) != -1) {
        switch (ch) {
        case 'r':
            samplerate = 
    return 0;
}
