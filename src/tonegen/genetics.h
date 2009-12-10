@interface Genetics : NSObject {
    NSNumber *samplerate;
    NSNumber *samplesize;
    NSNumber *freq;
}

-(void)initWithSamplerate:(NSNumber*)newrate samplesize: (NSNumber*)newsize freq: (NSNumber*)newfreq;
-(void)setSamplerate:(NSNumber*)newrate;
-(void)setSampleSize:(NSNumber*)newsize;
-(void)setFreq:(NSNumber*)newfreq;
@end