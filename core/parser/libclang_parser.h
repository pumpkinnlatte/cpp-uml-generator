#pragma once
/**
 * @file libclang_parser.h
 * @brief Parser adapter using libclang to build the unified UML model for C++ code.
 *
 * Provides an interface to parse a single C++ source file and produce a TranslationUnit,
 * mapping libclang constructs to cppuml::model.
 */

#include "../model/UnifiedModel.h"
#include <string>
#include <vector>
#include <memory>

namespace cppuml {

/**
 * @brief C++ parser powered by libclang.
 */
class LibClangParser {
public:
    /**
     * @brief Parse a source file and return a TranslationUnit (or nullptr on error).
     * @param filepath Path to C++ source file
     * @param compile_args Compilation arguments (e.g., {"-std=c++17"})
     * @return Parsed TranslationUnit or nullptr on error
     */
    std::shared_ptr<model::TranslationUnit> parseFile(
        const std::string& filepath,
        const std::vector<std::string>& compile_args);
};

} // namespace cppuml