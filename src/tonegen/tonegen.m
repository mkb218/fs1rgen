#import <Foundation/Foundation.h>
#import "ToneGenApp.h"
#include <getopt.h>

int main (int argc, const char *argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    NSNumber *sampleRate = [NSNumber numberWithInteger: 44100];
    NSNumber *sampleSize = [NSNumber numberWithInteger: 2];
    NSNumber *freq = [NSNumber numberWithInteger: 440];
    
    struct option options[] = { { "samplerate", required_argument, NULL, 'r' },
        { "samplesize", required_argument, NULL, 's'},
        { "freq", required_argument, NULL, 'f'},
        { NULL, 0, NULL, 0} };
    char ch;
    while (ch = getopt_long(argc, (char * const *)argv, "rsf:", options, NULL) != -1) {
        switch (ch) {
        case 'r':
            [sampleRate release];
            sampleRate = [NSNumber numberWithInteger: [[NSString stringWithCString: optarg encoding: 4] intValue]];
            break;
        case 's':
            [sampleSize release];
            sampleSize = [NSNumber numberWithInteger: [[NSString stringWithCString: optarg encoding: 4] intValue]];
            break;
        case 'f':
            [freq release];
            freq = [NSNumber numberWithInteger: [[NSString stringWithCString: optarg encoding: 4] intValue]];
            break;
        default:
            NSLog(@"Unrecognized option, jerkface!");
        }
    }
    
    ToneGenApp *tg = [[ToneGenApp alloc] init];
    [tg go];
    
    [pool drain];
    return 0;
}
