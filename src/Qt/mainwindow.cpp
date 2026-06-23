#include "mainwindow.h"
#include "ui_mainwindow.h"

// 1. Undefine 'emit' so it doesn't break ANTLR's Lexer::emit method
#ifdef emit
#undef emit
#endif

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"

// 2. Redefine 'emit' back to nothing so Qt's framework continues working smoothly
#ifndef emit
#define emit
#endif

#include <QTreeWidgetItem>
#include <QFile>
#include <QTextStream>

using namespace antlr4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load your input source file text directly into the editor view on boot
    QFile file("/home/incidence/Desktop/CompilerCpp/src/C++00/input.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        ui->codeEditor->setPlainText(in.readAll());
        file.close();
    }

    connect(ui->codeEditor,
            &QPlainTextEdit::textChanged,
            this,
            &MainWindow::onTextChanged);

    // Initial tree build using your file data layout mapping
    buildAST(ui->codeEditor->toPlainText().toStdString());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onTextChanged()
{
    buildAST(ui->codeEditor->toPlainText().toStdString());
}

void MainWindow::buildAST(const std::string &code) const
{
    ui->astTree->clear();
    if (code.empty()) return;

    antlr4::ANTLRInputStream input(code);
    CppLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    CppParser parser(&tokens);

    tree::ParseTree *tree = parser.translationUnit();

    // Recursive lambda designed to mirror your favorite vertical cascade indentation format
    std::function<void(tree::ParseTree*, QTreeWidgetItem*)> visit;
    visit = [&](tree::ParseTree *node, QTreeWidgetItem *parent)
    {
        if (!node) return;

        // Case A: Node is a Structural Rule Context
        if (auto *ruleContext = dynamic_cast<antlr4::ParserRuleContext*>(node)) {
            std::string ruleName = parser.getRuleNames()[ruleContext->getRuleIndex()];

            // Eliminate structural rule nodes that didn't consume any code tokens
            if (ruleName == "declarationModifiers" && node->children.empty()) {
                return;
            }

            QTreeWidgetItem *item = parent ? new QTreeWidgetItem(parent)
                                           : new QTreeWidgetItem(ui->astTree);

            item->setText(0, QString::fromStdString(ruleName));
            item->setForeground(0, QBrush(Qt::darkBlue));

            for (size_t i = 0; i < node->children.size(); i++) {
                visit(node->children[i], item);
            }
        }
        // Case B: Node is a physical token leaf terminal string
        else if (auto *terminalNode = dynamic_cast<tree::TerminalNode*>(node)) {
            std::string tokenText = terminalNode->getText();

            // Ignore empty whitespace tokens that leak through parser channels
            if (tokenText.find_first_not_of(" \t\r\n") == std::string::npos) {
                return;
            }

            QTreeWidgetItem *item = parent ? new QTreeWidgetItem(parent)
                                           : new QTreeWidgetItem(ui->astTree);

            item->setText(0, QString::fromStdString(tokenText));

            if (tokenText == "<EOF>") {
                item->setForeground(0, QBrush(Qt::darkRed));
            } else {
                item->setForeground(0, QBrush(Qt::darkGreen));
            }
        }
    };

    visit(tree, nullptr);
    ui->astTree->expandAll();
}
