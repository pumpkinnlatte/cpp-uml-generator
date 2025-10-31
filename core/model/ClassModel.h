#pragma once

#include <string>
#include <vector>
#include <optional>

namespace c2uml::model {

enum class Visibility { Public, Protected, Private, Default };

struct TypeName {
    std::string name;
    // expand later with template arguments, qualifiers, etc.
};

struct Attribute {
    Visibility visibility = Visibility::Default;
    TypeName type;
    std::string name;
    std::optional<std::string> defaultValue;
};

struct Parameter {
    TypeName type;
    std::string name;
};

struct Method {
    Visibility visibility = Visibility::Default;
    TypeName returnType;
    std::string name;
    std::vector<Parameter> params;
    bool isConst = false;
    bool isVirtual = false;
    bool isStatic = false;
};

struct ClassModel {
    std::string name;
    std::vector<std::string> baseClasses;
    std::vector<Attribute> attributes;
    std::vector<Method> methods;
};

} // namespace c2uml::model