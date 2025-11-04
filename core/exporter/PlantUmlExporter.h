#pragma once
// Exportador PlantUML minimalista (header-only).
// Convierte cppuml::model::Project en texto PlantUML.
// Ideal como punto de partida para UI o CLI.

#include <sstream>
#include <algorithm>
#include "../model/UnifiedModel.h"

namespace cppuml::exporters {

using namespace cppuml::model;

// Escape helper sencillo (amplía si hace falta)
inline std::string escape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else out += c;
    }
    return out;
}

inline std::string visibilitySymbol(Access a) {
    return std::string(1, accessToSymbol(a));
}

// Render a single class
inline void renderClass(std::ostringstream& os, const Class& cls) {
    os << (cls.isStruct ? "struct " : "class ") << escape(cls.name);
    if (cls.isTemplate && !cls.templateParams.empty()) {
        os << " <<template>>";
    }
    os << " {\n";
    // fields
    for (const auto& f : cls.fields) {
        os << "    " << visibilitySymbol(f.access) << " "
           << escape(f.type.toString()) << " " << escape(f.name);
        if (f.isStatic) os << " {static}";
        if (f.defaultValue) os << " = " << escape(*f.defaultValue);
        os << "\n";
    }
    // methods
    for (const auto& m : cls.methods) {
        os << "    " << visibilitySymbol(m.access) << " ";
        if (m.isStatic) os << "{static} ";
        os << escape(m.returnType.toString()) << " " << escape(m.name) << "(";
        for (size_t i=0;i<m.params.size();++i) {
            const auto& p = m.params[i];
            os << escape(p.type.toString());
            if (!p.name.empty()) os << " " << escape(p.name);
            if (i + 1 < m.params.size()) os << ", ";
        }
        os << ")";
        if (m.isConst) os << " const";
        if (m.isPureVirtual) os << " = 0";
        os << "\n";
    }
    os << "}\n\n";
}

// Render relationships (inheritance, associations).
inline void renderRelationships(std::ostringstream& os, const Project& p) {
    for (const auto& r : p.relationships) {
        // Safe: only render if both ends are known (fallback: use usr text)
        const std::string from = r.from_usr;
        const std::string to = r.to_usr;
        switch (r.kind) {
            case Relationship::Kind::Inheritance:
                os << escape(from) << " <|-- " << escape(to);
                break;
            case Relationship::Kind::Association:
                os << escape(from) << " --> " << escape(to);
                break;
            case Relationship::Kind::Aggregation:
                os << escape(from) << " o-- " << escape(to);
                break;
            case Relationship::Kind::Composition:
                os << escape(from) << " *-- " << escape(to);
                break;
            case Relationship::Kind::Dependency:
                os << escape(from) << " ..> " << escape(to);
                break;
        }
        if (r.label) os << " : " << escape(*r.label);
        os << "\n";
    }

    // Additionally, produce inheritance arrows from class.bases_usr
    for (const auto& kv : p.classes_by_usr) {
        const auto& cls = kv.second;
        if (!cls) continue;
        for (const auto& base_usr : cls->bases_usr) {
            // use names when possible (prefer short names, fallback to USR)
            std::string derived = cls->name.empty() ? cls->usr : cls->name;
            std::string base = base_usr;
            // try to locate base in project
            auto it = p.classes_by_usr.find(base_usr);
            if (it != p.classes_by_usr.end() && it->second) base = it->second->name;
            os << escape(base) << " <|-- " << escape(derived) << "\n";
        }
    }
}

// Main entrypoint: produce PlantUML content for whole project.
inline std::string toPlantUml(const Project& p, const std::string& title = "UML Diagram") {
    std::ostringstream os;
    os << "@startuml\n";
    os << "title " << escape(title) << "\n\n";

    // Render classes (use classes_by_usr to ensure unique output)
    std::vector<std::shared_ptr<Class>> classes;
    classes.reserve(p.classes_by_usr.size());
    for (const auto& kv : p.classes_by_usr) {
        if (kv.second) classes.push_back(kv.second);
    }
    // Optionally sort by name for stable output
    std::sort(classes.begin(), classes.end(), [](const auto& a, const auto& b){
        return a->name < b->name;
    });

    for (const auto& cptr : classes) {
        renderClass(os, *cptr);
    }

    // relationships
    renderRelationships(os, p);

    os << "\n@enduml\n";
    return os.str();
}

} // namespace cppuml::exporters