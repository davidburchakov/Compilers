//
// Created by incidence on 6/23/26.
//

module;

// Global Module Fragment: Include non-module ANTLR runtime headers
#include "antlr4-runtime.h"
#include "CppBaseVisitor.h"
#include "absl/status/status.h"
#include "CppParser.h"
#include <string>
#include <any>

#include "absl/strings/str_cat.h"


export module SymbolTableVisitorModule;

import SymbolTableModule;
import Reports;

namespace CppZero {
    export class SymbolTableVisitor : public CppBaseVisitor {
    private:
        SymbolTable &symbolTable;
        Reports<Report> &reports;
        // Recursive helper to unwrap the nested declarator rule variants (*, &, &&, [])
        void unwindDeclarator(CppParser::DeclaratorContext *ctx, Symbol &outSymbol) {
            if (!ctx) return;

            // 1. Base Case: Reached the core variable name
            if (auto baseCtx = dynamic_cast<CppParser::BaseIdentifierContext *>(ctx)) {
                outSymbol.name = baseCtx->IDENTIFIER()->getText();
                outSymbol.declaration_line = baseCtx->IDENTIFIER()->getSymbol()->getLine();
                return;
            }

            // 2. Recursive Case: Pointer (*)
            if (auto ptrCtx = dynamic_cast<CppParser::PointerModifierContext *>(ctx)) {
                outSymbol.type.pointer_count++;
                unwindDeclarator(ptrCtx->declarator(), outSymbol);
                return;
            }

            // 3. Recursive Case: Lvalue Reference (&)
            if (auto lrefCtx = dynamic_cast<CppParser::LvalueRefModifierContext *>(ctx)) {
                outSymbol.type.is_lvalue_reference = true;
                unwindDeclarator(lrefCtx->declarator(), outSymbol);
                return;
            }

            // 4. Recursive Case: Rvalue Reference (&&)
            if (auto rrefCtx = dynamic_cast<CppParser::RvalueRefModifierContext *>(ctx)) {
                outSymbol.type.is_rvalue_reference = true;
                unwindDeclarator(rrefCtx->declarator(), outSymbol);
                return;
            }

            // 5. Recursive Case: Array Bracket Layout ([])
            if (auto arrCtx = dynamic_cast<CppParser::ArrayModifierContext *>(ctx)) {
                outSymbol.type.array_dimensions++;
                unwindDeclarator(arrCtx->declarator(), outSymbol);
                return;
            }
        }


        absl::Status extractModifiers(CppParser::DeclarationModifiersContext *ctx, Type &out_type,
                                      size_t line_num) const {
            if (ctx == nullptr) return absl::NotFoundError("No modifiers found in context.");

            // declarationModifiers
            //     : (cvQualifier | typeSpecifier | storageSpecifier | compileTimeSpecifier )*
            //     ;

            /* cvQualifier */
            if (!ctx->cvQualifier().empty()) {
                int const_counter{}, volatile_counter{};
                for (auto *modifier: ctx->cvQualifier()) {
                    const std::string &text = modifier->getText();
                    if (text == "const") {
                        out_type.is_const = true;
                        const_counter++;
                    } else if (text == "volatile") {
                        out_type.is_volatile = true;
                        volatile_counter++;
                    }
                }

                if (const_counter > 1) {
                    reports.warnings.emplace_back(
                        absl::StrCat("at line ", line_num, ": Redundant 'const' qualifier. Defined ",
                                     const_counter, " times. Consider removal."),
                        ErrorCodeEnum::kWarning);
                }

                if (volatile_counter > 1) {
                    reports.warnings.emplace_back(
                        absl::StrCat("at line ", line_num, ": Redundant 'volatile' qualifier. Defined ",
                                     volatile_counter, " times. Consider removal."),
                        ErrorCodeEnum::kWarning);
                }
            }

            /* typeSpecifier */
            if (!ctx->typeSpecifier().empty()) {
                int signed_counter{}, unsigned_counter{};
                for (CppParser::TypeSpecifierContext *specifier: ctx->typeSpecifier()) {
                    if (specifier->getText() == "unsigned") {
                        out_type.is_signed = false;
                        unsigned_counter++;
                    } else if (specifier->getText() == "signed") {
                        signed_counter++;
                    }
                }
                if (signed_counter > 0 && unsigned_counter > 0) {
                    reports.errors.emplace_back(
                        absl::StrCat("at line ", line_num, ": Conflicting type specifiers: both 'signed' (",
                                     signed_counter, "x) and 'unsigned' (",
                                     unsigned_counter, "x) specified for the same type."),
                        ErrorCodeEnum::kFailure);
                } else if (signed_counter == 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Values are signed by default. Consider removing 'signed' specifier."),
                        ErrorCodeEnum::kSuggestion);
                } else if (signed_counter > 0) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'signed' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                } else if (unsigned_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'unsigned' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
            }

            /* storageSpecifier */
            if (!ctx->storageSpecifier().empty()) {
                int static_counter = 0;
                int extern_counter = 0;
                int thread_local_counter = 0;
                int mutable_counter = 0;

                for (auto *storage: ctx->storageSpecifier()) {
                    const std::string text = storage->getText();
                    if (text == "static") {
                        out_type.storage = StorageClass::kStatic;
                        static_counter++;
                    } else if (text == "extern") {
                        out_type.storage = StorageClass::kExtern;
                        extern_counter++;
                    } else if (text == "thread_local") {
                        out_type.storage = StorageClass::kThreadLocal;
                        thread_local_counter++;
                    } else if (text == "mutable") {
                        out_type.storage = StorageClass::kMutable;
                        mutable_counter++;
                    }
                }

                // In C++, you cannot mix static, extern, and mutable.
                int conflicting_bases = (static_counter > 0) + (extern_counter > 0) + (mutable_counter > 0);

                if (conflicting_bases > 1) {
                    reports.errors.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Conflicting storage specifiers: cannot combine 'static' (",
                                     static_counter, "x), 'extern' (", extern_counter,
                                     "x), or 'mutable' (", mutable_counter, "x)."),
                        ErrorCodeEnum::kFailure);
                }

                // Check for duplicates (e.g., "static static")
                if (static_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'static' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
                if (extern_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'extern' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
                if (thread_local_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'thread_local' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
                if (mutable_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Duplicate definition of 'mutable' specifier. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
            }

            /* compileTimeSpecifier */
            if (!ctx->compileTimeSpecifier().empty()) {
                int constexpr_counter = 0;
                int constinit_counter = 0;
                int inline_counter = 0;

                for (CppParser::CompileTimeSpecifierContext *specifier: ctx->compileTimeSpecifier()) {
                    const std::string text = specifier->getText();
                    if (text == "constexpr") {
                        out_type.is_constexpr = true;
                        constexpr_counter++;
                    } else if (text == "constinit") {
                        out_type.is_constinit = true;
                        constinit_counter++;
                    } else if (text == "inline") {
                        out_type.is_inline = true;
                        inline_counter++;
                    } else if (text == "consteval") {
                        /* a variable cannot be declared 'consteval' */
                    }
                }

                // --- Hard Compilation Conflict Validation ---

                if (constexpr_counter > 0 && constinit_counter > 0) {
                    reports.errors.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Conflicting specifiers: cannot combine 'constexpr' and 'constinit' on the same declaration."),
                        ErrorCodeEnum::kFailure);
                }

                // --- Redundancy Warnings / Suggestions ---

                if (constexpr_counter > 0 && inline_counter > 0) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num,
                                     ": Redundant 'inline' specifier. 'constexpr' implies 'inline'."),
                        ErrorCodeEnum::kSuggestion);
                }

                // --- Duplicate Handling ---

                if (constexpr_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num, ": Duplicate definition of 'constexpr' specifier. Defined ",
                                     constexpr_counter, " times. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }

                if (constinit_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num, ": Duplicate definition of 'constinit' specifier. Defined ",
                                     constinit_counter, " times. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }

                if (inline_counter > 1) {
                    reports.suggestions.emplace_back(
                        absl::StrCat("at line ", line_num, ": Duplicate definition of 'inline' specifier. Defined ",
                                     inline_counter, " times. Consider removing duplicates."),
                        ErrorCodeEnum::kSuggestion);
                }
            }

            return absl::OkStatus();
        }

    public:
        explicit SymbolTableVisitor(SymbolTable &table, Reports<Report> &reports)
            : symbolTable(table), reports(reports) {
        }

        /*
         *  const char *ptr1, *ptr2, *ptr3;
         *  Declaration modifiers and Types are declared once
         *  but can be multiple identifiers and values
         */
        std::any visitVariableDeclarationClause(CppParser::VariableDeclarationClauseContext *ctx) override {
            if (!ctx) return std::any();
            Symbol symbol;
            symbol.declaration_line = ctx->getStart()->getLine();
            symbol.tree_node_name = "variableDeclaration";
            symbol.type.base_type = CppZero::NormalizeType(ctx->primitiveType()->getText());

            absl::Status extract_status = extractModifiers(ctx->declarationModifiers(), symbol.type, symbol.declaration_line);
            if (!extract_status.ok()) {
                reports.info.emplace_back(extract_status.ToString(), ErrorCodeEnum::kFailure);
            }

            if (!symbolTable.Insert(symbol.name, symbol)) {
                reports.errors.emplace_back(
                    absl::StrCat("Duplicate declaration of variable ", symbol.name,
                                 " at line: ", symbol.declaration_line,
                                 ", during ", symbol.tree_node_name),
                    ErrorCodeEnum::kFailure);
            }

            return visitChildren(ctx);
        }

        std::any visitInitializationLeaf(CppParser::InitializationLeafContext *ctx) override {
            return visitChildren(ctx);
            // Symbol symbol;
            // symbol.type.baseType = ctx->primitiveType()->getText();
            //
            // extractModifiers(ctx->declarationModifiers(), symbol.type);
            // unwindDeclarator(ctx->declarator(), symbol);
            // symbol.treeNodeName = "variableInitialization";
            //
            // if(!symbolTable.insert(symbol.name, symbol)) {
            //     reports.errors.emplace_back("Duplicate declaration of variable " + symbol.name +
            //         " at line: " + std::to_string(symbol.declarationLine) + ", during " + symbol.treeNodeName, ErrorCodeEnum::FAILURE);
            // }
            // return visitChildren(ctx);
        }
    };
} // namespace CppZero
