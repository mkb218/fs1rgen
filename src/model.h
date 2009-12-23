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
#include <stdexcept>
#include "ga/ga.h"
#include <list>
#include <vector>
#include <boost/any.hpp>
#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <memory>
#include <iostream>

#define foreach BOOST_FOREACH

namespace fs1rgen {
    class Value;
    class Genome;
    class Param;
    
    typedef boost::any ValueData;
    typedef std::vector<char> SynthData;
    typedef std::auto_ptr<SynthData> SynthDataPtr;
    typedef std::list<Value> ValueList;
    typedef std::list<boost::reference_wrapper<Param> > ParamList;
    
    class Param {
    public:
        Param(const std::string & name) : name_(name), forInterface_(false) {}
        const std::string & name() const { return name_; }
        virtual Value * newValue() const = 0;
        virtual int mutate(Value &, float) const = 0;
        virtual float compare(const Value &, const Value&) const = 0;
        virtual int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const = 0;
        virtual const SynthDataPtr toBinary(const Value &) const = 0;
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
        Value & operator=(const Value & other) throw (std::logic_error);
        int mutate(float p) { if (!locked_) { return param_.mutate(*this, p); } else { return 0; } }
        float compare(const Value & other) const { return param_.compare(*this, other); }
        int crossover(const Value & other, Value * bro, Value * sis) const { if (!locked_) return param_.crossover(*this, other, bro, sis); else return lockedCrossover(bro, sis); }
        Value * clone() const { return new Value(*this); }
        const Param & param() { return param_; }
        const ValueData & value() { return value_; }
        void lock() { locked_ = true; }
        void unlock() { locked_ = false; }
        int lockedCrossover(Value * bro, Value * sis) const;
    private:
        const Param & param_;
        ValueData value_;
        bool locked_;
    };
    
    class Model {
    public:
        Model(const Model & other) : params_(other.params_) {}
        Model(const ParamList & params) : params_(params) {}
        void initialize(Genome &) const;
    private:
        ParamList params_;
    };
    
    class Evaluator { // interface for evaluator
        // this is an abstract base class because we may be running in either a command-line environment
        // or a graphical one
    public:
        static const Evaluator * getInstance() { return instance_; }
        static void setInstance(Evaluator * i) { instance_ = i; }
        virtual float evaluate(ValueList &) const throw (std::runtime_error) = 0;
    private:
        void sendToSynth() throw (std::runtime_error);
        Evaluator() {}; // prevent instantiation
        //        static lock_t singletonLock_;
        static Evaluator * instance_;
    };
    
    /* A Genome object fulfills a given model. The static methods given to 
     GALib will use the model to perform the genetic operators. */
    class Genome : public GAGenome {
    public:
        GADefineIdentity("fs1rgen_genome", 218);
        friend class Model;
        static void Initializer(GAGenome & g) { dynamic_cast<Genome&>(g).initialize(); }
        void initialize() {
            model_.initialize(*this);
        }
        static float Objective(GAGenome & g) { return dynamic_cast<Genome &>(g).evaluate(); }
        float evaluate();
        static int Mutator(GAGenome & g, float p)  { return dynamic_cast<Genome&>(g).mutate(p); }
        int mutate(float p) {
            int count = 0;
            foreach(Value & v, values_) {
                count += v.mutate(p);
            }
            return count;
        }
        static float Comparator(const GAGenome & g, const GAGenome & h)  { return dynamic_cast<const Genome&>(g).compare(dynamic_cast<const Genome&>(h)); }
        float compare(const Genome & other) const {
            float diff = 0.0;
            size_t i = 0;
            ValueList::const_iterator it = other.values_.begin();
            foreach(const Value & v, values_) {
                diff += v.compare(*it);
                ++i;
                ++it;
            }
            return diff;
        }
        static int Crossover(const GAGenome & mom, const GAGenome & dad, GAGenome *bro, GAGenome *sis)  { return dynamic_cast<const Genome&>(mom).spawn(dynamic_cast<const Genome &>(dad),bro,sis); }
        int spawn(const Genome & dad, GAGenome *bro, GAGenome *sis) const;
        const ValueList & values() const { return values_; }
        virtual void copy(const GAGenome & g) { values_ = dynamic_cast<const Genome &>(g).values_; }
        virtual GAGenome * clone(GAGenome::CloneMethod) { return new Genome(*this); };
        Genome(const Model & model) : GAGenome(Initializer, Mutator, Comparator), model_(model) { crossover(Crossover); evaluator(Objective); }
        Genome(GAGenome & other) :model_(dynamic_cast<Genome&>(other).model_){ copy(other); }
        Genome & operator=(const GAGenome& orig) {
            if(&orig != this) copy(orig);
            return *this;
        }
    private:
        ValueList values_;
//        std::list<boost::ref<Value &> > what was this supposed to be?
        const Model & model_;
    };
    
    
}  

#endif