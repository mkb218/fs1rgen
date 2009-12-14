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
    pop.initialize();
    GASimpleGA ga(pop);
    ga.elitist(gaTrue);
    while (!ga.done()) {
        ga.step();
        std::cout << "Generation: " << ga.nGenerations();
        std::cout << " with " << ga.population().size() << std::endl;
        for (unsigned int i = 0; i < ga.population().size(); ++i) {
            std::cout << "Individual " << i << " with checksum " << ((Genome &)(ga.population().individual(i))).checksum() << std::endl;
        }
    }
    NSLog(@"winnar!ga.statistics().bestIndividual()");
}
                     
                     

@end
