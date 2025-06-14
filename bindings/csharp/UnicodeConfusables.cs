using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UnicodeConfusables
{
    /// <summary>
    /// Unicode normalization types
    /// </summary>
    public enum NormalizationType
    {
        /// <summary>Normalization Form Composed</summary>
        NFC = 0,
        /// <summary>Normalization Form Decomposed</summary>
        NFD = 1,
        /// <summary>Normalization Form Compatibility Composed</summary>
        NFKC = 2,
        /// <summary>Normalization Form Compatibility Decomposed</summary>
        NFKD = 3
    }

    /// <summary>
    /// Provides utilities for detecting and normalizing Unicode confusable characters.
    /// </summary>
    public static class ConfusablesDetector
    {
        private const string LibraryName = "unicode_confusables_csharp";

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr unicode_confusables_contains_confusables([MarshalAs(UnmanagedType.LPUTF8Str)] string input);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void unicode_confusables_free_set(IntPtr handle);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int unicode_confusables_set_size(IntPtr handle);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr unicode_confusables_set_get(IntPtr handle, int index);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr unicode_confusables_normalize_confusables([MarshalAs(UnmanagedType.LPUTF8Str)] string input);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr unicode_confusables_unicode_normalize([MarshalAs(UnmanagedType.LPUTF8Str)] string input, int type, int stripZeroWidth);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void unicode_confusables_free_string(IntPtr str);

        /// <summary>
        /// Returns the set of confusable Unicode characters found in the input string.
        /// </summary>
        /// <param name="input">The input string to analyze</param>
        /// <returns>A hash set containing the confusable characters found</returns>
        /// <exception cref="ArgumentNullException">Thrown when input is null</exception>
        public static HashSet<string> ContainsConfusables(string input)
        {
            if (input == null)
                throw new ArgumentNullException(nameof(input));

            var result = new HashSet<string>();
            IntPtr handle = unicode_confusables_contains_confusables(input);
            
            if (handle == IntPtr.Zero)
                return result;

            try
            {
                int size = unicode_confusables_set_size(handle);
                for (int i = 0; i < size; i++)
                {
                    IntPtr itemPtr = unicode_confusables_set_get(handle, i);
                    if (itemPtr != IntPtr.Zero)
                    {
                        string item = Marshal.PtrToStringUTF8(itemPtr);
                        if (item != null)
                        {
                            result.Add(item);
                        }
                    }
                }
            }
            finally
            {
                unicode_confusables_free_set(handle);
            }

            return result;
        }

        /// <summary>
        /// Returns a new string with confusable characters replaced by their canonical equivalents.
        /// </summary>
        /// <param name="input">The input string to normalize</param>
        /// <returns>A normalized string with confusables replaced</returns>
        /// <exception cref="ArgumentNullException">Thrown when input is null</exception>
        public static string NormalizeConfusables(string input)
        {
            if (input == null)
                throw new ArgumentNullException(nameof(input));

            IntPtr resultPtr = unicode_confusables_normalize_confusables(input);
            if (resultPtr == IntPtr.Zero)
                return input; // Return original string if normalization fails

            try
            {
                return Marshal.PtrToStringUTF8(resultPtr) ?? input;
            }
            finally
            {
                unicode_confusables_free_string(resultPtr);
            }
        }

        /// <summary>
        /// Returns a new string with Unicode normalization applied.
        /// </summary>
        /// <param name="input">The input string to normalize</param>
        /// <param name="type">The type of normalization to apply</param>
        /// <param name="stripZeroWidth">If true, zero-width characters are removed after normalization</param>
        /// <returns>A normalized string</returns>
        /// <exception cref="ArgumentNullException">Thrown when input is null</exception>
        public static string UnicodeNormalize(string input, NormalizationType type, bool stripZeroWidth = false)
        {
            if (input == null)
                throw new ArgumentNullException(nameof(input));

            IntPtr resultPtr = unicode_confusables_unicode_normalize(input, (int)type, stripZeroWidth ? 1 : 0);
            if (resultPtr == IntPtr.Zero)
                return input; // Return original string if normalization fails

            try
            {
                return Marshal.PtrToStringUTF8(resultPtr) ?? input;
            }
            finally
            {
                unicode_confusables_free_string(resultPtr);
            }
        }

        /// <summary>
        /// Returns a new string with NFKD normalization applied.
        /// </summary>
        /// <param name="input">The input string to normalize</param>
        /// <param name="stripZeroWidth">If true, zero-width characters are removed after normalization</param>
        /// <returns>A NFKD normalized string</returns>
        /// <exception cref="ArgumentNullException">Thrown when input is null</exception>
        [Obsolete("Use UnicodeNormalize with NormalizationType.NFKD instead")]
        public static string UnicodeNormalizeKd(string input, bool stripZeroWidth = false)
        {
            return UnicodeNormalize(input, NormalizationType.NFKD, stripZeroWidth);
        }
    }
}
