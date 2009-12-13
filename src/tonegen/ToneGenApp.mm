//
//  ToneGenApp.m
//  tonegen
//
//  Created by Kane, Matthew on 12/7/09.
//

#import "ToneGenApp.h"
#include "ga/ga.h"
#include "genome.h"

using namespace h2p_tonegen;
@implementation ToneGenApp

-(void)goWithSampleRate: (int)sampleRate sampleSize: (int)sampleSize freq: (int)freq outputfile: (NSString*) filename {
    Genome genome(sampleRate, sampleSize, freq);
    GAPopulation pop(genome, 10);
    GASteadyStateGA ga(pop);
    ga.evolve();
    NSLog(@"winnar!ga.statistics().bestIndividual()");
}
                     
                     

@end
