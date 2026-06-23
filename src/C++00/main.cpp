// 2. Place ALL traditional #include files inside this fragment block
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <fstream>

// Clean Qt's 'emit' macro before loading ANTLR internals
#undef emit

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"
#include "../Qt/mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <fstream>
#include <iostream>

#undef emit

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"
#include "../Qt/mainwindow.h"


// 4. NOW import your C++23 Module cleanly
import SymbolTableModule;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Load your input source stream file next to the binary
    std::ifstream fileStream("/home/incidence/Desktop/CompilerCpp/src/C++00/input.txt");
    if (!fileStream.is_open()) {
        qCritical() << "Semantic Error: Could not locate input.txt in the runtime folder!";
        return 1;
    }

    // Feed the data through the Lexer and generate tokens
    antlr4::ANTLRInputStream input(fileStream);
    CppLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);

    // Instantiate the parser and build the AST
    CppParser parser(&tokens);
    CppParser::TranslationUnitContext* tree = parser.translationUnit();

    CppZero::SymbolTable symbolTable;

    qInfo() << "AST semantic processing complete. Launching interface tree viewer...";

    // Launch GUI View
    MainWindow window;
    window.show();

    return QApplication::exec();
}
