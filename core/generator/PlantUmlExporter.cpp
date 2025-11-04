#include "PlantUmlExporter.h"
#include <sstream>
#include <functional>

using cppuml::Access;
using cppuml::TranslationUnit;
using cppuml::Namespace;
using cppuml::Class;
using cppuml::Field;
using cppuml::Method;
using cppuml::exporters::PlantUmlExporter;

static char accessSymbol(Access a) {
    switch(a) {
        case Access::Public: return '+';
        case Access::Protected: return '#';
        case Access::Private: return '-';
        default: return '~';
    }
}

std::string PlantUmlExporter::exportTranslationUnit(const std::shared_ptr<TranslationUnit>& tu) {
    std::ostringstream out;
    out << "@startuml\n";
    out << "skinparam classAttributeIconSize 0\n\n";

    // Use an explicitly-typed std::function and a different name to avoid analyzer confusion
    std::function<void(const cppuml::Namespace&)> dump_ns;

    dump_ns = [&](const cppuml::Namespace& ns) {
        for (auto &cls : ns.classes) {
            out << "class \"" << cls->name << "\" {\n";
            for (auto &f : cls->fields) {
                out << "  " << accessSymbol(f.access) << " " << f.name << " : " << f.type << "\n";
            }
            for (auto &m : cls->methods) {
                out << "  " << accessSymbol(m.access) << " " << m.name << "(";
                for (size_t i = 0; i < m.params.size(); ++i) {
                    out << m.params[i].first;
                    if (i + 1 < m.params.size()) out << ", ";
                }
                out << ")";
                if (!m.returnType.empty()) out << " : " << m.returnType;
                out << "\n";
            }
            out << "}\n\n";
            // bases
            for (auto &busr : cls->bases_usr) {
                auto it = tu->classes_by_usr.find(busr);
                if (it != tu->classes_by_usr.end()) {
                    out << "\"" << it->second->name << "\" <|-- \"" << cls->name << "\"\n";
                } else {
                    out << "\"" << busr << "\" <|-- \"" << cls->name << "\"\n";
                }
            }
        }
        for (auto &sub : ns.namespaces) dump_ns(*sub);
    };

    for (auto &ns : tu->namespaces) {
        dump_ns(*ns);
    }

    out << "\n@enduml\n";
    return out.str();
}