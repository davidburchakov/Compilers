//
// Created by David Burchakov on 8/3/26.
//

// AssociativityTests.cpp

#include <gtest/gtest.h>

#include <string>

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"


namespace {

struct ExpressionParseResult {
    int syntaxErrors = 0;
    CppParser::ExpressionContext* expression = nullptr;
};

ExpressionParseResult parseExpression(const std::string& source) {
    antlr4::ANTLRInputStream input(source);

    CppLexer lexer(&input);

    antlr4::CommonTokenStream tokens(&lexer);

    CppParser parser(&tokens);

    CppParser::ExpressionContext* expression =
        parser.expression();

    return {
        static_cast<int>(parser.getNumberOfSyntaxErrors()),
        expression
    };
}

void expectValidExpression(const std::string& source) {
    const ExpressionParseResult result =
        parseExpression(source);

    ASSERT_NE(result.expression, nullptr);

    EXPECT_EQ(result.syntaxErrors, 0)
        << "Expression failed to parse:\n"
        << source;
}

} // namespace


// ============================================================
// Multiplicative precedence
// ============================================================

TEST(AssociativityTests, MultiplicationBindsBeforeAddition)
{
    const auto result =
        parseExpression("a + b * c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    ASSERT_EQ(addition->expression().size(), 2u);

    EXPECT_NE(
        dynamic_cast<CppParser::VariableIdentifierContext*>(
            addition->expression(0)
        ),
        nullptr
    );

    EXPECT_NE(
        dynamic_cast<CppParser::MultiplicationContext*>(
            addition->expression(1)
        ),
        nullptr
    );
}


TEST(AssociativityTests, DivisionBindsBeforeAddition)
{
    const auto result =
        parseExpression("a + b / c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::DivisionContext*>(
            addition->expression(1)
        ),
        nullptr
    );
}


TEST(AssociativityTests, AdditionBindsAfterMultiplication)
{
    const auto result =
        parseExpression("a * b + c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::MultiplicationContext*>(
            addition->expression(0)
        ),
        nullptr
    );
}


// ============================================================
// Left associativity
// ============================================================

TEST(AssociativityTests, AdditionIsLeftAssociative)
{
    const auto result =
        parseExpression("a + b + c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    ASSERT_EQ(outer->expression().size(), 2u);

    auto* inner =
        dynamic_cast<CppParser::AdditionContext*>(
            outer->expression(0)
        );

    ASSERT_NE(inner, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::VariableIdentifierContext*>(
            outer->expression(1)
        ),
        nullptr
    );
}


TEST(AssociativityTests, SubtractionIsLeftAssociative)
{
    const auto result =
        parseExpression("a - b - c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::SubtractionContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::SubtractionContext*>(
            outer->expression(0)
        );

    ASSERT_NE(inner, nullptr);
}


TEST(AssociativityTests, MultiplicationIsLeftAssociative)
{
    const auto result =
        parseExpression("a * b * c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::MultiplicationContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::MultiplicationContext*>(
            outer->expression(0)
        );

    ASSERT_NE(inner, nullptr);
}


TEST(AssociativityTests, DivisionIsLeftAssociative)
{
    const auto result =
        parseExpression("a / b / c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::DivisionContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::DivisionContext*>(
            outer->expression(0)
        );

    ASSERT_NE(inner, nullptr);
}


// ============================================================
// Shift precedence
// ============================================================

TEST(AssociativityTests, AdditionBindsBeforeLeftShift)
{
    const auto result =
        parseExpression("a + b << c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* shift =
        dynamic_cast<CppParser::BitwiseLeftShiftContext*>(
            result.expression
        );

    ASSERT_NE(shift, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::AdditionContext*>(
            shift->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, AdditionBindsBeforeRightShift)
{
    const auto result =
        parseExpression("a + b >> c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* shift =
        dynamic_cast<CppParser::BitwiseRightShiftContext*>(
            result.expression
        );

    ASSERT_NE(shift, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::AdditionContext*>(
            shift->expression(0)
        ),
        nullptr
    );
}


// ============================================================
// Relational precedence
// ============================================================

TEST(AssociativityTests, RelationalBindsAfterArithmetic)
{
    const auto result =
        parseExpression("a + b < c * d");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* less =
        dynamic_cast<CppParser::LessThanContext*>(
            result.expression
        );

    ASSERT_NE(less, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::AdditionContext*>(
            less->expression(0)
        ),
        nullptr
    );

    EXPECT_NE(
        dynamic_cast<CppParser::MultiplicationContext*>(
            less->expression(1)
        ),
        nullptr
    );
}


TEST(AssociativityTests, RelationalOperatorsAreLeftAssociative)
{
    const auto result =
        parseExpression("a < b < c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::LessThanContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::LessThanContext*>(
            outer->expression(0)
        );

    ASSERT_NE(inner, nullptr);
}


// ============================================================
// Equality precedence
// ============================================================

TEST(AssociativityTests, EqualityBindsAfterRelational)
{
    const auto result =
        parseExpression("a < b == c < d");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* equality =
        dynamic_cast<CppParser::EqualityAttemptContext*>(
            result.expression
        );

    ASSERT_NE(equality, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::LessThanContext*>(
            equality->expression(0)
        ),
        nullptr
    );

    EXPECT_NE(
        dynamic_cast<CppParser::LessThanContext*>(
            equality->expression(1)
        ),
        nullptr
    );
}


// ============================================================
// Bitwise precedence
// ============================================================

TEST(AssociativityTests, BitwiseAndBindsAfterEquality)
{
    const auto result =
        parseExpression("a == b & c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* bitwiseAnd =
        dynamic_cast<CppParser::BitwiseAndContext*>(
            result.expression
        );

    ASSERT_NE(bitwiseAnd, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::EqualityAttemptContext*>(
            bitwiseAnd->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, BitwiseXorBindsAfterBitwiseAnd)
{
    const auto result =
        parseExpression("a & b ^ c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* xorExpression =
        dynamic_cast<CppParser::BitwiseXorContext*>(
            result.expression
        );

    ASSERT_NE(xorExpression, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::BitwiseAndContext*>(
            xorExpression->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, BitwiseOrBindsAfterBitwiseXor)
{
    const auto result =
        parseExpression("a ^ b | c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* orExpression =
        dynamic_cast<CppParser::BitwiseOrContext*>(
            result.expression
        );

    ASSERT_NE(orExpression, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::BitwiseXorContext*>(
            orExpression->expression(0)
        ),
        nullptr
    );
}


// ============================================================
// Logical precedence
// ============================================================

TEST(AssociativityTests, LogicalAndBindsBeforeLogicalOr)
{
    const auto result =
        parseExpression("a || b && c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* logicalOr =
        dynamic_cast<CppParser::LogicalOrContext*>(
            result.expression
        );

    ASSERT_NE(logicalOr, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::LogicalAndContext*>(
            logicalOr->expression(1)
        ),
        nullptr
    );
}


TEST(AssociativityTests, LogicalAndIsLeftAssociative)
{
    const auto result =
        parseExpression("a && b && c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::LogicalAndContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::LogicalAndContext*>(
            outer->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, LogicalOrIsLeftAssociative)
{
    const auto result =
        parseExpression("a || b || c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::LogicalOrContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::LogicalOrContext*>(
            outer->expression(0)
        ),
        nullptr
    );
}


// ============================================================
// Assignment associativity
// ============================================================

TEST(AssociativityTests, AssignmentIsRightAssociative)
{
    const auto result =
        parseExpression("a = b = c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::AssignmentContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    ASSERT_EQ(outer->expression().size(), 2u);

    EXPECT_NE(
        dynamic_cast<CppParser::VariableIdentifierContext*>(
            outer->expression(0)
        ),
        nullptr
    );

    auto* inner =
        dynamic_cast<CppParser::AssignmentContext*>(
            outer->expression(1)
        );

    ASSERT_NE(inner, nullptr);
}


TEST(AssociativityTests, MultiplyAssignmentIsRightAssociative)
{
    const auto result =
        parseExpression("a *= b *= c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::AssignmentMultContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::AssignmentMultContext*>(
            outer->expression(1)
        );

    ASSERT_NE(inner, nullptr);
}


TEST(AssociativityTests, PlusAssignmentIsRightAssociative)
{
    const auto result =
        parseExpression("a += b += c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::AssignmentPlusContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::AssignmentPlusContext*>(
            outer->expression(1)
        );

    ASSERT_NE(inner, nullptr);
}


TEST(AssociativityTests, MinusAssignmentIsRightAssociative)
{
    const auto result =
        parseExpression("a -= b -= c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* outer =
        dynamic_cast<CppParser::AssignmentMinusContext*>(
            result.expression
        );

    ASSERT_NE(outer, nullptr);

    auto* inner =
        dynamic_cast<CppParser::AssignmentMinusContext*>(
            outer->expression(1)
        );

    ASSERT_NE(inner, nullptr);
}


// ============================================================
// Parentheses override precedence
// ============================================================

TEST(AssociativityTests, ParenthesesOverrideAdditionAndMultiplication)
{
    const auto result =
        parseExpression("(a + b) * c");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* multiplication =
        dynamic_cast<CppParser::MultiplicationContext*>(
            result.expression
        );

    ASSERT_NE(multiplication, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::ParentExpressionContext*>(
            multiplication->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, ParenthesesCanForceAdditionRoot)
{
    const auto result =
        parseExpression("a * (b + c)");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* multiplication =
        dynamic_cast<CppParser::MultiplicationContext*>(
            result.expression
        );

    ASSERT_NE(multiplication, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::ParentExpressionContext*>(
            multiplication->expression(1)
        ),
        nullptr
    );
}


// ============================================================
// Unary precedence
// ============================================================

TEST(AssociativityTests, UnaryMinusBindsBeforeMultiplication)
{
    const auto result =
        parseExpression("-a * b");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* multiplication =
        dynamic_cast<CppParser::MultiplicationContext*>(
            result.expression
        );

    ASSERT_NE(multiplication, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::UnaryMinusContext*>(
            multiplication->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, LogicalNotBindsBeforeLogicalAnd)
{
    const auto result =
        parseExpression("!a && b");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* logicalAnd =
        dynamic_cast<CppParser::LogicalAndContext*>(
            result.expression
        );

    ASSERT_NE(logicalAnd, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::LogicalNotContext*>(
            logicalAnd->expression(0)
        ),
        nullptr
    );
}


// ============================================================
// Postfix precedence
// ============================================================

TEST(AssociativityTests, FunctionCallBindsTightly)
{
    const auto result =
        parseExpression("foo(a) + b");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::FunctionCallContext*>(
            addition->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, ArraySubscriptBindsTightly)
{
    const auto result =
        parseExpression("a[i] + b");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::ArraySubscriptContext*>(
            addition->expression(0)
        ),
        nullptr
    );
}


TEST(AssociativityTests, MemberAccessBindsTightly)
{
    const auto result =
        parseExpression("object.value + x");

    ASSERT_EQ(result.syntaxErrors, 0);
    ASSERT_NE(result.expression, nullptr);

    auto* addition =
        dynamic_cast<CppParser::AdditionContext*>(
            result.expression
        );

    ASSERT_NE(addition, nullptr);

    EXPECT_NE(
        dynamic_cast<CppParser::MemberAccessDotContext*>(
            addition->expression(0)
        ),
        nullptr
    );
}