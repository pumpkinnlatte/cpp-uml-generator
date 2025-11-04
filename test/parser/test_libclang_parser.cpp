#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <core/parser/libclang_parser.h>
#include <fstream>
#include <sstream>

// Helper to load source code from test_inputs
std::string loadSource(const std::string& filename) {
    std::ifstream file(filename);
    REQUIRE(file); // Fail test if file not found
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Helper for test input file path
std::string testInputPath(const std::string& name) {
    return std::string(TEST_INPUT_DIR) + "/" + name;
}

TEST_CASE("Parse simple class and struct", "[libclang_parser]") {
    cppuml::LibClangParser parser;

    // Set compile arguments (C++17 as minimum)
    std::vector<std::string> args = {"-std=c++17"};

    SECTION("Parse simple struct Point") {
        auto tu = parser.parseFile(testInputPath("level1_simple_struct.cpp"), args);
        REQUIRE(tu);
        auto& classes = tu->classes_by_usr;
        // Should contain struct Point
        bool foundPoint = false;
        for (const auto& [usr, klass] : classes) {
            if (klass->name == "Point") foundPoint = true;
            // You can add specific checks here (fields, methods, typedefs)
        }
        REQUIRE(foundPoint);
    }

    SECTION("Parse simple class Person") {
        auto tu = parser.parseFile(testInputPath("level1_simple_class.cpp"), args);
        REQUIRE(tu);
        bool foundPerson = false;
        for (const auto& [usr, klass] : tu->classes_by_usr) {
            if (klass->name == "Person") foundPerson = true;
            // You can add checks for public/private fields, method names, etc
        }
        REQUIRE(foundPerson);
    }
    // More sections for each test input!
}