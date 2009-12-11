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
#include <cstdio>
using namespace h2p_tonegen;

static AudioUnit outputUnit;
static bool isOutputUnitReady = false;
static Genome *currentlyPlayingGenome = NULL;

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
    float score = -1.0;
    do {
        OSStatus err = noErr;
        if (!isOutputUnitReady) {
            
            ComponentDescription desc;
            desc.componentType = kAudioUnitType_Output;
            desc.componentSubType = kAudioUnitSubType_DefaultOutput;
            desc.componentManufacturer = kAudioUnitManufacturer_Apple;
            desc.componentFlags = 0;
            desc.componentFlagsMask = 0;
            
            Component comp = FindNextComponent(NULL, &desc);
            if (comp == NULL) { 
                // need better way to die. maybe throw
                std::cerr << "FindNextComponent" << std::endl; return 0.0;
            }
            
            err = OpenAComponent(comp, &outputUnit);
            if (comp == NULL) {
                // need better way to die. maybe throw
                std::cerr << "OpenAComponent=" << static_cast<long int>(err) << std::endl; return 0.0;
            }
            
            // Set up a callback function to generate output to the output unit
            AURenderCallbackStruct input;
            input.inputProc = playSample;
            input.inputProcRefCon = NULL;
            
            err = AudioUnitSetProperty (outputUnit, 
                                        kAudioUnitProperty_SetRenderCallback, 
                                        kAudioUnitScope_Input,
                                        0, 
                                        &input, 
                                        sizeof(input));
            if (err) { std::cerr << "AudioUnitSetProperty-CB=" << err << std::endl; return 0.0; }
        }
        playTheSample(genome);
        err = AudioUnitReset(outputUnit, kAudioUnitScope_Input, 0);
        if (err) { std::cerr << "AudioUnitReset returned " << err << std::endl; return 0.0; }

        std::cout << "Rate what you just heard from 0 to 10, or -1 to listen again:" << std::flush;
        std::cin >> score;
    } while (score >= 0.0 and score <= 10.0);
    
    return score;
}

OSStatus	Genome::playSample(void 				*inRefCon, 
                       AudioUnitRenderActionFlags 	*ioActionFlags, 
                       const AudioTimeStamp 		*inTimeStamp, 
                       UInt32 						inBusNumber, 
                       UInt32 						inNumberFrames, 
                       AudioBufferList 			*ioData)
{
    static Genome *lastGenome = NULL;
    static size_t lastIndex = 0;
    size_t maxSize = currentlyPlayingGenome->wave_.size();
    if (lastGenome != currentlyPlayingGenome) {
        lastGenome = currentlyPlayingGenome;
        lastIndex = 0;
    }
    
    for (UInt32 frame = 0; frame < inNumberFrames; ++frame) {
        for (size_t byte = 0; byte < currentlyPlayingGenome->samplesize_; ++byte) {
            static_cast<unsigned char *>(ioData->mBuffers[0].mData)[frame*currentlyPlayingGenome->samplesize_+byte] = currentlyPlayingGenome->wave_[lastIndex+byte];
        }
        if (++lastIndex <= maxSize) {
            lastIndex = 0;
        }
    }
    
    return noErr;
}


void Genome::playTheSample(Genome & genome) {
    currentlyPlayingGenome = &genome;
	OSStatus err = noErr;
    
	// We tell the Output Unit what format we're going to supply data to it
	// this is necessary if you're providing data through an input callback
	// AND you want the DefaultOutputUnit to do any format conversions
	// necessary from your format to the device's format.
	AudioStreamBasicDescription streamFormat;
    streamFormat.mSampleRate = genome.samplerate_;		//	the sample rate of the audio stream
    streamFormat.mFormatID = kAudioFormatLinearPCM;			//	the specific encoding type of audio stream
    streamFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;		//	flags specific to each format
    streamFormat.mBytesPerPacket = genome.samplesize_;	
    streamFormat.mFramesPerPacket = 1;	
    streamFormat.mBytesPerFrame = genome.samplesize_;
    streamFormat.mChannelsPerFrame = 1;	
    streamFormat.mBitsPerChannel = genome.samplesize_ * 8;	
    
	err = AudioUnitSetProperty (outputUnit,
                                kAudioUnitProperty_StreamFormat,
                                kAudioUnitScope_Input,
                                0,
                                &streamFormat,
                                sizeof(AudioStreamBasicDescription));
	if (err) { std::cerr << "AudioUnitSetProperty-SF=" << (char*)&err << (long int)err << std::endl; return; }
	
    // Initialize unit
	err = AudioUnitInitialize(outputUnit);
	if (err) { std::cerr <<  "AudioUnitInitialize=" << (long int)err << std::endl; return; }
    
	Float64 outSampleRate;
	UInt32 size = sizeof(Float64);
	err = AudioUnitGetProperty (outputUnit,
                                kAudioUnitProperty_SampleRate,
                                kAudioUnitScope_Output,
                                0,
                                &outSampleRate,
                                &size);
	if (err) { printf ("AudioUnitSetProperty-GF=%4.4s, %ld\n", (char*)&err, (long int)err); return; }
    
	// Start the rendering
	// The DefaultOutputUnit will do any format conversions to the format of the default device
	err = AudioOutputUnitStart (outputUnit);
	if (err) { printf ("AudioOutputUnitStart=%ld\n", (long int)err); return; }
    
    // we call the CFRunLoopRunInMode to service any notifications that the audio
    // system has to deal with
	CFRunLoopRunInMode(kCFRunLoopDefaultMode, 2, false);
    
    // REALLY after you're finished playing STOP THE AUDIO OUTPUT UNIT!!!!!!	
    // but we never get here because we're running until the process is nuked...	
	verify_noerr (AudioOutputUnitStop (outputUnit));
	
    err = AudioUnitUninitialize (outputUnit);
	if (err) { printf ("AudioUnitUninitialize=%ld\n", (long int)err); return; }
}   

Genome::Genome(int samplerate, int samplesize, int freq) {
    samplerate_ = samplerate;
    samplesize_ = samplesize;
    freq_ = freq;
}

Genome::Genome(Genome& other) {
    samplerate_ = other.samplerate_;
    samplesize_ = other.samplesize_;
    freq_ = other.freq_;
    wave_ = other.wave_;
}

Genome& Genome::operator=(const GAGenome & other) {
    copy(other);
    return *this;
}
 
void Genome::copy(const GAGenome & other) {
    const Genome & src = dynamic_cast<const Genome&>(other);
    samplerate_ = src.samplerate_;
    samplesize_ = src.samplesize_;
    freq_ = src.freq_;
    wave_ = src.wave_;
}

Genome::~Genome() {
    // jeffries tube - GNDN
}
