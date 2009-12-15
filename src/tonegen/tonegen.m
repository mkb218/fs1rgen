#import <Foundation/Foundation.h>
#import "ToneGenApp.h"
#include <getopt.h>

int main (int argc, const char *argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int sampleRate = 44100;
    int sampleSize = 2;
    int freq = 110;

    struct option options[] = { { "samplerate", required_argument, NULL, 'r' },
        { "samplesize", required_argument, NULL, 's'},
        { "freq", required_argument, NULL, 'f'},
        { NULL, 0, NULL, 0} };
    char ch;
    while (ch = getopt_long(argc, (char * const *)argv, "rsf:", options, NULL) != -1) {
        switch (ch) {
        case 'r':
            sampleRate = [[NSString stringWithCString: optarg encoding: 4] intValue];
            break;
        case 's':
            sampleSize = [[NSString stringWithCString: optarg encoding: 4] intValue];
            break;
        case 'f':
            freq = [[NSString stringWithCString: optarg encoding: 4] intValue];
            break;
        default:
            NSLog(@"Unrecognized option, jerkface!");
        }
    }
    
    [[[ToneGenApp alloc] init] goWithSampleRate: sampleRate sampleSize: sampleSize freq: freq outputfile: @"out"];
    
    [pool drain];
    return 0;
}
