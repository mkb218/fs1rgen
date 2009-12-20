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
#include <boost/ref.hpp>
#include <memory>

#define foreach BOOST_FOREACH

typedef boost::any ValueType;
typedef std::auto_ptr<vector<char> > SynthData;

namespace fs1rgen {
    class Param {
    public:
        Param(const std::string & name) : name_(name), forInterface_(false) {}
        const std::string & name() const { return name_; }
        virtual Value * newValue() const = 0;
        virtual int mutate(Value &, float) const = 0;
        virtual float compare(const Value &, const Value&) const = 0;
        virtual int crossover(const Value & other, Value & bro, Value & sis) const = 0;
        virtual const SynthData toBinary(const Value &) const = 0;
        virtual bool IsForInterface() const { return forInterface_; }
        void forInterface(bool valid) { forInterface_ = valid; }
    protected:
        bool forInterface_;
        std::string name_;
    };
    
    class Value {
    public:
        Value(const Param & p, boost::any & v) : param_(p), value_(v), locked_(false) {}
        Value(const Value & other) : param_(other.param_), value_(other.value_), locked_(other.locked_) {};
        int mutate(float p) { if (!locked_) { return param_.mutate(*this, p); } else { return 0; } }
        float compare(const Value & other) const { return param_.compare(*this, other); }
        int crossover(const Value & other, Value & bro, Value & sis) const { if (!locked) return param_.crossover(*this, other, bro, sis) else return lockedCrossover; }
        Value * clone() const { return new (*this); }
        const Param & param() { return param_; }
        const boost::any & value() { return value_; }
        void lock() { locked_ = true; }
    private:
        const Param & param_;
        boost::any value_;
        bool locked_;
    };

    typedef std::list<Value> ValueList;
    typedef std::list<Param> ParamList;
    
    class Model {
    public:
        Model(const Model & other) : params_(other.params_) {}
        Model(const ParamList & params) : params_(params) {}
        void initialize(Genome &) const;
    private:
        ParamList params_;
    };
    
    /* A Genome object fulfills a given model. The static methods given to 
     GALib will use the model to perform the genetic operators. */
    class Genome: public GAGenome {
    public:
        GADefineIdentity("fs1rgen_genome", 218);
        friend class Model;
        static void Initializer(GAGenome & g) { dynamic_cast<Genome&>(g).initialize(); }
        void initialize() {
            model_.initialize(*this);
        }
        static float Evaluator(GAGenome & g) { dynamic_cast<Genome&>(g).evaluate(); }
        float evaluate() {
            Evaluator *eval = Evaluator::getInstance();
            _evaluated = gaFalse;
            assert (eval != NULL); // this means programmer error
            float score = 0.0;
            try {
                score = eval->evaluate(*this);
                _evaluated = gaTrue;
            } catch (Exception & e) {
                // TODO: something notification-like with this exception
                score = -1;
            }
            return score;
        }
        static int Mutator(GAGenome & g, float p)  { return dynamic_cast<Genome&>(g).mutate(p); }
        int mutate(float p) {
            int count = 0;
            foreach(Value & v, values_) {
                count += v.mutate(p);
            }
            return count;
        }
        static float Comparator(const GAGenome & g, const GAGenome & h)  { dynamic_cast<const Genome&>(g).compare(h); }
        float compare(const Genome & other) const {
            float diff = 0.0;
            foreach(Value & v, values_) {
                diff += v.compare(other);
            }
            return diff;
        }
        static int Crossover(const GAGenome & mom, const GAGenome & dad, GAGenome *bro, GAGenome *sis)  { return dynamic_cast<const Genome&>(mom).spawn(dynamic_cast<const Genome &>(dad),bro,sis); }
        int spawn(const Genome & dad, GAGenome *bro, GAGenome *sis);
        const ValueList & values() { return values_; }
        virtual void copy(const GAGenome & g) { GAGenomevalues_ = other.values_; }
        virtual GAGenome * clone(GAGenome::CloneMethod) { return new Genome(*this); };
        Genome(const Model & model) : GAGenome(Initializer, Mutator, Comparator), model_(model) { crossover(Crossover); evaluator(Evaluator); }
        Genome(GAGenome & other) { copy(other); }
        Genome & operator=(const GAGenome& orig) {
            if(&orig != this) copy(orig);
            return *this;
        }
    private:
        ValueList values_;
        std::list<boost::ref<Value &>>
        const Model & model_;
    };
    
    class Exception : public std::exception {
    };
    
    class Evaluator { // interface for evaluator
        // this is an abstract base class because we may be running in either a command-line environment
        // or a graphical one
    public:
        static Evaluator * getInstance();
        static void setInstance(Evaluator *);
        virtual float evaluate(ValueList &) throw (Exception) = 0;
    private:
        void sendToSynth() throw (Exception);
        Evaluator() {}; // prevent instantiation
//        static lock_t singletonLock_;
        static Evaluator * instance_;
    };
    
}  

#endif