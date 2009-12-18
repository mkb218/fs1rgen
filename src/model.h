/*
 *  model.h
 *  fs1rgen
 *
 *  Created by Matthew J Kane on 12/15/09.
 *  Copyright 2009 Akamai Technologies. All rights reserved.
 *
 */
#ifndef H2P_FS1RGEN_MODEL
#define H2P_FS1RGEN_MODEL

#include <exception>
#include "ga/ga.h"
#include <list>
#include <boost/any.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

typedef boost::any ValueType;

namespace fs1rgen {
    class Value {
    public:
        Value(const Param & p, boost::any & v) : param_(p), value_(v){}
        Value(const Value & other) : param_(other.param_), value_(other.value_){};
        int mutate(float p) { param_.mutate(*this, p); }
        float compare(const Value & other) { return param_.compare(*this, other); }
        int crossover(const Value & other, Value & bro, Value & sis) { return param_.crossover(*this, other, bro, sis); }
        Value * clone() { return new (*this); }
    private:
        const Param & param_;
        boost::any value_;
    };

    typedef std::list<Value> ValueList;
    
    /* A Genome object fulfills a given model. The static methods given to 
     GALib will use the model to perform the genetic operators. */
    class Genome: public GAGenome {
    public:
        GADefineIdentity("fs1rgen_genome", 218);
        static void Initializer(GAGenome & g) { }
        static float Evaluator(GAGenome & g) { return dynamic_cast<Genome&>(g).model_->evaluate(g); }
        static int Mutator(GAGenome & g, float p)  { return dynamic_cast<Genome&>(g).model_->mutate(g, p); }
        static float Comparator(const GAGenome & g, const GAGenome & h)  { dynamic_cast<const Genome&>(g).model_->compare(g,h); }
        static int Crossover(const GAGenome & mom, const GAGenome & dad, GAGenome *bro, GAGenome *sis)  { return dynamic_cast<const Genome&>(mom).model_->spawn(mom,dad,bro,sis); }
        virtual void copy(const GAGenome & g) { model_->copy( dynamic_cast<const Genome&>(g).model_ ); };
        virtual GAGenome * clone(GAGenome::CloneMethod) { return new Genome(*this); };
        Genome() : GAGenome(Initializer, Mutator, Comparator) { crossover(Crossover); evaluator(Evaluator); }
        Genome(GAGenome & other) { copy(other); }
        Genome & operator=(const GAGenome& orig) {
            if(&orig != this) copy(orig);
            return *this;
        }
    private:
        // what should model be?
        ValueList values_;
        const Model & model_;
    };
    
    
    class Exception : public std::exception {
    };
    
    class Evaluator { // interace for evaluator
    public:
        static Evaluator * getInstance();
        static void setInstance(Evaluator *);
        virtual float evaluate(GAGenome &) = 0;
        void sendToSynth() throw (Exception);
    private:
        Evaluator() {}; // prevent instantiation
//        static lock_t singletonLock_;
        static Evaluator * instance_;
    };
        
}  

#endif