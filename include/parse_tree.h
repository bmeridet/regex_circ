#pragma once
#include "tnode.h"
#include <string>
#include <stack>
#include <iostream>

class parse_tree {

private:

    // variables
    std::stack<tnode*> nodes;
    std::stack<char> ops;
    tnode* root;

    // functions
    std::string add_concat (std::string);
    bool is_input (char);
    void push_tnode (std::string);
    bool priority (char, char);

    void do_op ();
    void print (tnode* t);

    void _star ();
    void _concat ();
    void _union ();
    void _plus ();
    void _quest ();

public:

    parse_tree ();
    ~parse_tree ();
    tnode* generate_ptree (std::string);
    void print_tree (tnode*);

};