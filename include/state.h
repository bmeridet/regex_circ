#pragma once
#include <vector>
#include <string>

class state {

private:

    int sid;
    std::string label;
    std::vector<state*> in_conn;
    std::vector<state*> out_conn;
    state* or_link;

public:

    state ();

};

// state id
// state label
// out connections
// in connections
// or link?