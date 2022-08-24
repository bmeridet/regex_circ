#include "../include/nfa.h"
#include "../include/hdl.h"

using namespace std;

int main () {

    std::string re;
    cout << "Enter regular expression\n> ";
    cin >> re;

    state* start = nfa::generate_nfa (re); // (a|b)*\\|[xyz]ef (works), ab(c|d)*ef (works), a[bcd]\\*ef (works), aaabbb (works)
    state::traverse (start);

    //for (int i = 0; i < (int) state::modules.size (); ++i) state::print_state (state::modules[i]);

    hdl H (state::modules);
    H.print_hdl ();
    
    return 0;
}