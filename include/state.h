#pragma once
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

class state {

private:

    std::string label;
    int type;
    int state_id;
    int visits;
    static int nstate;

public:

    static const int normal = -1;
    static const int start = 0;
    static const int match = 256;
    static const int split = 257;

    state* out1;
    state* out2;

    std::vector<state*> out;
    std::vector<state*> in;
    static std::unordered_map<std::string, int> bram_lu;
    static std::vector<state*> modules;

    state (std::string, int, state*, state*);

    std::string get_state ();
    int get_stateid ();
    int get_type ();
    void inc_visit ();
    int get_visits ();

    static int num_states ();
    static void traverse (state*);
    static void print_state (state*);

};