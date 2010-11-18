using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
namespace EclipseGridIO
{
    class EclipseSpecgrid:KeyResolve
    {
        public override void Resolve(StreamReader sr, GridData gd)
        {
            string DataLine = "";
            DataLine = sr.ReadLine();
            string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
            gd.GridX = Convert.ToInt32(StrTemp[0]);
            gd.GridY = Convert.ToInt32(StrTemp[1]);
            gd.GridZ = Convert.ToInt32(StrTemp[2]);
            gd.DPointX = new int[gd.GridX * gd.GridY * gd.GridZ];
            gd.DPointY = new int[gd.GridX * gd.GridY * gd.GridZ];
            gd.DPointZ = new int[gd.GridX * gd.GridY * gd.GridZ];
            return;
        }

        public override void DataWrite(StreamWriter sw, GridData gd)
        {
            sw.WriteLine("SPECGRID");
            sw.WriteLine(gd.GridX.ToString() + " " + gd.GridY.ToString() + " " + gd.GridZ.ToString()+" 1 F /");
            sw.WriteLine();
        }
    }
}
