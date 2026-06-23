module;

#include <string>
#include <unordered_map>
#include <iostream>

export module SymbolTableModule;

export namespace CppZero {

    struct Symbol {
        std::string name;
        std::string type;
    };

    class SymbolTable {
    private:
        std::unordered_map<std::string, Symbol> table;

    public:

        void insert(const std::string& name,
                    const std::string& type)
        {
            if (table.contains(name)) {
                std::cerr
                    << "Warning: Symbol '"
                    << name
                    << "' redefinition detected.\n";
                return;
            }

            table[name] = Symbol{name, type};
        }

        bool exists(const std::string& name) const {
            return table.contains(name);
        }

        Symbol get(const std::string& name) const {
            auto it = table.find(name);

            if (it != table.end())
                return it->second;

            return {};
        }
    };
}