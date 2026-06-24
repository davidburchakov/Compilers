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

// ============================================================
// 3. BRING IN COMPILER MODULES FOR LIVE RE-COMPILATION
// ============================================================
import ASTOptimizer;
import AssemblyGenerator;

using namespace antlr4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load initial source data stream file text directly on boot
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

    // Initial parsing, tree generation, and assembly generation pass
    onTextChanged();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onTextChanged()
{
    std::string code = ui->codeEditor->toPlainText().toStdString();

    // Clear display panes immediately if source box is empty
    if (code.empty()) {
        ui->astTree->clear();
        ui->assemblyViewer->clear();
        return;
    }

    // 1. Lexing & Parsing Stage
    antlr4::ANTLRInputStream input(code);
    CppLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    CppParser parser(&tokens);

    tree::ParseTree *tree = parser.translationUnit();

    // Syntax Safeguard: Abort assembly generation if the user is typing broken code
    if (parser.getNumberOfSyntaxErrors() > 0) {
        ui->assemblyViewer->setPlainText("# Syntax Error: Fix input code to generate assembly...");
        buildAST(code); // Update tree layout anyway to show broken nodes
        return;
    }

    // 2. Live Optimization Phase (Constant Folding)
    CppZero::ASTOptimizer optimizer;
    std::any optimizationResult = optimizer.optimize(tree);

    // 3. Live Assembly Generation Phase
    CppZero::AssemblyGenerator generator(optimizationResult);
    std::string dynamicAssembly = generator.generateAssembly(tree);

    // 4. Update the Viewports Side-by-Side
    ui->assemblyViewer->setPlainText(QString::fromStdString(dynamicAssembly));
    buildAST(code);
}

void MainWindow::setAssemblyText(const std::string &assemblyCode) {
    ui->assemblyViewer->setPlainText(QString::fromStdString(assemblyCode));
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

    std::function<void(tree::ParseTree*, QTreeWidgetItem*)> visit;
    visit = [&](tree::ParseTree *node, QTreeWidgetItem *parent)
    {
        if (!node) return;

        if (auto *ruleContext = dynamic_cast<antlr4::ParserRuleContext*>(node)) {
            std::string ruleName = parser.getRuleNames()[ruleContext->getRuleIndex()];

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
        else if (auto *terminalNode = dynamic_cast<tree::TerminalNode*>(node)) {
            std::string tokenText = terminalNode->getText();

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
