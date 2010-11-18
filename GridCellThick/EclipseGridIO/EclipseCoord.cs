using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace EclipseGridIO
{
    class EclipseCoord : KeyResolve
    {

        public override void Resolve(System.IO.StreamReader sr, GridData gd)
        {
            string DataLine = "";
            List<double> DoubleList = new List<double>();
            while ((DataLine = sr.ReadLine()) != null && DataLine!="/")
            {
                if (DataLine != "")
                {
                    if (!DataLine.Contains("/"))
                    {
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            DoubleList.Add(Convert.ToDouble(StrTemp[i]));
                        }
                    }
                    else
                    {
                        DataLine = DataLine.Replace("/", "");
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            DoubleList.Add(Convert.ToDouble(StrTemp[i]));
                        }
                        break;
                    }
                }
            }
            gd.LineX1 = new double[DoubleList.Count / 6];
            gd.LineX2 = new double[DoubleList.Count / 6];
            gd.LineY1 = new double[DoubleList.Count / 6];
            gd.LineY2 = new double[DoubleList.Count / 6];
            gd.LineZ1 = new double[DoubleList.Count / 6];
            gd.LineZ2 = new double[DoubleList.Count / 6];
            for (int i = 0; i < DoubleList.Count / 6; i++)
            {
                gd.LineX1[i] = DoubleList[i * 6];
                gd.LineY1[i] = DoubleList[i * 6 + 1];
                gd.LineZ1[i] = DoubleList[i * 6 + 2];
                gd.LineX2[i] = DoubleList[i * 6 + 3];
                gd.LineY2[i] = DoubleList[i * 6 + 4];
                gd.LineZ2[i] = DoubleList[i * 6 + 5];
            }
            return;
        }

        public override void DataWrite(System.IO.StreamWriter sw, GridData gd)
        {
            sw.WriteLine("COORD");
            for (int i = 0; i < gd.LineX1.Length; i++)
            {
                sw.WriteLine((gd.LineX1[i].ToString("#0.000000") + " " + gd.LineY1[i].ToString("#0.000000") + " " + gd.LineZ1[i].ToString("#0.000000")
                    + " " + gd.LineX2[i].ToString("#0.000000") + " " + gd.LineY2[i].ToString("#0.000000") + " " + gd.LineZ2[i].ToString("#0.000000")));
            }
            sw.WriteLine("/");
            sw.WriteLine();
        }
    }
}
