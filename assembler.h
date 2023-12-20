#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>

// Class for symbol table
class SymbolTable {
private:
    std::map<std::string, int> table;
public:
    // Constructor & Destructor
    SymbolTable();

    ~SymbolTable();

    // Function to add a label and its corresponding address to the symbol table
    void addLabel(const std::string &label, int address);

    // Function to search for a label in the symbol table and return its address
    int searchLabel(const std::string &label);
};

// Class for assembler
class Assembler {
private:

public:

    static const int VAR = 0;
    static const int JMP = 0;
    static const int JRP = 1;
    static const int LDN = 2;
    static const int STO = 3;
    static const int SUB = 4;
    static const int CMP = 6;
    static const int STP = 7;
    static const int LDP = 8;
    static const int ADD = 9;
    static const int DIV = 10;
    static const int MOD = 11;
    static const int LAN = 12;
    static const int LOR = 13;
    static const int LNT = 14;
    static const int SHL = 15;
    static const int SHR = 16;

    // Constructor & Destructor
    Assembler();

    ~Assembler();

    // Function to perform the assembly process, taking a SymbolTable and logging the process
    static void assemble(const SymbolTable &table);

    // Function to process the assemble language
    static std::vector<std::string> processAssembleCode(SymbolTable table, std::vector<std::string> &log);

    // Function to export binary code to a file
    static void exportToFile(const std::vector<std::string> &binaryCode);

    // Function to get the opcode corresponding to the instruction
    static int getOpCode(const std::string &instruction);

    // Function to export log messages to a file
    static void exportToLog(const std::vector<std::string> &log);
};


#endif //ASSEMBLER_H
