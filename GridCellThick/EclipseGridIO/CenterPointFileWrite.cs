using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace EclipseGridIO
{
    /// <summary>
    /// 中心点生成类
    /// </summary>
    public class CenterPointFileWrite
    {
        /// <summary>
        /// 生成中心点文件
        /// </summary>
        /// <param name="TrackFileName">追踪后的网格文件</param>
        /// <param name="SaveFileName">生成后的文件名</param>
        /// <param name="IsWriteThick">是否写入加密网格中心点</param>
        /// <param name="Number">编号</param>
        /// <returns>是否成功</returns>
        public bool WriteCenterPointFile(string TrackFileName, string SaveFileName, bool IsWriteThick,string Number)
        {
            try
            {
                FileStream OpenFileFs = new FileStream(TrackFileName, FileMode.Open);
                BinaryReader OpenFileBr = new BinaryReader(OpenFileFs, Encoding.Unicode);
                List<float> SaveFilePointX = new List<float>();
                List<float> SaveFilePointY = new List<float>();
                List<float> SaveFilePointZ = new List<float>();
                OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃GridX,GridY,GridZ
                int CellCount = OpenFileBr.ReadInt32();
                for (int i = 0; i < CellCount; i++)
                {
                    OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃I，J，K
                    List<float> PointX = new List<float>();
                    List<float> PointY = new List<float>();
                    List<float> PointZ = new List<float>();
                    for (int j = 0; j < 8; j++)
                    {
                        PointX.Add(OpenFileBr.ReadSingle());
                        PointY.Add(OpenFileBr.ReadSingle());
                        PointZ.Add(OpenFileBr.ReadSingle());
                    }
                    SaveFilePointX.Add(GetCenterPoint(PointX));
                    SaveFilePointY.Add(GetCenterPoint(PointY));
                    SaveFilePointZ.Add(GetCenterPoint(PointZ));
                    //读加密网格个数
                    int ChildrenCount = OpenFileBr.ReadInt32();
                    for (int j = 0; j < ChildrenCount; j++)
                    {
                        if (IsWriteThick)//是否写入加密网格坐标
                        {
                            PointX.Clear();
                            PointY.Clear();
                            PointZ.Clear();
                            for (int k = 0; k < 8; k++)
                            {
                                PointX.Add(OpenFileBr.ReadSingle());
                                PointY.Add(OpenFileBr.ReadSingle());
                                PointZ.Add(OpenFileBr.ReadSingle());
                            }
                            SaveFilePointX.Add(GetCenterPoint(PointX));
                            SaveFilePointY.Add(GetCenterPoint(PointY));
                            SaveFilePointZ.Add(GetCenterPoint(PointZ));
                        }
                        else
                        {
                            OpenFileFs.Seek(12 * 8, SeekOrigin.Current);
                        }
                    }
                }
                OpenFileBr.Close();
                FileStream SaveFileFs = new FileStream(SaveFileName, FileMode.Create);
                StreamWriter SaveFileSw = new StreamWriter(SaveFileFs, Encoding.Default);
                for (int i = 0; i < SaveFilePointX.Count; i++)
                {
                    SaveFileSw.WriteLine(SaveFilePointX[i] + "\t" + -SaveFilePointY[i] + "\t" + SaveFilePointZ[i] + "\t" + Number);
                }
                SaveFileSw.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 获取中心点某个轴的值
        /// </summary>
        /// <param name="PointList">某个轴点坐标列表</param>
        /// <returns>中心点某个轴的值</returns>
        private float GetCenterPoint(List<float> PointList)
        {
            float Max = -999999999999;
            float Min = 999999999999;
            for (int i = 0; i < PointList.Count; i++)
            {
                if (Max < PointList[i])
                {
                    Max = PointList[i];
                }
                if (Min > PointList[i])
                {
                    Min = PointList[i];
                }
            }
            return Min + (Max - Min) / 2;
        }
    }
}
