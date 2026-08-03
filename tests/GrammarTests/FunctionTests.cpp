//
// Created by David Burchakov on 8/3/26.
//

// FunctionTests.cpp


#include <gtest/gtest.h>

#include "GrammarTestUtils.h"

using GrammarTestUtils::expectInvalid;
using GrammarTestUtils::expectValid;


// ============================================================
// Function declarations
// ============================================================

TEST(FunctionTests, ParsesFunctionDeclaration)
{
    expectValid(R"(
        int add(int x, int y);
    )");
}

TEST(FunctionTests, ParsesVoidFunctionDeclaration)
{
    expectValid(R"(
        void print();
    )");
}

TEST(FunctionTests, ParsesFunctionWithOneParameter)
{
    expectValid(R"(
        int square(int x);
    )");
}

TEST(FunctionTests, ParsesFunctionWithManyParameters)
{
    expectValid(R"(
        int calculate(int a, double b, float c, bool enabled);
    )");
}

TEST(FunctionTests, ParsesFunctionWithNoParameters)
{
    expectValid(R"(
        int getValue();
    )");
}

TEST(FunctionTests, ParsesMultipleFunctionDeclarations)
{
    expectValid(R"(
        int add(int x, int y);
        int subtract(int x, int y);
        int multiply(int x, int y);
    )");
}


// ============================================================
// Function definitions
// ============================================================

TEST(FunctionTests, ParsesEmptyFunction)
{
    expectValid(R"(
        void foo() {
        }
    )");
}

TEST(FunctionTests, ParsesFunctionReturningInteger)
{
    expectValid(R"(
        int getValue() {
            return 42;
        }
    )");
}

TEST(FunctionTests, ParsesFunctionReturningExpression)
{
    expectValid(R"(
        int add(int x, int y) {
            return x + y;
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithLocalVariable)
{
    expectValid(R"(
        int calculate(int x) {
            int y = x + 10;
            return y;
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithMultipleLocalVariables)
{
    expectValid(R"(
        int calculate(int x, int y) {
            int a = x + y;
            int b = a * 2;
            int c = b - 1;
            return c;
        }
    )");
}

TEST(FunctionTests, ParsesMultipleFunctions)
{
    expectValid(R"(
        int add(int x, int y) {
            return x + y;
        }

        int subtract(int x, int y) {
            return x - y;
        }

        int multiply(int x, int y) {
            return x * y;
        }
    )");
}


// ============================================================
// Return statements
// ============================================================

TEST(FunctionTests, ParsesReturnWithoutExpression)
{
    expectValid(R"(
        void stop() {
            return;
        }
    )");
}

TEST(FunctionTests, ParsesReturnInteger)
{
    expectValid(R"(
        int get() {
            return 42;
        }
    )");
}

TEST(FunctionTests, ParsesReturnVariable)
{
    expectValid(R"(
        int get(int value) {
            return value;
        }
    )");
}

TEST(FunctionTests, ParsesReturnExpression)
{
    expectValid(R"(
        int get(int value) {
            return value + 1;
        }
    )");
}

TEST(FunctionTests, ParsesMultipleReturns)
{
    expectValid(R"(
        int test(int x) {
            int result = x;
            result += 1;
            return result;
        }
    )");
}


// ============================================================
// Function prefix specifiers
// ============================================================

TEST(FunctionTests, ParsesInlineFunction)
{
    expectValid(R"(
        inline int add(int x, int y) {
            return x + y;
        }
    )");
}

TEST(FunctionTests, ParsesConstexprFunction)
{
    expectValid(R"(
        constexpr int square(int x) {
            return x * x;
        }
    )");
}

TEST(FunctionTests, ParsesConstevalFunction)
{
    expectValid(R"(
        consteval int square(int x) {
            return x * x;
        }
    )");
}

TEST(FunctionTests, ParsesConstinitFunction)
{
    expectValid(R"(
        constinit int value() {
            return 42;
        }
    )");
}

TEST(FunctionTests, ParsesVirtualFunction)
{
    expectValid(R"(
        virtual int getValue() {
            return 42;
        }
    )");
}

TEST(FunctionTests, ParsesExplicitFunction)
{
    expectValid(R"(
        explicit int convert(int x) {
            return x;
        }
    )");
}

TEST(FunctionTests, ParsesMultiplePrefixSpecifiers)
{
    expectValid(R"(
        inline constexpr int calculate(int x) {
            return x * 2;
        }
    )");
}


// ============================================================
// Function postfix specifiers
// ============================================================

TEST(FunctionTests, ParsesNoexceptFunction)
{
    expectValid(R"(
        int foo() noexcept {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesConstFunction)
{
    expectValid(R"(
        int foo() const {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesVolatileFunction)
{
    expectValid(R"(
        int foo() volatile {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesFinalFunction)
{
    expectValid(R"(
        int foo() final {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesOverrideFunction)
{
    expectValid(R"(
        int foo() override {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesMultiplePostfixSpecifiers)
{
    expectValid(R"(
        int foo() const noexcept {
            return 1;
        }
    )");
}


// ============================================================
// Function parameter types
// ============================================================

TEST(FunctionTests, ParsesBoolParameter)
{
    expectValid(R"(
        int test(bool value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesDoubleParameter)
{
    expectValid(R"(
        int test(double value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesFloatParameter)
{
    expectValid(R"(
        int test(float value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesCharParameter)
{
    expectValid(R"(
        int test(char value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesWideCharacterParameter)
{
    expectValid(R"(
        int test(wchar_t value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, ParsesMixedParameterTypes)
{
    expectValid(R"(
        int test(
            int a,
            double b,
            float c,
            bool d,
            char16_t e,
            char32_t f,
            wchar_t g
        ) {
            return 1;
        }
    )");
}


// ============================================================
// Function bodies
// ============================================================

TEST(FunctionTests, ParsesFunctionWithPointerVariable)
{
    expectValid(R"(
        int test(int value) {
            int* ptr = &value;
            return *ptr;
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithArrayAccess)
{
    expectValid(R"(
        int test(int index) {
            int values[];
            return values[index];
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithFunctionCall)
{
    expectValid(R"(
        int test(int x) {
            return calculate(x);
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithNestedExpression)
{
    expectValid(R"(
        int test(int x, int y) {
            return (x + y) * (x - y);
        }
    )");
}

TEST(FunctionTests, ParsesFunctionWithAssignments)
{
    expectValid(R"(
        int test(int x) {
            x += 10;
            x *= 2;
            return x;
        }
    )");

}


// ============================================================
// Function syntax errors
// ============================================================

TEST(FunctionTests, RejectsMissingFunctionName)
{
    expectInvalid(R"(
        int () {
            return 1;
        }
    )");
}

TEST(FunctionTests, RejectsMissingParameterName)
{
    expectInvalid(R"(
        int foo(int) {
            return 1;
        }
    )");
}

TEST(FunctionTests, RejectsMissingParameterType)
{
    expectInvalid(R"(
        int foo(value) {
            return 1;
        }
    )");
}

TEST(FunctionTests, RejectsMissingRightParenthesis)
{
    expectInvalid(R"(
        int foo(int x {
            return x;
        }
    )");
}

TEST(FunctionTests, RejectsMissingFunctionBody)
{
    expectInvalid(R"(
        int foo()
    )");
}

TEST(FunctionTests, RejectsMissingFunctionClosingBrace)
{
    expectInvalid(R"(
        int foo() {
            return 1;
    )");
}

TEST(FunctionTests, RejectsMissingReturnSemicolon)
{
    expectInvalid(R"(
        int foo() {
            return 1
        }
    )");
}

TEST(FunctionTests, RejectsMissingParameterComma)
{
    expectInvalid(R"(
        int foo(int x int y) {
            return x + y;
        }
    )");
}