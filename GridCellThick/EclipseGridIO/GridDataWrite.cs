using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace EclipseGridIO
{
    /// <summary>
    /// 数据写入类
    /// </summary>
    class GridDataWrite
    {
        /// <summary>
        /// 写GridData文件成VC可识别格式
        /// </summary>
        /// <param name="gd">GridData对象</param>
        /// <param name="FileName">文件名</param>
        public void GridDataWriteToVC(GridData gd, string FileName)
        {
            FileStream fs = new FileStream(FileName, FileMode.Create);
            BinaryWriter bw = new BinaryWriter(fs, Encoding.Unicode);
            byte[] FileHead = new byte[16] { 0xff, 0xff, 0x00, 0x00, 0x0a, 0x00, 0x43, 0x47, 0x72, 0x69, 0x64, 0x4d, 0x6f, 0x64, 0x65, 0x6c };
            //FileHead = BitConverter.GetBytes();
            bw.Write(FileHead);//写类名
            byte[] IsConerPointGrid = new byte[4] { 0x00,0x00,0x00,0x28 };
            //IsConerPointGrid = (byte[])1;
            bw.Write(IsConerPointGrid);//写是否是角点网格
            //写网格NX。NY。NZ
            WriteValue(bw, gd.GridX);
            WriteValue(bw, gd.GridY);
            WriteValue(bw, gd.GridZ);
            //写网格最大值最小值
            WriteValue(bw, gd.MinX);
            WriteValue(bw, gd.MinY);
            WriteValue(bw, gd.MinZ);
            WriteValue(bw, gd.MaxX);
            WriteValue(bw, gd.MaxY);
            WriteValue(bw, gd.MaxZ);
            //写网格线
            for (int i = 0; i < gd.LineX1.Length; i++)
            {
                WriteValue(bw, gd.LineX1[i]);
                WriteValue(bw, gd.LineY1[i]);
                WriteValue(bw, gd.LineZ1[i]);
                WriteValue(bw, gd.LineX2[i]);
                WriteValue(bw, gd.LineY2[i]);
                WriteValue(bw, gd.LineZ2[i]);
            }
            //写网格点深度
            for (int i = 0; i < gd.PointZcron.Length; i++)
            {
                WriteValue(bw, gd.PointZcron[i]);
            }
            //写加密网格个数
            WriteValue(bw, gd.DGridDataList.Count);
            for (int i = 0; i < gd.DGridDataList.Count; i++)
            {
                //写加密网格内容
                //X,Y,Z都是从1开始的。
                WriteValue(bw, gd.DGridDataList[i].Name);
                WriteValue(bw, gd.DGridDataList[i].X1);
                WriteValue(bw, gd.DGridDataList[i].X2);
                WriteValue(bw, gd.DGridDataList[i].Y1);
                WriteValue(bw, gd.DGridDataList[i].Y2);
                WriteValue(bw, gd.DGridDataList[i].Z1);
                WriteValue(bw, gd.DGridDataList[i].Z2);
                WriteValue(bw, gd.DGridDataList[i].DX);
                WriteValue(bw, gd.DGridDataList[i].DY);
                WriteValue(bw, gd.DGridDataList[i].DZ);
                WriteValue(bw, gd.DGridDataList[i].NWMAX);
                WriteValue(bw, gd.DGridDataList[i].GLOBAL);
            }
            //写加密网格名字个数
            WriteValue(bw, gd.DNameList.Count);
            for (int i = 0; i < gd.DNameList.Count; i++)
            {
                //写加密网格名字下面的索引个数
                WriteValue(bw, gd.DNameList[i].ID.Count);
                for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                {
                    //写加密网格名字下面的索引
                    WriteValue(bw, gd.DNameList[i].ID[j]);
                }
            }
                //for (int i = 0; i < gd.DPointX.Length; i++)
                //{
                //    WriteValue(bw, gd.DPointX[i]);
                //    WriteValue(bw, gd.DPointY[i]);
                //    WriteValue(bw, gd.DPointZ[i]);
                //}
                //for (int i = 0; i < gd.PointX.Length; i++)
                //{
                //    WriteValue(bw, gd.PointX[i]);
                //    WriteValue(bw, gd.PointY[i]);
                //    WriteValue(bw, gd.PointZ[i]);
                //}
                bw.Close();
        }

        /// <summary>
        /// 写二进制值
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="DoubleValue">值</param>
        private void WriteValue(BinaryWriter bw, double DoubleValue)
        {
            byte[] ByteValue = BitConverter.GetBytes(DoubleValue);
            //Array.Reverse(ByteValue);
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
            //Array.Reverse(ByteValue);
            bw.Write(ByteValue);
        }
        /// <summary>
        /// 写二进制字符，如果不够8位，补齐，如果超过，截断
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="str">字符</param>
        private void WriteValue(BinaryWriter bw, string str)
        {
            if (str.Length > 8)
            {
                str = str.Substring(0, 8);
            }
            else
            {
                int KongCount = 8 - str.Length;
                for (int i = 0; i < KongCount; i++)
                {
                    str = str + " ";
                }
            }
            byte[] ByteValue = Encoding.Unicode.GetBytes(str);
            bw.Write(ByteValue);
        }
    }
}
