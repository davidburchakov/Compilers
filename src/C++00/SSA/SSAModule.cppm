//
// Created by David Burchakov on 8/2/26.
//

// TODO:
// 1. SSAValue
//       ↓
// 2. SSAInstruction
//       ↓
// 3. SSABasicBlock
//       ↓
// 4. SSAFunction
//       ↓
// 5. SSAProgram
//       ↓
// 6. Parse-tree → basic blocks
//       ↓
// 7. CFG
//       ↓
// 8. Dominators
//       ↓
// 9. Dominance frontiers
//       ↓
// 10. φ insertion
//       ↓
// 11. Variable renaming

module;

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "antlr4-runtime.h"
#include "CppParser.h"

export module SSAModule;

import SymbolTableModule;


export namespace CppZero {
    // ============================================================
    // SSA Value
    // ============================================================
    //
    // Represents a value in SSA form.
    //
    // Examples:
    //
    //     x_0
    //     x_1
    //     temporary_0
    //
    // The important property is that every SSA value has exactly
    // one definition.
    //
    class SSAValue {
    public:
        SSAValue() = default;

        SSAValue(std::string name, int version)
            : name(std::move(name)), version(version) {
        }

        const std::string &getName() const {
            return name;
        }

        int getVersion() const {
            return version;
        }

        std::string toString() const {
            return name + "_" + std::to_string(version);
        }

    private:
        std::string name{};
        int version = 0;
    };


    // ============================================================
    // SSA Instruction
    // ============================================================
    //
    // Base representation for an SSA instruction.
    //
    // Examples of future instructions:
    //
    //     x_0 = 10
    //     x_1 = x_0 + 5
    //     x_2 = phi(x_0, x_1)
    //     return x_2
    //
    class SSAInstruction {
    public:
        virtual ~SSAInstruction() = default;

        virtual std::string toString() const = 0;
    };


    // ============================================================
    // SSA Basic Block
    // ============================================================
    //
    // A basic block is a straight-line sequence of instructions
    // with one entry and one exit.
    //
    // Future CFG:
    //
    //                    entry
    //                      |
    //                  condition
    //                   /     \
    //                  /       \
    //               then      else
    //                  \       /
    //                   \     /
    //                    merge
    //
    class SSABasicBlock {
    public:
        SSABasicBlock() = default;

        explicit SSABasicBlock(std::string name)
            : name(std::move(name)) {
        }

        const std::string &getName() const { return name; }

        void addInstruction(std::unique_ptr<SSAInstruction> instruction) {
            instructions.push_back(std::move(instruction));
        }

        const std::vector<std::unique_ptr<SSAInstruction> > &getInstructions() const {
            return instructions;
        }

    private:
        std::string name;
        std::vector<std::unique_ptr<SSAInstruction> > instructions;
    };


    // ============================================================
    // SSA Function
    // ============================================================
    //
    // Represents one function in the SSA program.
    //
    // Example:
    //
    //     function main:
    //         entry:
    //             x_0 = 10
    //             x_1 = x_0 + 5
    //             return x_1
    //
    class SSAFunction {
    public:
        SSAFunction() = default;

        explicit SSAFunction(std::string name)
            : name(std::move(name)) { }

        const std::string &getName() const { return name; }

        void addBlock(std::unique_ptr<SSABasicBlock> block) {
            blocks.push_back(std::move(block));
        }

        const std::vector<std::unique_ptr<SSABasicBlock> > &getBlocks() const { return blocks; }

    private:
        std::string name;
        std::vector<std::unique_ptr<SSABasicBlock> > blocks;
    };


    // ============================================================
    // SSA Program
    // ============================================================
    //
    // The complete intermediate representation after SSA
    // construction.
    //
    // Translation Unit
    //      |
    //      +-- SSAFunction main
    //      |       |
    //      |       +-- entry
    //      |       +-- ...
    //      |
    //      +-- SSAFunction foo
    //              |
    //              +-- entry
    //              +-- ...
    //
    class SSAProgram {
    public:
        SSAProgram() = default;

        void addFunction(std::unique_ptr<SSAFunction> function) {
            functions.push_back(std::move(function));
        }

        const std::vector<std::unique_ptr<SSAFunction>> &getFunctions() const { return functions; }

        std::string toString() const {
            // TODO:
            //
            // Convert the internal SSA representation into
            // human-readable SSA text for the GUI.
            //
            // Example:
            //
            // function main:
            // entry:
            //     x_0 = 10
            //     x_1 = x_0 + 5
            //     return x_1
            //
            return {"asd"};
        }

    private:
        std::vector<std::unique_ptr<SSAFunction>> functions;
    };


    // ============================================================
    // SSA Converter
    // ============================================================
    //
    // Converts the semantically-validated parse tree into SSA.
    //
    // Pipeline:
    //
    //     ANTLR ParseTree: Lexer, Parser
    //            |
    //            v
    //     Semantic Analysis
    //            |
    //            v
    //       SSAConverter
    //            |
    //            +--> Build CFG
    //            |
    //            +--> Build Basic Blocks
    //            |
    //            +--> Find Definitions
    //            |
    //            +--> Insert Phi Nodes
    //            |
    //            +--> Rename Variables
    //            |
    //            v
    //        SSAProgram
    //
    class SSAConverter {
    public:
        SSAConverter() = default;
        ~SSAConverter() = default;

        // --------------------------------------------------------
        // Main entry point
        // --------------------------------------------------------
        //
        // The parse tree has already passed:
        //
        //     1. Lexing
        //     2. Parsing
        //     3. Symbol-table construction
        //     4. Semantic analysis
        //
        // The converter does NOT perform semantic analysis.
        //
        SSAProgram convert(CppParser::TranslationUnitContext *tree, const SymbolTable &symbolTable);

    private:
        // ========================================================
        // Intermediate State
        // ========================================================
        CppParser::TranslationUnitContext *tree = nullptr;
        const SymbolTable *symbolTable = nullptr;
        std::unique_ptr<SSAProgram> program;

        // ========================================================
        // Variable Version Management
        // ========================================================
        //
        // Source:
        //
        //     x
        //
        // SSA:
        //
        //     x_0
        //     x_1
        //     x_2
        //
        // Each source variable has its own version counter.
        //
        std::unordered_map<std::string, int> nextVersion;

        // The currently active SSA version for each variable.
        //
        // Example:
        //
        //     x -> 2
        //
        // means:
        //
        //     the current value of x is x_2
        //
        std::unordered_map<std::string, int> currentVersion;


        // ========================================================
        // Phase 1: Build SSA Program
        // ========================================================

        void buildProgram();


        // ========================================================
        // Phase 2: Find Functions
        // ========================================================
        //
        // Locate function definitions in the ANTLR parse tree and
        // create corresponding SSAFunction objects.
        //
        void buildFunctions(CppParser::TranslationUnitContext *translationUnit);


        // ========================================================
        // Phase 3: Build Control Flow Graph
        // ========================================================
        //
        // Convert statements into basic blocks.
        //
        // Example:
        //
        //     if (x) {
        //         y = 1;
        //     } else {
        //         y = 2;
        //     }
        //
        // becomes:
        //
        //              entry
        //             /     \
        //          then     else
        //             \     /
        //              merge
        //
        void buildControlFlowGraph();


        // ========================================================
        // Phase 4: Build Basic Blocks
        // ========================================================

        void buildBasicBlocks();


        // ========================================================
        // Phase 5: Analyze Definitions
        // ========================================================
        //
        // Find where each source variable is defined.
        //
        // Example:
        //
        //     x = 10;
        //     x = 20;
        //
        // Definitions:
        //
        //     x -> { definition #1, definition #2 }
        //
        void findDefinitions();


        // ========================================================
        // Phase 6: Compute Dominators
        // ========================================================
        //
        // SSA construction normally requires CFG dominance
        // information.
        //
        // Eventually this should compute:
        //
        //     dominator(block)
        //
        // and preferably the:
        //
        //     dominance frontier
        //
        // for every basic block.
        //
        void computeDominators();


        // ========================================================
        // Phase 7: Compute Dominance Frontiers
        // ========================================================
        //
        // Dominance frontiers are used to determine where phi
        // functions must be inserted.
        //
        // Example:
        //
        //              entry
        //             /     \
        //          then     else
        //             \     /
        //              merge
        //
        // If both then/else define x:
        //
        //              merge:
        //                  x_2 = phi(x_0, x_1)
        //
        void computeDominanceFrontiers();


        // ========================================================
        // Phase 8: Insert Phi Nodes
        // ========================================================
        //
        // For variables with multiple possible incoming definitions,
        // insert phi functions at appropriate CFG merge points.
        //
        // Example:
        //
        //     if (condition)
        //         x = 10;
        //     else
        //         x = 20;
        //
        // becomes:
        //
        //     x_2 = phi(x_0, x_1)
        //
        void insertPhiNodes();


        // ========================================================
        // Phase 9: SSA Variable Renaming
        // ========================================================
        //
        // Convert:
        //
        //     x = 10;
        //     x = x + 1;
        //
        // into:
        //
        //     x_0 = 10;
        //     x_1 = x_0 + 1;
        //
        void renameVariables();


        // ========================================================
        // Phase 10: Rename One Basic Block
        // ========================================================
        //
        // Recursive SSA renaming algorithm.
        //
        // This will eventually walk the dominator tree and rename
        // definitions and uses according to the current version
        // stack.
        //
        void renameBlock(SSABasicBlock &block);


        // ========================================================
        // SSA Version Creation
        // ========================================================

        int createVersion(const std::string &variable);


        // ========================================================
        // Get Current SSA Version
        // ========================================================

        int getCurrentVersion(const std::string &variable) const;


        // ========================================================
        // Reset Converter State
        // ========================================================

        void reset();
    };


    // ============================================================
    // SSAConverter Implementation
    // ============================================================

    inline SSAProgram SSAConverter::convert(CppParser::TranslationUnitContext *inputTree, const SymbolTable &inputSymbolTable) {
        reset();

        tree = inputTree;
        symbolTable = &inputSymbolTable;

        if (tree == nullptr) {
            return {};
        }

        /*
         * SSA construction pipeline.
         *
         * Keep these calls in this order when implementing the
         * actual algorithms.
         */

        buildProgram();

        return std::move(*program);
    }


    inline void SSAConverter::reset() {
        tree = nullptr;
        symbolTable = nullptr;

        program = std::make_unique<SSAProgram>();

        nextVersion.clear();
        currentVersion.clear();
    }


    inline void SSAConverter::buildProgram() {
        /*
         * TODO:
         *
         * 1. Locate the translation unit.
         * 2. Find all function definitions.
         * 3. For every function:
         *      - create SSAFunction
         *      - create entry block
         *      - build CFG
         *      - convert statements to SSA instructions
         *
         * Do NOT implement optimization here.
         *
         * This class is responsible for constructing SSA.
         */

        if (tree == nullptr) {
            return;
        }

        buildFunctions(tree);
    }


    inline void SSAConverter::buildFunctions(CppParser::TranslationUnitContext *translationUnit) {
        /*
         * TODO:
         *
         * Walk the ANTLR parse tree and identify function
         * definitions.
         *
         * For each function:
         *
         *     auto function =
         *         std::make_unique<SSAFunction>("main");
         *
         *     auto entry =
         *         std::make_unique<SSABasicBlock>("entry");
         *
         *     function->addBlock(std::move(entry));
         *
         *     program->addFunction(std::move(function));
         */
        (void) translationUnit;
    }


    inline void SSAConverter::buildControlFlowGraph() {
        /*
         * TODO:
         *
         * Construct the CFG.
         *
         * Statements such as:
         *
         *     if
         *     while
         *     for
         *     do
         *     switch
         *     break
         *     continue
         *     return
         *
         * affect control flow.
         */
    }


    inline void SSAConverter::buildBasicBlocks() {
        /*
         * TODO:
         *
         * Split the function into basic blocks.
         *
         * A basic block:
         *
         *     - has one entry point
         *     - has one exit
         *     - contains straight-line code
         *
         * Terminators include:
         *
         *     branch
         *     conditional branch
         *     return
         *     jump
         */
    }


    inline void SSAConverter::findDefinitions() {
        /*
         * TODO:
         *
         * Find every assignment/declaration that creates a new
         * value.
         *
         * Example:
         *
         *     int x = 10;
         *     x = 20;
         *
         * gives two definitions of source variable x.
         */
    }


    inline void SSAConverter::computeDominators() {
        /*
         * TODO:
         *
         * Compute the dominator tree for every function CFG.
         *
         * A block A dominates block B if every path from the
         * entry block to B passes through A.
         */
    }


    inline void SSAConverter::computeDominanceFrontiers() {
        /*
         * TODO:
         *
         * Compute dominance frontiers.
         *
         * These are required for the classical Cytron-style
         * SSA construction algorithm.
         */
    }


    inline void SSAConverter::insertPhiNodes() {
        /*
         * TODO:
         *
         * Insert phi functions at merge points.
         *
         * Example:
         *
         *     if (condition) {
         *         x = 10;
         *     } else {
         *         x = 20;
         *     }
         *
         * becomes:
         *
         *     merge:
         *         x_2 = phi(x_0, x_1)
         */
    }


    inline void SSAConverter::renameVariables() {
        /*
         * TODO:
         *
         * Perform SSA renaming.
         *
         * Each definition receives a new version.
         *
         *     x = 10
         *     x = x + 1
         *
         * becomes:
         *
         *     x_0 = 10
         *     x_1 = x_0 + 1
         */
    }


    inline void SSAConverter::renameBlock(SSABasicBlock &block) {
        /*
         * TODO:
         *
         * Rename:
         *
         *     1. definitions in this block
         *     2. uses in this block
         *     3. operands of phi nodes in successor blocks
         *
         * Then recursively process children in the dominator tree.
         */
        (void) block;
    }


    inline int SSAConverter::createVersion(const std::string &variable) {
        /*
         * TODO:
         *
         * Example:
         *
         *     x -> 0
         *     x -> 1
         *     x -> 2
         *
         * Every call must return a unique version for the variable.
         */

        int &version = nextVersion[variable];
        return version++;
    }


    inline int SSAConverter::getCurrentVersion(const std::string &variable) const {
        auto iterator = currentVersion.find(variable);

        if (iterator == currentVersion.end()) {
            return -1;
        }

        return iterator->second;
    }
}
