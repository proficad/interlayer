using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace EclipseGridIO
{
    class EclipseCarfin:KeyResolve
    {

        //1 Name of the local grid refinement (up to 8 characters)

        //2 I1 Lower I-coordinate of the box in the parent grid.

        //3 I2 Upper I-coordinate of the box in the parent grid.

        //4 J1 Lower J-coordinate of the box in the parent grid.

        //5 J2 Upper J-coordinate of the box in the parent grid.

        //6 K1 Lower K-coordinate of the box in the parent grid.

        //7 K2 Upper K-coordinate of the box in the parent grid.

        //8 NX Number of refined cells along the X-direction.

        //9 NY Number of refined cells along the Y-direction.

        //10 NZ Number of refined cells along the Z-direction.

        //11 NWMAX Maximum number of wells this local refined grid will contain.

        //12 Name of parent LGR - up to 8 characters. This item may be set to either a null string or to the string ‘GLOBAL’ to indicate that the parent grid is global (that is, this is not a nested refinement).


        public override void Resolve(System.IO.StreamReader sr, GridData gd)
        {
            string DataLine = "";
            List<string> StrList = new List<string>();
            while ((DataLine = sr.ReadLine()) != null && DataLine!="/")
            {
                if (DataLine != "")
                {
                    if (!DataLine.Contains("/"))
                    {
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            StrList.Add(StrTemp[i]);
                        }
                    }
                    else
                    {
                        DataLine = DataLine.Replace("/", "");
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            StrList.Add(StrTemp[i]);
                        }
                        break;
                    }
                }
            }
            //for (int i = Convert.ToInt32(StrList[5]) - 1; i < Convert.ToInt32(StrList[6]); i++)//Z方向循环
            //{
            //    for (int j = Convert.ToInt32(StrList[3]) - 1; j < Convert.ToInt32(StrList[4]); j++)//Y方向循环
            //    {
            //        for (int k = Convert.ToInt32(StrList[1]) - 1; k < Convert.ToInt32(StrList[2]); k++)//X方向
            //        {
            //            gd.DPointX[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[7]) / (Convert.ToInt32(StrList[2]) - Convert.ToInt32(StrList[1]) + 1);
            //            gd.DPointY[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[8]) / (Convert.ToInt32(StrList[4]) - Convert.ToInt32(StrList[3]) + 1);
            //            gd.DPointZ[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[9]) / (Convert.ToInt32(StrList[6]) - Convert.ToInt32(StrList[5]) + 1);
            //        }
            //    }
            //}
            DGridData dgd = new DGridData();
            dgd.Name = StrList[0];
            dgd.X1 = Convert.ToInt32(StrList[1]);
            dgd.X2 = Convert.ToInt32(StrList[2]);
            dgd.Y1 = Convert.ToInt32(StrList[3]);
            dgd.Y2 = Convert.ToInt32(StrList[4]);
            dgd.Z1 = Convert.ToInt32(StrList[5]);
            dgd.Z2 = Convert.ToInt32(StrList[6]);
            dgd.DX = Convert.ToInt32(StrList[7]);
            dgd.DY = Convert.ToInt32(StrList[8]);
            dgd.DZ = Convert.ToInt32(StrList[9]);
            dgd.NWMAX = Convert.ToInt32(StrList[10]);
            dgd.GLOBAL = StrList[11];
            gd.DGridDataList.Add(dgd);
        }

        public override void DataWrite(System.IO.StreamWriter sw, GridData gd)
        {
            if (gd.DNameList != null && gd.DNameList.Count > 0)
            {
                for (int i = 0; i < gd.DNameList.Count; i++)
                {
                    for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                    {
                        sw.WriteLine("CARFIN");
                        sw.WriteLine(gd.DGridDataList[gd.DNameList[i].ID[j]].Name + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].X1.ToString() 
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].X2.ToString()+ " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Y1.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Y2.ToString()+ " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Z1.ToString() 
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Z2.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DX.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DY.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DZ.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].NWMAX.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].GLOBAL);
                        sw.WriteLine("ENDFIN");
                    }
                }
            }
            sw.WriteLine("/");


        }
    }
}
