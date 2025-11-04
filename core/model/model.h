#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace cppuml {

enum class Access { Public, Protected, Private, None };

struct Field {
    std::string name;
    std::string type;
    Access access = Access::None;
    std::string usr; // unique id
};

struct Method {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string/*type*/, std::string/*name*/>> params;
    bool isStatic = false;
    bool isConst = false;
    bool isVirtual = false;
    bool isPureVirtual = false;
    Access access = Access::None;
    std::string usr;
};

struct Relationship {
    enum class Kind { Inheritance, Association } kind;
    std::string from_usr;
    std::string to_usr;
    std::string label; // optional
};

struct Class {
    std::string name;
    std::string usr;
    bool isStruct = false;
    std::vector<Field> fields;
    std::vector<Method> methods;
    std::vector<std::string> bases_usr; // USR of base classes
};

struct Namespace {
    std::string name;
    std::vector<std::shared_ptr<Class>> classes;
    std::vector<std::shared_ptr<Namespace>> namespaces;
};

struct TranslationUnit {
    std::string filename;
    std::vector<std::shared_ptr<Namespace>> namespaces; // top-level namespaces
    std::map<std::string, std::shared_ptr<Class>> classes_by_usr; // convenience
};

} // namespace cppuml