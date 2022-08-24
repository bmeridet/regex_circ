#pragma once
#include "frag.h"
#include <string>
#include <stack>
#include <iostream>

class nfa {

private:

    // variables
    static std::stack<frag*> frags;
    static std::stack<char> ops;

    // functions
    static std::string add_concat (std::string);
    static bool is_input (char);
    static void push_frag (std::string);
    static bool priority (char, char);

    static void do_op ();

    static void _star ();
    static void _concat ();
    static void _union ();
    static void _plus ();
    static void _quest ();

public:

    static state* generate_nfa (std::string);

};