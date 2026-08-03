#include <gtest/gtest.h>

#include <functional>
#include <string>
#include <vector>

#include "antlr4-runtime.h"
#include "CppLexer.h"
#include "CppParser.h"

namespace {
    using Expression = CppParser::ExpressionContext;

    void expectValidExpression(const std::string &source, const std::function<void(Expression *)> &inspect) {
        antlr4::ANTLRInputStream input(source);
        CppLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        CppParser parser(&tokens);
        Expression *expression = parser.expression();

        ASSERT_NE(expression, nullptr);
        ASSERT_EQ(parser.getNumberOfSyntaxErrors(), 0) << "Expression failed to parse:\n" << source;

        inspect(expression);
    }

    template<typename T>
    T *expectType(Expression *expression) {
        T *result = dynamic_cast<T *>(expression);
        EXPECT_NE(result, nullptr);
        return result;
    }

    template<typename T>
    T *expectChild(const std::vector<Expression *> &children, size_t index) {
        if (index >= children.size()) {
            ADD_FAILURE() << "Expected child at index " << index << ", but only " << children.size() <<
 " children exist.";
            return nullptr;
        }

        T *result = dynamic_cast<T *>(children[index]);
        EXPECT_NE(result, nullptr);
        return result;
    }

    void expectVariable(Expression *expression, const std::string &name) {
        auto *variable = expectType<CppParser::VariableIdentifierContext>(expression);
        if (variable != nullptr) {
            EXPECT_EQ(variable->getText(), name);
        }
    }

    void expectInteger(Expression *expression, const std::string &value) {
        auto *integer = expectType<CppParser::IntLiteralContext>(expression);
        if (integer != nullptr) {
            EXPECT_EQ(integer->getText(), value);
        }
    }

    template<typename T>
    void expectBinary(Expression *expression) {
        auto *binary = expectType<T>(expression);
        if (binary != nullptr) {
            EXPECT_EQ(binary->expression().size(), 2u);
        }
    }
} // namespace

// ============================================================================
// Multiplicative precedence
// ============================================================================

TEST(AssociativityTests, MultiplicationBindsBeforeAddition) {
    expectValidExpression("a + b * c", [](Expression *expression) {
        auto *addition = expectType<CppParser::AdditionContext>(expression);
        if (addition == nullptr) return;

        ASSERT_EQ(addition->expression().size(), 2u);

        expectVariable(addition->expression(0), "a");
        auto *multiplication = expectChild<CppParser::MultiplicationContext>(addition->expression(), 1);

        if (multiplication != nullptr) {
            ASSERT_EQ(multiplication->expression().size(), 2u);
            expectVariable(multiplication->expression(0), "b");
            expectVariable(multiplication->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, DivisionBindsBeforeAddition) {
    expectValidExpression("a + b / c", [](Expression *expression) {
        auto *addition = expectType<CppParser::AdditionContext>(expression);
        if (addition == nullptr) return;

        ASSERT_EQ(addition->expression().size(), 2u);

        expectVariable(addition->expression(0), "a");
        auto *division = expectChild<CppParser::DivisionContext>(addition->expression(), 1);

        if (division != nullptr) {
            ASSERT_EQ(division->expression().size(), 2u);
            expectVariable(division->expression(0), "b");
            expectVariable(division->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, ModuloBindsBeforeAddition) {
    expectValidExpression("a + b % c", [](Expression *expression) {
        auto *addition = expectType<CppParser::AdditionContext>(expression);
        if (addition == nullptr) return;

        ASSERT_EQ(addition->expression().size(), 2u);

        expectVariable(addition->expression(0), "a");
        auto *modulo = expectChild<CppParser::ModuloContext>(addition->expression(), 1);

        if (modulo != nullptr) {
            ASSERT_EQ(modulo->expression().size(), 2u);
            expectVariable(modulo->expression(0), "b");
            expectVariable(modulo->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, MultiplicationBindsBeforeSubtraction) {
    expectValidExpression("a - b * c", [](Expression *expression) {
        auto *subtraction = expectType<CppParser::SubtractionContext>(expression);
        if (subtraction == nullptr) return;

        ASSERT_EQ(subtraction->expression().size(), 2u);

        expectVariable(subtraction->expression(0), "a");
        auto *multiplication = expectChild<CppParser::MultiplicationContext>(subtraction->expression(), 1);

        if (multiplication != nullptr) {
            expectVariable(multiplication->expression(0), "b");
            expectVariable(multiplication->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, MultiplicationBindsBeforeShift) {
    expectValidExpression("a << b * c", [](Expression *expression) {
        auto *shift = expectType<CppParser::BitwiseLeftShiftContext>(expression);
        if (shift == nullptr) return;

        ASSERT_EQ(shift->expression().size(), 2u);

        expectVariable(shift->expression(0), "a");
        auto *multiplication = expectChild<CppParser::MultiplicationContext>(shift->expression(), 1);

        if (multiplication != nullptr) {
            expectVariable(multiplication->expression(0), "b");
            expectVariable(multiplication->expression(1), "c");
        }
    });
}

// ============================================================================
// Multiplication / division / modulo left associativity
// ============================================================================

TEST(AssociativityTests, MultiplicationIsLeftAssociative) {
    expectValidExpression("a * b * c", [](Expression *expression) {
        auto *outer = expectType<CppParser::MultiplicationContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::MultiplicationContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, DivisionIsLeftAssociative) {
    expectValidExpression("a / b / c", [](Expression *expression) {
        auto *outer = expectType<CppParser::DivisionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::DivisionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, ModuloIsLeftAssociative) {
    expectValidExpression("a % b % c", [](Expression *expression) {
        auto *outer = expectType<CppParser::ModuloContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::ModuloContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Addition / subtraction left associativity
// ============================================================================

TEST(AssociativityTests, AdditionIsLeftAssociative) {
    expectValidExpression("a + b + c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AdditionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::AdditionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, SubtractionIsLeftAssociative) {
    expectValidExpression("a - b - c", [](Expression *expression) {
        auto *outer = expectType<CppParser::SubtractionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::SubtractionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Mixed multiplicative operators
// ============================================================================

TEST(AssociativityTests, MultiplicationAndDivisionAreSamePrecedence) {
    expectValidExpression("a * b / c", [](Expression *expression) {
        auto *outer = expectType<CppParser::DivisionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::MultiplicationContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, DivisionAndMultiplicationAreSamePrecedence) {
    expectValidExpression("a / b * c", [](Expression *expression) {
        auto *outer = expectType<CppParser::MultiplicationContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::DivisionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, MultiplicationAndModuloAreSamePrecedence) {
    expectValidExpression("a * b % c", [](Expression *expression) {
        auto *outer = expectType<CppParser::ModuloContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::MultiplicationContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, DivisionAndModuloAreSamePrecedence) {
    expectValidExpression("a / b % c", [](Expression *expression) {
        auto *outer = expectType<CppParser::ModuloContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::DivisionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Addition/subtraction mixed associativity
// ============================================================================

TEST(AssociativityTests, AdditionAndSubtractionAreSamePrecedence) {
    expectValidExpression("a + b - c", [](Expression *expression) {
        auto *outer = expectType<CppParser::SubtractionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::AdditionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, SubtractionAndAdditionAreSamePrecedence) {
    expectValidExpression("a - b + c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AdditionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::SubtractionContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Multiplicative vs additive precedence
// ============================================================================

TEST(AssociativityTests, AdditionHasLowerPrecedenceThanMultiplication) {
    expectValidExpression("a * b + c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AdditionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *multiplication = expectChild<CppParser::MultiplicationContext>(outer->expression(), 0);

        if (multiplication != nullptr) {
            expectVariable(multiplication->expression(0), "a");
            expectVariable(multiplication->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, SubtractionHasLowerPrecedenceThanDivision) {
    expectValidExpression("a / b - c", [](Expression *expression) {
        auto *outer = expectType<CppParser::SubtractionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *division = expectChild<CppParser::DivisionContext>(outer->expression(), 0);

        if (division != nullptr) {
            expectVariable(division->expression(0), "a");
            expectVariable(division->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, ModuloHasHigherPrecedenceThanAddition) {
    expectValidExpression("a + b % c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AdditionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *modulo = expectChild<CppParser::ModuloContext>(outer->expression(), 1);

        if (modulo != nullptr) {
            expectVariable(modulo->expression(0), "b");
            expectVariable(modulo->expression(1), "c");
        }

        expectVariable(outer->expression(0), "a");
    });
}

// ============================================================================
// Shift precedence
// ============================================================================

TEST(AssociativityTests, ShiftIsLeftAssociative) {
    expectValidExpression("a << b << c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseLeftShiftContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::BitwiseLeftShiftContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, RightShiftIsLeftAssociative) {
    expectValidExpression("a >> b >> c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseRightShiftContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::BitwiseRightShiftContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, AdditionBindsBeforeShift) {
    expectValidExpression("a + b << c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseLeftShiftContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *addition = expectChild<CppParser::AdditionContext>(outer->expression(), 0);

        if (addition != nullptr) {
            expectVariable(addition->expression(0), "a");
            expectVariable(addition->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, ShiftBindsBeforeRelationalComparison) {
    expectValidExpression("a << b < c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LessThanContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *shift = expectChild<CppParser::BitwiseLeftShiftContext>(outer->expression(), 0);

        if (shift != nullptr) {
            expectVariable(shift->expression(0), "a");
            expectVariable(shift->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Relational precedence
// ============================================================================

TEST(AssociativityTests, RelationalOperatorsAreLeftAssociative) {
    expectValidExpression("a < b < c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LessThanContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::LessThanContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, GreaterThanOperatorsAreLeftAssociative) {
    expectValidExpression("a > b > c", [](Expression *expression) {
        auto *outer = expectType<CppParser::GreaterThanContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::GreaterThanContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Equality precedence
// ============================================================================

TEST(AssociativityTests, EqualityBindsAfterRelational) {
    expectValidExpression("a < b == c", [](Expression *expression) {
        auto *outer = expectType<CppParser::EqualityAttemptContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *lessThan = expectChild<CppParser::LessThanContext>(outer->expression(), 0);

        if (lessThan != nullptr) {
            expectVariable(lessThan->expression(0), "a");
            expectVariable(lessThan->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, InequalityBindsAfterRelational) {
    expectValidExpression("a > b != c", [](Expression *expression) {
        auto *outer = expectType<CppParser::InequalityAttemptContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *greaterThan = expectChild<CppParser::GreaterThanContext>(outer->expression(), 0);

        if (greaterThan != nullptr) {
            expectVariable(greaterThan->expression(0), "a");
            expectVariable(greaterThan->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Bitwise precedence
// ============================================================================

TEST(AssociativityTests, BitwiseAndIsLeftAssociative) {
    expectValidExpression("a & b & c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseAndContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::BitwiseAndContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, BitwiseXorIsLeftAssociative) {
    expectValidExpression("a ^ b ^ c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseXorContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::BitwiseXorContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, BitwiseOrIsLeftAssociative) {
    expectValidExpression("a | b | c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::BitwiseOrContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, BitwiseAndBindsBeforeBitwiseXor) {
    expectValidExpression("a & b ^ c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseXorContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *bitwiseAnd = expectChild<CppParser::BitwiseAndContext>(outer->expression(), 0);

        if (bitwiseAnd != nullptr) {
            expectVariable(bitwiseAnd->expression(0), "a");
            expectVariable(bitwiseAnd->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, BitwiseXorBindsBeforeBitwiseOr) {
    expectValidExpression("a ^ b | c", [](Expression *expression) {
        auto *outer = expectType<CppParser::BitwiseOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *bitwiseXor = expectChild<CppParser::BitwiseXorContext>(outer->expression(), 0);

        if (bitwiseXor != nullptr) {
            expectVariable(bitwiseXor->expression(0), "a");
            expectVariable(bitwiseXor->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Logical precedence
// ============================================================================

TEST(AssociativityTests, LogicalAndIsLeftAssociative) {
    expectValidExpression("a && b && c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LogicalAndContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::LogicalAndContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, LogicalOrIsLeftAssociative) {
    expectValidExpression("a || b || c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LogicalOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *inner = expectChild<CppParser::LogicalOrContext>(outer->expression(), 0);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "a");
            expectVariable(inner->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, LogicalAndBindsBeforeLogicalOr) {
    expectValidExpression("a && b || c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LogicalOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *logicalAnd = expectChild<CppParser::LogicalAndContext>(outer->expression(), 0);

        if (logicalAnd != nullptr) {
            expectVariable(logicalAnd->expression(0), "a");
            expectVariable(logicalAnd->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, BitwiseOrBindsBeforeLogicalAnd) {
    expectValidExpression("a | b && c", [](Expression *expression) {
        auto *outer = expectType<CppParser::LogicalAndContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *bitwiseOr = expectChild<CppParser::BitwiseOrContext>(outer->expression(), 0);

        if (bitwiseOr != nullptr) {
            expectVariable(bitwiseOr->expression(0), "a");
            expectVariable(bitwiseOr->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Assignment associativity
// ============================================================================

TEST(AssociativityTests, AssignmentIsRightAssociative) {
    expectValidExpression("a = b = c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, MultiplicationAssignmentIsRightAssociative) {
    expectValidExpression("a *= b *= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentMultContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentMultContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, DivisionAssignmentIsRightAssociative) {
    expectValidExpression("a /= b /= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentDivisionContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentDivisionContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, AdditionAssignmentIsRightAssociative) {
    expectValidExpression("a += b += c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentPlusContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentPlusContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, SubtractionAssignmentIsRightAssociative) {
    expectValidExpression("a -= b -= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentMinusContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentMinusContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, AndAssignmentIsRightAssociative) {
    expectValidExpression("a &= b &= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentAndContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentAndContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, OrAssignmentIsRightAssociative) {
    expectValidExpression("a |= b |= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentOrContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

TEST(AssociativityTests, XorAssignmentIsRightAssociative) {
    expectValidExpression("a ^= b ^= c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentXorContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *inner = expectChild<CppParser::AssignmentXorContext>(outer->expression(), 1);

        if (inner != nullptr) {
            expectVariable(inner->expression(0), "b");
            expectVariable(inner->expression(1), "c");
        }
    });
}

// ============================================================================
// Assignment has lower precedence than logical operators
// ============================================================================

TEST(AssociativityTests, LogicalOrBindsBeforeAssignment) {
    expectValidExpression("a || b = c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *logicalOr = expectChild<CppParser::LogicalOrContext>(outer->expression(), 0);

        if (logicalOr != nullptr) {
            expectVariable(logicalOr->expression(0), "a");
            expectVariable(logicalOr->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, AdditionBindsBeforeAssignment) {
    expectValidExpression("a + b = c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *addition = expectChild<CppParser::AdditionContext>(outer->expression(), 0);

        if (addition != nullptr) {
            expectVariable(addition->expression(0), "a");
            expectVariable(addition->expression(1), "b");
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Parentheses override precedence
// ============================================================================

TEST(AssociativityTests, ParenthesesOverrideMultiplicationPrecedence) {
    expectValidExpression("(a + b) * c", [](Expression *expression) {
        auto *outer = expectType<CppParser::MultiplicationContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *parent = expectChild<CppParser::ParentExpressionContext>(outer->expression(), 0);

        if (parent != nullptr) {
            auto *addition = expectType<CppParser::AdditionContext>(parent->expression());
            if (addition != nullptr) {
                expectVariable(addition->expression(0), "a");
                expectVariable(addition->expression(1), "b");
            }
        }

        expectVariable(outer->expression(1), "c");
    });
}

TEST(AssociativityTests, ParenthesesOverrideAdditionPrecedence) {
    expectValidExpression("a * (b + c)", [](Expression *expression) {
        auto *outer = expectType<CppParser::MultiplicationContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        expectVariable(outer->expression(0), "a");

        auto *parent = expectChild<CppParser::ParentExpressionContext>(outer->expression(), 1);

        if (parent != nullptr) {
            auto *addition = expectType<CppParser::AdditionContext>(parent->expression());
            if (addition != nullptr) {
                expectVariable(addition->expression(0), "b");
                expectVariable(addition->expression(1), "c");
            }
        }
    });
}

TEST(AssociativityTests, ParenthesesOverrideAssignmentAssociativity) {
    expectValidExpression("(a = b) = c", [](Expression *expression) {
        auto *outer = expectType<CppParser::AssignmentContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *parent = expectChild<CppParser::ParentExpressionContext>(outer->expression(), 0);

        if (parent != nullptr) {
            auto *assignment = expectType<CppParser::AssignmentContext>(parent->expression());
            if (assignment != nullptr) {
                expectVariable(assignment->expression(0), "a");
                expectVariable(assignment->expression(1), "b");
            }
        }

        expectVariable(outer->expression(1), "c");
    });
}

// ============================================================================
// Deep precedence chains
// ============================================================================

TEST(AssociativityTests, FullArithmeticPrecedenceChain) {
    expectValidExpression("a + b * c - d / e", [](Expression *expression) {
        auto *subtraction = expectType<CppParser::SubtractionContext>(expression);
        if (subtraction == nullptr) return;

        ASSERT_EQ(subtraction->expression().size(), 2u);

        auto *addition = expectChild<CppParser::AdditionContext>(subtraction->expression(), 0);

        if (addition != nullptr) {
            expectVariable(addition->expression(0), "a");

            auto *multiplication = expectChild<CppParser::MultiplicationContext>(addition->expression(), 1);

            if (multiplication != nullptr) {
                expectVariable(multiplication->expression(0), "b");
                expectVariable(multiplication->expression(1), "c");
            }
        }

        auto *division = expectChild<CppParser::DivisionContext>(subtraction->expression(), 1);

        if (division != nullptr) {
            expectVariable(division->expression(0), "d");
            expectVariable(division->expression(1), "e");
        }
    });
}

TEST(AssociativityTests, FullLogicalPrecedenceChain) {
    expectValidExpression("a || b && c || d", [](Expression *expression) {
        auto *outer = expectType<CppParser::LogicalOrContext>(expression);
        if (outer == nullptr) return;

        ASSERT_EQ(outer->expression().size(), 2u);

        auto *left = expectChild<CppParser::LogicalOrContext>(outer->expression(), 0);

        if (left != nullptr) {
            expectVariable(left->expression(0), "a");

            auto *logicalAnd = expectChild<CppParser::LogicalAndContext>(left->expression(), 1);

            if (logicalAnd != nullptr) {
                expectVariable(logicalAnd->expression(0), "b");
                expectVariable(logicalAnd->expression(1), "c");
            }
        }

        expectVariable(outer->expression(1), "d");
    });
}

TEST(AssociativityTests, FullBitwisePrecedenceChain) {
    expectValidExpression("a & b ^ c | d", [](Expression *expression) {
        auto *bitwiseOr = expectType<CppParser::BitwiseOrContext>(expression);
        if (bitwiseOr == nullptr) return;

        ASSERT_EQ(bitwiseOr->expression().size(), 2u);

        auto *bitwiseXor = expectChild<CppParser::BitwiseXorContext>(bitwiseOr->expression(), 0);

        if (bitwiseXor != nullptr) {
            auto *bitwiseAnd = expectChild<CppParser::BitwiseAndContext>(bitwiseXor->expression(), 0);

            if (bitwiseAnd != nullptr) {
                expectVariable(bitwiseAnd->expression(0), "a");
                expectVariable(bitwiseAnd->expression(1), "b");
            }

            expectVariable(bitwiseXor->expression(1), "c");
        }

        expectVariable(bitwiseOr->expression(1), "d");
    });
}
