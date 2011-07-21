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
            //search for swi or other stuff
            for (int i = 0; i < insram.Length - 1; i+=1)
            {
                /*if (args[2] == "true")
                {
                    if (Feld[i + 1] == 0x47 && (Feld[i] & 0x7) == 0)
                    {
                        Console.WriteLine("found BX, BLX" + (Feld[i] >> 3) + "at " + i.ToString() + "hope it is thumb and patch to BKPT");
                        Feld[i + 1] = 0xBE;
                        Feld[i] = (Byte)(((Feld[i] >> 3) & 0x1F) | 0x40);
                    }
                    if ((Feld[i + 3] & 0xF) == 0x1 && Feld[i + 2] == 0x2F && Feld[i + 1] == 0xFF && (Feld[i] & 0xD0) == 0x10)
                    {
                        Console.WriteLine("found BX, BLX" + Feld[i] + "at " + i.ToString() + "hope it is arm and patch to BKPT");
                        Feld[i + 1] = 0xBE;
                        Feld[i] = (Byte)((Feld[i] & 0xF) | (Feld[i] & 0x20 >> 1) | 0x40);
                    }
                }*/
                if (Feld[i + 1] == 0xDF && Feld[i] < 0x2A /*&& !(Feld[i] == 0x4 || Feld[i] == 0x5)*/)
                {

                    Console.WriteLine("found swi" + Feld[i] + "at " + i.ToString() + "hope it is thumb and patch to BKPT");
                    Feld[i + 1] = 0xBE;
                }
                if (Feld[i + 1] == 0xEF && Feld[i] < 0x2A && args[2] =="+arm" /*&& !(Feld[i] == 0x4 || Feld[i] == 0x5)*/)
                {

                    Console.WriteLine("found swi" + Feld[i] + "at " + i.ToString() + "hope it is arm and patch to BKPT");
                    Feld[i - 1] = Feld[i];
                    Feld[i + 1] = 0xE1;
                    Feld[i] = 0x20;
                    Feld[i - 2] = 0x70;
                }
            }
            FileStream outram = new FileStream(args[1], FileMode.OpenOrCreate);
            outram.Write(Feld, 0, Feld.Length);
            Console.ReadLine();
        }
    }
}
