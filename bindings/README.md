# Unicode Confusables Bindings

This directory contains language bindings for the Unicode Confusables C++ library.

## Available Bindings

### C# Bindings

Located in `csharp/` directory.

#### Prerequisites
- .NET 6.0 or later
- CMake 3.10+
- ICU development libraries

#### Building
```bash
cd bindings/csharp
./build.sh
```

#### Usage
```csharp
using UnicodeConfusables;

// Detect confusable characters
var confusables = ConfusablesDetector.ContainsConfusables("Ηello Wοrld!");
Console.WriteLine($"Found {confusables.Count} confusables");

// Normalize confusables
string normalized = ConfusablesDetector.NormalizeConfusables("Ηello Wοrld!");
Console.WriteLine($"Normalized: {normalized}");

// Unicode NFKD normalization
string nfkd = ConfusablesDetector.UnicodeNormalizeKd("café", stripZeroWidth: true);
```

#### Testing
```bash
cd bindings/csharp
dotnet run TestProgram.cs
```

### Python Bindings

Located in `python/` directory.

#### Prerequisites
- Python 3.6+
- pybind11
- CMake 3.10+
- ICU development libraries

#### Building
```bash
cd bindings/python
./build.sh
```

#### Usage
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

#### Testing
```bash
cd bindings/python
python test_unicode_confusables.py
```

#### Installing from Source
```bash
cd bindings/python
pip install .
```

## Library Functions

All bindings expose the same three core functions:

1. **`contains_confusables(input)`** - Returns a set/collection of confusable characters found in the input string
2. **`normalize_confusables(input)`** - Returns a string with confusable characters replaced by their canonical equivalents  
3. **`unicode_normalize_kd(input, strip_zero_width=false)`** - Returns a NFKD normalized string, optionally removing zero-width characters

## Platform Support

- **Linux**: Fully supported for both C# and Python bindings
- **macOS**: Supported with appropriate ICU installation
- **Windows**: Supported (may require additional ICU setup)

## Development

### Adding New Language Bindings

1. Create a new directory under `bindings/`
2. Implement bindings for the three core functions from `unicode_confusables.h`
3. Add build configuration to the main `CMakeLists.txt`
4. Create build scripts and documentation
5. Add tests

### ICU Requirements

All bindings require ICU (International Components for Unicode) libraries:
- Ubuntu/Debian: `sudo apt-get install libicu-dev`
- macOS: `brew install icu4c`
- Windows: Download from ICU website or use vcpkg

## License

MIT License - same as the main Unicode Confusables library.
