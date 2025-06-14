#!/bin/bash
# Build script for Python bindings

set -e

echo "Building Python bindings for Unicode Confusables..."

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$(dirname "$(dirname "$SCRIPT_DIR")")"

echo "Project root: $PROJECT_ROOT"

cd "$SCRIPT_DIR"

# Check if virtual environment should be created
if [ ! -d "venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv venv
fi

# Activate virtual environment
echo "Activating virtual environment..."
source venv/bin/activate

# Install requirements
echo "Installing Python requirements..."
pip install -r requirements.txt

# First, ensure the data files are generated
echo "Ensuring Unicode confusables data is generated..."
BUILD_DIR="$PROJECT_ROOT/build"
if [ ! -f "$PROJECT_ROOT/include/unicode_confusables_data.h" ]; then
    echo "Generating confusables data..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake "$PROJECT_ROOT"
    make generate_confusables_header
    cd "$SCRIPT_DIR"
fi

# Build the Python extension
echo "Building Python extension..."
python setup.py build_ext --inplace

# Install in development mode
echo "Installing package in development mode..."
pip install -e .

echo "Python bindings built successfully!"
echo "You can test the bindings by running: python test_unicode_confusables.py"
echo "Or by importing: import unicode_confusables"

echo "To use the bindings, activate the virtual environment with:"
echo "source $SCRIPT_DIR/venv/bin/activate"
