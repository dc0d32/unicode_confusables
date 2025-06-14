#!/usr/bin/env python3
"""
Test script for Unicode Confusables Python bindings.
"""

import sys
import os

# Add the parent directory to the path so we can import unicode_confusables
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import unicode_confusables
    print("Unicode Confusables Python Test")
    print("================================")

    # Test string with confusables
    test_input = "Ηello Wοrld! Тhis hаs cοnfusаble chаrаcters."
    print(f"Input: {test_input}")

    # Test contains_confusables
    try:
        confusables = unicode_confusables.contains_confusables(test_input)
        print(f"Found {len(confusables)} confusable characters:")
        for confusable in sorted(confusables):
            print(f"  - '{confusable}'")
    except Exception as ex:
        print(f"Error detecting confusables: {ex}")

    # Test normalize_confusables
    try:
        normalized = unicode_confusables.normalize_confusables(test_input)
        print(f"Normalized: {normalized}")
    except Exception as ex:
        print(f"Error normalizing confusables: {ex}")

    # Test unicode_normalize_kd
    try:
        kd_normalized = unicode_confusables.unicode_normalize_kd(test_input, strip_zero_width=True)
        print(f"NFKD Normalized: {kd_normalized}")
    except Exception as ex:
        print(f"Error with NFKD normalization: {ex}")

    print("\nAll tests completed successfully!")

except ImportError as e:
    print(f"Failed to import unicode_confusables: {e}")
    print("Make sure to build and install the Python extension first.")
    sys.exit(1)
except Exception as e:
    print(f"Unexpected error: {e}")
    sys.exit(1)
