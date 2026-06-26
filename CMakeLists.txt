cmake_minimum_required(VERSION 3.28)

project(CompilerCpp)

# Enforce globally pure, strict C++23 architecture (Required for modules & import std;)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Silence FetchContent timestamps warnings safely
cmake_policy(SET CMP0135 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------
# 1. FETCH ABSEIL (Done early to ensure compiler flags align)
# ---------------------------
include(FetchContent)
FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG        20240722.0 # Stable LTS release with solid C++23 support
)
# Force Abseil to respect our global C++23 standard option
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(absl)

# ---------------------------
# 2. FIND PACKAGES
# ---------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Java REQUIRED)

# ANTLR runtime FIRST
add_subdirectory(antlr4/runtime/Cpp)

set(ANTLR_JAR
        ${CMAKE_SOURCE_DIR}/tools/antlr-4.13.1-complete.jar
)

set(ANTLR_OUTPUT_DIR
        ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${ANTLR_OUTPUT_DIR})

# Fixed: Cleaned up list containing ONLY real generated files
set(ANTLR_GENERATED_SRC
        ${ANTLR_OUTPUT_DIR}/CppLexer.cpp
        ${ANTLR_OUTPUT_DIR}/CppParser.cpp
        ${ANTLR_OUTPUT_DIR}/CppBaseVisitor.cpp
        ${ANTLR_OUTPUT_DIR}/CppVisitor.cpp
        ${ANTLR_OUTPUT_DIR}/CppBaseListener.cpp
        ${ANTLR_OUTPUT_DIR}/CppListener.cpp
)

add_custom_command(
        OUTPUT ${ANTLR_GENERATED_SRC}
        COMMAND
        ${Java_JAVA_EXECUTABLE}
        -jar ${ANTLR_JAR}
        -Dlanguage=Cpp
        -visitor
        -listener
        -o ${ANTLR_OUTPUT_DIR}
        ${CMAKE_SOURCE_DIR}/src/C++00/grammar/Cpp.g4
        DEPENDS
        ${CMAKE_SOURCE_DIR}/src/C++00/grammar/Cpp.g4
)

add_custom_target(
        GenerateGrammar
        DEPENDS ${ANTLR_GENERATED_SRC}
)

# ---------------------------
# 3. EXECUTABLE TARGET
# ---------------------------
add_executable(AntlrQtEditor
        src/C++00/main.cpp
        src/Qt/mainwindow.cpp
        src/Qt/mainwindow.h
        src/Qt/mainwindow.ui
        ${ANTLR_GENERATED_SRC}
)

# ---------------------------
# 4. REGISTER ALL C++23 MODULES SEPARATELY
# ---------------------------
target_sources(AntlrQtEditor
        PRIVATE
        FILE_SET compiler_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        src/C++00/symbolTable/SymbolTable.cppm
        src/C++00/symbolTable/SymbolTableVisitor.cppm
        src/C++00/auxiliary/Logger.cppm
        src/C++00/semantics/SemanticAnalysis.cppm
        src/C++00/semantics/SemanticVisitor.cppm
        src/C++00/auxiliary/Reports.cppm
        src/C++00/auxiliary/LogColor.cppm
        src/C++00/optimizations/ASTOptimizer.cppm
        src/C++00/assembly/AssemblyGenerator.cppm
)

add_dependencies(AntlrQtEditor GenerateGrammar)

target_include_directories(AntlrQtEditor PRIVATE
        ${ANTLR_OUTPUT_DIR}
        antlr4/runtime/Cpp/runtime/src
)

# ---------------------------
# 5. LINK LIBRARIES & LINK ABSEIL COMPONENTS
# ---------------------------
target_link_libraries(AntlrQtEditor PRIVATE
        Qt6::Widgets
        Qt6::Concurrent
        antlr4_shared
        # Link only the granular Abseil components you actually need:
        absl::flat_hash_map
        absl::strings
        absl::statusor
)

target_compile_definitions(AntlrQtEditor PRIVATE
        ANTLR4CPP_USING_ABSEIL=0
)

# Suppress deprecation warnings from legacy ANTLR code structures
target_compile_options(AntlrQtEditor PRIVATE
        -Wno-deprecated-declarations
)