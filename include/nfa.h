#pragma once
#include "state.h"
#include "tnode.h"

class nfa {

private:

    state* head;
    void _star ();
    void _plus ();
    void _quest ();
    void _union ();
    void _concat ();

public:

    nfa ();

    void build_nfa (tnode*);

};

