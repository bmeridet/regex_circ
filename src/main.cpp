#include "../include/parse_tree.h"

using namespace std;

int main () {
    parse_tree t;
    tnode* root = t.generate_ptree ("a(a|b)abc");
    t.print_tree (root);
    return 0;
}