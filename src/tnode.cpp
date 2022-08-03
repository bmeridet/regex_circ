#include "../include/tnode.h"

int tnode::count = 0;

tnode::tnode (std::string c) {
    symbol = c;
    left = right = parent = nullptr;
    node_id = count++;
}

std::string tnode::get_sym () {
    return symbol;
}