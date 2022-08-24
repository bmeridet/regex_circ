#include "../include/frag.h"

frag::frag (state* _start, ptrlist* _out) {
    start = _start;
    out =  _out;
}

frag::ptrlist* frag::list1 (state** outp) {
    ptrlist* ret = (ptrlist*) outp;
    return ret;
}

void frag::patch (ptrlist* l, state* s) {
    ptrlist* nxt;
    for ( ; l; l = nxt) {
        nxt = l->nxt;
        l->st = s;
    }
}

frag::ptrlist* frag::append (ptrlist* l1, ptrlist* l2) {
    ptrlist* ret = l1;
    while (l1->nxt) l1 = l1->nxt;
    l1->nxt = l2;
    return ret;
}