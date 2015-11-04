#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>

#define MAX_SIZE 100000
#define TEXT_START 0x400000
#define DATA_START 0x10000000

// namespace declare
using namespace std;

// reg value array
int reg[32];

// instruction/data address -> value
unordered_map <int, vector<int>> textmap;
unordered_map <int, int> datamap;

int pc = TEXT_START;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int htoi(char* h) {
    string h_str = (string) h;

    stringstream ss;
    ss << hex << h;

    int n;
    ss >> n;

    return n;
}

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

void print_result() {
  int i;

  cout << "Current register values :" << endl;
  cout << "-------------------------------------" << endl;
  cout << "PC: 0x" << setfill('0') << setw(8) << setbase(16) << pc << endl;
  cout << "Registers:" << endl;
  for (i = 0; i < 32; i++)  {
    cout << setbase(10) << "R" << i << ": 0x";
    cout << setw(8) << setbase(16) << reg[i] << endl;
  }
  cout << endl;
}

int sign_extend(int original) {
    int value = (0x0000FFFF & original);
    int mask = 0x00008000;
    int sign = (mask & original) >> 15;
    if (sign == 1)
        value += 0xFFFF0000;
    return value;
}


int main(int argc, char* argv[]) {
  string filename = (string) argv[argc-1];
  ifstream inFile(filename);

  char buf[MAX_SIZE];
  string input_str;

  int data_section_size, text_section_size;

  vector<int> inst;
  int data;

  int i;
  int op, rs, rt, rd, sft, fn, imm, target;
  int count = 0;

  // Command-line option
  int num_instr = 100;
  if (cmdOptionExists(argv, argv+argc, "-n")) {
    num_instr = atoi(getCmdOption(argv, argv+argc, "-n"));
  }
  bool print_every = cmdOptionExists(argv, argv+argc, "-d");

  /*
  char* mdump_region;
  if (cmdOptionExists(argv, argv+argc, "-m")) {
    mdump_region = getCmdOption(argv, argv+argc, "-m");
  }

  int mem_start = htoi(strtok(mdump_region, ":"));
  int mem_end = htoi(strtok(NULL, " "));
  */

  // cout << "-m: " << mdump_region << endl;

  inFile.getline(buf, MAX_SIZE);
  input_str = (string) buf;

  // Initialize Register
  for (i = 0; i < 32 ; i++) {
    reg[i] = 0;
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

  cout << "Simulating for " << num_instr << " cycles..." << endl;
  cout << endl;

  while (pc < TEXT_START + text_section_size) {
    if (num_instr <= count) {
      break;
    }
    inst = textmap[pc];
    pc += 4;
    op = inst[0];

    switch(op)  {
      // Rtype
      case 0:
        rs = inst[1], rt = inst[2], rd = inst[3], sft = inst[4], fn = inst[5];

        switch (fn) {
          // addu
          case 0x21:
            reg[rd] = reg[rs] + reg[rt];
            break;

          // and
          case 0x24:
            reg[rd] = (reg[rs] & reg[rt]);
            break;

          // nor
          case 0x27:
            reg[rd] = ~(reg[rs] | reg[rt]);
            break;

          // or
          case 0x25:
            reg[rd] = (reg[rs] | reg[rt]);
            break;

          // sltu
          case 0x2b:
            if (static_cast<unsigned int>(reg[rs]) < static_cast<unsigned int>(reg[rt])) {
              reg[rd] = 1;
            }
            else  {
              reg[rd] = 0;
            }
            break;

          // sll
          case 0:
            reg[rd] = (reg[rt] << sft);
            break;

          // srl
          case 2:
            reg[rd] = (reg[rt] >> sft);
            break;

          // subu
          case 0x23:
            reg[rd] = reg[rs] - reg[rt];
            break;

          // jr
          case 8:
            pc = reg[rs];
            break;

        }
        break;

      // addiu
      case 9:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        reg[rt] = reg[rs] + imm;
        break;

      // andi
      case 0xc:
        rs = inst[1], rt = inst[2], imm = inst[3];
        reg[rt] = (reg[rs] & imm);
        break;

      // lui
      case 0xf:
        rs = inst[1], rt = inst[2], imm = inst[3];
        reg[rt] = (imm << 16);
        break;

      // ori
      case 0xd:
        rs = inst[1], rt = inst[2], imm = inst[3];
        reg[rt] = (reg[rs] | imm);
        break;

      // sltiu
      case 0xb:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        if (static_cast<unsigned int>(reg[rs]) < static_cast<unsigned int>(imm)) {
          reg[rt] = 1;
        }
        else  {
          reg[rt] = 0;
        }
        break;

      // beq
      case 4:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        if (reg[rs] == reg[rt])  {
          pc += (imm << 2);
        }
        break;

      // bne
      case 5:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        if (reg[rs] != reg[rt])  {
          pc += (imm << 2);
        }
        break;

      // lw
      case 0x23:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        reg[rt] = datamap[reg[rs] + imm];
        break;

      // sw
      case 0x2b:
        rs = inst[1], rt = inst[2], imm = sign_extend(inst[3]);
        datamap[reg[rs] + imm] = reg[rt];
        break;

      // j
      case 2:
        target = inst[1];
        pc = ((pc & 0xf0000000) | (target << 2));
        break;

      // jal
      case 3:
        target = inst[1];
        reg[31] = pc + 4;
        pc = ((pc & 0xf0000000) | (target << 2));
        break;
    }
    if (print_every)  {
      if (pc == TEXT_START + text_section_size) {
        cout << "Run bit unset pc: " << setbase(16) << pc << endl;
        print_result();
        cout << "Simulator halted" << endl;
        cout << endl;
      }
      else  {
        print_result();
      }
    }
    count++;
  }

  if (!print_every && (pc == TEXT_START + text_section_size)) {
    cout << "Run bit unset pc: " << setbase(16) << pc << endl;
    cout << "Simulator halted" << endl;
    cout << "Executed cycle: " << setbase(10) << count << endl;
    cout << endl;
  }

  if (!print_every) {
    print_result();
  }


  return 0;
}
