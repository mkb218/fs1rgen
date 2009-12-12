/*
 *  genome.h
 *  tonegen
 *
 *  Created by Kane, Matthew on 12/8/09.
 *
 */
#ifndef H2PTONEGEN_GENOME
#define H2PTONEGEN_GENOME

#include <iostream>
#include <vector>
#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>
#include <string>

#include "ga/ga.h"

namespace h2p_tonegen {
    class Genome : public GAGenome {
    public:
        GADefineIdentity("tonegen_genome", 1020);
        static void Initializer(GAGenome&);
        static int Mutator(GAGenome&, float);
        static float Comparator(const GAGenome&, const GAGenome&);
        static float Evaluator(GAGenome&);
        static int Crossover(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);
    public:
        Genome(int samplerate = 44100, int samplesize = 2, int freq = 220);
        Genome(Genome &);
        Genome& operator=(const GAGenome &);
        void toFile(std::string filename);
        virtual void copy(const GAGenome &);
        virtual int equal(const GAGenome &) const;
        virtual GAGenome * clone(GAGenome::CloneMethod);
        virtual ~Genome();
    private:
        static OSStatus playSample(void 				*inRefCon, 
                                AudioUnitRenderActionFlags 	*ioActionFlags, 
                                const AudioTimeStamp 		*inTimeStamp, 
                                UInt32 						inBusNumber, 
                                UInt32 						inNumberFrames, 
                                AudioBufferList 			*ioData);
        static void playTheSample(Genome&);
        static void makeChild(const Genome&, const Genome&, Genome&);
        int samplerate_;
        int samplesize_;
        int freq_;
        std::vector<unsigned char> wave_;
    };
}

#endif // H2PTONEGEN_GENOME