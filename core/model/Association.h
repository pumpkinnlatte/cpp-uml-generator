#ifndef CPP_UML_GENERATOR_CORE_MODEL_ASSOCIATION_H
#define CPP_UML_GENERATOR_CORE_MODEL_ASSOCIATION_H

#include <string>
#include <utility> // Para std::move

#include "Relationship.h"
#include "Element.h"

namespace cppuml {

/**
 * @brief Modela una relación de Asociación UML.
 *
 * Conecta dos Elementos (típicamente Clases) usando punteros no
 * propietarios.
 *
 * El Parser detectará esto (p.ej., un 'Field' de tipo 'OtherClass*')
 * y creará un objeto Association.
 */
class Association : public Relationship {
public:
    /**
     * @brief Constructor para una Asociación.
     * @param source El elemento que 'posee' la asociación (p.ej., la Clase con el 'Field').
     * @param destination El elemento al que se 'apunta' (p.ej., el tipo del 'Field').
     */
    Association(Element* source, Element* destination)
        : m_source(source), m_destination(destination) {}

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Association() override = default;

    // --- Implementación de la Interfaz Relationship ---

    RelationshipKind getKind() const override {
        return RelationshipKind::Association;
    }

    Element* getSource() const override { return m_source; }
    Element* getDestination() const override { return m_destination; }

    // --- Metadatos Específicos de Asociación (para UML/PlantUML) ---

    void setSourceMultiplicity(std::string mult) { m_sourceMultiplicity = std::move(mult); }
    const std::string& getSourceMultiplicity() const { return m_sourceMultiplicity; }

    void setDestinationMultiplicity(std::string mult) { m_destMultiplicity = std::move(mult); }
    const std::string& getDestinationMultiplicity() const { return m_destMultiplicity; }
    
    void setLabel(std::string label) { m_label = std::move(label); }
    const std::string& getLabel() const { return m_label; }

private:
    Element* m_source;      // Puntero no propietario (Lineamiento P3)
    Element* m_destination; // Puntero no propietario (Lineamiento P3)

    std::string m_label;
    std::string m_sourceMultiplicity; // p.ej., "1", "0..1"
    std::string m_destMultiplicity; // p.ej., "0..*", "1..*"
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_ASSOCIATION_H