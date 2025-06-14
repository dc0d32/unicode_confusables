using System;
using UnicodeConfusables;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Unicode Confusables C# Test");
        Console.WriteLine("============================");

        // Test string with confusables
        string testInput = "Ηello Wοrld! Тhis hаs cοnfusаble chаrаcters.";
        Console.WriteLine($"Input: {testInput}");

        // Test contains_confusables
        try
        {
            var confusables = ConfusablesDetector.ContainsConfusables(testInput);
            Console.WriteLine($"Found {confusables.Count} confusable characters:");
            foreach (var confusable in confusables)
            {
                Console.WriteLine($"  - '{confusable}'");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error detecting confusables: {ex.Message}");
        }

        // Test normalize_confusables
        try
        {
            string normalized = ConfusablesDetector.NormalizeConfusables(testInput);
            Console.WriteLine($"Normalized: {normalized}");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error normalizing confusables: {ex.Message}");
        }

        // Test unicode_normalize_kd
        try
        {
            string kdNormalized = ConfusablesDetector.UnicodeNormalizeKd(testInput, stripZeroWidth: true);
            Console.WriteLine($"NFKD Normalized: {kdNormalized}");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error with NFKD normalization: {ex.Message}");
        }
    }
}
