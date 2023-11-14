using System;


namespace Cube
{
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main constr");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello world from c#");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
    }
}