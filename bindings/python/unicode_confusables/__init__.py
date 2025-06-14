"""
Python wrapper for Unicode Confusables detection and normalization.

This module provides utilities for detecting and normalizing Unicode confusable characters.
"""

from typing import Set
from enum import IntEnum

try:
    import unicode_confusables_py as _backend
    # Import the enum from the backend
    NormalizationType = _backend.NormalizationType
except ImportError:
    # For development/testing when the module isn't built yet
    _backend = None
    
    # Define a fallback enum for when the backend isn't available
    class NormalizationType(IntEnum):
        """Unicode normalization types"""
        NFC = 0   # Normalization Form Composed
        NFD = 1   # Normalization Form Decomposed
        NFKC = 2  # Normalization Form Compatibility Composed
        NFKD = 3  # Normalization Form Compatibility Decomposed

__version__ = "1.0.0"
__all__ = ["contains_confusables", "normalize_confusables", "unicode_normalize", "unicode_normalize_kd", "NormalizationType"]


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


def unicode_normalize(input_text: str, normalization_type: NormalizationType, strip_zero_width: bool = False) -> str:
    """
    Returns a new string with Unicode normalization applied.
    
    Args:
        input_text: The input string to normalize
        normalization_type: The type of normalization to apply (NFC, NFD, NFKC, or NFKD)
        strip_zero_width: If True, zero-width characters are removed after normalization
        
    Returns:
        A normalized string
        
    Raises:
        TypeError: If arguments are not of the correct type
        RuntimeError: If the native module is not available
    """
    if _backend is None:
        raise RuntimeError("Native unicode_confusables_py module not available. Build the extension first.")
    
    if not isinstance(input_text, str):
        raise TypeError("input_text must be a string")
    if not isinstance(normalization_type, (NormalizationType, int)):
        raise TypeError("normalization_type must be a NormalizationType")
    if not isinstance(strip_zero_width, bool):
        raise TypeError("strip_zero_width must be a boolean")
    
    return _backend.unicode_normalize(input_text, normalization_type, strip_zero_width)


def unicode_normalize_kd(input_text: str, strip_zero_width: bool = False) -> str:
    """
    Returns a new string with NFKD normalization applied.
    
    DEPRECATED: Use unicode_normalize with NormalizationType.NFKD instead.
    
    Args:
        input_text: The input string to normalize
        strip_zero_width: If True, zero-width characters are removed after normalization
        
    Returns:
        A NFKD normalized string
        
    Raises:
        TypeError: If input_text is not a string or strip_zero_width is not a boolean
        RuntimeError: If the native module is not available
    """
    return unicode_normalize(input_text, NormalizationType.NFKD, strip_zero_width)
