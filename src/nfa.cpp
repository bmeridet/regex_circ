#include "../include/nfa.h"

std::stack<frag*> nfa::frags;
std::stack<char> nfa::ops;

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
    static char close[2] = {')', ']'};
    for (int i = 0; i < 2; ++i) {
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
std::string nfa::add_concat (std::string s) {
    //std::cout << "Before: " << s << std::endl;
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
    //std::cout << "After: " << re << std::endl;
    return re;
}

// return true if char is acceptable
bool nfa::is_input (char c) {
    if (op (c)) return false;
    return spec_char(c) || digchar(c);
}

void nfa::push_frag (std::string c) {
    state* n = new state (c, state::normal, nullptr, nullptr);
    if (state::bram_lu.find (n->get_state ()) == state::bram_lu.end ()) state::bram_lu[n->get_state ()] = state::bram_lu.size ();
    frags.push (new frag (n, frag::list1 (&n->out1)));
}

// return true if priority of s > f
bool nfa::priority (char f, char s) {
    if (f == s) return true;
    if (f == '*' || f == '+' || f == '?') return false;
    if (s == '*' || s == '+' || s == '?') return true;
    if (f == '&') return false;
    if (s == '&') return true;
    if (f == '|') return false;
    return true;
}

// do next operation on stack
void nfa::do_op () {
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

void nfa::_star () {
    frag* f = frags.top (); frags.pop ();
    state* s = new state ("", state::split, f->start, nullptr);
    frag::patch (f->out, s);
    frags.push (new frag (s, frag::list1 (&s->out2)));
}

void nfa::_concat () {
    frag* f2 = frags.top (); frags.pop ();
    frag* f1 = frags.top (); frags.pop ();
    frag::patch (f1->out, f2->start);
    frags.push (new frag (f1->start, f2->out));
}

void nfa::_union () {
    frag* f2 = frags.top (); frags.pop ();
    frag* f1 = frags.top (); frags.pop ();
    state* s = new state ("", state::split, f1->start, f2->start);
    frags.push (new frag (s, frag::append (f1->out,  f2->out)));
}

void nfa::_plus () {
    frag* f = frags.top (); frags.pop ();
    state* s = new state ("", state::split, f->start, nullptr);
    frag::patch (f->out, s);
    frags.push (new frag (f->start, frag::list1 (&s->out2)));
}

void nfa::_quest () {
    frag* f = frags.top (); frags.pop ();
    state* s = new state ("", state::split, f->start, nullptr);
    frags.push (new frag (s, frag::append (f->out, frag::list1 (&s->out2))));
}

state* nfa::generate_nfa (std::string s) {
    std::string re = add_concat (s);
    
    bool esc_flag = false;
    bool set_flag = false;
    for (int i = 0; i < (int) re.size (); ++i) {
        if (re[i] == '\\') { esc_flag = true; continue; }
        if (!esc_flag && re[i] == '[') { set_flag = true; continue; }
        if (esc_flag) {
            std::string t = "\\"; t += re[i];
            push_frag (t);
            esc_flag = false;
        }
        else if (set_flag) {
            std::string t = "";
            while (re[i] != ']') t += re[i++];
            push_frag (t);
            set_flag = false;
        }
        else if (is_input (re[i])) {
            std::string t (1, re[i]);
            push_frag (t);
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

    frag* nfa = frags.top (); frags.pop ();
    state* start = new state ("START", state::start, nfa->start, nullptr);
    state* end = new state ("MATCH", state::match, nullptr, nullptr);
    frag::patch (nfa->out, end);

    return start;
}