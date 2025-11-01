#!/usr/bin/env bash
# Minimal, safe build helper for cpp-uml-generator (Linux)
# Usage:
#   ./build.sh                # configure (UI+tests ON), build, run tests
#   ./build.sh --no-ui        # disable UI
#   ./build.sh --no-tests     # disable tests
#   ./build.sh --clean        # remove build/ and CMake caches before configure
#   ./build.sh --install-deps # install minimal apt deps (Ubuntu/Debian)
#   ./build.sh --jobs N       # set parallel jobs (default: $(nproc))
#   ./build.sh --build-type Debug|Release
#   ./build.sh --run-cli      # run the CLI after successful build
set -euo pipefail

# Defaults
BUILD_DIR=build
BUILD_TYPE=Debug
BUILD_UI=ON
BUILD_TESTS=ON
JOBS=${JOBS:-$(nproc)}
INSTALL_DEPS=0
CLEAN=0
RUN_CLI=0

usage() {
  cat <<EOF
Usage: $0 [options]
Options:
  --no-ui           Disable building the Qt UI
  --no-tests        Disable building unit tests
  --clean           Remove build/ and CMake caches before configuring
  --install-deps    Install minimal system deps (Ubuntu/Debian)
  --jobs N          Parallel build jobs (default: $JOBS)
  --build-type TYPE Build type: Debug or Release (default: $BUILD_TYPE)
  --run-cli         Run built CLI binary after successful build
  -h, --help        Show this help
EOF
  exit 1
}

# Parse args
while [[ $# -gt 0 ]]; do
  case "$1" in
    --no-ui) BUILD_UI=OFF; shift ;;
    --no-tests) BUILD_TESTS=OFF; shift ;;
    --clean) CLEAN=1; shift ;;
    --install-deps) INSTALL_DEPS=1; shift ;;
    --run-cli) RUN_CLI=1; shift ;;
    --jobs) JOBS="$2"; shift 2 ;;
    --build-type) BUILD_TYPE="$2"; shift 2 ;;
    -h|--help) usage ;;
    *) echo "Unknown arg: $1"; usage ;;
  esac
done

info() { echo -e "\033[1;34m[info]\033[0m $*"; }
warn() { echo -e "\033[1;33m[warn]\033[0m $*"; }
err() { echo -e "\033[1;31m[error]\033[0m $*"; }

# Optional: install deps on Debian/Ubuntu
if [[ $INSTALL_DEPS -eq 1 ]]; then
  if command -v apt-get >/dev/null 2>&1; then
    info "Installing deps (sudo apt-get)..."
    sudo apt-get update
    sudo apt-get install -y build-essential cmake pkg-config qtbase5-dev libqt5widgets5 git
    info "Dependencies installed."
  else
    warn "apt-get not found. Please install dependencies manually (cmake, build-essential, Qt5)."
  fi
fi

# Prevent in-source build artifacts
if [[ $CLEAN -eq 1 ]]; then
  info "Cleaning build directory and CMake caches..."
  rm -rf "${BUILD_DIR}" CMakeCache.txt CMakeFiles cmake_install.cmake Makefile
fi

# Configure
info "Configuring (build dir = ${BUILD_DIR})..."
cmake -S . -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DBUILD_UI="${BUILD_UI}" \
  -DBUILD_TESTS="${BUILD_TESTS}"

# Build
info "Building (jobs=${JOBS})..."
cmake --build "${BUILD_DIR}" -- -j"${JOBS}"

# Run tests if enabled
if [[ "${BUILD_TESTS}" == "ON" ]]; then
  if command -v ctest >/dev/null 2>&1; then
    info "Running tests..."
    ctest --test-dir "${BUILD_DIR}" --output-on-failure || {
      err "Some tests failed."
      exit 2
    }
    info "Tests passed."
  else
    warn "ctest not found; skipping tests."
  fi
fi

# Optionally run CLI
if [[ ${RUN_CLI} -eq 1 ]]; then
  CLI_PATH="${BUILD_DIR}/cli/cppuml_cli"
  if [[ -x "${CLI_PATH}" ]]; then
    info "Running CLI: ${CLI_PATH}"
    "${CLI_PATH}"
  else
    warn "CLI binary not found or not executable at ${CLI_PATH}"
  fi
fi

info "Build finished successfully."
exit 0