#include "../include/parse_tree.h"

bool unary (char c) {
    static char uni[3] = {'*', '?', '+'};
    for (int i = 0; i < 3; ++i) {
        if (c == uni[i]) return true;
    }
    return false;
}

bool binary (char c) {
    static char bin[2] = {'|', '&'};
    for (int i = 0; i < 2; ++i) {
        if (c == bin[i]) return true;
    }
    return false;
}

bool op (char c) {
    static char ops[5] = {'*', '|', '+', '&', '?'};
    for (int i = 0; i < 5; ++i) {
        if (c == ops[i]) return true;
    }
    return false;
}

bool opening (char c) {
    static char open[] = {'(', '[', '{'};
    for (int i = 0; i < 3; ++i) {
        if (c == open[i]) return true;
    }
    return false;
}

bool closing (char c) {
    static char close[1] = {')'};
    for (int i = 0; i < 1; ++i) {
        if (c == close[i]) return true;
    }
    return false;
}

bool spec_char (char c) {
    static char schar[7] = {'#', '=', '_', '.', '/', '-', ' '};
    for (int i = 0; i < 7; ++i) {
        if (c == schar[i]) return true;
    }
    return false;
}

// true if char is [a-zA-Z0-9]
bool digchar (char c) {
    int _c = (int) c;
    return (_c >= 48 && _c <= 57) || (_c >= 65 && _c <= 90) || (c >= 97 && c <= 122);
}

// add concat symbol to regex -- ab -> a&b
std::string parse_tree::add_concat (std::string s) {
    std::cout << "Before: " << s << std::endl;
    std::string re = "";

    short set_flag = 0;
    short esc_flag = 0;
    for (int i = 0; i < (int) s.size() - 1; ++i) {
        if (s[i] == '\\') esc_flag = 1;
        if (s[i] == '[' && !esc_flag) set_flag = 1;
        if (s[i] == ']' && !esc_flag) set_flag = 0;
        if (esc_flag) { 
            re += s[i]; 
            re += s[i+1];
            if (i < (int) s.size() - 2 && !op (s[i+2]) && !closing (s[i+2])) re += '&';
            i++;
            esc_flag = 0;
        }
        else if (!set_flag) {
            if (unary (s[i])) { re += s[i]; re += '&'; }
            else if (binary (s[i])) re += s[i];
            else if (opening (s[i])) re += s[i];
            else if (closing (s[i]) && !op (s[i+1])) { re += s[i]; re += '&'; }
            else if (is_input (s[i+1]) || opening (s[i+1])) { re += s[i]; re += '&'; }
            else re += s[i];
        }
        else re += s[i];
    }

    re += s[s.size() - 1];
    std::cout << "After: " << re << std::endl;
    return re;
}

// return true if char is acceptable
bool parse_tree::is_input (char c) {
    if (op (c)) return false;
    return spec_char(c) || digchar(c);
}

void parse_tree::push_tnode (std::string c) {
    tnode* n = new tnode (c);
    nodes.push (n);
}

// return true if priority of s > f
bool parse_tree::priority (char f, char s) {
    if (f == s) return true;
    if (f == '*' || f == '+' || f == '?') return false;
    if (s == '*' || s == '+' || s == '?') return true;
    if (f == '&') return false;
    if (s == '&') return true;
    if (f == '|') return false;
    return true;
}

// do next operation on stack
void parse_tree::do_op () {
    if (ops.empty ()) return;

    char c = ops.top (); ops.pop ();
    switch (c) {
        case '*':
            _star ();
            break;
        case '+':
            _plus ();
            break;
        case '?':
            _quest ();
            break;
        case '&':
            _concat ();
            break;
        case '|':
            _union ();
            break;
        default:
            std::cout << "unknown operation: " << c << std::endl;
            break;
    }
}

void parse_tree::_star () {
    tnode* n = nodes.top (); nodes.pop ();

    tnode* p = new tnode ("*");
    p->left = n;
    n->parent = p;

    nodes.push (p);
}

void parse_tree::_concat () {
    tnode* n2 = nodes.top (); nodes.pop ();
    tnode* n1 = nodes.top (); nodes.pop ();

    tnode* p = new tnode ("&");
    p->left = n1;
    p->right = n2;
    n1->parent = p;
    n2->parent = p;

    nodes.push (p);
}

void parse_tree::_union () {
    tnode* n2 = nodes.top (); nodes.pop ();
    tnode* n1 = nodes.top (); nodes.pop ();

    tnode* p = new tnode ("|");
    p->left = n1;
    p->right = n2;
    n1->parent = p;
    n2->parent = p;

    nodes.push (p);
}

void parse_tree::_plus () {
    tnode* n = nodes.top (); nodes.pop ();

    tnode* p = new tnode ("+");
    p->left = n;
    n->parent = p;

    nodes.push (p);
}

void parse_tree::_quest () {
    tnode* n = nodes.top (); nodes.pop ();

    tnode* p = new tnode ("?");
    p->left = n;
    n->parent = p;

    nodes.push (p);
}

tnode* parse_tree::generate_ptree (std::string s) {
    std::string re = add_concat (s);
    
    bool esc_flag = false;
    bool set_flag = false;
    for (int i = 0; i < (int) re.size (); ++i) {
        if (re[i] == '\\') { esc_flag = true; continue; }
        if (!esc_flag && re[i] == '[') { set_flag = true; continue; }
        if (esc_flag) {
            std::string t = "\\"; t += re[i];
            push_tnode (t);
            esc_flag = false;
        }
        else if (set_flag) {
            std::string t = "";
            while (re[i] != ']') t += re[i++];
            push_tnode (t);
            set_flag = false;
        }
        else if (is_input (re[i])) {
            std::string t (1, re[i]);
            push_tnode (t);
        }
        else if (ops.empty ()) { ops.push (re[i]); }
        else if (re[i] == '(') { ops.push (re[i]); }
        else if (re[i] == ')') {
            // while loop, ops until ')'
            while (ops.top () != '(') { 
                do_op (); 
            }
            ops.pop ();
        }
        else {
            // while loop, ops until higher priority isn't on stack
            while (!ops.empty () && priority (re[i], ops.top ())) {
                do_op ();
            }
            ops.push (re[i]);
        }
    }

    // clear stack then return root nood
    while (!ops.empty ()) { do_op (); }

    return nodes.top ();
}

void parse_tree::print (tnode* t) {
    if (t == nullptr) return;
    print (t->left);
    std::cout << t->get_sym ();
    print (t->right);
}

void parse_tree::print_tree (tnode* t) {
    print (t);
    std::cout << std::endl;
}

parse_tree::parse_tree () {
    root = nullptr;
    nodes.empty ();
    ops.empty ();
}

parse_tree::~parse_tree () {
    // delete tree
}