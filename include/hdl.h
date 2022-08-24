#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "state.h"

class hdl {

private:

    std::string hdl_bram;
    std::string hdl_modules;
    std::string hdl_main;
    std::string hdl_complete;
    int bram_sz;

    void build_bram (std::vector<state*>&);
    void build_modules (std::vector<state*>&);
    void build_main (std::vector<state*>&);

    std::string mod_name (state*);
    void create_instance (state*, std::string&, std::string&, std::string&, int&);

public:

    hdl (std::vector<state*>&);
    void print_hdl ();

};