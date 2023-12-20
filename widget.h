#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QFormLayout>
#include <QTimer>

#include "baby.h"

// Contains all the components and functions for simulator GUI.
class Widget : public QWidget {
Q_OBJECT

public:
    Widget(ManchesterBaby *baby, QWidget *parent = 0);

    ~Widget();

    ManchesterBaby *baby;   // The Manchester Baby used.

    /* GUI Components */
    QMainWindow mainWindow;
    QSplitter *splitter;
    QTextEdit *machineCodeArea;
    QVBoxLayout *rightLayout;
    QWidget *rightWidget;
    QHBoxLayout *buttonLayout;
    QPushButton *loadButton;
    QPushButton *runButton;
    QPushButton *stopButton;
    QScrollArea *scrollArea;
    QFormLayout *infoLayout;
    QWidget *infoWidget;

    QLabel *roundTitle{};
    QLabel *prevCiTitle{};
    QLabel *piTitle{};
    QLabel *ciTitle{};
    QLabel *opcodeTitle{};
    QLabel *operandTitle{};
    QLabel *addressModeTitle{};
    QLabel *explanationTitle{};
    QLabel *accumulatorTitle{};
    QLabel *accumulatorDecTitle{};

    QLabel *round{};
    QLabel *prev_ci{};
    QLabel *pi{};
    QLabel *ci{};
    QLabel *opcode{};
    QLabel *operand{};
    QLabel *address_mode{};
    QLabel *explanation{};
    QLabel *accumulator{};
    QLabel *accumulatorDec{};

    /* End of GUI Components */

public slots:

    // Related to "Reload MC" button and other places.
    // Load the Machine Code from the file to the displaying area.
    void loadMachineCode();

    // Related to "Run" button.
    // Prepare to execute the Machine Code.
    void run();

    // A recursive function. Execute instructions one after another, and wait for a while between each.
    void execute();

    // Related to "Stop" button.
    // Terminates the MC execution progress, but not the program, unlike the console mode.
    void stop();

private:
    bool running{false};    // Flag to indicate whether the baby is running or not
};

#endif // WIDGET_H
