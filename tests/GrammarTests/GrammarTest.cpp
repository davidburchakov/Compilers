//
// Created by David Burchakov on 8/3/26.
//

#include <gtest/gtest.h>

#include <string>

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"


namespace {

    struct ParseResult {
        int syntaxErrors = 0;
        bool treeCreated = false;
    };


    ParseResult parseSource(const std::string& source) {
        antlr4::ANTLRInputStream input(source);

        CppLexer lexer(&input);

        antlr4::CommonTokenStream tokens(&lexer);

        CppParser parser(&tokens);

        CppParser::TranslationUnitContext* tree =
            parser.translationUnit();

        return {
            static_cast<int>(parser.getNumberOfSyntaxErrors()),
            tree != nullptr
        };
    }

}


TEST(GrammarTests, ParsesSimpleVariableDeclaration)
{
    const std::string source = R"(
        int x;
    )";

    const ParseResult result = parseSource(source);

    ASSERT_TRUE(result.treeCreated);

    EXPECT_EQ(result.syntaxErrors, 0);
}


TEST(GrammarTests, ParsesFunctionDefinition)
{
    const std::string source = R"(
        int add(int x, int y) {
            return x + y;
        }
    )";

    const ParseResult result = parseSource(source);

    ASSERT_TRUE(result.treeCreated);

    EXPECT_EQ(result.syntaxErrors, 0);
}


TEST(GrammarTests, ParsesMultipleFunctions)
{
    const std::string source = R"(
        int add(int x, int y) {
            return x + y;
        }

        int subtract(int x, int y) {
            return x - y;
        }
    )";

    const ParseResult result = parseSource(source);

    ASSERT_TRUE(result.treeCreated);

    EXPECT_EQ(result.syntaxErrors, 0);
}


TEST(GrammarTests, RejectsMissingClosingBrace)
{
    const std::string source = R"(
        int add(int x, int y) {
            return x + y;
    )";

    const ParseResult result = parseSource(source);

    ASSERT_TRUE(result.treeCreated);

    EXPECT_GT(result.syntaxErrors, 0);
}
