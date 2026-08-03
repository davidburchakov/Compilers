//
// Created by David Burchakov on 8/3/26.
//

// GrammarGeneralTests.cpp


#include <gtest/gtest.h>

#include "GrammarTestUtils.h"

using GrammarTestUtils::expectInvalid;
using GrammarTestUtils::expectValid;


// ============================================================
// Basic variable declarations
// ============================================================

TEST(GrammarGeneralTests, ParsesIntegerVariable)
{
    expectValid(R"(
        int x;
    )");
}

TEST(GrammarGeneralTests, ParsesMultipleVariables)
{
    expectValid(R"(
        int x, y, z;
    )");
}

TEST(GrammarGeneralTests, ParsesInitializedVariable)
{
    expectValid(R"(
        int x = 42;
    )");
}

TEST(GrammarGeneralTests, ParsesMultipleInitializedVariables)
{
    expectValid(R"(
        int x = 1, y = 2, z = 3;
    )");
}

TEST(GrammarGeneralTests, ParsesMixedInitializedAndUninitializedVariables)
{
    expectValid(R"(
        int x, y = 10, z;
    )");
}


// ============================================================
// Primitive types
// ============================================================

TEST(GrammarGeneralTests, ParsesBool)
{
    expectValid(R"(
        bool value;
    )");
}

TEST(GrammarGeneralTests, ParsesDouble)
{
    expectValid(R"(
        double value;
    )");
}

TEST(GrammarGeneralTests, ParsesFloat)
{
    expectValid(R"(
        float value;
    )");
}

TEST(GrammarGeneralTests, ParsesVoid)
{
    expectValid(R"(
        void value;
    )");
}

TEST(GrammarGeneralTests, ParsesChar16)
{
    expectValid(R"(
        char16_t value;
    )");
}

TEST(GrammarGeneralTests, ParsesChar32)
{
    expectValid(R"(
        char32_t value;
    )");
}

TEST(GrammarGeneralTests, ParsesWideCharacter)
{
    expectValid(R"(
        wchar_t value;
    )");
}

TEST(GrammarGeneralTests, ParsesInt8Aliases)
{
    expectValid(R"(
        int8_t a;
        char b;
        BYTE c;
    )");
}

TEST(GrammarGeneralTests, ParsesInt16Aliases)
{
    expectValid(R"(
        int16_t a;
        short b;
        WORD c;
    )");
}

TEST(GrammarGeneralTests, ParsesInt32Aliases)
{
    expectValid(R"(
        int32_t a;
        int b;
        long c;
        DWORD d;
    )");
}

TEST(GrammarGeneralTests, ParsesInt64Aliases)
{
    expectValid(R"(
        int64_t a;
        longlong b;
        ll c;
        QWORD d;
    )");
}

TEST(GrammarGeneralTests, ParsesInt128Aliases)
{
    expectValid(R"(
        int128_t a;
        longlonglonglong b;
        llll c;
        OCTOWORD d;
    )");
}


// ============================================================
// Declaration modifiers
// ============================================================

TEST(GrammarGeneralTests, ParsesConstVariable)
{
    expectValid(R"(
        const int x = 42;
    )");
}

TEST(GrammarGeneralTests, ParsesVolatileVariable)
{
    expectValid(R"(
        volatile int x;
    )");
}

TEST(GrammarGeneralTests, ParsesSignedVariable)
{
    expectValid(R"(
        signed int x;
    )");
}

TEST(GrammarGeneralTests, ParsesUnsignedVariable)
{
    expectValid(R"(
        unsigned int x;
    )");
}

TEST(GrammarGeneralTests, ParsesStaticVariable)
{
    expectValid(R"(
        static int x;
    )");
}

TEST(GrammarGeneralTests, ParsesExternVariable)
{
    expectValid(R"(
        extern int x;
    )");
}

TEST(GrammarGeneralTests, ParsesThreadLocalVariable)
{
    expectValid(R"(
        thread_local int x;
    )");
}

TEST(GrammarGeneralTests, ParsesMutableVariable)
{
    expectValid(R"(
        mutable int x;
    )");
}

TEST(GrammarGeneralTests, ParsesAutoVariable)
{
    expectValid(R"(
        auto x;
    )");
}

TEST(GrammarGeneralTests, ParsesConstexprVariable)
{
    expectValid(R"(
        constexpr int x = 42;
    )");
}

TEST(GrammarGeneralTests, ParsesConstinitVariable)
{
    expectValid(R"(
        constinit int x = 42;
    )");
}

TEST(GrammarGeneralTests, ParsesMultipleDeclarationModifiers)
{
    expectValid(R"(
        static const unsigned int x = 42;
    )");
}


// ============================================================
// Pointer declarations
// ============================================================

TEST(GrammarGeneralTests, ParsesPointer)
{
    expectValid(R"(
        int* ptr;
    )");
}

TEST(GrammarGeneralTests, ParsesPointerWithInitialization)
{
    expectValid(R"(
        int* ptr = nullptr;
    )");
}

TEST(GrammarGeneralTests, ParsesPointerToPointer)
{
    expectValid(R"(
        int** ptr;
    )");
}

TEST(GrammarGeneralTests, ParsesTriplePointer)
{
    expectValid(R"(
        int*** ptr;
    )");
}


// ============================================================
// Reference declarations
// ============================================================

TEST(GrammarGeneralTests, ParsesLvalueReference)
{
    expectValid(R"(
        int& ref = value;
    )");
}

TEST(GrammarGeneralTests, ParsesRvalueReference)
{
    expectValid(R"(
        int&& ref = value;
    )");
}

TEST(GrammarGeneralTests, ParsesPointerToReferenceLikeDeclarator)
{
    expectValid(R"(
        int** ptr;
    )");
}


// ============================================================
// Array declarations
// ============================================================

TEST(GrammarGeneralTests, ParsesArray)
{
    expectValid(R"(
        int values[];
    )");
}

TEST(GrammarGeneralTests, ParsesMultipleArrays)
{
    expectValid(R"(
        int a[], b[], c[];
    )");
}

TEST(GrammarGeneralTests, ParsesMultidimensionalArray)
{
    expectValid(R"(
        int matrix[][];
    )");
}

TEST(GrammarGeneralTests, ParsesThreeDimensionalArray)
{
    expectValid(R"(
        int cube[][][];
    )");
}


// ============================================================
// Integer literals
// ============================================================

TEST(GrammarGeneralTests, ParsesZero)
{
    expectValid(R"(
        int x = 0;
    )");
}

TEST(GrammarGeneralTests, ParsesPositiveInteger)
{
    expectValid(R"(
        int x = 123456;
    )");
}

TEST(GrammarGeneralTests, ParsesNegativeInteger)
{
    expectValid(R"(
        int x = -123;
    )");
}


// ============================================================
// Floating-point literals
// ============================================================

TEST(GrammarGeneralTests, ParsesDecimalWithDigitsOnBothSides)
{
    expectValid(R"(
        double x = 12.34;
    )");
}

TEST(GrammarGeneralTests, ParsesDecimalWithoutIntegerPart)
{
    expectValid(R"(
        double x = .5;
    )");
}

TEST(GrammarGeneralTests, ParsesDecimalWithoutFractionalPart)
{
    expectValid(R"(
        double x = 5.;
    )");
}


// ============================================================
// Boolean literals
// ============================================================

TEST(GrammarGeneralTests, ParsesTrue)
{
    expectValid(R"(
        bool x = true;
    )");
}

TEST(GrammarGeneralTests, ParsesFalse)
{
    expectValid(R"(
        bool x = false;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeTrueLiteral)
{
    expectValid(R"(
        bool x = True;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeFalseLiteral)
{
    expectValid(R"(
        bool x = False;
    )");
}

TEST(GrammarGeneralTests, ParsesYesLiteral)
{
    expectValid(R"(
        bool x = yes;
    )");
}

TEST(GrammarGeneralTests, ParsesNoLiteral)
{
    expectValid(R"(
        bool x = no;
    )");
}

TEST(GrammarGeneralTests, ParsesNullptr)
{
    expectValid(R"(
        int* ptr = nullptr;
    )");
}


// ============================================================
// Character and string literals
// ============================================================

TEST(GrammarGeneralTests, ParsesCharacterLiteral)
{
    expectValid(R"(
        char c = 'A';
    )");
}

TEST(GrammarGeneralTests, ParsesEscapedCharacter)
{
    expectValid(R"(
        char c = '\n';
    )");
}

TEST(GrammarGeneralTests, ParsesStringLiteral)
{
    expectValid(R"(
        char* text = "hello";
    )");
}

TEST(GrammarGeneralTests, ParsesEscapedString)
{
    expectValid(R"(
        char* text = "hello\nworld";
    )");
}

TEST(GrammarGeneralTests, ParsesStringWithEscapedQuote)
{
    expectValid(R"(
        char* text = "say \"hello\"";
    )");
}


// ============================================================
// Basic expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesIdentifierExpression)
{
    expectValid(R"(
        int x;
        x;
    )");
}

TEST(GrammarGeneralTests, ParsesParenthesizedExpression)
{
    expectValid(R"(
        int x = (1 + 2);
    )");
}

TEST(GrammarGeneralTests, ParsesAddition)
{
    expectValid(R"(
        int x = a + b;
    )");
}

TEST(GrammarGeneralTests, ParsesSubtraction)
{
    expectValid(R"(
        int x = a - b;
    )");
}

TEST(GrammarGeneralTests, ParsesMultiplication)
{
    expectValid(R"(
        int x = a * b;
    )");
}

TEST(GrammarGeneralTests, ParsesDivision)
{
    expectValid(R"(
        int x = a / b;
    )");
}

TEST(GrammarGeneralTests, ParsesModulo)
{
    expectValid(R"(
        int x = a % b;
    )");
}

TEST(GrammarGeneralTests, ParsesModuloAlternativeKeyword)
{
    expectValid(R"(
        int x = a mod b;
    )");
}


// ============================================================
// Comparison expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesLessThan)
{
    expectValid(R"(
        bool result = a < b;
    )");
}

TEST(GrammarGeneralTests, ParsesGreaterThan)
{
    expectValid(R"(
        bool result = a > b;
    )");
}

TEST(GrammarGeneralTests, ParsesLessThanOrEqual)
{
    expectValid(R"(
        bool result = a <= b;
    )");
}

TEST(GrammarGeneralTests, ParsesGreaterThanOrEqual)
{
    expectValid(R"(
        bool result = a >= b;
    )");
}

TEST(GrammarGeneralTests, ParsesEquality)
{
    expectValid(R"(
        bool result = a == b;
    )");
}

TEST(GrammarGeneralTests, ParsesInequality)
{
    expectValid(R"(
        bool result = a != b;
    )");
}


// ============================================================
// Logical expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesLogicalAnd)
{
    expectValid(R"(
        bool result = a && b;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeLogicalAnd)
{
    expectValid(R"(
        bool result = a and b;
    )");
}

TEST(GrammarGeneralTests, ParsesLogicalOr)
{
    expectValid(R"(
        bool result = a || b;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeLogicalOr)
{
    expectValid(R"(
        bool result = a or b;
    )");
}

TEST(GrammarGeneralTests, ParsesLogicalNot)
{
    expectValid(R"(
        bool result = !value;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeLogicalNot)
{
    expectValid(R"(
        bool result = not value;
    )");
}


// ============================================================
// Bitwise expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesBitwiseAnd)
{
    expectValid(R"(
        int result = a & b;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeBitwiseAnd)
{
    expectValid(R"(
        int result = a bitand b;
    )");
}

TEST(GrammarGeneralTests, ParsesBitwiseOr)
{
    expectValid(R"(
        int result = a | b;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeBitwiseOr)
{
    expectValid(R"(
        int result = a bitor b;
    )");
}

TEST(GrammarGeneralTests, ParsesBitwiseXor)
{
    expectValid(R"(
        int result = a ^ b;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeBitwiseXor)
{
    expectValid(R"(
        int result = a xor b;
    )");
}

TEST(GrammarGeneralTests, ParsesBitwiseNot)
{
    expectValid(R"(
        int result = ~value;
    )");
}

TEST(GrammarGeneralTests, ParsesAlternativeBitwiseNot)
{
    expectValid(R"(
        int result = compl value;
    )");
}


// ============================================================
// Shift expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesLeftShift)
{
    expectValid(R"(
        int x = value << 2;
    )");
}

TEST(GrammarGeneralTests, ParsesRightShift)
{
    expectValid(R"(
        int x = value >> 2;
    )");
}


// ============================================================
// Unary expressions
// ============================================================

TEST(GrammarGeneralTests, ParsesUnaryPlus)
{
    expectValid(R"(
        int x = +value;
    )");
}

TEST(GrammarGeneralTests, ParsesUnaryMinus)
{
    expectValid(R"(
        int x = -value;
    )");
}

TEST(GrammarGeneralTests, ParsesAddressOf)
{
    expectValid(R"(
        int* p = &value;
    )");
}

TEST(GrammarGeneralTests, ParsesDereference)
{
    expectValid(R"(
        int x = *ptr;
    )");
}

TEST(GrammarGeneralTests, ParsesPreIncrement)
{
    expectValid(R"(
        ++value;
    )");
}

TEST(GrammarGeneralTests, ParsesPreDecrement)
{
    expectValid(R"(
        --value;
    )");
}

TEST(GrammarGeneralTests, ParsesPostIncrement)
{
    expectValid(R"(
        value++;
    )");
}

TEST(GrammarGeneralTests, ParsesPostDecrement)
{
    expectValid(R"(
        value--;
    )");
}

TEST(GrammarGeneralTests, ParsesSizeof)
{
    expectValid(R"(
        int x = sizeof value;
    )");
}

TEST(GrammarGeneralTests, ParsesNew)
{
    expectValid(R"(
        int* x = new int;
    )");
}

TEST(GrammarGeneralTests, ParsesDelete)
{
    expectValid(R"(
        delete ptr;
    )");
}

TEST(GrammarGeneralTests, ParsesThis)
{
    expectValid(R"(
        this;
    )");
}


// ============================================================
// Function calls
// ============================================================

TEST(GrammarGeneralTests, ParsesFunctionCall)
{
    expectValid(R"(
        foo();
    )");
}

TEST(GrammarGeneralTests, ParsesFunctionCallWithOneArgument)
{
    expectValid(R"(
        foo(x);
    )");
}

TEST(GrammarGeneralTests, ParsesFunctionCallWithMultipleArguments)
{
    expectValid(R"(
        foo(x, y, z);
    )");
}

TEST(GrammarGeneralTests, ParsesNestedFunctionCall)
{
    expectValid(R"(
        foo(bar(x));
    )");
}

TEST(GrammarGeneralTests, ParsesFunctionCallWithExpressions)
{
    expectValid(R"(
        foo(a + b, x * y, value == 42);
    )");
}


// ============================================================
// Member access
// ============================================================

TEST(GrammarGeneralTests, ParsesDotMemberAccess)
{
    expectValid(R"(
        object.value;
    )");
}

TEST(GrammarGeneralTests, ParsesArrowMemberAccess)
{
    expectValid(R"(
        object->value;
    )");
}

TEST(GrammarGeneralTests, ParsesChainedDotMemberAccess)
{
    expectValid(R"(
        object.first.second;
    )");
}

TEST(GrammarGeneralTests, ParsesChainedArrowMemberAccess)
{
    expectValid(R"(
        object->first->second;
    )");
}


// ============================================================
// Array access
// ============================================================

TEST(GrammarGeneralTests, ParsesArraySubscript)
{
    expectValid(R"(
        int x = values[index];
    )");
}

TEST(GrammarGeneralTests, ParsesNestedArraySubscript)
{
    expectValid(R"(
        int x = matrix[i][j];
    )");
}

TEST(GrammarGeneralTests, ParsesArraySubscriptExpression)
{
    expectValid(R"(
        int x = values[i + 1];
    )");
}


// ============================================================
// Assignment operators
// ============================================================

TEST(GrammarGeneralTests, ParsesAssignment)
{
    expectValid(R"(
        x = 10;
    )");
}

TEST(GrammarGeneralTests, ParsesMultiplyAssignment)
{
    expectValid(R"(
        x *= 10;
    )");
}

TEST(GrammarGeneralTests, ParsesDivideAssignment)
{
    expectValid(R"(
        x /= 10;
    )");
}

TEST(GrammarGeneralTests, ParsesPlusAssignment)
{
    expectValid(R"(
        x += 10;
    )");
}

TEST(GrammarGeneralTests, ParsesMinusAssignment)
{
    expectValid(R"(
        x -= 10;
    )");
}

TEST(GrammarGeneralTests, ParsesAndAssignment)
{
    expectValid(R"(
        x &= 10;
    )");
}

TEST(GrammarGeneralTests, ParsesOrAssignment)
{
    expectValid(R"(
        x |= 10;
    )");
}

TEST(GrammarGeneralTests, ParsesXorAssignment)
{
    expectValid(R"(
        x ^= 10;
    )");
}


// ============================================================
// Comments
// ============================================================

TEST(GrammarGeneralTests, IgnoresLineComment)
{
    expectValid(R"(
        // this is a comment
        int x;
    )");
}

TEST(GrammarGeneralTests, IgnoresBlockComment)
{
    expectValid(R"(
        /*
         * multiline comment
         */
        int x;
    )");
}

TEST(GrammarGeneralTests, IgnoresInlineComment)
{
    expectValid(R"(
        int x; // comment
    )");
}


// ============================================================
// Invalid general syntax
// ============================================================

TEST(GrammarGeneralTests, RejectsMissingSemicolon)
{
    expectInvalid(R"(
        int x
    )");
}

TEST(GrammarGeneralTests, RejectsMissingVariableName)
{
    expectInvalid(R"(
        int;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingInitializerExpression)
{
    expectInvalid(R"(
        int x = ;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingRightParenthesis)
{
    expectInvalid(R"(
        foo(1, 2;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingRightBracket)
{
    expectInvalid(R"(
        int x = values[0;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingExpressionAfterBinaryOperator)
{
    expectInvalid(R"(
        int x = a +;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingOperand)
{
    expectInvalid(R"(
        int x = *;
    )");
}

TEST(GrammarGeneralTests, RejectsMissingClosingBrace)
{
    expectInvalid(R"(
        int foo() {
            int x;
    )");
}