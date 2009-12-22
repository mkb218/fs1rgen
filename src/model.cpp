/*
 *  model.cpp
 *  fs1rgen
 *
 *  Created by Matthew J Kane on 12/15/09.
 *  Copyright 2009 Akamai Technologies. All rights reserved.
 *
 */

#include "model.h"

namespace fs1rgen {
    int Genome::spawn(const Genome & dad, GAGenome *bro, GAGenome *sis) const {
        int nc = 0;
        ValueList::const_iterator momit = values_.begin();
        ValueList::const_iterator dadit = dad.values().begin();
        ValueList::iterator sisit, broit;
        Genome *realbro = NULL;
        Genome *realsis = NULL;
        if (bro != NULL) {
            realbro = dynamic_cast<Genome *>(bro);
            realbro->copy(dad);
            broit = realbro->values_.begin();
            ++nc;
        }
        if (sis != NULL) {
            realsis = dynamic_cast<Genome *>(sis);
            realsis->copy(*this);
            sisit = realsis->values_.begin();
            ++nc;
        }
        
        if (nc == 0) {
            return nc; // done!
        }
        
        while (momit != values().end() && dadit != dad.values().end() && 
               (realbro == NULL or broit != realbro->values_.end()) &&
               (realsis == NULL or sisit != realsis->values_.end())) {
            Value *broval = NULL;
            Value *sisval = NULL;
            if (realbro != NULL) {
                broval = &(*broit);
                ++broit;
            }
            if (realbro != NULL) {
                sisval = &(*sisit);
                ++sisit;
            }
            momit->crossover(*dadit, broval, sisval);
            ++momit;
            ++dadit;
        }
        
        return nc;
    }
    
    void Model::initialize(Genome & genome) const {
        genome.values_.clear();
        foreach ( const Param & p, params_) {
            Value *newValue = p.newValue();
            genome.values_.push_back(*newValue);
            delete newValue;
        }
    }

    float Genome::evaluate() {
        fs1rgen::Evaluator *eval = fs1rgen::Evaluator::getInstance();
        _evaluated = gaFalse;
        assert (eval != NULL); // this means programmer error
        float score = 0.0;
        try {
            score = eval->evaluate(values_);
            _evaluated = gaTrue;
        } catch (Exception & e) {
            // TODO: something notification-like with this exception
            score = -1;
        }
        return score;
    }
}