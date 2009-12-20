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
    int Genome::spawn(const Genome & dad, GAGenome *bro, GAGenome *sis) {
        int nc = 0;
        ValueList::iterator momit = values_.begin();
        ValueList:const_iterator dadit = dad.values().begin();
        ValueList::iterator sisit, broit;
        Genome *realbro = NULL;
        Genome *realsis = NULL;
        if (bro != NULL) {
            realbro = dynamic_cast<Genome *>(bro);
            realbro->copy(dad);
            broit = realbro->begin();
            ++nc;
        }
        if (sis != NULL) {
            realsis = dynamic_cast<Genome *>(sis);
            realsis->copy(*this);
            sisit = realsis->begin();
            ++nc;
        }
        
        if (nc == 0) {
            return nc; // done!
        }
        
        while (momit < values_.end() && dadit < dad.values().end() && 
               (realbro == NULL or broit < realbro->values().end()) &&
               (realsis == NULL or sisit < realsis->values().end())) {
            if (realbro != NULL) {
                *broit = dadit->cross(*momit);
                ++broit;
            }
            if (realbro != NULL) {
                *sisit = momit->cross(*dadit);
                ++sisit;
            }
            ++momit;
            ++dadit;
        }
        
        return nc;
    }
    
    void Model::initialize(Genome & genome) const {
        genome.values_.clear();
        foreach ( const Param & p, params_) {
            genome.values_.push_back(p.newValue());
        }
    }

}