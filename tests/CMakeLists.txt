# tests/CMakeLists.txt

# ============================================================
# CompilerCpp Tests
# ============================================================

include(GoogleTest)

# ============================================================
# Grammar Tests
# ============================================================

add_executable(
GrammarTests
${CMAKE_CURRENT_SOURCE_DIR}/GrammarTests/GrammarTest.cpp
)

# ------------------------------------------------------------
# Test executable output
# ------------------------------------------------------------
#
# The source tree contains:
#
#     tests/GrammarTests/
#
# Therefore do not let CMake place the executable at:
#
#     ${CMAKE_BINARY_DIR}/tests/GrammarTests
#
# because that path is already used as a directory.
#
# Put test executables into a dedicated binary directory.
#
# ------------------------------------------------------------

set_target_properties(
GrammarTests
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY
${CMAKE_BINARY_DIR}/bin
)

# ------------------------------------------------------------
# C++ standard
# ------------------------------------------------------------

target_compile_features(
GrammarTests
PRIVATE
cxx_std_23
)

# ------------------------------------------------------------
# Link CompilerCore
# ------------------------------------------------------------
#
# GrammarTests does NOT compile:
#
#     CppParser.cpp
#     CppLexer.cpp
#     *.cppm
#
# itself.
#
# Those are owned by CompilerCore.
#
# ------------------------------------------------------------

target_link_libraries(
GrammarTests
PRIVATE
CompilerCore
GTest::gtest_main
)

# ------------------------------------------------------------
# Test compiler configuration
# ------------------------------------------------------------

target_compile_definitions(
GrammarTests
PRIVATE
ANTLR4CPP_USING_ABSEIL=0
)

target_compile_options(
GrammarTests
PRIVATE
-Wno-deprecated-declarations
)

# ------------------------------------------------------------
# Discover GoogleTest tests
# ------------------------------------------------------------

gtest_discover_tests(
GrammarTests
)
