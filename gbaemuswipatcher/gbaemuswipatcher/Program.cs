using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace gbaemuswipatcher
{
    class Program
    {
        static void Main(string[] args)
        {
            FileStream insram = new FileStream(args[0], FileMode.Open);
            Byte[] Feld = new Byte[insram.Length];
            insram.Read(Feld, 0, (int)insram.Length);
            //search for swi
            for (int i = 0; i < insram.Length - 1; i+=1)
            {
                if (Feld[i + 1] == 0xDF && Feld[i] < 0x2A /*&& !(Feld[i] == 0x4 || Feld[i] == 0x5)*/)
                {
                    Console.WriteLine("found swi" + Feld[i] + "at " + i.ToString() + "hope it is thumb and patch to BKPT");
                    Feld[i + 1] = 0xBE;
                }
            }
            FileStream outram = new FileStream(args[1], FileMode.OpenOrCreate);
            outram.Write(Feld, 0, Feld.Length);
            Console.ReadLine();
        }
    }
}
