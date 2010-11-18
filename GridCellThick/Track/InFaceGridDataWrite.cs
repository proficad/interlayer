using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Track
{
    class InFaceGridDataWrite
    {
        /// <summary>
        /// 生成追踪后的网格文件
        /// </summary>
        /// <param name="FileName">保存文件名</param>
        /// <param name="InFaceDataList">网格列表</param>
        /// <param name="InFaceDataIndexList">追踪后的网格列表</param>
        /// <param name="GridX">X个数（I方向个数）</param>
        /// <param name="GridY">Y个数（J方向个数）</param>
        /// <param name="GridZ">Z个数（K方向个数）</param>
        public void Write(string FileName, List<InFaceGridData> InFaceDataList, int GridX, int GridY, int GridZ)
        {
            FileStream fs = new FileStream(FileName, FileMode.Create);
            BinaryWriter bw = new BinaryWriter(fs, Encoding.Unicode);
            //写网格大小
            WriteValue(bw, GridX);
            WriteValue(bw, GridY);
            WriteValue(bw, GridZ);
            WriteValue(bw, InFaceDataList.Count);
            for (int i = 0; i < InFaceDataList.Count; i++)
            {
                //I,J,K
                WriteValue(bw, InFaceDataList[i].X);
                WriteValue(bw, InFaceDataList[i].Y);
                WriteValue(bw, InFaceDataList[i].Z);
                //八个点网格坐标
                for (int j = 0; j < InFaceDataList[i].PointX.Count; j++)
                {
                    WriteValue(bw, InFaceDataList[i].PointX[j]);
                    WriteValue(bw, InFaceDataList[i].PointY[j]);
                    WriteValue(bw, InFaceDataList[i].PointZ[j]);
                }
                WriteValue(bw, InFaceDataList[i].ChildrenGridDataList.Count);
                for (int j = 0; j < InFaceDataList[i].ChildrenGridDataList.Count; j++)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        WriteValue(bw, InFaceDataList[i].ChildrenGridDataList[j].PointX[k]);
                        WriteValue(bw, InFaceDataList[i].ChildrenGridDataList[j].PointY[k]);
                        WriteValue(bw, InFaceDataList[i].ChildrenGridDataList[j].PointZ[k]);
                    }
                }
            }
            bw.Close();
        }
        /// <summary>
        /// 生成追踪后的网格文件
        /// </summary>
        /// <param name="FileName">保存文件名</param>
        /// <param name="InFaceDataList">网格列表</param>
        /// <param name="InFaceDataIndexList">追踪后的网格列表</param>
        /// <param name="GridX">X个数（I方向个数）</param>
        /// <param name="GridY">Y个数（J方向个数）</param>
        /// <param name="GridZ">Z个数（K方向个数）</param>
        public void Write(string FileName, List<InFaceGridData> InFaceDataList, List<int> InFaceDataIndexList,int GridX,int GridY,int GridZ)
        {
            FileStream fs = new FileStream(FileName, FileMode.Create);
            BinaryWriter bw = new BinaryWriter(fs, Encoding.Unicode);
            //写网格大小
            WriteValue(bw, GridX);
            WriteValue(bw, GridY);
            WriteValue(bw, GridZ);
            WriteValue(bw, InFaceDataIndexList.Count);
            for (int i = 0; i < InFaceDataIndexList.Count; i++)
            {
                //I,J,K
                WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].X);
                WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].Y);
                WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].Z);
                //八个点网格坐标
                for(int j=0;j<InFaceDataList[InFaceDataIndexList[i]].PointX.Count;j++)
                {
                    WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].PointX[j]);
                    WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].PointY[j]);
                    WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].PointZ[j]);
                }
                WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].ChildrenGridDataList.Count);
                for (int j = 0; j < InFaceDataList[InFaceDataIndexList[i]].ChildrenGridDataList.Count; j++)
                {
                    for (int k = 0; k < 8; k++)
                    {
                        WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].ChildrenGridDataList[j].PointX[k]);
                        WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].ChildrenGridDataList[j].PointY[k]);
                        WriteValue(bw, InFaceDataList[InFaceDataIndexList[i]].ChildrenGridDataList[j].PointZ[k]);
                    }
                }
            }
            bw.Close();
        }
        /// <summary>
        /// 写二进制值
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="DoubleValue">值</param>
        private void WriteValue(BinaryWriter bw, float FloatValue)
        {
            byte[] ByteValue = BitConverter.GetBytes(FloatValue);
            bw.Write(ByteValue);
        }
        /// <summary>
        /// 写二进制值
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="IntValue">值</param>
        private void WriteValue(BinaryWriter bw, int IntValue)
        {
            byte[] ByteValue = BitConverter.GetBytes(IntValue);
            bw.Write(ByteValue);
        }
    }
}
