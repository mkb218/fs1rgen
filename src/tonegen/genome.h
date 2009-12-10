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

#include "ga/ga.h"

namespace h2p_tonegen {
    class Genome : public GAGenome {
    public:
        GADefineIdentity("tonegen_genome", 1020);
        static void Initializer(GAGenome&);
        static int Mutator(GAGenome&, float);
        static float Comparator(const GAGenome&, const GAGenome&);
        static float Evaluator(GAGenome&);
        static void PathInitializer(GAGenome&);
    public:
        Genome(int samplerate = 44100, int samplesize = 2, int freq = 440);
        Genome(Genome &);
        Genome& operator=(const GAGenome &);
        virtual void copy(const GAGenome &);
        virtual GAGenome * clone(GAGenome::CloneMethod);
        virtual ~Genome();
    private:
        int samplerate_;
        int samplesize_;
        int freq_;
        std::vector<unsigned char> wave_;
    };
}

#endif // H2PTONEGEN_GENOME