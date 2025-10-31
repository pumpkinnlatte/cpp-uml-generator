// Minimal stub for generator target so CMake has a source to compile.
// Replace with real PlantUML/LaTeX generator implementation later.

#include <string>

namespace c2uml {
namespace generator {

// A small stub function to ensure the generator library builds.
std::string plantuml_generator_stub() {
    return "@startuml\n' c2uml generator stub\n@enduml\n";
}

} // namespace generator
} // namespace c2uml