#!/bin/sh
# Builder installation script
# Usage: curl -fsSL https://builder.opifex.org/install.sh | sh
#
# Environment variables:
#   PREFIX          - Installation prefix (default: /usr/local)
#   BUILDER_BRANCH  - Git branch to install from (default: master)

set -e

# Configuration
REPO_URL="https://github.com/devOpifex/builder.git"
BRANCH="${BUILDER_BRANCH:-master}"
PREFIX="${PREFIX:-/usr/local}"

# Colors (disabled if not a terminal)
if [ -t 1 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[0;33m'
    BLUE='\033[0;34m'
    NC='\033[0m'
else
    RED=''
    GREEN=''
    YELLOW=''
    BLUE=''
    NC=''
fi

info() {
    printf "${BLUE}==>${NC} %s\n" "$1"
}

success() {
    printf "${GREEN}==>${NC} %s\n" "$1"
}

warn() {
    printf "${YELLOW}Warning:${NC} %s\n" "$1"
}

error() {
    printf "${RED}Error:${NC} %s\n" "$1" >&2
    exit 1
}

# Banner
printf "\n"
printf "${BLUE}  ____        _ _     _           ${NC}\n"
printf "${BLUE} | __ ) _   _(_) | __| | ___ _ __ ${NC}\n"
printf "${BLUE} |  _ \\| | | | | |/ _\` |/ _ \\ '__|${NC}\n"
printf "${BLUE} | |_) | |_| | | | (_| |  __/ |   ${NC}\n"
printf "${BLUE} |____/ \\__,_|_|_|\\__,_|\\___|_|   ${NC}\n"
printf "\n"
printf "  R source preprocessor\n"
printf "\n"

# Check dependencies
info "Checking dependencies..."

# Check for git
if ! command -v git >/dev/null 2>&1; then
    error "git is not installed. Please install git first."
fi

# Check for make
if ! command -v make >/dev/null 2>&1; then
    error "make is not installed. Please install make first."
fi

# Check for R
if ! command -v R >/dev/null 2>&1; then
    error "R is not installed. Please install R first."
fi

# Check for C compiler (via R)
if ! R CMD config CC >/dev/null 2>&1; then
    error "C compiler not found. R CMD config CC failed."
fi

success "All dependencies found"

# Create temp directory
TMPDIR=$(mktemp -d)
cleanup() {
    rm -rf "$TMPDIR"
}
trap cleanup EXIT

info "Cloning repository (branch: $BRANCH)..."
git clone --depth 1 --branch "$BRANCH" "$REPO_URL" "$TMPDIR/builder" >/dev/null 2>&1 || \
    error "Failed to clone repository"

cd "$TMPDIR/builder"

info "Building..."
make build >/dev/null 2>&1 || error "Build failed"

info "Installing to $PREFIX/bin..."

# Check if we need sudo
if [ -w "$PREFIX/bin" ] 2>/dev/null || [ -w "$PREFIX" ] 2>/dev/null; then
    make install PREFIX="$PREFIX" >/dev/null 2>&1 || error "Installation failed"
else
    warn "Need elevated permissions to install to $PREFIX"
    sudo make install PREFIX="$PREFIX" >/dev/null 2>&1 || error "Installation failed (sudo)"
fi

success "Installation complete!"
printf "\n"
printf "  Run ${GREEN}builder -help${NC} to get started\n"
printf "\n"

# Verify installation
if command -v builder >/dev/null 2>&1; then
    success "builder is available in PATH"
else
    warn "builder was installed but may not be in your PATH"
    printf "  Add ${YELLOW}%s/bin${NC} to your PATH\n" "$PREFIX"
fi
