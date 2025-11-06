## 1. Project Overview

`cpp-uml-generator` is a software visualization tool for C++. The objective is to provide a utility capable of analyzing C++ source code and generating accurate UML class diagrams.

## 2. Architecture Principles

The project design is governed by the following principles to ensure maintainability and extensibility:
- **Library-First:** The core (`core/`) is developed as a library (`libcppuml`) with a clean and decoupled public API.
- **Single Responsibility:** Parsing, data model, and exporters reside in `core/`. The `cli/` and `ui/` applications act as clients of this library.
- **Iterative Development:** The plan focuses on delivering a functional end-to-end flow (MVP) before adding complex features.

## 3. Architecture Decisions

The following fundamental technological decisions have been made to define the project scope.

| **Component** | **Decision** | **Justification** |
|---------------|--------------|-------------------|
| **Analysis Backend** | **libclang (LLVM/Clang 18+)** | Provides complete syntactic and semantic analysis (AST), necessary for precision in C++. |
| **UI Framework** | **Qt 6 (Widgets)** | A native C++ framework for cross-platform desktop applications. |
| **UML Rendering** | **Local (PlantUML.jar)** | Ensures offline functionality and user code privacy, eliminating dependency on external servers. |
| **Target Platform** | **Linux (Primary)** | Initial development and testing will focus on Linux distributions. |

## 4. Repository Structure

| **Directory** | **Purpose** |
|---------------|-------------|
| **`core/`** | Main library (`libcppuml`): Parser, internal data model, and exporters. |
| **`cli/`** | Command-Line Interface (CLI) for integration and scripting. |
| **`ui/`** | Graphical User Interface (GUI) application based on Qt. |
| **`test/`** | Unit tests (for `core/`) and integration tests (for `cli/`). |
| **`build/`** | Build artifacts directory (ignored by Git). |
| **`CMakeLists.txt`** | Main CMake project configuration. |

---

## 5. Roadmap by Phases

The development is divided into four main phases, each with specific milestones.

### Phase I: Foundations

The objective of this phase is to achieve a complete flow from analyzing a C++ file to generating a valid `.puml` file, all through a CLI tool.

#### Milestone 1: Analysis (AST) and Data Model

- **Objective:** Implement the core logic to analyze C++ code using `libclang` and store it in an internal data model.
- **Key Tasks:**
  - Design the model classes in `core/model` (e.g., `Class`, `Method`, `Field`, `Relationship`).
  - Implement a `libclang` adapter in `core/parser` that visits the AST.
  - Populate the model with namespaces, classes/structs, members (with visibility), and inheritance relationships.
- **Acceptance Criteria:** Unit tests can pass a C++ snippet to the parser and verify that the resulting data model is correct.
- **Focus:** `core/`, `test/`

#### Milestone 2: PlantUML Exporter

- **Objective:** Convert the internal data model into a textual PlantUML representation.
- **Key Tasks:**
  - Implement a `PlantUmlExporter` in `core/exporters/`.
  - Map model entities (Class, Inheritance) to PlantUML syntax.
- **Acceptance Criteria:** Tests can take a known data model, pass it through the exporter, and validate the resulting `.puml` text string.
- **Focus:** `core/exporters/`, `test/`

#### Milestone 3: Command-Line Interface (CLI)

- **Objective:** Expose the `core` functionality through a command-line tool.
- **Key Tasks:**
  - Implement `cli/main.cpp` that accepts arguments (input file, `compile_commands.json`, output file).
  - Invoke the `libcppuml` API to analyze and export the diagram.
  - Support reading `compile_commands.json` to provide the correct compilation flags to `libclang`.
- **Acceptance Criteria:** `cppuml_cli --input my_class.cpp --output diagram.puml` generates a valid class diagram.
- **Focus:** `cli/`

---

### Phase II: Interactive Minimum Viable Product

The objective of this phase is to provide a graphical interface that uses the `core` library to offer an interactive visualization experience.

#### Milestone 4: Minimal Graphical Interface with Qt

- **Objective:** Create a desktop application to open a C++ project and visualize the UML diagram.
- **Key Tasks:**
  - Implement the `QMainWindow` application structure in `ui/`.
  - Add controls to select a project (and its `compile_commands.json`).
  - Invoke the `core` analysis in a worker thread to avoid blocking the UI.
  - Implement local rendering: The UI will generate the `.puml`, invoke `plantuml.jar` locally to produce an SVG/PNG, and display that image in a preview panel (`QSvgWidget` or similar).
- **Acceptance Criteria:** A user can open a small project, press "Generate," and see a rendered class diagram within the application.
- **Focus:** `ui/`

---

### Phase III: Advanced Features

The objective of this phase is to improve analysis accuracy and UI usability.

#### Milestone 5: UI Interactivity

- **Objective:** Synchronize the diagram view with a code explorer.
- **Key Tasks:**
  - Add an "Inspector" panel that displays details of the selected class.
  - Implement basic filtering (e.g., hide private members, hide namespaces).
  - Add export buttons (PNG, SVG, PUML).
- **Acceptance Criteria:** Clicking on a class in the diagram displays its details in the Inspector panel.
- **Focus:** `ui/`

#### Milestone 6: Advanced Analysis and Relationships

- **Objective:** Enhance the parser to handle more complex C++ constructs.
- **Key Tasks:**
  - Add support for templates.
  - Detect Association and Composition relationships (via heuristics on member types).
  - Properly handle `typedefs` and aliases (`using`).
- **Acceptance Criteria:** A medium-sized C++ project (e.g., a small library) generates a coherent and useful diagram.
- **Focus:** `core/parser/`, `core/model/`

---

### Phase IV: Distribution and Polish
#### Milestone 7: Packaging and Documentation

- **Objective:** Facilitate the installation and use of the tool.
- **Key Tasks:**
  - Enhance the `README.md` with screenshots, GIFs, and usage examples.
  - Create a distribution package (e.g., `AppImage` or `Flatpak` for Linux).
  - Document the public API of `libcppuml`.
- **Acceptance Criteria:** A user can download a single executable file (`AppImage`) and run the graphical application.
- **Focus:** `README.md`, `assets/`, `CMakeLists.txt` (CPack)