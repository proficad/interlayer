using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace EclipseGridIO
{
    class EclipseAmalgam:KeyResolve
    {
        public override void Resolve(System.IO.StreamReader sr, GridData gd)
        {
            string DataLine = "";
            List<string> StrList = new List<string>();
            bool IsDoubleRead = false;
            while ((DataLine = sr.ReadLine()) != null)
            {
                if (DataLine == "/")
                {
                    if (!IsDoubleRead)
                    {
                        IsDoubleRead = true;
                        DataLine = sr.ReadLine();
                        if (DataLine == null)
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (DataLine != "")
                {
                    if (!DataLine.Contains("/"))
                    {
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            StrList.Add(StrTemp[i]);
                        }
                        IsDoubleRead = false;
                    }
                    else
                    {
                        DataLine = DataLine.Replace("/", "");
                        string[] StrTemp = Regex.Replace(DataLine, "[\\s]+", " ").Trim().Split(' ');
                        for (int i = 0; i < StrTemp.Length; i++)
                        {
                            StrList.Add(StrTemp[i]);
                        }
                        if (!IsDoubleRead)
                        {
                            IsDoubleRead = true;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                LGRName lgrn = new LGRName();
                for (int i = 0; i < StrList.Count; i++)
                {
                    lgrn.Name.Add(StrList[i]);
                }
                gd.DNameList.Add(lgrn);
                StrList.Clear();
            }
            for (int i = 0; i < gd.DNameList.Count; i++)
            {
                for (int j = 0; j < gd.DNameList[i].Name.Count; j++)
                {
                    for (int k = 0; k < gd.DGridDataList.Count; k++)
                    {
                        if (gd.DNameList[i].Name[j] == gd.DGridDataList[k].Name)
                        {
                            gd.DNameList[i].ID.Add(k);
                        }
                    }
                }
            }
        }

        public override void DataWrite(System.IO.StreamWriter sw, GridData gd)
        {
            if (gd.DNameList != null && gd.DNameList.Count > 0)
            {
                for (int i = 0; i < gd.DNameList.Count; i++)
                {
                    for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                    {
                        sw.Write(gd.DGridDataList[gd.DNameList[i].ID[j]].Name + " ");
                    }
                    sw.Write("\n");
                    if (gd.DNameList[i].ID.Count > 1)
                    { 
                        sw.WriteLine("/");
                    }
                }
            }
            sw.WriteLine("/");
            sw.WriteLine();
        }
    }
}
