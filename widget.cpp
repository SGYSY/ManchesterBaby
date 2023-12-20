#include <iostream>
#include <algorithm>
#include <QApplication>

#include "widget.h"

// Related to "Reload MC" button and other places.
// Load the Machine Code from the file to the displaying area.
void Widget::loadMachineCode() {
    machineCodeArea->clear();
    for (std::bitset<SIZE_32_BIT> line: baby->memory) {
        QString str = QString::fromStdString(line.to_string());
        machineCodeArea->append(str);
    }
    mainWindow.update();
}

// Related to "Run" button.
// Prepare to execute the Machine Code.
void Widget::run() {
    // Avoid collusion
    if (running) {
        return;
    }
    running = true;
    // Recover the Manchester Baby if it is currently
    baby->setHalt(false);
    try {
        // Execute MC
        execute();
    } catch (const std::runtime_error &e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
}

// A recursive function. Execute instructions one after another, and wait for a while between each.
void Widget::execute() {
    if (!baby->isHalted()) {
        // Fetch, Decode and Execute
        baby->fetch();
        baby->decodeAndExecute();
        baby->increment_ci();

        // Update GUI information panel
        round->setText(QString::fromStdString(std::to_string(baby->curRound)));
        prev_ci->setText(QString::fromStdString(std::to_string(baby->prev_ci)));
        pi->setText(QString::fromStdString(baby->pi.to_string()));
        ci->setText(QString::fromStdString(std::to_string(baby->ci)));
        opcode->setText(QString::fromStdString(std::to_string(baby->curOpCode)));
        operand->setText(QString::fromStdString(std::to_string(baby->curOperand)));
        QString inImAddressing = baby->curImAddressing ? "Immediate Addressing" : "Default";
        address_mode->setText(inImAddressing);
        accumulator->setText(QString::fromStdString(baby->accumulator.to_string()));
        accumulatorDec->setText(QString::fromStdString(
                std::to_string(ManchesterBaby::binToDec(ManchesterBaby::convertInstruction(baby->accumulator)))));
        QString expString;  // For Explanation
        switch (baby->curOpCode) {
            case 0:
                expString = "JMP: CI = ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 1:
                expString = "JRP: CI += ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 2:
                expString = "LDN: A = -";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 3:
                expString = "STO: S = A";
                break;
            case 4:
                expString = "SUB: A -= ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 6:
                expString = "CMP: CI += 1 if A < 0";
                break;
            case 7:
                expString = "STOP";
                break;
            case 8:
                expString = "LDP: A = ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 9:
                expString = "ADD: A += ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 10:
                expString = "DIV: A /= ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 11:
                expString = "MOD: A %= ";
                if (baby->curImAddressing) {
                    expString.append("OPERAND");
                } else {
                    expString.append("S");
                }
                break;
            case 12:
                expString = "LAN: A = A & S";
                break;
            case 13:
                expString = "LOR: A = A | S";
                break;
            case 14:
                expString = "LNT: A = ~A";
                break;
            case 15:
                expString = "SHL: A <<= 1";
                break;
            case 16:
                expString = "SHR: A >>= 1";
                break;
            default:
                expString = "Invalid OPCODE!";
                break;
        }

        explanation->setText(expString);
        loadMachineCode();

        // Wait for a while
        QTimer::singleShot(1000, this, SLOT(execute()));
    } else {
        baby->reset();
    }
}

// Related to "Stop" button.
// Terminates the MC execution progress, but not the program, unlike the console mode.
void Widget::stop() {
    running = false;
    baby->setHalt(true);
    baby->reset();
}

// Constructor
Widget::Widget(ManchesterBaby *baby, QWidget *parent)
        : QWidget(parent) {
    this->baby = baby;

    // Initializes and sets attributes of all the components.
    splitter = new QSplitter(&mainWindow);
    machineCodeArea = new QTextEdit();
    rightLayout = new QVBoxLayout();
    rightWidget = new QWidget();
    buttonLayout = new QHBoxLayout();
    scrollArea = new QScrollArea();
    infoLayout = new QFormLayout();
    infoWidget = new QWidget();
    loadButton = new QPushButton("Reload MC");
    runButton = new QPushButton("Run");
    stopButton = new QPushButton("Stop");

    // Window Frame
    mainWindow.setCentralWidget(splitter);
    mainWindow.resize(1000, 750);

    // Machine Code displaying area
    machineCodeArea->setFixedWidth(QFontMetrics(machineCodeArea->font()).averageCharWidth() * 32);
    machineCodeArea->setReadOnly(true);
    splitter->addWidget(machineCodeArea);

    // Widget on the right
    rightWidget->setLayout(rightLayout);
    splitter->addWidget(rightWidget);
    rightLayout->addLayout(buttonLayout);
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(stopButton);
    scrollArea->setWidgetResizable(true);
    rightLayout->addWidget(scrollArea);
    infoWidget->setLayout(infoLayout);
    scrollArea->setWidget(infoWidget);

    // Action Listener for the buttons
    connect(loadButton, &QPushButton::pressed, this, &Widget::loadMachineCode);
    connect(runButton, &QPushButton::pressed, this, &Widget::run);
    connect(stopButton, &QPushButton::pressed, this, &Widget::stop);

    // Information Panel display setting
    QStringList infoLabels = {"Round", "CI", "PI", "New CI", "OPCODE", "OPERAND", "Address Mode", "Accumulator",
                              "Accumulator (DEC)",
                              "Explanation"};

    // Round
    roundTitle = new QLabel(infoLabels[0] + ":");
    round = new QLabel(QString::fromStdString(std::to_string(baby->curRound)));
    infoLayout->addRow(roundTitle, round);

    // CI
    prevCiTitle = new QLabel(infoLabels[1] + ":");
    prev_ci = new QLabel(QString::fromStdString(std::to_string(baby->prev_ci)));
    infoLayout->addRow(prevCiTitle, prev_ci);

    // PI
    piTitle = new QLabel(infoLabels[2] + ":");
    pi = new QLabel(QString::fromStdString(baby->pi.to_string()));
    infoLayout->addRow(piTitle, pi);

    // New CI
    ciTitle = new QLabel(infoLabels[3] + ":");
    ci = new QLabel(QString::fromStdString(std::to_string(baby->ci)));
    infoLayout->addRow(ciTitle, ci);

    // OPCODE
    opcodeTitle = new QLabel(infoLabels[4] + ":");
    opcode = new QLabel(QString::fromStdString(std::to_string(baby->curOpCode)));
    infoLayout->addRow(opcodeTitle, opcode);

    // OPERAND
    operandTitle = new QLabel(infoLabels[5] + ":");
    operand = new QLabel(QString::fromStdString(std::to_string(baby->curOperand)));
    infoLayout->addRow(operandTitle, operand);

    // Address Mode
    addressModeTitle = new QLabel(infoLabels[6] + ":");
    QString inImAddressing = baby->curImAddressing ? "Immediate Addressing" : "Default";
    address_mode = new QLabel(inImAddressing);
    infoLayout->addRow(addressModeTitle, address_mode);

    // Accumulator
    accumulatorTitle = new QLabel(infoLabels[7] + ":");
    accumulator = new QLabel(QString::fromStdString(baby->accumulator.to_string()));
    infoLayout->addRow(accumulatorTitle, accumulator);

    // Accumulator (DEC)
    accumulatorDecTitle = new QLabel(infoLabels[8] + ":");
    accumulatorDec = new QLabel(QString::fromStdString(
            std::to_string(ManchesterBaby::binToDec(ManchesterBaby::convertInstruction(baby->accumulator)))));
    infoLayout->addRow(accumulatorDecTitle, accumulatorDec);

    // Explanation
    explanationTitle = new QLabel(infoLabels[9] + ":");
    QString expString = "--";
    explanation = new QLabel(expString);
    infoLayout->addRow(explanationTitle, explanation);

    // Load the Machine Code
    loadMachineCode();
}

// Destructor
Widget::~Widget()
= default;
