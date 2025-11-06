#!/usr/bin/env bash
# Robust build script for cpp-uml-generator (Linux)
#
# Options:
#   ./build.sh                # configure, build, test (Debug)
#   ./build.sh --no-ui        # disable UI build
#   ./build.sh --no-tests     # disable test build
#   ./build.sh --clean        # clean 'build/' directory
#   ./build.sh --install-deps # install dependencies (Ubuntu/Debian)
#   ./build.sh --jobs N       # parallel build jobs
#   ./build.sh --build-type Debug|Release
#   ./build.sh --run-cli      # run CLI after build
#
set -euo pipefail

# --- Script Root Directory ---
# Move to the directory where the script is located
# This allows the script to be run from anywhere.
cd "$(dirname "$0")"

# --- Default Options ---
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
  --no-ui           Disable building the Qt GUI
  --no-tests        Disable building unit tests
  --clean           Remove the '$BUILD_DIR' directory before configuring
  --install-deps    Install minimal system dependencies (Ubuntu/Debian)
  --jobs N          Parallel build jobs (Default: $JOBS)
  --build-type TYPE Build type: Debug or Release (Default: $BUILD_TYPE)
  --run-cli         Run the CLI after a successful build
  -h, --help        Show this help
EOF
  exit 1
}

# --- Argument Parsing ---
while [[ $# -gt 0 ]]; do
  case "$1" in
    --no-ui) BUILD_UI=OFF; shift ;;
    --no-tests) BUILD_TESTS=OFF; shift ;;
    --clean) CLEAN=1; shift ;;
    --install-deps) INSTALL_DEPS=1; shift ;;
    --run-cli) RUN_CLI=1; shift ;;
    
    # <-- IMPROVEMENT: Robust argument validation -->
    --jobs)
      if [[ -z "$2" || "$2" == -* ]]; then
        err "Option '$1' requires a numeric argument."
        usage
      fi
      JOBS="$2"; shift 2 ;;
    --build-type)
      if [[ -z "$2" || "$2" == -* ]]; then
        err "Option '$1' requires an argument (Debug or Release)."
        usage
      fi
      BUILD_TYPE="$2"; shift 2 ;;
    # <-- END OF IMPROVEMENT -->
    
    -h|--help) usage ;;
    *) echo "Unknown argument: $1"; usage ;;
  esac
done

# --- Logging Functions ---
info() { echo -e "\033[1;34m[info]\033[0m $*"; }
warn() { echo -e "\033[1;33m[warn]\033[0m $*"; }
err() { echo -e "\033[1;31m[error]\033[0m $*"; }

# --- 1. Install Dependencies (Optional) ---
if [[ $INSTALL_DEPS -eq 1 ]]; then
  if command -v apt-get >/dev/null 2>&1; then
    info "Installing dependencies (sudo apt-get)..."
    DEPS=(
        build-essential
        cmake
        pkg-config
        git
        ca-certificates
        curl
        qt6-base-dev   # <-- IMPROVEMENT: Qt 6
        libclang-18-dev # <-- UPDATED: As per user's LLVM 18.1.3
    )
    sudo apt-get update
    sudo apt-get install -y "${DEPS[@]}"
    info "Dependencies installed."
  else
    warn "apt-get not found. Please install dependencies manually."
  fi
fi

# --- 2. Clean (Optional) ---
if [[ $CLEAN -eq 1 ]]; then
  info "Cleaning build directory: ${BUILD_DIR}" # <-- IMPROVEMENT: Simplified logic
  rm -rf "${BUILD_DIR}"
fi

# --- 3. Configuration (Only if necessary) ---
# <-- IMPROVEMENT: Do not reconfigure if 'CMakeCache.txt' already exists -->
if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
  info "Configuring (build dir = ${BUILD_DIR})..."
  cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DBUILD_UI="${BUILD_UI}" \
    -DBUILD_TESTS="${BUILD_TESTS}"
else
  info "Existing configuration found in ${BUILD_DIR}. Skipping reconfiguration."
  warn "Use --clean if you want to change CMake options."
fi

# --- 4. Build ---
info "Building (type=${BUILD_TYPE}, jobs=${JOBS})..."
cmake --build "${BUILD_DIR}" -- -j"${JOBS}"

# --- 5. Tests (Optional) ---
if [[ "${BUILD_TESTS}" == "ON" ]]; then
  if command -v ctest >/dev/null 2>&1; then
    info "Running tests..."
    ctest --test-dir "${BUILD_DIR}" --output-on-failure || {
      err "Some tests failed!"
      exit 2 # Exit with a specific error code for failed tests
    }
    info "All tests passed."
  else
    warn "ctest not found; skipping tests."
  fi
fi

# --- 6. Run CLI (Optional) ---
if [[ ${RUN_CLI -eq 1} ]]; then
  CLI_PATH="${BUILD_DIR}/cli/cppuml_cli"
  if [[ -x "${CLI_PATH}" ]]; then
    info "Running CLI: ${CLI_PATH} --help"
    "${CLI_PATH}" --help # <-- IMPROVEMENT: Running with --help is more useful for a test run
  else
    warn "CLI binary not found at ${CLI_PATH} (did it build successfully?)"
  fi
fi

info "Build script finished successfully."
exit 0