# Unicode Confusables Library

This library provides utilities to detect, normalize, and map confusable Unicode characters to their canonical equivalents in C++.

## Features
- Detect confusable Unicode characters in strings
- Normalize confusables to canonical forms
- Simple API for integration
- **Language bindings for C# and Python**

## Build

```bash
mkdir build && cd build
cmake .. && make
```

## Test

```bash
ctest
```

## Language Bindings

This library includes bindings for multiple programming languages:

### Python Bindings
```bash
cd bindings/python
./build.sh
```

Usage:
```python
import unicode_confusables

# Detect confusables
confusables = unicode_confusables.contains_confusables("Ηello Wοrld!")
print(f"Found confusables: {confusables}")

# Normalize confusables  
normalized = unicode_confusables.normalize_confusables("Ηello Wοrld!")
print(f"Normalized: {normalized}")
```

### C# Bindings
```bash
cd bindings/csharp
./build.sh
```

Usage:
```csharp
using UnicodeConfusables;

// Detect confusables
var confusables = ConfusablesDetector.ContainsConfusables("Ηello Wοrld!");
Console.WriteLine($"Found {confusables.Count} confusables");

// Normalize confusables
string normalized = ConfusablesDetector.NormalizeConfusables("Ηello Wοrld!");
Console.WriteLine($"Normalized: {normalized}");
```

See `bindings/` directory for detailed setup instructions and documentation.

## Usage

Include the header and link against the library in your project.

---
