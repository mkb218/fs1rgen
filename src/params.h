/*
 *  params.h
 *  fs1rgen
 *
 *  Created by Matthew J Kane on 12/18/09.
 *
 */

#ifndef FS1RGEN_PARAMS
#define FS1RGEN_PARAMS

#include "model.h"
#include <vector>
#include <string>
#include <map>

namespace fs1rgen {
    class StringParam : public Param{
    public:
        StringParam(const std::string & name, size_t length) : Param(name), length_(length) {}
        Value * newValue() const;
        int mutate(Value &, float) { return 0; }
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        bool IsForInterface() const { return true; }
        const SynthDataPtr toBinary(const Value &) const;
        // TODO this has nothing to do with GA. how does this interact with UI?
    private:
        size_t length_;
    };
    
    class ConstParam : public Param {
    public:
        ConstParam(const std::string & name, size_t length, const Value & value) : Param(name), length_(length), value_(value_) {}
        Value * newValue() const { return value_.clone(); }
        int mutate(Value &, float) { return 0; }
        float compare(const Value &, const Value &) const { return 0.0; }
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
    private:
        size_t length_;
        Value value_;
    };
    
    typedef std::pair<std::string, boost::reference_wrapper<Value> > EnumValue;
    
    class EnumParam : public Param {
    public:
        EnumParam(const std::string & name);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
        void setValue(int, const Value &);
        void deleteValue(int);
        const EnumValue & getValue(int) const;
        const std::map<int, boost::reference_wrapper<EnumValue> > & values() const;
    private:
        std::map<int, boost::reference_wrapper<EnumValue> > values_;
    };
    
    class BitShiftParam : public Param {
    public:
        BitShiftParam(const std::string & name, const Param & param, int shift);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
    private:
        const Param & param_;
        int shift_;
    };
    
    class MultiTuple {
    public:
        MultiTuple(const Param & p, int l, int h) : param(p), low(l), high(h) {}
        const Param & param;
        int low;
        int high;
    };
    
    
    class MultiParam : public Param {
    public:
        MultiParam(const std::string & name);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
        void setParamWithRange(const Param &, int min, int max);
        const Param & currentValue() const;
        const Param & valueForIndex() const;
    private:
        std::vector<boost::reference_wrapper<MultiTuple> > values_;
    };
    
    class BitfieldParam : public Param {
    public:
        BitfieldParam(const std::string & name);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
        void setBitName(size_t, const std::string &);
        const std::string & getBitName(size_t) const;
        void enableBit(size_t, bool enable = true);
        void disableBit(size_t);
        void isBitEnabled(size_t) const;
        size_t maxBit() const;
    private:
        std::vector<bool> bits;
        std::vector<boost::reference_wrapper<std::string> > names;
    };
    
    class OrParam : public Param {
    public:
        OrParam(const std::string & name);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
        void addParam(const Param &);
        const std::vector<boost::reference_wrapper<Param> > & params() const;
    private:
        std::vector<boost::reference_wrapper<Param> > params_;  
    };

    class IntParam : public Param {
    public:
        IntParam(const std::string & name);
        Value * newValue() const;
        int mutate(Value &, float);
        float compare(const Value &, const Value &) const;
        int crossover(const Value & mom, const Value & dad, Value * bro, Value * sis) const;
        const SynthDataPtr toBinary(const Value &) const;
        void setRange(int min, int max) { min_ = min; max_ = max; }
        void setShift(int shift) { shift_ = shift; }
        int getMin() const { return min_; }
        int getMax() const { return max_; }
        int getShift() const { return shift_; }
    private:
        int min_, max_, shift_;
    };
}

#endif