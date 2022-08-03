#pragma once
#include <string>

class tnode {

private:

    static int count;
    int node_id;
    std::string symbol;

public:

    tnode (std::string);

    tnode* left;
    tnode* right;
    tnode* parent;

    std::string get_sym ();

};