using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace EclipseGridIO
{
    /// <summary>
    /// 追踪后网格面输出
    /// </summary>
    public class MultFileWrite
    {
        /// <summary>
        /// 生成曲面追踪后的文件
        /// </summary>
        /// <param name="FaceFileName">曲面追踪后的文件</param>
        /// <param name="SaveFileName">生成的文件名</param>
        /// <returns>是否成功</returns>
        public bool WriteMultFile(string TrackFileName, string SaveFileName)
        {
            try
            {
                FileStream OpenFileFs = new FileStream(TrackFileName, FileMode.Open);
                BinaryReader OpenFileBr = new BinaryReader(OpenFileFs, Encoding.Unicode);
                //网格大小
                int GridX = OpenFileBr.ReadInt32();
                int GridY = OpenFileBr.ReadInt32();
                int GridZ = OpenFileBr.ReadInt32();
                int Count = OpenFileBr.ReadInt32();
                
                List<int> XList = new List<int>();
                List<int> YList = new List<int>();
                List<int> ZList = new List<int>();

                for (int i = 0; i < Count; i++)
                {
                    XList.Add(OpenFileBr.ReadInt32());
                    YList.Add(OpenFileBr.ReadInt32());
                    ZList.Add(OpenFileBr.ReadInt32());
                    //跳跃坐标点
                    OpenFileFs.Seek(4 * 3 * 8, SeekOrigin.Current);
                    int ChildrenCount = OpenFileBr.ReadInt32();
                    if (ChildrenCount > 0)
                    {
                        OpenFileFs.Seek(ChildrenCount * 4 * 3 * 8, SeekOrigin.Current);
                    }
                }
                OpenFileBr.Close();

                for (int i = 0; i < XList.Count; i++)
                {
                    int XValue = XList[i];
                    XList[i] = XValue + YList[i] * GridX + GridX * GridY * ZList[i];
                    YList[i] = XValue + YList[i] * GridX + GridX * GridY * ZList[i];
                    ZList[i] = XValue + YList[i] * GridX + GridX * GridY * ZList[i];
                }

                FileStream SaveFileFs = new FileStream(SaveFileName, FileMode.Create);
                StreamWriter SaveFileSw = new StreamWriter(SaveFileFs, Encoding.Default);
                int[] MultValue = GetMultValue(XList, GridX, GridY, GridZ, "X");
                if (MultValue != null && MultValue.Length > 0)
                {
                    SaveFileSw.WriteLine("MultX");
                }
                for (int i = 0; i < MultValue.Length; i++)
                {
                    if (i % GridX == 0 && i != 0)
                    {
                        SaveFileSw.Write("\n");
                    }
                    SaveFileSw.Write(MultValue[i].ToString() + "\t");
                }
                SaveFileSw.Write("\n");
                MultValue = null;
                GC.Collect();
                MultValue = GetMultValue(YList, GridX, GridY, GridZ, "Y");
                if (MultValue != null && MultValue.Length > 0)
                {
                    SaveFileSw.WriteLine("MultY");
                }
                for (int i = 0; i < MultValue.Length; i++)
                {
                    if (i % GridX == 0 && i != 0)
                    {
                        SaveFileSw.Write("\n");
                    }
                    SaveFileSw.Write(MultValue[i].ToString() + "\t");
                }
                SaveFileSw.Write("\n");
                MultValue = null;
                GC.Collect();
                MultValue = GetMultValue(ZList, GridX, GridY, GridZ, "Z");
                if (MultValue != null && MultValue.Length > 0)
                {
                    SaveFileSw.WriteLine("MultZ");
                }
                for (int i = 0; i < MultValue.Length; i++)
                {
                    if (i % GridX == 0 && i != 0)
                    {
                        SaveFileSw.Write("\n");
                    }
                    SaveFileSw.Write(MultValue[i].ToString() + "\t");
                }
                SaveFileSw.Write("\n");
                SaveFileSw.Close();

                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 获取Mult某个方向的值
        /// </summary>
        /// <param name="XYZList">某个方向的网格索引列表</param>
        /// <param name="GridX">X个数（I方向个数）</param>
        /// <param name="GridY">Y个数（J方向个数）</param>
        /// <param name="GridZ">Z个数（K方向个数）</param>
        /// <param name="Direction"></param>
        /// <returns>Mult某个方向的值</returns>
        private int[] GetMultValue(List<int> XYZList, int GridX, int GridY, int GridZ, string Direction)
        {
            int[] MultValue = null;//返回的Mult值
            switch (Direction)
            {
                case "X":
                    MultValue = new int[(GridX + 1) * GridY * GridZ];
                    for (int i = 0; i < GridZ; i++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            int Start = -1;
                            int End = -1;
                            for (int k = 0; k < GridX; k++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    if (Start == -1)
                                    {
                                        Start = k;
                                        End = k;
                                        MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + k] = 1;
                                    }
                                    else
                                    {
                                        if (k - End == 1)
                                        {
                                            End = k;
                                            MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + k] = 0;
                                        }
                                    }
                                }
                                else
                                {
                                    if (Start != -1)
                                    {
                                        MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + k] = 1;
                                        Start = End = -1;
                                    }
                                    else
                                    {
                                        MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + k] = 0;
                                    }
                                }
                            }
                            if (End == GridX - 1)
                            {
                                MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + GridX] = 1;
                            }
                            else
                            {
                                MultValue[i * (GridX + 1) * GridY + j * (GridX + 1) + GridX] = 0;
                            }

                        }
                    }
                    break;
                //case "X-":
                //    break;
                case "Y":
                    MultValue = new int[GridX * (GridY + 1) * GridZ];
                    for (int i = 0; i < GridZ; i++)
                    {
                        for (int k = 0; k < GridX; k++)
                        {
                            int Start = -1;
                            int End = -1;
                            for (int j = 0; j < GridY; j++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    if (Start == -1)
                                    {
                                        Start = j;
                                        End = j;
                                        MultValue[i * GridX * (GridY + 1) + j * GridX + k] = 1;
                                    }
                                    else
                                    {
                                        if (j - End == 1)
                                        {
                                            End = j;
                                            MultValue[i * GridX * (GridY + 1) + j * GridX + k] = 0;
                                        }
                                    }
                                }
                                else
                                {
                                    if (Start != -1)
                                    {
                                        MultValue[i * GridX * (GridY + 1) + j * GridX + k] = 1;
                                        Start = End = -1;
                                    }
                                    else
                                    {
                                        MultValue[i * GridX * (GridY + 1) + j * GridX + k] = 0;
                                    }
                                }
                            }
                            if (End == GridY - 1)
                            {
                                MultValue[i * GridX * (GridY + 1) + GridY * GridX + k] = 1;
                            }
                            else
                            {
                                MultValue[i * GridX * (GridY + 1) + GridY * GridX + k] = 0;
                            }
                        }
                    }
                    break; 
                //case "Y-":
                //    break;
                case "Z":
                    MultValue = new int[GridX * GridY * (GridZ + 1)];
                    for (int k = 0; k < GridX; k++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            int Start = -1;
                            int End = -1;
                            for (int i = 0; i < GridZ; i++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    if (Start == -1)
                                    {
                                        Start = i;
                                        End = i;
                                        MultValue[i * GridX * GridY + j * GridX + k] = 1;
                                    }
                                    else
                                    {
                                        if (i - End == 1)
                                        {
                                            End = i;
                                            MultValue[i * GridX * GridY + j * GridX + k] = 0;
                                        }
                                    }
                                }
                                else
                                {
                                    if (Start != -1)
                                    {
                                        MultValue[i * GridX * GridY + j * GridX + k] = 1;
                                        Start = End = -1;
                                    }
                                    else
                                    {
                                        MultValue[i * GridX * GridY + j * GridX + k] = 0;
                                    }
                                }
                            }
                            if (End == GridZ - 1)
                            {
                                MultValue[GridZ * GridX * GridY + j * GridX + k] = 1;
                            }
                            else
                            {
                                MultValue[GridZ * GridX * GridY + j * GridX + k] = 0;
                            }
                        }
                    }
                    break;
                //case "Z-":
                //    break;

            }
            return MultValue;
        }
    }
}
