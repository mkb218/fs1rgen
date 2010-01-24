/*
 *  params.cpp
 *  fs1rgen
 *
 *  Created by Matthew J Kane on 12/18/09.
 *  Copyright 2009 Akamai Technologies. All rights reserved.
 *
 */

#include "params.h"
#include "ga/ga.h"
#include <algorithm>

namespace fs1rgen {
    ValuePtr StringParam::newValue() const {
        // TODO sensible default
        boost::any a(std::string(""));
        Value * v = new Value(*this, a);
        v->lock();
        return v;
    }
    
    float StringParam::compare(const Value & left, const Value & right) const {
        float result = 0.0;
        try {
            const std::string & l = boost::any_cast<const std::string &>(left.value());
            const std::string & r = boost::any_cast<const std::string &>(right.value());
            size_t minsize = std::min(l.size(), r.size());
            size_t maxsize = std::max(std::max(l.size(), r.size()), length_);
            for (size_t i = 0; i < minsize; ++i) {
                if (l[i] != r[i]) {
                    result += 1.0;
                }
            }
            result += (maxsize - minsize);
            result /= maxsize;
        } catch (boost::bad_any_cast &) {
            throw std::logic_error(__FILE__ ": StringParam compared a non-string value.");
        }
        return result;
    }
    
    int StringParam::crossover(const Value & mom, const Value & dad,  Value * bro, Value * sis) const {
        int kids = 0;
        if (bro != NULL) {
            *bro = dad;
            ++kids;
        }
        if (sis != NULL) {
            *sis = mom;
            ++kids;
        }
        return kids;
    }
    
    SynthDataPtr StringParam::toBinary(const Value & v) const {
        SynthDataPtr out(new SynthData);
        const std::string & s = boost::any_cast<const std::string &>(v.value());
        for (size_t i = 0; i < s.size() && i < length_; ++i) {
            out->push_back(s[i]);
        }
        return out;
    }

    int ConstParam::crossover(const Value & mom, const Value & dad,  Value * bro, Value * sis) const {
        int kids = 0;
        if (bro != NULL) {
            *bro = dad;
            ++kids;
        }
        if (sis != NULL) {
            *sis = mom;
            ++kids;
        }
        return kids;
    }
    
    SynthDataPtr ConstParam::toBinary(const Value & v) const {
        SynthDataPtr out(new SynthData);
        const std::string & s = boost::any_cast<const std::string &>(v.value());
        for (size_t i = 0; i < s.size() && i < length_; ++i) {
            out->push_back(s[i]);
        }
        return out;
    }
    
    ValuePtr EnumParam::newValue() const {
        GARandomSeed(0);
        int random = GARandomInt();
        return NULL;
    }
}