#ifndef BABY_H
#define BABY_H

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <QApplication>

const int SIZE_32_BIT = 32;

// Defining operands as enums
enum OpCode {   // Digit No.14 - No.19 in machine code
    /* Classic Instructions */
    JMP = 0,    // 00000
    JRP = 1,    // 10000
    LDN = 2,    // 01000
    STO = 3,    // 11000
    SUB = 4,    // 00100, or 5 10100
    CMP = 6,    // 01100
    STP = 7,    // 11100
    /* Additional Instructions */
    LDP = 8,    // 00010
    ADD = 9,    // 10010
    DIV = 10,   // 01010
    MOD = 11,   // 11010
    LAN = 12,   // 00110
    LOR = 13,   // 10110
    LNT = 14,   // 01110
    SHL = 15,   // 11110
    SHR = 16    // 00001
};

// Class for simulating Manchester Baby
class ManchesterBaby {
private:
    int instruction_num{};

    unsigned long OPERAND_MASK{((1UL << 13) - 1) << 19};    // Mask for obtaining operand in instruction
    unsigned long OPCODE_MASK{31UL << 14};                  // Mask for obtaining opcode in instruction
    unsigned long ADDRESSING_MASK{1UL << 1};                // Mask for obtaining address mode in instruction

    bool halted{false};     // HALT mark
public:
    std::vector<std::bitset<SIZE_32_BIT>> memory;           // Memory

    int curOpCode{};                            // current opcode
    unsigned long curOperand{};                 // current operand
    bool curImAddressing{false};                // if current addressing mode is immediate
    int curRound{0};                            // Current Round
    int prev_ci{0};                             // The last Control instruction
    int ci{0};                                  // The new Control instruction
    std::bitset<SIZE_32_BIT> pi;                // Present instruction
    std::bitset<SIZE_32_BIT> accumulator;       // Accumulator
    bool inGuiMode{};                           // Whether in GUI mode or not

    // Initialize ManchesterBaby
    ManchesterBaby();

    /* Classic Manchester Baby Instructions: */

    // 0-JMP: Set CI to content of Store location (CI = S)
    // Immediate Addressing is available for this opcode: CI = OPERAND
    void jmp(unsigned long operand);

    // 1-JRP: Add content of Store location to CI (CI = CI + S)
    // Immediate Addressing is available for this opcode: CI = CI + OPERAND
    void jrp(unsigned long operand);

    // 2-LDN: Load Accumulator with negative form of Store content (A = -S)
    // Immediate Addressing is available for this opcode: A = -OPERAND
    void ldn(unsigned long operand);

    // 3-STO: Copy Accumulator to Store location (Location of S = A)
    void sto(unsigned long operand);

    // 4(5)-SUB: Subtract content of Store location from Accumulator (A = A - S)
    // Immediate Addressing is available for this opcode: A = A - OPERAND
    void sub(unsigned long operand);

    // 6-CMP: Increment CI if Accumulator value is negative, otherwise do nothing (A < 0 ? CI = CI + 1 : nothing)
    void cmp();

    // 7-STP: Set Stop lamp and halt machine (Program ends)
    void stp();

    /* Additional Instructions: */

    // 8-LDP: Load Accumulator with POSITIVE form of Store content (A = S)
    // Immediate Addressing is available for this opcode: A = OPERAND
    void ldp(unsigned long operand);

    // 9-ADD: Add the content of Store location to Accumulator (A = A + S)
    // Immediate Addressing is available for this opcode: A = A + OPERAND
    void add(unsigned long operand);

    // 10-DIV: Divide Accumulator with the content of Store location (A = A / S)
    // Immediate Addressing is available for this opcode: A = A / OPERAND
    void div(unsigned long operand);

    // 11-MOD: Find the remainder in the DIV division (A = A % S)
    // Immediate Addressing is available for this opcode: A = A % OPERAND
    void mod(unsigned long operand);

    // 12-LAN: Logical AND operation between Accumulator and the content of Store location (A = A & S)
    void lan(unsigned long operand);

    // 13-LOR: Logical OR operation between Accumulator and the content of Store location (A = A | S)
    void lor(unsigned long operand);

    // 14-LNT: Logical NOT operation of the Accumulator (A = ~A)
    void lnt();

    // 15-SHL: Digits in Accumulator left shift by 1 digit (A <<= 1)
    void shl();

    // 16-SHR: Digits in Accumulator right shift by 1 digit (A >>= 1)
    void shr();

    // Convert binary to decimal
    static int binToDec(const std::bitset<SIZE_32_BIT> &binary);

    // Load the machine code from the file.
    void loadProgram(const std::string &filename);

    // Fetch the current instruction.
    void fetch();

    // Decode and run the current instruction.
    void decodeAndExecute();

    void increment_ci();

    // Display the current state in the console. For debugging.
    [[maybe_unused]] void printState();

    // Check the operational state.
    [[nodiscard]] bool isHalted() const;

    // Manually HALT or recover the Manchester Baby. Used in GUI mode.
    void setHalt(bool wannaStop);

    // Convert opcode into standard binary bits.
    static unsigned long convertOpCode(unsigned long num);

    // Convert operand into standard binary bits.
    static unsigned long convertOperand(unsigned long num);

    // Convert Instruction (Accumulator or a specific one in memory) into standard binary bits.
    [[nodiscard]] static unsigned long convertInstruction(std::bitset<SIZE_32_BIT> ins);

    // Reset the Manchester Baby - Used in GUI mode ("Stop" Button)
    void reset();
};

#endif //BABY_H
