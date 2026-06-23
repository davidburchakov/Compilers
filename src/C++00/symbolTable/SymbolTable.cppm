module;

// 1. Global Module Fragment: Keep legacy includes isolated up here
#include <string>
#include <unordered_map>
#include <iostream>

// 2. Begin C++23 Module Declaration context
export module SymbolTableModule;

namespace CppZero {

    // Tracks lifetime rules and memory layout scopes
    export enum class StorageClass {
        Auto,
        Static,
        Extern,
        ThreadLocal,
        Mutable
    };

    // Tracks the structural signature of the data format
    export struct Type {
        std::string baseType;       // e.g., "int", "double", "MyClass"
        int pointerCount = 0;       // 0 = raw, 1 = ptr*, 2 = ptr**
        int arrayDimensions = 0;    // 0 = single element, 1 = arr[], 2 = matrix[][]
        bool isLvalueReference = false; // true = standard reference (&)
        bool isRvalueReference = false; // true = move reference (&&)
        bool isConst = false;           // true = constant modifier
        bool isVolatile = false;        // true = volatile modifier
        StorageClass storage = StorageClass::Auto;
    };

    // Tracks the concrete variable instance identity
    export struct Symbol {
        std::string name;          // Variable name identifier (e.g., "matrix")
        Type type;                 // Composition mapping block
        int declarationLine = 0;   // Line mapping location context for compiler debugging
    };

    // Container matching scope mappings
    export class SymbolTable {
    private:
        std::unordered_map<std::string, Symbol> table;

    public:
        void insert(const std::string& name, const Symbol& symbol) {
            if (table.contains(name)) {
                std::cerr << "Semantic Error: Redefinition of variable '" << name
                          << "' on line " << symbol.declarationLine << "\n";
                return;
            }
            table[name] = symbol;
        }

        bool exists(const std::string& name) const {
            return table.contains(name);
        }

        Symbol get(const std::string& name) const {
            if (table.contains(name)) {
                return table.at(name);
            }
            return Symbol{};
        }

        // 🚀 NEW: Iterates and prints the exact structural layout of every collected variable
        void printAll() const {
            std::cout << "\n========================================";
            std::cout << "\n  SYMBOL TABLE CONTENTS (CppZero)";
            std::cout << "\n========================================\n";

            if (table.empty()) {
                std::cout << " (No variables declared)\n";
                std::cout << "========================================\n";
                return;
            }

            for (const auto& [name, symbol] : table) {
                std::cout << "• Variable: " << name << "\n";
                std::cout << "  - Line:       " << symbol.declarationLine << "\n";

                // Print Modifiers
                std::cout << "  - Modifiers:  ";
                if (symbol.type.isConst) std::cout << "const ";
                if (symbol.type.isVolatile) std::cout << "volatile ";

                if (symbol.type.storage == StorageClass::Static) std::cout << "[static] ";
                if (symbol.type.storage == StorageClass::Extern) std::cout << "[extern] ";
                if (symbol.type.storage == StorageClass::ThreadLocal) std::cout << "[thread_local] ";
                std::cout << "\n";

                // Print Type Shape
                std::cout << "  - Base Type:  " << symbol.type.baseType << "\n";
                std::cout << "  - Pointers:   " << std::string(symbol.type.pointerCount, '*') << " (" << symbol.type.pointerCount << ")\n";
                std::cout << "  - References: ";
                if (symbol.type.isLvalueReference) std::cout << "lvalue (&)";
                else if (symbol.type.isRvalueReference) std::cout << "rvalue (&&)";
                else std::cout << "none";
                std::cout << "\n";

                // Print Array Shape
                std::cout << "  - Dimensions: " << symbol.type.arrayDimensions << " "
                          << std::string(symbol.type.arrayDimensions * 2, ']') // Emulates [][] layouts
                          << "\n";
                std::cout << "----------------------------------------\n";
            }
        }
    };

} // namespace CppZero
