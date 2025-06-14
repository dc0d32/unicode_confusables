#!/bin/bash
# Build script for C# bindings

set -e

echo "Building C# bindings for Unicode Confusables..."

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "Project root: $PROJECT_ROOT"

# Create build directory
BUILD_DIR="$PROJECT_ROOT/build_csharp"
mkdir -p "$BUILD_DIR"

# Configure and build with CMake
echo "Configuring CMake..."
cd "$BUILD_DIR"
cmake -DBUILD_CSHARP_BINDINGS=ON "$PROJECT_ROOT"

echo "Building native library..."
make -j$(nproc) unicode_confusables_csharp

# Create runtime directories for different platforms
CSHARP_DIR="$PROJECT_ROOT/bindings/csharp"
mkdir -p "$CSHARP_DIR/runtimes/linux-x64/native"
mkdir -p "$CSHARP_DIR/runtimes/osx-x64/native"
mkdir -p "$CSHARP_DIR/runtimes/win-x64/native"

# Copy the built library to the appropriate runtime directory
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cp libunicode_confusables_csharp.so "$CSHARP_DIR/runtimes/linux-x64/native/"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    cp libunicode_confusables_csharp.dylib "$CSHARP_DIR/runtimes/osx-x64/native/"
fi

echo "Building C# package..."
cd "$CSHARP_DIR"
dotnet build

echo "C# bindings built successfully!"
echo "The shared library is located at: $BUILD_DIR/libunicode_confusables_csharp.*"
echo "You can test the bindings by running: dotnet run --project $CSHARP_DIR TestProgram.cs"
