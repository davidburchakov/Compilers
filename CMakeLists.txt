# CompilerCpp/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)

project(CompilerCpp LANGUAGES CXX)

# ============================================================
# Global C++ configuration
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# C++ module dependency scanning.
#
# CMake 3.28+ supports CXX_MODULES file sets and compiler
# module dependency scanning.
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Silence FetchContent timestamp warnings safely.
cmake_policy(SET CMP0135 NEW)

# ============================================================
# Qt
# ============================================================

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ============================================================
# Dependencies
# ============================================================

include(FetchContent)

# ------------------------------------------------------------
# Abseil
# ------------------------------------------------------------

FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG 20240722.0
)

set(
        ABSL_PROPAGATE_CXX_STD
        ON
        CACHE BOOL "" FORCE
)

FetchContent_MakeAvailable(absl)

# ------------------------------------------------------------
# GoogleTest
# ------------------------------------------------------------

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)

# Recommended by GoogleTest for Windows.
set(
        gtest_force_shared_crt
        ON
        CACHE BOOL "" FORCE
)

FetchContent_MakeAvailable(googletest)

# ------------------------------------------------------------
# Qt / Java
# ------------------------------------------------------------

find_package(
        Qt6
        REQUIRED
        COMPONENTS
        Widgets
        Concurrent
)

find_package(Java REQUIRED)

# ============================================================
# ANTLR Runtime
# ============================================================

add_subdirectory(
        antlr4/runtime/Cpp
)

# ============================================================
# ANTLR Grammar Generation
# ============================================================

set(
        ANTLR_JAR
        ${CMAKE_SOURCE_DIR}/tools/antlr-4.13.1-complete.jar
)

set(
        ANTLR_OUTPUT_DIR
        ${CMAKE_BINARY_DIR}/generated
)

file(
        MAKE_DIRECTORY
        ${ANTLR_OUTPUT_DIR}
)

# ANTLR generates both .cpp and .h files.
set(
        ANTLR_GENERATED_SRC

        ${ANTLR_OUTPUT_DIR}/CppLexer.cpp
        ${ANTLR_OUTPUT_DIR}/CppParser.cpp

        ${ANTLR_OUTPUT_DIR}/CppBaseVisitor.cpp
        ${ANTLR_OUTPUT_DIR}/CppVisitor.cpp

        ${ANTLR_OUTPUT_DIR}/CppBaseListener.cpp
        ${ANTLR_OUTPUT_DIR}/CppListener.cpp
)

set(
        ANTLR_GENERATED_HEADERS

        ${ANTLR_OUTPUT_DIR}/CppLexer.h
        ${ANTLR_OUTPUT_DIR}/CppParser.h

        ${ANTLR_OUTPUT_DIR}/CppBaseVisitor.h
        ${ANTLR_OUTPUT_DIR}/CppVisitor.h

        ${ANTLR_OUTPUT_DIR}/CppBaseListener.h
        ${ANTLR_OUTPUT_DIR}/CppListener.h
)

set(
        ANTLR_GENERATED_FILES
        ${ANTLR_GENERATED_SRC}
        ${ANTLR_GENERATED_HEADERS}
)

add_custom_command(
        OUTPUT
        ${ANTLR_GENERATED_FILES}

        COMMAND
        ${Java_JAVA_EXECUTABLE}
        -jar
        ${ANTLR_JAR}
        -Dlanguage=Cpp
        -visitor
        -listener
        -o
        ${ANTLR_OUTPUT_DIR}
        ${CMAKE_SOURCE_DIR}/src/C++00/grammar/Cpp.g4

        DEPENDS
        ${CMAKE_SOURCE_DIR}/src/C++00/grammar/Cpp.g4
        ${ANTLR_JAR}

        COMMENT
        "Generating C++ parser from Cpp.g4"

        VERBATIM
)

add_custom_target(
        GenerateGrammar
        DEPENDS
        ${ANTLR_GENERATED_FILES}
)

# ============================================================
# CompilerCore
# ============================================================
#
# This is the important architectural change.
#
# ALL compiler modules live here.
#
# The GUI executable and test executables LINK to this target.
# They do not compile the module files themselves.
#
# ============================================================

add_library(
        CompilerCore
        STATIC

        ${ANTLR_GENERATED_SRC}
)

# ------------------------------------------------------------
# C++23 modules
# ------------------------------------------------------------

target_sources(
        CompilerCore
        PUBLIC

        FILE_SET
        compiler_modules
        TYPE
        CXX_MODULES

        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}

        FILES

        src/C++00/symbolTable/SymbolTable.cppm
        src/C++00/symbolTable/SymbolTableVisitor.cppm

        src/C++00/auxiliary/Logger.cppm
        src/C++00/auxiliary/Reports.cppm
        src/C++00/auxiliary/LogColor.cppm

        src/C++00/semantics/SemanticAnalysis.cppm
        src/C++00/semantics/SemanticVisitor.cppm

        src/C++00/optimizations/ASTOptimizer.cppm

        src/C++00/assembly/AssemblyGenerator.cppm

        src/C++00/SSA/SSAModule.cppm
)

add_dependencies(
        CompilerCore
        GenerateGrammar
)

# ------------------------------------------------------------
# CompilerCore include paths
# ------------------------------------------------------------

target_include_directories(
        CompilerCore
        PUBLIC

        ${ANTLR_OUTPUT_DIR}

        ${CMAKE_SOURCE_DIR}/antlr4/runtime/Cpp/runtime/src
)

# ------------------------------------------------------------
# CompilerCore dependencies
# ------------------------------------------------------------

target_link_libraries(
        CompilerCore
        PUBLIC

        antlr4_shared

        absl::flat_hash_map
        absl::strings
        absl::statusor
)

target_compile_definitions(
        CompilerCore
        PUBLIC

        ANTLR4CPP_USING_ABSEIL=0
)

target_compile_options(
        CompilerCore
        PRIVATE

        -Wno-deprecated-declarations
)

# ============================================================
# GUI EXECUTABLE
# ============================================================

add_executable(
        AntlrQtEditor

        src/C++00/main.cpp

        src/Qt/mainwindow.cpp
        src/Qt/mainwindow.h
        src/Qt/mainwindow.ui
)

# The GUI does NOT list:
#
#     *.cppm
#     CppParser.cpp
#     CppLexer.cpp
#
# because those belong to CompilerCore.

target_link_libraries(
        AntlrQtEditor
        PRIVATE

        CompilerCore

        Qt6::Widgets
        Qt6::Concurrent
)

target_compile_options(
        AntlrQtEditor
        PRIVATE

        -Wno-deprecated-declarations
)

# ============================================================
# Testing
# ============================================================

enable_testing()


add_subdirectory(tests)
