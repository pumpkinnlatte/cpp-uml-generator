# Cpp-UML-Generator


`Cpp-UML-Generator` is a visualization tool for C++. It is designed to parse C++ projects and generate UML class diagrams to aid in code comprehension, documentation, and refactoring.

This project uses **libclang** to parse a full Abstract Syntax Tree (AST). This allows a semantic analysis of the C++ code, resulting in accurate UML representation that correctly handles namespaces, inheritance, and member visibility.

The parsed code model is then exported to the **PlantUML** format, enabling the generation of clear and maintainable class diagrams.



## Key Features

* **Robust Parsing:** Uses `libclang` for accurate syntactic and semantic analysis of C++ code (including C++17/20).
* **PlantUML Output:** Generates class diagrams in the human-readable PlantUML (`.puml`) format.

## Table of Contents

* [Installation and Building (Quickstart)](#installation-and-building-quickstart)
* [Running (CLI / UI)](#running-cli--ui)
* [Running Tests](#running-tests)
* [Project Details](#project-details)

## Installation and Building (Quickstart)

### Requirements

* Build tools: `build-essential`, `cmake`, `pkg-config`, `git`, `ca-certificates`, `curl`.
* Qt 6: `qt6-base-dev` (or the equivalent development package for your distribution).
* LLVM/Clang 12+ (for `libclang`) - 

### 1. Clone the Repository

```bash
git clone [https://github.com/pumpkinnlatte/cpp-uml-generator.git](https://github.com/pumpkinnlatte/cpp-uml-generator.git)
cd cpp-uml-generator
```

### 2\. Build (Option A: Helper Script)

The `build.sh` script automates the configure, build, and (optionally) test steps.

```bash
./build.sh
```

**Useful script options:**

  * `--no-ui`: Disable building the Qt GUI.
  * `--no-tests`: Disable building unit tests.
  * `--clean`: Remove the `build/` directory before configuring.
  * `--install-deps`: Try to install dependencies (Ubuntu/Debian).
  * `--jobs N`: Specify the number of parallel jobs (e.g., `--jobs $(nproc)`).
  * `--run-cli`: Run the CLI after a successful build.

### 3\. Build (Option B: Manual CMake)

You can configure and build the project manually:

```bash
# Configure the project (enabling UI and Tests)
cmake -S . -B build -DBUILD_UI=ON -DBUILD_TESTS=ON

# Build
cmake --build build -- -j$(nproc)
```

### 4. Running Tests

If the project was configured with `-DBUILD_TESTS=ON`, you can run the tests using CTest:

```bash
ctest --test-dir build --output-on-failure
```

## Project Details

### Project Structure

  * `CMakeLists.txt`: Main CMake configuration and options.
  * `core/`: Core library (`libcppuml`): parser, data model, and exporters.
  * `cli/`: Command-line interface executable.
  * `ui/`: GUI application (Qt Widgets).
  * `test/`: Unit and integration tests.
  * `build.sh`: Build helper script.
  * `.github/workflows/`: Continuous Integration (CI) configuration.

### CMake Options

You can customize the build by passing these variables to CMake:

  * `BUILD_UI` (ON/OFF): Build the Qt GUI application. (Default: ON)
  * `BUILD_TESTS` (ON/OFF): Enable building the tests. (Default: ON)

**Example (minimal CLI-only build):**

```bash
cmake -S . -B build -DBUILD_UI=OFF -DBUILD_TESTS=OFF
cmake --build build
```

## Current Project Status

- This project is under active development.  
- The current work focuses on completing Phases I and II of this roadmap to achieve the Minimum Viable Product (MVP).  
- The MVP objective is: To produce a tool capable of analyzing a small or medium-sized C++ project, generating a readable PlantUML diagram, and displaying it within the Qt application with export options.
