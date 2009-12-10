#import "genetics.h"

@implementation Genetics



-(void)setSamplerate:(NSNumber*)newrate {
    [samplerate release];
    [newrate retain];
    samplerate = newrate;
}

-(void)setSampleSize:(NSNumber*)newsize {
    [samplesize release];
    [newsize retain];
    samplesize = newsize;
}

-(void)setFreq:(NSNumber*)newfreq {
    [freq release];
    [newfreq retain];
    freq = newfreq;
}

@end