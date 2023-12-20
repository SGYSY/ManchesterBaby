#include<string>
#include<iostream>
#include<vector>
#include<ctime>

#include "assembler.h"

using namespace std;

// Function to find the index of a character in a string
int findChar(string str, char c) {
    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == c) {
            return i;
        }
    }
    return -1;// Return -1 if the character is not found in the string
}

// Function to extract a substring from a string
string substr(string str, int start, int end) {
    string s;
    if (start < 0) return s;// Return an empty string if the start index is invalid
    for (int i = start; i < end; ++i) {
        s += str[i];
    }
    return s;
}

// Function to translate an assembly instruction into a machine code representation
string translateInstruction(const string &instruction, int address, int addressingMode) {
    string s = "00000000000000000000000000000000";// Initialize a string with 32 bits, initially filled with zeros
    int temp = 0;// Temporary variable to track bit position while converting address to binary
    int flag = address >= 0 ? 1 : 0;
    address = flag ? address : -address;

    // Convert the address to binary and update the corresponding bits in the string
    while (address) {
        if (address % 2)s[temp] = '1';
        address /= 2;
        ++temp;
    }
    if (instruction == "VAR") {
        if (!flag) {
            for (int i = 0; i < 32; ++i) {
                if (s[i] == '0')s[i] = '1';
                else s[i] = '0';
            }
            s[0] += 1;
            for (int i = 0; i < 31; ++i) {
                if (s[i] == '2') {
                    s[i] = '0';
                    s[i + 1] += 1;
                } else break;
            }
            if (s[31] == '2')
                s[31] = '0';
        }
        return s;
    }
    int value = Assembler::getOpCode(instruction);// Get the opcode value from the instruction
    temp = 13;// Reset the temporary variable for updating bits in the string
    // Convert the opcode value to binary and update the corresponding bits in the string
    while (value) {
        if (value % 2)s[temp] = '1';
        value /= 2;
        ++temp;
    }
    // Update the addressing mode bit (bit 30) if the instruction is not "VAR"
    s[30] = (char) (addressingMode + '0');
    return s;
}

// Function to perform the assembly process, taking a SymbolTable and logging the process
void Assembler::assemble(const SymbolTable &table) {
    vector <string> binaryCode;// Vector to store binary code generated during assembly
    vector <string> log;// Vector to store log messages during assembly
    time_t now = time(nullptr);// Get the current time
    // Add a compilation start message to the log
    log.push_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                  "] Compilation Start: assemble.txt");
    // Attempt to process the assembly code and generate binary code
    try {
        binaryCode = Assembler::processAssembleCode(table, log);
    }
    catch (const std::invalid_argument &e) {
        // Handle exceptions related to assembly errors
        string s = e.what();
        // Extract error code from the exception message
        int errorCode = 0;
        for (int i = 0; i < 3; ++i) {
            errorCode = errorCode * 10 + s[i] - '0';
        }
        // Extract error line number from the exception message
        int errorLine = 0;
        for (int i = 5; i > 2; --i) {
            errorLine = errorLine * 10 + s[i] - '0';
        }
        switch (errorCode) {
            case 100:
                // Label definition error (already defined elsewhere)
                log.emplace_back("");
                log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                                 "] Error: Label '" + substr(s, 6, s.length()) + "' definition error");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Line number: " + std::to_string(errorLine));
                log.emplace_back("- Description: Label '" + substr(s, 6, s.length()) + "' is defined more than once");
                log.emplace_back("- Suggestion: Check whether the label name is spelled correctly");
                break;
            case 101:
                // Label definition error (not defined)
                log.emplace_back("");
                log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                                 "] Error: Label '" + substr(s, 6, s.length()) + "' definition error");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Line number: " + std::to_string(errorLine));
                log.emplace_back("- Description: Label '" + substr(s, 6, s.length()) + "' is not defined");
                log.emplace_back(
                        "- Suggestion: Check whether the operand name in the instruction is spelled correctly");
                break;
            case 102:
                // Error when a value is not a signed 32-bit integer
                log.emplace_back("");
                log.emplace_back(
                        "[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) + "] Error: '" +
                        substr(s, 6, s.length()) + "' is not a value");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Line number: " + std::to_string(errorLine));
                log.emplace_back(
                        "- Description: '" + substr(s, 6, s.length()) + "' should be a signed 32-bit integer but not");
                log.emplace_back("- Suggestion: Check whether the value is entered correctly");
                break;
            case 103:
                // Error when an instruction is not in the instruction set
                log.emplace_back("");
                log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                                 "] Error: Instruction '" + substr(s, 6, s.length()) + "' not exist");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Line number: " + std::to_string(errorLine));
                log.emplace_back(
                        "- Description: Instruction '" + substr(s, 6, s.length()) + "' is not in the instruction set");
                log.emplace_back("- Suggestion: Check whether the instruction is spelled correctly");
                break;
            case 104:
                // Error when a label can't support immediate addressing
                log.emplace_back("");
                log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                                 "] Error: Wrong addressing way");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Line number: " + std::to_string(errorLine));
                log.emplace_back(
                        "- Description: Label '" + substr(s, 6, s.length()) + "' can't support immediate addressing");
                log.emplace_back("- Suggestion: Check whether the instruction is spelled correctly");
                break;
            default:
                // Handle any other unknown assembly error
                log.emplace_back("");
                log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) +
                                 "] Error: Unexpected error");
                log.emplace_back("- File: assemble.txt");
                log.emplace_back("- Description: Unknown assembly error occurred");
                log.emplace_back("- Error: " + s);
                break;
        }
    }
    // Log generation for the code generation phase
    log.emplace_back("");
    log.emplace_back(
            "[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) + "] Phase: Code generating");
    // Check if there were errors during code generation
    if (binaryCode.empty())
        log.emplace_back("- Skip due to error");
    else {
        // Export binary code to a file
        Assembler::exportToFile(binaryCode);
        log.emplace_back("- Code generating completion time: " +
                         ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1));
    }
    // Additional log entries for compiler configuration
    log.emplace_back("");
    log.emplace_back("Compiler Configuration:");
    log.emplace_back("- Assembler name: Assembler baby");
    log.emplace_back("- Assembler version: v1.0");
    log.emplace_back("");
    log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) + "] Compilation end");
    // Export the final log
    Assembler::exportToLog(log);
}

// Function to process the assemble language
vector <string> Assembler::processAssembleCode(SymbolTable table, vector <string> &log) {
    string line; // Each line of the file
    vector <string> assembleCode;
    vector <string> binaryCode;
    int addr = 0; // Variables representing label line numbers
    std::ifstream inputFile("assemble.txt");
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    // Log file loading success
    log.emplace_back("- Load file successfully");
    time_t now = time(nullptr);
    log.emplace_back("");
    log.emplace_back(
            "[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) + "] Phase: Preprocessing");
    log.emplace_back("- Scan labels and except empty lines");
    // Process each line of the input file at the first time
    while (getline(inputFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';')continue;
        // Add the line to the assembleCode vector
        assembleCode.push_back(line);
        // Check for label presence
        int colonPos = findChar(line, ':');
        if (colonPos != -1) {
            string label = substr(line, 0, colonPos);
            // Check for duplicate labels
            if (table.searchLabel(label) != -1) {
                string temps;
                for (int ii = 0; ii < 3; ++ii) {
                    temps += (char) (addr % 10 + '0');
                    addr /= 10;
                }
                temps += label;
                throw invalid_argument("100" + temps);
            }
            // Log the addition of a label to the symbol table
            log.emplace_back("- Add label '" + label + "' to symbol table");
            table.addLabel(label, addr);
        }
        addr++;
    }
    // Log completion of preprocessing phase
    log.emplace_back("- Preprocessing completion time: " +
                     ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1));
    inputFile.close();
    addr = 0;
    log.emplace_back("");
    log.emplace_back("[" + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1) + "] Phase: Parsing");
    log.emplace_back("- Construct SymbolTable and parsing instructions");
    // Process each assemble code file in the second time
    for (auto &i: assembleCode) {
        // Log the current line being assembled
        log.emplace_back("- Assembling line " + to_string(addr) + ": " + i);
        // Extract opcode and operand from the current line
        int start = findChar(i, ':') + 1;
        while (i[start] == ' ' || i[start] == 9) ++start;
        int end = start;
        while (i[end] != ' ' && end < i.length()) ++end;
        string opcodeString = substr(i, start, end);
        start = end;
        while (i[start] == ' ') ++start;
        end = start;
        while (i[end] != ' ' && end < i.length()) ++end;
        string operandString = substr(i, start, end);
        // Initialize variables for address and addressing mode
        int address = 0;
        int addressingMode = 0;
        // If the opcode is not "VAR"
        if (opcodeString != "VAR") {
            try {
                getOpCode(opcodeString);
            }
            catch (const std::exception &e) {
                // Handle invalid opcode exception
                string temps;
                for (int ii = 0; ii < 3; ++ii) {
                    temps += (char) (addr % 10 + '0');
                    addr /= 10;
                }
                temps += opcodeString;
                throw invalid_argument("103" + temps);
            }
            // Check for special case where it is empty and opcode is not "CMP" or "STP" or "LNT" or "SHL" or "SHR"
            if (!((operandString == ";" || operandString.empty()) &&
                  (opcodeString == "CMP" || opcodeString == "STP" || opcodeString == "LNT" || opcodeString == "SHL" ||
                   opcodeString == "SHR"))) {
                if (operandString[0] != '#') {
                    if (table.searchLabel(operandString) == -1) {
                        // Handle undefined label exception
                        string temps;
                        for (int ii = 0; ii < 3; ++ii) {
                            temps += (char) (addr % 10 + '0');
                            addr /= 10;
                        }
                        temps += operandString;
                        throw invalid_argument("101" + temps);
                    }
                    address = table.searchLabel(operandString);
                } else {
                    // Handle immediate addressing mode for certain opcodes
                    if (opcodeString != "JMP" && opcodeString != "JRP" && opcodeString != "LDN" &&
                        opcodeString != "SUB" && opcodeString != "LDP" && opcodeString != "ADD" &&
                        opcodeString != "DIV" && opcodeString != "MOD") {
                        // Handle invalid immediate operand for this opcode exception
                        string temps;
                        for (int ii = 0; ii < 3; ++ii) {
                            temps += (char) (addr % 10 + '0');
                            addr /= 10;
                        }
                        temps += opcodeString;
                        throw invalid_argument("104" + temps);
                    }
                    // Change the immediate operand value to value
                    int flag = 1;
                    for (int j = 1; j < operandString.length(); ++j) {
                        if (operandString[j] < '0' || operandString[j] > '9')
                            flag = 0;
                        address = address * 10 + (operandString[j] - '0');
                    }
                    // Handle non-numeric immediate operand exception
                    if (!flag) {
                        string temps;
                        for (int ii = 0; ii < 3; ++ii) {
                            temps += (char) (addr % 10 + '0');
                            addr /= 10;
                        }
                        temps += operandString;
                        throw invalid_argument("102" + temps);
                    }
                    // Set addressing mode to immediate
                    addressingMode = 1;
                }

            }
        }// Handle the case where operand is a numeric value
        else {
            try {
                // Convert the operandString to an integer
                stoi(operandString);
            }
            catch (const std::exception &e) {
                // Handle invalid numeric operand exception
                string temps;
                for (int ii = 0; ii < 3; ++ii) {
                    temps += (char) (addr % 10 + '0');
                    addr /= 10;
                }
                temps += operandString;
                throw invalid_argument("102" + temps);
            }
            // Convert the numeric operandString to an integer address
            if (operandString[0] == '-') {
                for (int j = 1; j < operandString.length(); ++j)
                    address = address * 10 - (operandString[j] - '0');
            } else {
                for (char j: operandString)
                    address = address * 10 + (j - '0');
            }
        }
        // Translate the opcode and operand into binary machine code
        string BinaryCode = translateInstruction(opcodeString, address, addressingMode);
        // Store the binary machine code in the binaryCode vector
        binaryCode.push_back(BinaryCode);
        // Log the completion of assembling the current code line
        log.emplace_back("- Complete assembling code: " + BinaryCode);
        // Move to the next memory address
        addr++;
    }
    // Log the completion time of parsing
    log.emplace_back(
            "- Parsing completion time: " + ((string) ctime(&now)).substr(0, ((string) ctime(&now)).length() - 1));
    return binaryCode;

}

// Function to get the opcode corresponding to the instruction
int Assembler::getOpCode(const std::string &instruction) {
    if (instruction == "JMP")
        return Assembler::JMP;
    else if (instruction == "JRP")
        return Assembler::JRP;
    else if (instruction == "CMP")
        return Assembler::CMP;
    else if (instruction == "LDN")
        return Assembler::LDN;
    else if (instruction == "SUB")
        return Assembler::SUB;
    else if (instruction == "STO")
        return Assembler::STO;
    else if (instruction == "STP")
        return Assembler::STP;
    else if (instruction == "VAR")
        return Assembler::VAR;
    else if (instruction == "LDP")
        return Assembler::LDP;
    else if (instruction == "ADD")
        return Assembler::ADD;
    else if (instruction == "DIV")
        return Assembler::DIV;
    else if (instruction == "MOD")
        return Assembler::MOD;
    else if (instruction == "LAN")
        return Assembler::LAN;
    else if (instruction == "LOR")
        return Assembler::LOR;
    else if (instruction == "LNT")
        return Assembler::LNT;
    else if (instruction == "SHL")
        return Assembler::SHL;
    else if (instruction == "SHR")
        return Assembler::SHR;
    else
        throw std::runtime_error("Invalid enum value");
}

// Function to export binary code to a file
void Assembler::exportToFile(const std::vector <std::string> &binaryCode) {
    // Open the output file for writing
    std::ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }
    // Write each line of binary code to the file
    for (const string &line: binaryCode) {
        outputFile << line << '\n';
    }
    outputFile.close();
}

// Function to export log messages to a file
void Assembler::exportToLog(const std::vector <std::string> &log) {
    // Open the log file for writing
    std::ofstream outputFile("log.txt");
    if (!outputFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    // Write each log message to the file
    for (const string &line: log) {
        outputFile << line << '\n';
    }

    outputFile.close();
}

// Default constructor for the Assembler class
Assembler::Assembler() = default;

// Default destructor for the Assembler class
Assembler::~Assembler() = default;

// Function to add a label and its corresponding address to the symbol table
void SymbolTable::addLabel(const std::string &label, int address) {
    table[label] = address;
}

// Function to search for a label in the symbol table and return its address
int SymbolTable::searchLabel(const std::string &label) {
    if (table.count(label) != 0) {
        return table[label];
    } else {
        return -1;
    }
}

// Default constructor for the SymbolTable class
SymbolTable::SymbolTable() = default;

// Default destructor for the SymbolTable class
SymbolTable::~SymbolTable() = default;





