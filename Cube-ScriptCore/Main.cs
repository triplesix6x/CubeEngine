using System;
using System.Runtime.CompilerServices;


namespace Cube
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = x;
            Z = x;
        }
    }

    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            CppFunction();
            Console.WriteLine("Main constr");
        }
        public void PrintMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void CppFunction();
    }
}