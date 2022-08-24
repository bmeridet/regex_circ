#pragma once
#include <string>
#include "state.h"

class frag {

public:

    union ptrlist {
        state* st;
        ptrlist* nxt;
    };

    frag (state*, ptrlist*);

    state* start;
    ptrlist* out;

    static ptrlist* list1 (state**);
    static void patch (ptrlist*, state*);
    static ptrlist* append (ptrlist*, ptrlist*);
    
};