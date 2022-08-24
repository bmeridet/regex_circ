#include "../include/hdl.h"

hdl::hdl (std::vector<state*>& mods) {
    bram_sz = mods.size () - 2;
    build_bram (mods);
    build_modules (mods);
    build_main (mods);
    hdl_complete = hdl_bram + hdl_modules + hdl_main;
}

void hdl::build_bram (std::vector<state*>& modules) {

    std::string bram = "module bram (clk, in, out);\ninput clk;\ninput [7:0] in;\n";
    bram += "output reg [" + std::to_string (state::bram_lu.size () - 1) + ":0] out;\n";
    bram += "reg [255:0] _bram [" + std::to_string (state::bram_lu.size () - 1) + ":0];\ninitial begin\n\tout = 0;\n\t";
    bram += "for (int i=0; i<" + std::to_string (state::bram_lu.size ()) + "; i=i+1) begin\n\t\t_bram[i] = 0;\n\tend\n";

    for (auto i = state::bram_lu.begin (); i != state::bram_lu.end (); i++) {
        std::string label = i->first;
        for (int j = 0; j < (int) label.size (); ++j) {
            if (label[j] == '\\') continue;
            bram += "\t_bram[" + std::to_string (i->second) + "][\"" + label[j] +"\"] = 1;\n";
        }
    }

    bram += "end\n";
    bram += "always @(posedge clk) begin\n";

    for (auto i = state::bram_lu.begin (); i != state::bram_lu.end (); i++) bram += "\tout[" + std::to_string (i->second) + "] <= _bram[" + std::to_string (i->second) + "][in];\n";
    bram += "end\nendmodule\n";

    hdl_bram = bram;
}

/*
void hdl::build_bram (std::vector<state*>& modules) {

    std::string bram = "module bram (clk, in, out);\ninput clk;\ninput [7:0] in;\n";
    bram += "output reg [" + std::to_string (bram_sz - 1) + ":0] out;\n";
    bram += "reg [255:0] _bram [" + std::to_string (bram_sz - 1) + ":0];\ninitial begin\n\tout = 0;\n\t";
    bram += "for (int i=0; i<" + std::to_string (bram_sz) + "; i=i+1) begin\n\t\t_bram[i] = 0;\n\tend\n";

    for (int i = 1; i < (int) modules.size () - 1; ++i) {
        std::string label = modules[i]->get_state ();
        for (int j = 0; j < (int) label.size (); ++j) {
            if (label[j] == '\\') continue;
            bram += "\t_bram[" + std::to_string (i - 1) + "][\"" + label[j] +"\"] = 1;\n";
        }
    }

    bram += "end\n";
    bram += "always @(posedge clk) begin\n";

    for (int i = 0; i < bram_sz; ++i) bram += "\tout[" + std::to_string (i) + "] <= _bram[" + std::to_string (i) + "][in];\n";
    bram += "end\nendmodule\n";

    hdl_bram = bram;
}
*/

void hdl::build_modules (std::vector<state*>& modules) {

    std::string decs = "";

    for (int i = 0; i < (int) modules.size (); ++i) {
        state* s = modules[i];

        if (s->get_type() == state::start) {
            decs += "module " + mod_name (s) + " (clk, in, ";
            decs += "o_" + mod_name(s) + ");\n";
            decs += "input clk, in;\noutput reg o_" + mod_name (s) + ";\n";
            decs += "always @(posedge clk) begin\n\to_" + mod_name (s) + " <= in;\nend\nendmodule\n";
            continue;
        }

        if (s->get_type () == state::match) {
            decs += "module " + mod_name (s) + " (clk, ";
            int in_size = s->in.size ();
            for (int j = 0; j < in_size; ++j) decs += mod_name (s->in[j]) + ", ";
            decs += "out);\n";
            decs += "input ";
            for (int j = 0; j < in_size; ++j) decs += mod_name (s->in[j]) + ", ";
            decs += "clk;\noutput reg out;\n";
            decs += "always @(posedge clk) begin\n\tout <= ";
            for (int j = 0; j < in_size - 1; ++j) decs += mod_name (s->in[j]) + " | ";
            decs += mod_name (s->in[in_size - 1]) + ";\nend\nendmodule\n";
            continue;
        }

        decs += "module " + mod_name (s) + " (clk, m_bit, ";
        int in_size = s->in.size ();
        for (int j = 0; j < in_size; ++j) decs += "l_" + mod_name (s->in[j]) + ", ";
        decs += "o_" + mod_name(s) + ");\n";
        decs += "input ";
        for (int j = 0; j < in_size; ++j) decs += "l_" + mod_name (s->in[j]) + ", ";
        decs += "clk, m_bit;\noutput reg o_" + mod_name (s) + ";\n";
        decs += "always @(posedge clk) begin\n\to_" + mod_name (s) + " <= m_bit & (";
        for (int j = 0; j < in_size - 1; ++j) decs += "l_" + mod_name (s->in[j]) + " | ";
        decs += "l_" + mod_name (s->in[in_size - 1]) + ");\nend\nendmodule\n";
    }

    hdl_modules = decs;
}

void hdl::build_main (std::vector<state*>& modules) {
    
    std::string header = "module main (clk, in, char_in, out);\n";
    std::string footer = "endmodule\n";
    std::string inputs = "input clk, in;\ninput [7:0] char_in;\n";
    std::string outputs = "output out;\n";
    std::string wires = "wire [" + std::to_string (state::bram_lu.size () - 1) + ":0] match;\nwire ";
    std::string assign_regs = "";
    std::string inst_mod = "";

    inst_mod += "bram bram (clk, char_in, match);\n";

    int bram_count = 0;
    for (int i = 0; i < (int) modules.size (); ++i) {
        state* s = modules[i];

        if (s->get_type () == state::start) {
            wires += "w_" + mod_name (s) + ", ";
            inst_mod += mod_name (s) + " " + mod_name (s) + " (clk, in, w_" + mod_name (s) + ");\n";
            continue;
        }

        if (s->get_type () == state::match) {
            inst_mod += mod_name (s) + " " + mod_name (s) + " (clk, ";
            int in_size = s->in.size ();
            for (int j = 0; j < (int) in_size; ++j) inst_mod += "w_" + mod_name (s->in[j]) + ", ";
            inst_mod += "out);\n";
            wires += "w_" + mod_name (s) + ";\n";
            continue;
        }

        wires += "w_" + mod_name (s) + ", ";
        create_instance (s, inst_mod, wires, assign_regs, bram_count);
    }
    hdl_main = header + inputs + outputs + wires + inst_mod + assign_regs + footer;
}

std::string hdl::mod_name (state* s) {
    return "m_" + std::to_string (s->get_stateid ());
}

void hdl::create_instance (state* s, std::string& mod, std::string& wires, std::string& assign_regs, int& bram_count) {
    std::string name = mod_name (s);
    mod += name + " " + name + " (clk, match[" + std::to_string (state::bram_lu[s->get_state ()]) + "], ";

    int in_size = s->in.size ();
    for (int i = 0; i < in_size - 1; ++i) {
        if (s == s->in[i]) {
            wires += "self_" + name + ", ";
            assign_regs += "assign self_" + name + " = w_" + name + ";\n";
            mod += "self_" + name + ", ";
        } 
        else mod +=  "w_" + mod_name (s->in[i]) + ", ";
    }

    if (s == s->in[in_size - 1]) {
        wires += "self_" + name + ", ";
        assign_regs += "assign self_" + name + " = w_" + name + ";\n";
        mod += "self_" + name + ", w_" + name + ");\n";
    }
    else mod +=  "w_" + mod_name (s->in[in_size - 1]) + ", w_" + name + ");\n";
}

void hdl::print_hdl () {
    std::cout << hdl_complete << std::endl;
}