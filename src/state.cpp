#include "../include/state.h"
#include <stack>
#include <iostream>

int state::nstate = 0;

std::vector<state*> state::modules;
std::unordered_map<std::string, int> state::bram_lu;

state::state (std::string _c, int t, state* o1, state* o2) {
    label = _c;
    type = t;
    out1 = o1;
    out2 = o2;
    state_id = nstate++;
    visits = 0;
}

std::string state::get_state () {
    return label;
}

int state::get_stateid () {
    return state_id;
}

int state::get_type () {
    return type;
}

void state::inc_visit () {
    visits++;
}

int state::get_visits () {
    return visits;
}

int state::num_states () {
    return nstate;
}

void state::traverse (state* start) {
    std::stack<state*> stack;
    std::stack<state*> sub_stack;

    stack.push (start);
    state* s, *t;

    //std::cout << "Number of States: " << state::num_states() << std::endl;

    while (!stack.empty()) {
        s = stack.top (); stack.pop ();

        if (s->get_visits () > 0) break;
        s->inc_visit ();
        modules.push_back (s);

        //std::cout << "sid: " << s->get_stateid() << " state: " << s->get_state() << " can reach: ";

        if (s->out1) sub_stack.push (s->out1);
        if (s->out2) sub_stack.push (s->out2);
        
        while (!sub_stack.empty ()) {
            t = sub_stack.top (); sub_stack.pop ();
            if (t->get_type () == state::split) {
                if (t->out1) sub_stack.push (t->out1);
                if (t->out2) sub_stack.push (t->out2);
            }
            else {
                //std::cout << t->get_state () << " ";
                s->out.push_back (t);
                t->in.push_back (s);
                if (t->get_visits () == 0) stack.push (t);
            }
        }

        //std::cout << std::endl;
    }

    //std::cout << "\nFinished traversal" << std::endl;
}

void state::print_state (state* s) {
    std::cout << std::endl;
    std::cout << "label: " << s->get_state () << "\ninputs: ";
    for (int i = 0; i < (int) s->in.size (); ++i) std::cout << s->in[i]->get_state () << " ";
    std::cout << "\noutputs: ";
    for (int i = 0; i < (int) s->out.size (); ++i) std::cout << s->out[i]->get_state () << " ";
    std::cout << "\n" << std::endl;
}