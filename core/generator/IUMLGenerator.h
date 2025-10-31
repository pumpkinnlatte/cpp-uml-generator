#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../model/ClassModel.h"

namespace c2uml::generator {

struct GeneratorOptions {
    // target specific options e.g. layout hints
    std::optional<std::string> diagramName;
};

class IUMLGenerator {
public:
    virtual ~IUMLGenerator() = default;
    virtual std::string generate(const std::vector<c2uml::model::ClassModel>& classes,
                                 const GeneratorOptions& opts) = 0;
};

} // namespace c2uml::generator