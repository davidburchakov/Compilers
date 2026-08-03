//
// Created by David Burchakov on 8/3/26.
//

#ifndef COMPILERCPP_GRAMMARTESTUTILS_H
#define COMPILERCPP_GRAMMARTESTUTILS_H


#include <gtest/gtest.h>

#include <string>

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"

namespace GrammarTestUtils {
    struct ParseResult {
        int syntaxErrors = 0;
        bool treeCreated = false;
    };

    inline ParseResult parseSource(const std::string &source) {
        antlr4::ANTLRInputStream input(source);

        CppLexer lexer(&input);

        antlr4::CommonTokenStream tokens(&lexer);

        CppParser parser(&tokens);

        CppParser::TranslationUnitContext *tree =
                parser.translationUnit();

        return {
            static_cast<int>(parser.getNumberOfSyntaxErrors()),
            tree != nullptr
        };
    }

    inline void expectValid(const std::string &source) {
        const ParseResult result = parseSource(source);

        ASSERT_TRUE(result.treeCreated);
        EXPECT_EQ(result.syntaxErrors, 0)
            << "Source failed to parse:\n"
            << source;
    }

    inline void expectInvalid(const std::string &source) {
        const ParseResult result = parseSource(source);

        ASSERT_TRUE(result.treeCreated);
        EXPECT_GT(result.syntaxErrors, 0)
            << "Source unexpectedly parsed successfully:\n"
            << source;
    }
} // namespace GrammarTestUtils

#endif //COMPILERCPP_GRAMMARTESTUTILS_H
