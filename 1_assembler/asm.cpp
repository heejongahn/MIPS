#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>

#define MAX_SIZE 10000
#define TEXT_START 0x400000
#define DATA_START 0x10000000

// namespace declare
using namespace std;

// opcode map
unordered_map <string, int> opmap = {{"addu", 0}, {"and", 0}, {"nor", 0}, {"or", 0},
    {"sltu", 0}, {"sll", 0}, {"srl", 0}, {"subu", 0}, {"jr", 0}, {"addiu", 9},
    {"andi", 12}, {"lui", 15}, {"ori", 13}, {"sltiu", 11}, {"beq", 4}, {"bne", 5},
    {"lw", 35}, {"sw", 43}, {"j", 2}, {"jal", 3}, {"la", 99}};

// fn map
unordered_map <string, int> fnmap = {{"addu", 0x21}, {"and", 0x24}, {"nor", 0x27},
    {"or", 0x25}, {"sltu", 0x2b}, {"sll", 0}, {"srl", 2}, {"subu", 0x23},
    {"jr", 8}};

// data value, data address map, label map
unordered_map <string, int> v_map;
unordered_map <string, int> a_map;
unordered_map <string, int> labelmap;

char delim[] = " ,$\t";
char delim2[] = " \t";

int pc = DATA_START;

// class definition
class Rtype {
    private:
        bitset<6> opcode;
        bitset<5> rs;
        bitset<5> rt;
        bitset<5> rd;
        bitset<5> sft;
        bitset<6> fn;
    public:
        void SetInfo(int _opcode, int _rs, int _rt, int _rd, int _sft, int _fn) {
            opcode = bitset<6>(_opcode);
            rs = bitset<5>(_rs);
            rt = bitset<5>(_rt);
            rd = bitset<5>(_rd);
            sft = bitset<5>(_sft);
            fn = bitset<6>(_fn);
        }

        string format() {
            return opcode.to_string() + rs.to_string() + rt.to_string() + rd.to_string() + sft.to_string() + fn.to_string();
        }
    };

class Itype {
    private:
        bitset<6> opcode;
        bitset<5> rs;
        bitset<5> rt;
        bitset<16> imm;
    public:
        void SetInfo(int _opcode, int _rs, int _rt, int _imm)  {
            opcode = bitset<6>(_opcode);
            rs = bitset<5>(_rs);
            rt = bitset<5>(_rt);
            imm = bitset<16>(_imm);
        }

        string format() {
            return opcode.to_string() + rs.to_string() + rt.to_string() + imm.to_string();
        }
    };

class Jtype {
    private:
        bitset<6> opcode;
        bitset<26> target;
    public:
        void SetInfo(int _opcode, int _target) {
            opcode = bitset<6>(_opcode);
            target = bitset<26>(_target);
        }

        string format() {
            return opcode.to_string() + target.to_string();
        }
    };


/* htoi
 *
 * input: hex c-string ex) "0xf"
 * output: integer ex) 15
 */

int htoi(char* h) {
    string h_str = (string) h;

    stringstream ss;
    ss << hex << h;

    int n;
    ss >> n;

    return n;
}

/*  extract_instruction
 *
 *  input: an preprocessed assembly line
 *  output: appropriate 32/64(for la) bit formatted instruction
 */
string interpret_instruction(char* line) {
    int op;

    char* token = strtok(line, delim);

    op = opmap[(string) token];

    int data_address;
    int rs, rt, rd, sft, fn;
    int target, offset, imm;

    Rtype r;
    Itype i, i1, i2;
    Jtype j;

    // We can figure which type this operation is based on opname

    switch (op) {
        // la
        case 99:

            token = strtok(NULL, delim);
            rt = atoi(token);

            token = strtok(NULL, delim);
            data_address = a_map[(string) token];

            if (data_address > DATA_START)  {
                i1.SetInfo(0xf, 0, rt, 0x1000);
                i2.SetInfo(0xd, rt, rt, (data_address & 0xffff));

                return (i1.format() + i2.format());
            }

            else    {
                i1.SetInfo(0xf, 0, rt, 0x1000);
                return i1.format();
            }


        // R type instructions
        case 0  :
            fn = fnmap[token];

            token = strtok(NULL, delim);
            rd = atoi(token);

            switch (fn) {
                // sll
                case 0:
                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, delim);
                    sft = atoi(token);

                    r.SetInfo(op, 0, rt, rd, sft, 0);
                    break;

                // srl
                case 2:
                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, delim);
                    sft = atoi(token);

                    r.SetInfo(op, 0, rt, rd, sft, 2);
                    break;

                // jr
                case 8:
                    r.SetInfo(op, rd, 0, 0, 0, 8);
                    break;

                // other R type instructions
                default:
                    token = strtok(NULL, delim);
                    rs = atoi(token);

                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    r.SetInfo(op, rs, rt, rd, 0, fn);
                    break;
            }

            return r.format();

        // J type instructions
        case 2:
        case 3:
            token = strtok(NULL, delim);

            // label
            target = labelmap[(string) token];

            j.SetInfo(op, target/4);
            return j.format();

        // I type instructions
        default:
            switch(op)   {
                // beq, bne
                case 4:
                case 5:
                    token = strtok(NULL, delim);
                    rs = atoi(token);

                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, delim);
                    target = labelmap[(string) token];

                    offset = (target - (pc+4)) / 4;
                    i.SetInfo(op, rs, rt, offset);

                    break;

                // lw, sw;
                case 0x23:
                case 0x2b:
                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, " \t(");

                    offset = atoi(token);

                    token = strtok(NULL, "$) ");
                    rs = atoi(token);
                    i.SetInfo(op, rs, rt, offset);

                    break;

                // lui
                case 0xf:
                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, delim);
                    if (strstr(token, "0x"))   {
                        imm = htoi(token);
                    }

                    else    {
                        imm = atoi(token);
                    }
                    i.SetInfo(op, 0, rt, imm);
                    break;

                // other I type instructions
                default:
                    token = strtok(NULL, delim);
                    rt = atoi(token);

                    token = strtok(NULL, delim);
                    rs = atoi(token);

                    token = strtok(NULL, delim);
                    if (strstr(token, "0x"))   {
                        imm = htoi(token);
                    }

                    else    {
                        imm = atoi(token);
                    }
                    i.SetInfo(op, rs, rt, imm);

                    break;
            }

            return i.format();
    }
}

int main(int argc, char* argv[]) {
    string filename = (string) argv[1];
    char input_string[MAX_SIZE];

    string data_name;
    string initial_values="";
    int initial_value;

    char* token, end;

    int data_section_size = 0;
    int text_section_size = 0;

    int target_address;

    string formatted_instruction;

    // open file for 1st read

    ifstream inFile(filename);

    // ------------------------------- read data section
    inFile.getline(input_string, MAX_SIZE);

    while(1)    {
        inFile.getline(input_string, MAX_SIZE);

        // end reading data section (data section size determined)
        if (strstr(input_string, ".text"))
            { break; }

        if (strstr(input_string, ":")) {
            token = strtok(input_string, " \t:");

            data_name = (string) token;
            a_map[data_name] = pc;

            token = strtok(NULL, delim2);  // .word

            token = strtok(NULL, delim2);

            if (strstr(token, "0x"))   {
                initial_value = htoi(token);
            }

            else    {
                initial_value = atoi(token);
            }
            v_map[data_name] = initial_value;
        }

        else    {
            token = strtok(input_string, delim2); // .word

            token = strtok(NULL, delim2);

            if (strstr(token, "0x"))   {
                initial_value = htoi(token);
            }

            else    {
                initial_value = atoi(token);
            }
        }

        initial_values += (bitset<32>(initial_value)).to_string();

        data_section_size += 4;
        pc += 4;    // increment program counter
    }

    // ------------------------------- read text section
    pc = TEXT_START;
    while (inFile.getline(input_string, MAX_SIZE))   {
        token = strtok(input_string, delim);

        // if label, save its address to an unordered map
        if (strstr(token, ":"))   {
            labelmap[(string) strtok(token, ":")] = pc; // save (current pc + 4)
        }

        else {
            if (strstr(token, "la")) {
                token = strtok(NULL, delim);
                token = strtok(NULL, delim);    // data name
                target_address = a_map[(string) token];

                if (target_address > DATA_START)    {
                    pc += 4;
                    text_section_size += 4;
                }

                pc += 4;
                text_section_size += 4;
            }

            // any other instructions
            else    {
                pc += 4;
                text_section_size += 4;
            }
        }
    }

    // close file
    inFile.close();

    // ----- *-_-* ----- @_@ ----- ^3^ ----- *_* ----- >3< ----- +_+ ----- //

    // open two files for 2nd read & write

    ifstream inFile2(filename);
    ofstream outFile(filename.substr(0, filename.size()-2) + ".o");

    // write data section size, text section size
    outFile << (bitset<32>(text_section_size)).to_string();
    outFile << (bitset<32>(data_section_size)).to_string();

    // ------------------------------- read data section
    while (1)   {
        inFile2.getline(input_string, MAX_SIZE);

        if (strstr(input_string, ".text")) {
            break;
        }
    }

    // ------------------------------- read text section
    pc = TEXT_START;

    inFile2.getline(input_string, MAX_SIZE);
    while (inFile2.getline(input_string, MAX_SIZE)) {
        // if label, pass
        if (strstr(input_string, ":"))   {
            continue;
        }

        // instruction
        else    {
            formatted_instruction = interpret_instruction(input_string);

            outFile << formatted_instruction;
            pc += (formatted_instruction.size() / 8);
        }
    }


    // close input files
    inFile2.close();

    // write data section and close
    outFile << initial_values;
    outFile.close();

    return 0;
}
