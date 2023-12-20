#include "baby.h"

// Constructor
ManchesterBaby::ManchesterBaby() {
    memory.resize(SIZE_32_BIT);
    pi.reset();
    accumulator.reset();
    loadProgram("output.txt");
}

/* Classic Manchester Baby Instructions: */

// 0-JMP: Set CI to content of Store location (CI = S)
// Immediate Addressing is available for this opcode: CI = OPERAND
void ManchesterBaby::jmp(unsigned long operand) {
    if (curImAddressing) {
        ci = (int) operand;
    } else {
        ci = (int) convertInstruction(memory[operand]);
    }
}

// 1-JRP: Add content of Store location to CI (CI = CI + S)
// Immediate Addressing is available for this opcode: CI = CI + OPERAND
void ManchesterBaby::jrp(unsigned long operand) {
    if (curImAddressing) {
        ci += (int) operand;
    } else {
        ci += (int) convertInstruction(memory[operand]);
    }
}

// 2-LDN: Load Accumulator with negative form of Store content (A = -S)
// Immediate Addressing is available for this opcode: A = -OPERAND
void ManchesterBaby::ldn(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(-operand));
    } else {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(-convertInstruction(memory[operand])));
    }
}

// 3-STO: Copy Accumulator to Store location (Location of S = A)
void ManchesterBaby::sto(unsigned long operand) {
    memory[operand] = accumulator;
}

// 4(5)-SUB: Subtract content of Store location from Accumulator (A = A - S)
// Immediate Addressing is available for this opcode: A = A - OPERAND
void ManchesterBaby::sub(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) - operand));
    } else {
        accumulator = convertInstruction(
                std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) - convertInstruction(memory[operand])));
    }
}

// 6-CMP: Increment CI if Accumulator value is negative, otherwise do nothing (A < 0 ? CI = CI + 1 : nothing)
void ManchesterBaby::cmp() {
    if (binToDec(convertInstruction(accumulator)) < 0) {
        ci++;
    }
}

// 7-STP: Set Stop lamp and halt machine (Program ends)
void ManchesterBaby::stp() {
    std::cout << "STOP!" << std::endl << std::endl;
    halted = true;
}

/* Additional Instructions: */

// 8-LDP: Load Accumulator with POSITIVE form of Store content (A = S)
// Immediate Addressing is available for this opcode: A = OPERAND
void ManchesterBaby::ldp(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(operand));
    } else {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(convertInstruction(memory[operand])));
    }
}

// 9-ADD: Add the content of Store location to Accumulator (A = A + S)
// Immediate Addressing is available for this opcode: A = A + OPERAND
void ManchesterBaby::add(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) + operand));
    } else {
        accumulator = convertInstruction(
                std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) + convertInstruction(memory[operand])));
    }
}

// 10-DIV: Divide Accumulator with the content of Store location (A = A / S)
// Immediate Addressing is available for this opcode: A = A / OPERAND
void ManchesterBaby::div(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) / operand));
    } else {
        accumulator = convertInstruction(
                std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) / convertInstruction(memory[operand])));
    }
}

// 11-MOD: Find the remainder in the DIV division (A = A % S)
// Immediate Addressing is available for this opcode: A = A % OPERAND
void ManchesterBaby::mod(unsigned long operand) {
    if (curImAddressing) {
        accumulator = convertInstruction(std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) % operand));
    } else {
        accumulator = convertInstruction(
                std::bitset<SIZE_32_BIT>(convertInstruction(accumulator) % convertInstruction(memory[operand])));
    }
}

// 12-LAN: Logical AND operation between Accumulator and the content of Store location (A = A & S)
void ManchesterBaby::lan(unsigned long operand) {
    accumulator &= memory[operand];
}

// 13-LOR: Logical OR operation between Accumulator and the content of Store location (A = A | S)
void ManchesterBaby::lor(unsigned long operand) {
    accumulator |= memory[operand];
}

// 14-LNT: Logical NOT operation of the Accumulator (A = ~A)
void ManchesterBaby::lnt() {
    accumulator = ~accumulator;
}

// 15-SHL: Digits in Accumulator left shift by 1 digit (A <<= 1)
void ManchesterBaby::shl() {
    accumulator <<= 1;
}

// 16-SHR: Digits in Accumulator right shift by 1 digit (A >>= 1)
void ManchesterBaby::shr() {
    accumulator >>= 1;
}


// Convert binary to decimal
int ManchesterBaby::binToDec(const std::bitset<SIZE_32_BIT> &binary) {
    if (binary.test(SIZE_32_BIT - 1) - 1) {
        std::bitset<SIZE_32_BIT> negated = (~binary);
        return -(static_cast<int>(negated.to_ulong()) + 1);
    } else {
        return static_cast<int>(binary.to_ulong());
    }
}

// Load the machine code from the file.
void ManchesterBaby::loadProgram(const std::string &filename) {
    std::ifstream file(filename);
    std::string line;
    int address = 0;

    if (file.is_open()) {
        // File open successful, read each line in the file
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
            }
            // Detect if each line in the machine code file is in 32-bit
            if (line.size() == SIZE_32_BIT) {
                memory[address] = std::bitset<SIZE_32_BIT>(line);
            } else {
                std::cerr << "Error: line " << address + 1 << " in file does not have a valid number of bits."
                          << std::endl;
                throw std::runtime_error("Invalid line length in program file.");
            }
            ++address;
        }
        instruction_num = address + 1;
        file.close();
    } else {
        // Something unusual happens during file opening
        std::cerr << "Unable to open file" << std::endl;
        throw std::runtime_error("Unable to open program file.");
    }
}

// Fetch the current instruction.
void ManchesterBaby::fetch() {
    pi = memory[ci];
}

// Decode and run the current instruction.
void ManchesterBaby::decodeAndExecute() {
    unsigned long pi_value;
    unsigned long standard_operand;
    unsigned long operand;
    unsigned long standard_opcode_value;
    unsigned long opcode_value;
    unsigned long immediate_addressing;

    pi_value = pi.to_ulong();                               // transform PI to unsigned long type
    standard_operand = (pi_value & OPERAND_MASK) >> 19;     // Apply mask to get operand
    standard_opcode_value = (pi_value & OPCODE_MASK) >> 14; // Apply mask to get opcode

    // Their values in standard binary are what we actually need
    operand = convertOperand(standard_operand);
    opcode_value = convertOpCode(standard_opcode_value);

    if (opcode_value == 5) opcode_value--;      // OPCODE 5 is the same as 4 so make it easier

    curOpCode = (int) opcode_value;             // Set the current opcode
    curOperand = operand;                       // ...as well as the current operand

    // For opcode No. 0/1/2/4(5)/8/9/10/11, a addressing mode check is needed
    if ((opcode_value <= 2) || (opcode_value == 4) ||
        (opcode_value >= 8 && opcode_value <= 11)) {
        immediate_addressing =
                (pi_value & ADDRESSING_MASK) >> 1;   // Apply mask to check if using immediate addressing or not
        curImAddressing = immediate_addressing == 1;
    }

    // Do operations respectively
    switch (opcode_value) { // Starts at digit No.14 in 32-bit machine code
        case JMP: /* Classic Instructions */
            jmp(operand);   // 0 (00000)
            break;
        case JRP:
            jrp(operand);   // 1 (10000)
            break;
        case LDN:
            ldn(operand);   // 2 (01000)
            break;
        case STO:
            sto(operand);   // 3 (11000)
            break;
        case SUB:
            sub(operand);   // 4 (00100) or 5 (10100)
            break;
        case CMP:
            cmp();          // 6 (01100)
            break;
        case STP:
            stp();          // 7 (11100)
            break;
        case LDP: /* Additional Instructions - Explained at their implementations */
            ldp(operand);   // 8 (00010)
            break;
        case ADD:
            add(operand);   // 9 (10010)
            break;
        case DIV:
            div(operand);   // 10 (01010)
            break;
        case MOD:
            mod(operand);   // 11 (11010)
            break;
        case LAN:
            lan(operand);   // 12 (00110)
            break;
        case LOR:
            lor(operand);   // 13 (10110)
            break;
        case LNT:
            lnt();          // 14 (01110)
            break;
        case SHL:
            shl();          // 15 (11110)
            break;
        case SHR:
            shr();          // 16 (00001)
            break;
        default:
            std::cerr << "Unknown opcode: " << standard_opcode_value << std::endl;
            halted = true;
            break;
    }
    curRound++;     // One more round!
}

// Increment CI
void ManchesterBaby::increment_ci() {
    prev_ci = ci;
    ci = (ci + 1) % SIZE_32_BIT;
}

// Display the current state in the console.
[[maybe_unused]] void ManchesterBaby::printState() {
    std::cout << "Round:        " << curRound << std::endl;
    std::cout << "CI:           " << prev_ci << std::endl;
    std::cout << "PI:           " << pi << std::endl;
    std::cout << "New CI:       " << ci << std::endl;
    std::cout << "OPCODE:       " << curOpCode << std::endl;
    std::cout << "OPERAND:      " << curOperand << std::endl;
    std::cout << "Address Mode: ";
    if (curImAddressing) {
        std::cout << "Immediate Addressing" << std::endl << std::endl;
    } else {
        std::cout << "Default" << std::endl << std::endl;
    }
    std::cout << "Accumulator:  " << binToDec(convertInstruction(accumulator)) << std::endl;
    std::cout << accumulator << std::endl << std::endl;


    std::cout << "Memory:" << std::endl;
    for (int i = 0; i < instruction_num; ++i) {
        std::cout << i << ": " << memory[i] << std::endl;
    }

    std::cout << "--------------------------------------------------------------" << std::endl;
}

// Check the operational state.
[[nodiscard]] bool ManchesterBaby::isHalted() const {
    return halted;
}

// Manually HALT or recover the Manchester Baby. Used in GUI mode.
void ManchesterBaby::setHalt(bool wannaStop) {
    halted = wannaStop;
}

// Convert opcode into standard binary bits.
unsigned long ManchesterBaby::convertOpCode(unsigned long num) {
    std::bitset<5> bits(num);
    std::string str = bits.to_string();
    std::reverse(str.begin(), str.end());
    std::bitset<5> reversedBits(str);
    return reversedBits.to_ulong();
}

// Convert operand into standard binary bits.
unsigned long ManchesterBaby::convertOperand(unsigned long num) {
    std::bitset<13> bits(num);
    std::string str = bits.to_string();
    std::reverse(str.begin(), str.end());
    std::bitset<13> reversedBits(str);
    return reversedBits.to_ulong();
}

// Convert Instruction (Accumulator or a specific one in memory) into standard binary bits.
[[nodiscard]] unsigned long ManchesterBaby::convertInstruction(std::bitset<SIZE_32_BIT> ins) {
    std::string str = ins.to_string();
    std::reverse(str.begin(), str.end());
    std::bitset<SIZE_32_BIT> reversedBits(str);
    return reversedBits.to_ulong();
}

// Reset the Manchester Baby - Used in GUI mode ("Stop" Button)
void ManchesterBaby::reset() {
    memory.resize(SIZE_32_BIT);
    pi.reset();
    accumulator.reset();
    curOpCode = 0;
    curOperand = 0;
    curImAddressing = false;
    curRound = 0;
    prev_ci = 0;
    ci = 0;
}
