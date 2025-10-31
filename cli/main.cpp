#include <iostream>
#include "cppuml/model/ClassModel.h"

int main(int argc, char** argv) {
    std::cout << "cpp-to-uml CLI (placeholder)\n";
    // Later: parse args -> call parser -> generate PlantUML/LaTeX output
    using namespace c2uml::model;
    ClassModel example;
    example.name = "Example";
    std::cout << "Parsed example class name: " << example.name << "\n";
    return 0;
}