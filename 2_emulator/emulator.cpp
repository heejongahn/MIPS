#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <vector>

#define MAX_SIZE 100000
#define TEXT_START 0x400000
#define DATA_START 0x10000000

// namespace declare
using namespace std;

// class definition
// opcode map
unordered_map <string, int> opmap = {{"addu", 0}, {"and", 0}, {"nor", 0}, {"or", 0},
    {"sltu", 0}, {"sll", 0}, {"srl", 0}, {"subu", 0}, {"jr", 0}, {"addiu", 9},
    {"andi", 12}, {"lui", 15}, {"ori", 13}, {"sltiu", 11}, {"beq", 4}, {"bne", 5},
    {"lw", 35}, {"sw", 43}, {"j", 2}, {"jal", 3}, {"la", 99}};

// fn map
unordered_map <string, int> fnmap = {{"addu", 0x21}, {"and", 0x24}, {"nor", 0x27},
    {"or", 0x25}, {"sltu", 0x2b}, {"sll", 0}, {"srl", 2}, {"subu", 0x23},
    {"jr", 8}};

// opcoe -> instruction type (0: Rtype, 1: Itype, 2: Jtype)
unordered_map <int, int> typemap;

// reg value array
int regmap[32];

// instruction/data address -> value
unordered_map <int, vector<int>> textmap;
unordered_map <int, int> datamap;

int pc = TEXT_START;

vector<int> parse_inst(string raw_str) {
    int op = stoi(raw_str.substr(0, 6), nullptr, 2);
    int rs, rt, rd, sft, fn, imm, target;
    vector<int> result;

    result.push_back(op);

    switch (op) {
        case 0:
            rs = stoi(raw_str.substr(6, 5), nullptr, 2);
            rt = stoi(raw_str.substr(11, 5), nullptr, 2);
            rd = stoi(raw_str.substr(16, 5), nullptr, 2);
            sft = stoi(raw_str.substr(21, 5), nullptr, 2);
            fn = stoi(raw_str.substr(26, 6), nullptr, 2);

            result.push_back(rs);
            result.push_back(rt);
            result.push_back(rd);
            result.push_back(sft);
            result.push_back(fn);
            break;

        case 9:
        case 0xc:
        case 0xf:
        case 0xd:
        case 0xb:
        case 4:
        case 5:
        case 0x23:
        case 0x2b:
            rs = stoi(raw_str.substr(6, 5), nullptr, 2);
            rt = stoi(raw_str.substr(11, 5), nullptr, 2);
            imm = stoi(raw_str.substr(16, 16), nullptr, 2);

            result.push_back(rs);
            result.push_back(rt);
            result.push_back(imm);
            break;

        case 2:
        case 3:
            target = stoi(raw_str.substr(6, 26), nullptr, 2);

            result.push_back(target);
    }

    return result;
}

int main(int argc, char* argv[]) {
    string filename = (string) argv[1];
    ifstream inFile(filename);

    char buf[MAX_SIZE];
    string input_str;

    int data_section_size, text_section_size;
    int target_address;

    vector<int> inst;
    int data;
    int i;

    inFile.getline(buf, MAX_SIZE);
    input_str = (string) buf;
    cout << input_str << " " << input_str.size() << endl;

    // Initialize Register
    for (i = 0; i < 32 ; i++) {
      regmap[i] = 0;
    }

    // Get each section's size info
    text_section_size = stoi(input_str.substr(0, 32), nullptr, 2);
    input_str = input_str.substr(32);

    data_section_size = stoi(input_str.substr(0, 32), nullptr, 2);
    input_str = input_str.substr(32);

    // text section load
    for (i = 0; i < text_section_size ; i+=4) {
        inst = parse_inst(input_str.substr(0, 32));
        input_str = input_str.substr(32);

        textmap[TEXT_START + i] = inst;
    }

    // data section load
    for (i = 0; i < data_section_size ; i+=4) {
        data = stoi(input_str.substr(0, 32), nullptr, 2);
        input_str = input_str.substr(32);

        datamap[DATA_START + i] = data;

    }
    // ----- *-_-* ----- @_@ ----- ^3^ ----- *_* ----- >3< ----- +_+ ----- //

    // ----- *-_-* ----- @_@ ----- ^3^ ----- *_* ----- >3< ----- +_+ ----- //
    // write result to stdout



    return 0;
}
