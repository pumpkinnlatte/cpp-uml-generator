#pragma once
#include "../model/model.h"
#include <string>
#include <vector>
#include <memory>

namespace cppuml {

class LibClangParser {
public:
    // parseFile parses a single source file given compile arguments (e.g. {"-std=c++17"})
    // returns nullptr on fatal errors.
    std::shared_ptr<TranslationUnit> parseFile(const std::string &filepath,
                                               const std::vector<std::string> &compile_args);
};

} // namespace cppuml