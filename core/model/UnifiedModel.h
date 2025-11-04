#pragma once
/**
 * @file UnifiedModel.h
 * @brief Unified internal model for C++ source code to UML transformation.
 *
 * This header defines the data structures used as the intermediate representation
 * between parsing C++ code and exporting it to UML formats (such as PlantUML or Graphviz).
 * The model is designed to be reflected in class diagrams, including template arguments, access specifiers,
 * types, namespaces, relationships, and more.
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>

namespace cppuml::model {

/**
 * @brief Specifies the C++ access level of class/struct members.
 */
enum class Access { Public, Protected, Private, None };

/**
 * @brief Returns the PlantUML symbol for a given access specifier.
 * @param a The access specifier.
 * @return The corresponding PlantUML character: '+', '#', '-', or '~'.
 */
inline char accessToSymbol(Access a) {
    switch (a) {
        case Access::Public: return '+';
        case Access::Protected: return '#';
        case Access::Private: return '-';
        case Access::None: return '~';
    }
    return '~';
}

/**
 * @brief Represents a C++ type, including template parameters and qualifiers.
 *
 * Example:
 * - name: "std::vector"
 * - templateArgs: [TypeName{name="int"}]
 * - isConst: true
 * - pointerDepth: 1
 * Would represent: "const std::vector<int>*"
 */
struct TypeName {
    /// The base type name, e.g., "Widget", "std::vector".
    std::string name;
    /// Template argument types, e.g., <T, Allocator>.
    std::vector<TypeName> templateArgs;
    /// True if type is const-qualified.
    bool isConst = false;
    /// True if type is a reference.
    bool isReference = false;
    /// Pointer level (e.g., 0 for not a pointer, 1 for *, 2 for **, etc.)
    unsigned pointerDepth = 0;

    /**
     * @brief Returns a canonical string representation, suitable for display.
     * Example: "const std::vector<int>*"
     */
    std::string toString() const {
        std::string out;
        if (isConst) out += "const ";
        out += name;
        if (!templateArgs.empty()) {
            out += "<";
            for (size_t i = 0; i < templateArgs.size(); ++i) {
                out += templateArgs[i].toString();
                if (i + 1 < templateArgs.size()) out += ", ";
            }
            out += ">";
        }
        for (unsigned i = 0; i < pointerDepth; ++i) out += "*";
        if (isReference) out += "&";
        return out;
    }
};

/**
 * @brief Represents a function or method parameter.
 */
struct Parameter {
    /// Type of the parameter.
    TypeName type;
    /// Name of the parameter.
    std::string name;
    /// Default value, if specified.
    std::optional<std::string> defaultValue;
};

/**
 * @brief Represents a member variable of a class/struct.
 */
struct Field {
    /// Name of the member variable.
    std::string name;
    /// Type of the field.
    TypeName type;
    /// Access level (public/protected/private/none).
    Access access = Access::None;
    /// Default value (which may be present in class/struct definition).
    std::optional<std::string> defaultValue;
    /// Optional unique symbol identifier (for cross-references).
    std::string usr;
    /// True if static member variable.
    bool isStatic = false;
};

/**
 * @brief Represents a member function (method) of a class/struct.
 *
 * Includes support for virtual, static, pure, and const-qualified methods.
 * The parameter list can handle default values as well.
 */
struct Method {
    /// Name of the method.
    std::string name;
    /// Return type of the method.
    TypeName returnType;
    /// List of parameters.
    std::vector<Parameter> params;
    /// True if the method is static.
    bool isStatic = false;
    /// True if the method is const-qualified.
    bool isConst = false;
    /// True if the method is virtual.
    bool isVirtual = false;
    /// True if the method is pure-virtual (abstract in UML).
    bool isPureVirtual = false;
    /// True if declared as inline.
    bool isInline = false;
    /// Access specifier.
    Access access = Access::None;
    /// Unique symbol identifier (for cross-references and relationships).
    std::string usr;
    // Future extension: qualifiers, exception clauses, template params, etc.
};

/**
 * @brief Represents a relationship between classes.
 *
 * UML relationships can have different kinds:
 * - Inheritance: A "is a" relationship (generalization)
 * - Association: A has-a or uses relationship
 * - Aggregation: Association with shared lifetimes
 * - Composition: Stronger form of aggregation (part-of and lifetime tied)
 * - Dependency: Uses or depends on (e.g., parameter types)
 *
 * Relationships are recorded by the USR (unique symbol id) of the source and target.
 */
struct Relationship {
    /// Kind of the UML relationship.
    enum class Kind { Inheritance, Association, Aggregation, Composition, Dependency } kind;
    /// USR of the owner ("from": class holding the relationship)
    std::string from_usr;
    /// USR of the target ("to": related class)
    std::string to_usr;
    /// Optional label (e.g., role name, multiplicity)
    std::optional<std::string> label;
    // Future: direction, multiplicity, role names.
};

/**
 * @brief Represents a C++ class or struct.
 *
 * Includes base classes, fields, methods, template parameters, nested classes, etc.
 */
struct Class {
    /// Short class/struct name (not qualified).
    std::string name;
    /// Unique symbol id (USR, as returned by libclang if possible).
    std::string usr;
    /// True if this is a struct (affects default access & UML display).
    bool isStruct = false;
    /// True if this is a template class.
    bool isTemplate = false;
    /// Template parameter names (if template); textual.
    std::vector<std::string> templateParams;
    /// USRs of base classes (in multiple/virtual inheritance, order is preserved).
    std::vector<std::string> bases_usr;
    /// Fields defined in this class/struct.
    std::vector<Field> fields;
    /// Member functions (methods) defined in this class/struct.
    std::vector<Method> methods;
    /// Nested (inner) classes.
    std::vector<std::shared_ptr<Class>> nested;
    /// Optional pointer to owning namespace (to avoid ownership cycles, use weak_ptr).
    std::weak_ptr<struct Namespace> parentNamespace;
};

/**
 * @brief Represents a C++ namespace, possibly nested.
 *
 * Allows for organization of classes and further nested namespaces.
 */
struct Namespace {
    /// Name of namespace (single segment only, e.g., "std", not qualified).
    std::string name;
    /// Classes directly within this namespace.
    std::vector<std::shared_ptr<Class>> classes;
    /// Namespaces nested inside this namespace.
    std::vector<std::shared_ptr<Namespace>> namespaces;
    /// Optional parent namespace (for qualified name calculation).
    std::weak_ptr<Namespace> parent;

    /**
     * @brief Computes the fully qualified name of the namespace.
     * @return String with the full name (e.g., "foo::bar").
     */
    std::string fullName() const {
        std::string out = name;
        auto p = parent.lock();
        if (p) {
            std::string prefix = p->fullName();
            if (!prefix.empty()) out = prefix + "::" + out;
        }
        return out;
    }
};

/**
 * @brief Represents a translation unit (source file) that has been parsed.
 *
 * Contains the discovered namespaces and a convenience map for class lookup.
 */
struct TranslationUnit {
    /// The source file name associated with this TU.
    std::string filename;
    /// Top-level namespaces found in this translation unit.
    std::vector<std::shared_ptr<Namespace>> namespaces;
    /// Map of USR to class for all classes in this translation unit.
    std::map<std::string, std::shared_ptr<Class>> classes_by_usr;
};

/**
 * @brief Represents a project (or compilation) as a whole.
 *
 * Typically aggregates all translation units discovered by the parser, and provides
 * a global cross-TU map of classes and relationships for diagram construction.
 */
struct Project {
    /// List of all translation units in the project (header and/or source files).
    std::vector<TranslationUnit> translationUnits;
    /// Global map of USR to class instance across all translation units.
    std::map<std::string, std::shared_ptr<Class>> classes_by_usr;
    /// All recorded relationships in the project (inheritance, association, etc).
    std::vector<Relationship> relationships;
};

} // namespace cppuml::model