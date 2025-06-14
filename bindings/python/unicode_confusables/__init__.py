"""
Python wrapper for Unicode Confusables detection and normalization.

This module provides utilities for detecting and normalizing Unicode confusable characters.
"""

from typing import Set
try:
    import unicode_confusables_py as _backend
except ImportError:
    # For development/testing when the module isn't built yet
    _backend = None

__version__ = "1.0.0"
__all__ = ["contains_confusables", "normalize_confusables", "unicode_normalize_kd"]


def contains_confusables(input_text: str) -> Set[str]:
    """
    Returns the set of confusable Unicode characters found in the input string.
    
    Args:
        input_text: The input string to analyze
        
    Returns:
        A set containing the confusable characters found
        
    Raises:
        TypeError: If input_text is not a string
        RuntimeError: If the native module is not available
    """
    if _backend is None:
        raise RuntimeError("Native unicode_confusables_py module not available. Build the extension first.")
    
    if not isinstance(input_text, str):
        raise TypeError("input_text must be a string")
    
    return _backend.contains_confusables(input_text)


def normalize_confusables(input_text: str) -> str:
    """
    Returns a new string with confusable characters replaced by their canonical equivalents.
    
    Args:
        input_text: The input string to normalize
        
    Returns:
        A normalized string with confusables replaced
        
    Raises:
        TypeError: If input_text is not a string
        RuntimeError: If the native module is not available
    """
    if _backend is None:
        raise RuntimeError("Native unicode_confusables_py module not available. Build the extension first.")
    
    if not isinstance(input_text, str):
        raise TypeError("input_text must be a string")
    
    return _backend.normalize_confusables(input_text)


def unicode_normalize_kd(input_text: str, strip_zero_width: bool = False) -> str:
    """
    Returns a new string with NFKD normalization applied.
    
    Args:
        input_text: The input string to normalize
        strip_zero_width: If True, zero-width characters are removed after normalization
        
    Returns:
        A NFKD normalized string
        
    Raises:
        TypeError: If input_text is not a string or strip_zero_width is not a boolean
        RuntimeError: If the native module is not available
    """
    if _backend is None:
        raise RuntimeError("Native unicode_confusables_py module not available. Build the extension first.")
    
    if not isinstance(input_text, str):
        raise TypeError("input_text must be a string")
    if not isinstance(strip_zero_width, bool):
        raise TypeError("strip_zero_width must be a boolean")
    
    return _backend.unicode_normalize_kd(input_text, strip_zero_width)
