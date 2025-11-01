# cpp-uml-generator

A lightweight tool to generate UML class diagrams from C++ source.  

## Table of Contents
- [Quickstart](#quickstart)
- [Requirements](#requirements)
- [Build (recommended)](#build-recommended)
- [Build script (build.sh)](#build-script-buildsh)
- [Run (CLI / UI)](#run-cli--ui)
- [Tests](#tests)
- [Project layout](#project-layout)
- [CMake options](#cmake-options)

## Quickstart
1. **Clone**:
   ```bash
   git clone https://github.com/pumpkinnlatte/cpp-uml-generator.git
   cd cpp-uml-generator
   ```

2. **Install minimal dependencies** (Ubuntu/Debian example):
   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake pkg-config git ca-certificates curl
   # Qt6 development package — name may vary by distro:
   sudo apt install -y qt6-base-dev
   ```

3. **Build and test using the helper script**:
   ```bash
   ./build.sh                # default: BUILD_UI=ON, BUILD_TESTS=ON
   ```

   Alternatively, **manual configure + build**:
   ```bash
   cmake -S . -B build -DBUILD_UI=ON -DBUILD_TESTS=ON
   cmake --build build -- -j$(nproc)
   ctest --test-dir build --output-on-failure
   ```

## Requirements
- Build-essential tools: `build-essential`, `cmake`, `pkg-config`, `git`, `ca-certificates`, `curl`.
- Qt6: `qt6-base-dev` (or equivalent for your distribution).

## Build script (build.sh)
A helper script automates configure → build → test.  

**Useful options**:
- `--no-ui`: Disable building the Qt UI.
- `--no-tests`: Disable unit tests.
- `--clean`: Remove `build/` and caches before configuring.
- `--install-deps`: Try to install deps (Ubuntu/Debian).
- `--jobs N`: Parallel jobs.
- `--run-cli`: Run CLI after successful build.

## Run (CLI / UI)
- **CLI**:
  ```bash
  ./build/cli/cppuml_cli
  ```

- **UI** (if built with UI enabled):
  ```bash
  ./build/ui/cppuml_ui
  ```

## Tests
- **Run tests**:
  ```bash
  ctest --test-dir build --output-on-failure
  ```

## Project layout
- `CMakeLists.txt` — Top-level configuration and options.
- `core/` — Core library: parser, model, export.
- `cli/` — CLI executable.
- `ui/` — Qt (Widgets) application.
- `test/` — Tests and CMake configuration for tests.
- `build.sh` — Build helper script.
- `.github/workflows/` — CI configuration (build + tests).

## CMake options
- `BUILD_UI` (ON/OFF) — Build Qt UI.
- `BUILD_TESTS` (ON/OFF) — Enable/disable tests.

**Example**:
```bash
cmake -S . -B build -DBUILD_UI=OFF -DBUILD_TESTS=ON
```