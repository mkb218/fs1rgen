/*
 *  genome.cpp
 *  tonegen
 *
 *  Created by Kane, Matthew on 12/8/09.
 *  Copyright 2009 Akamai Technologies. All rights reserved.
 *
 */

#include "genome.h"
#include "ga/ga.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace h2p_tonegen;

static unsigned char randombyte() {
    static bool srandcalled = false;
    if (!srandcalled) {
        srandom(time(NULL));
    }
    return random() & 0xff;
}

void Genome::Initializer(GAGenome& genome) {
    try {
        Genome & target = dynamic_cast<Genome&> (genome);
        target.wave_.clear();
        size_t bytes = target.samplerate_ * target.samplesize_ / target.freq_;
        target.wave_.reserve(bytes);
        for (size_t i = 0; i < bytes; ++i) {
            target.wave_[i] = randombyte();
        }
    } catch (std::bad_cast & e) {
        std::cerr << "initializer was passed a non tonegen genome" << std::endl;
    }
}

int Genome::Mutator(GAGenome& genome, float p) {
    unsigned char count;
    Genome & target = dynamic_cast<Genome&> (genome);
    for (std::vector<unsigned char>::iterator it = target.wave_.begin(); it < target.wave_.end(); ++it) {
        unsigned char flipper = 0;
        for (int i = 0; i < 8; ++i) {
            flipper <<= 1;
            if (randombyte() < 256*p) {
                flipper |= 1;
                count += 1;
            }
        }
        *it ^= flipper;
    }
    
    return count;
}

float Genome::Comparator(const GAGenome& left, const GAGenome& right) {
    const Genome & cast_left = dynamic_cast<const Genome&>(left);
    const Genome & cast_right = dynamic_cast<const Genome&>(right);
    
    double diff = 0.0;
    double size = static_cast<double>(cast_left.wave_.size());
    for (std::vector<unsigned char>::const_iterator itl = cast_left.wave_.begin(), itr = cast_right.wave_.begin();
         itl < cast_left.wave_.end() && itr < cast_right.wave_.end(); ++itl, ++itr) {
        diff += abs(*itl - *itr);
    }
    
    return static_cast<float>(diff / size);
}

float Genome::Evaluator(GAGenome& target) {
    Genome & genome = dynamic_cast<Genome&> (target);
    AudioUnit outputUnit;
    OSStatus err = noErr;
    
    ComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    Component comp = FindNextComponent(NULL, &desc);
    if (comp == NULL) { 
        // need better way to die. maybe throw
        std::cerr << "FindNextComponent" << std::endl; return;
    }
    
    err = OpenAComponent(comp, &gOutputUnit);
    if (comp == NULL) {
        // need better way to die. maybe throw
        std::cerr << "OpenAComponent=" << static_cast<long int>(err) << std::endl; return 0.0;
    }
    
    // Set up a callback function to generate output to the output unit
    AURenderCallbackStruct input;
    input.inputProc = playSample;
    input.inputProcRefCon = NULL;
    
    err = AudioUnitSetProperty (gOutputUnit, 
                                kAudioUnitProperty_SetRenderCallback, 
                                kAudioUnitScope_Input,
                                0, 
                                &input, 
                                sizeof(input));
    if (err) { printf ("AudioUnitSetProperty-CB=%ld\n", (long int)err); return; }
    std::cout << "
    }

Genome::Genome(int samplerate, int samplesize, int freq) {
}