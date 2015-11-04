#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <iomanip>

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
  cout << "Current register values :" << endl;
  cout << "-------------------------------------" << endl;
  cout << "PC: 0x" << setfill('0') << setw(8) << setbase(16) << pc << endl;
  cout << "Registers:" << endl;
  for (i = 0; i < 32; i++)  {
  cout << setbase(10) << "R" << i << ": 0x";
  cout << setw(8) << setbase(16) << reg[i] << endl;
  }
}

int main(int argc, char* argv[]) {
  string filename = (string) argv[1];
  ifstream inFile(filename);

  char buf[MAX_SIZE];
  string input_str;

  int data_section_size, text_section_size;

  vector<int> inst;
  int data;

  int i;
  int op, rs, rt, rd, sft, fn, imm, target;

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

  while (pc < TEXT_START + text_section_size) {
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
            break;

          // and
          case 0x24:
            break;

          // nor
          case 0x27:
            break;

          // or
          case 0x25:
            break;

          // sltu
          case 0x2b:
            break;

          // sll
          case 0:
            break;

          // srl
          case 2:
            break;

          // subu
          case 0x23:
            break;

          // jr
          case 8:
            break;

        }
        break;

      // addiu
      case 9:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // andi
      case 0xc:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // lui
      case 0xf:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // ori
      case 0xd:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // sltiu
      case 0xb:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // beq
      case 4:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // bne
      case 5:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // lw
      case 0x23:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // sw
      case 0x2b:
        rs = inst[1], rt = inst[2], imm = inst[3];
        break;

      // j
      case 2:
        target = inst[1];
        break;

      // jal
      case 3:
        target = inst[1];
        break;
    }
  }

  // completion
  print_result();

  return 0;
}
