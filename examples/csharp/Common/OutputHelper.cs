namespace OpticsExamples.Common;

public static class OutputHelper
{
    public static void EnsureOutputDir(string path = "output") =>
        Directory.CreateDirectory(path);

    public static void WriteCsv2Col(string path, string xName, string yName, double[] x, double[] y)
    {
        using var writer = new StreamWriter(path);
        writer.WriteLine($"{xName},{yName}");
        for (var i = 0; i < x.Length; i++)
            writer.WriteLine($"{x[i]:G17},{y[i]:G17}");
        Console.WriteLine($"wrote {path}");
    }

    public static void WriteCsvMulti(
        string path, string xName, string[] yNames, double[] x, double[][] ySets)
    {
        using var writer = new StreamWriter(path);
        writer.Write(xName);
        foreach (var name in yNames)
            writer.Write($",{name}");
        writer.WriteLine();

        for (var i = 0; i < x.Length; i++)
        {
            writer.Write($"{x[i]:G17}");
            foreach (var y in ySets)
                writer.Write($",{y[i]:G17}");
            writer.WriteLine();
        }
        Console.WriteLine($"wrote {path}");
    }
}
