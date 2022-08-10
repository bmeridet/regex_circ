#include "../include/parse_tree.h"

using namespace std;

int main () {
    parse_tree t;
    tnode* root = t.generate_ptree ("(a|b)*\\|[xyz]ef");
    t.print_tree (root);
    return 0;
}