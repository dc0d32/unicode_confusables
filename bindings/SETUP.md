# Unicode Confusables Bindings - Setup Guide

This guide explains how to build and use the C# and Python bindings for the Unicode Confusables library.

## Prerequisites

### System Requirements
- Linux, macOS, or Windows
- CMake 3.10 or later
- ICU development libraries
- C++17 compatible compiler

### Installing ICU Development Libraries

#### Ubuntu/Debian
```bash
sudo apt-get install libicu-dev pkg-config
```

#### macOS
```bash
brew install icu4c pkg-config
```

#### Windows
Download and install ICU from the official website or use vcpkg:
```bash
vcpkg install icu
```

## C# Bindings

### Prerequisites
- .NET 6.0 or later SDK

#### Installing .NET on Ubuntu/Debian
```bash
# Add Microsoft package repository
wget https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
sudo dpkg -i packages-microsoft-prod.deb
rm packages-microsoft-prod.deb

# Install .NET SDK
sudo apt-get update
sudo apt-get install -y dotnet-sdk-6.0
```

### Building C# Bindings
```bash
cd bindings/csharp
./build.sh
```

### Using C# Bindings
```csharp
using UnicodeConfusables;

class Program {
    static void Main() {
        // Detect confusable characters
        var confusables = ConfusablesDetector.ContainsConfusables("Ηello Wοrld!");
        Console.WriteLine($"Found {confusables.Count} confusables");
        
        // Normalize confusables
        string normalized = ConfusablesDetector.NormalizeConfusables("Ηello Wοrld!");
        Console.WriteLine($"Normalized: {normalized}");
        
        // Unicode NFKD normalization
        string nfkd = ConfusablesDetector.UnicodeNormalizeKd("café", stripZeroWidth: true);
        Console.WriteLine($"NFKD: {nfkd}");
    }
}
```

### Testing C# Bindings
```bash
cd bindings/csharp
dotnet run TestProgram.cs
```

## Python Bindings

### Prerequisites
- Python 3.6 or later
- Virtual environment support

### Building Python Bindings
```bash
cd bindings/python
./build.sh
```

This will:
1. Create a Python virtual environment
2. Install required dependencies (pybind11, etc.)
3. Build the native extension
4. Install the package in development mode

### Using Python Bindings
```python
import unicode_confusables

# Detect confusable characters
confusables = unicode_confusables.contains_confusables("Ηello Wοrld!")
print(f"Found {len(confusables)} confusables: {confusables}")

# Normalize confusables
normalized = unicode_confusables.normalize_confusables("Ηello Wοrld!")
print(f"Normalized: {normalized}")

# Unicode NFKD normalization
nfkd = unicode_confusables.unicode_normalize_kd("café", strip_zero_width=True)
print(f"NFKD: {nfkd}")
```

### Testing Python Bindings
```bash
cd bindings/python
source venv/bin/activate  # Activate virtual environment
python test_unicode_confusables.py
```

### Installing Python Package
To install the Python package system-wide or in another environment:
```bash
cd bindings/python
pip install .
```

## API Reference

All bindings expose the same three core functions:

### `contains_confusables(input_text)`
Returns a collection of confusable Unicode characters found in the input string.

**Parameters:**
- `input_text`: String to analyze

**Returns:**
- C#: `HashSet<string>` of confusable characters
- Python: `set` of confusable characters

### `normalize_confusables(input_text)`
Returns a string with confusable characters replaced by their canonical equivalents.

**Parameters:**
- `input_text`: String to normalize

**Returns:**
- String with confusables replaced

### `unicode_normalize_kd(input_text, strip_zero_width)`
Returns a string with NFKD (Normalization Form Compatibility Decomposition) applied.

**Parameters:**
- `input_text`: String to normalize
- `strip_zero_width`: Boolean - if true, removes zero-width characters after normalization

**Returns:**
- NFKD normalized string

## Troubleshooting

### Common Issues

1. **ICU libraries not found**
   - Make sure ICU development libraries are installed
   - On Linux, install `libicu-dev` and `pkg-config`
   - On macOS, use `brew install icu4c`

2. **pybind11 not found (Python)**
   - The build script should handle this automatically by creating a virtual environment
   - If issues persist, manually install: `pip install pybind11`

3. **.NET not found (C#)**
   - Install .NET 6.0 SDK from Microsoft's official website
   - On Linux, follow the official installation guide for your distribution

4. **Build fails with missing confusables data**
   - The build system should automatically download and generate the confusables data
   - If this fails, manually run the main project build first: `cmake . && make`

### Getting Help

If you encounter issues:
1. Check that all prerequisites are installed
2. Verify that the main C++ library builds correctly
3. Review the build output for specific error messages
4. Ensure ICU libraries are properly installed and detectable

## Examples

See the test files for complete examples:
- C#: `bindings/csharp/TestProgram.cs`
- Python: `bindings/python/test_unicode_confusables.py`
