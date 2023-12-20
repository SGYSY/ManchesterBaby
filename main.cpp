#include <QApplication>

#include "baby.h"
#include "widget.h"
#include "assembler.h"

/* main() function of the program */
int main(int argc, char *argv[]) {   
    // Assembler
    Assembler assembler;
    SymbolTable symbolTable;
    Assembler::assemble(symbolTable);

    // MB Simulator
    ManchesterBaby baby;    // Baby used

    // Call GUI
    QApplication a(argc, argv);
    Widget gui(&baby);
    gui.mainWindow.show();
    return QApplication::exec();
}
