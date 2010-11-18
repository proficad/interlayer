using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace EclipseGridIO
{
    class GridDataRead
    {
        /// <summary>
        /// 读VC识别格式的GridData
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>GridData对象</returns>
        public GridData VCGridDataRead(string FileName)
        {
            FileStream fs = new FileStream(FileName, FileMode.Open);
            BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
            GridData gd = new GridData();
            fs.Seek(16, SeekOrigin.Current);//类名
            fs.Seek(4, SeekOrigin.Current);//是否是正交网格标记

            gd.GridX = BitConverter.ToInt32(ReadValue(br, 4), 0);
            gd.GridY = BitConverter.ToInt32(ReadValue(br, 4), 0);
            gd.GridZ = BitConverter.ToInt32(ReadValue(br, 4), 0);
            gd.MinX = BitConverter.ToDouble(ReadValue(br, 8), 0);
            gd.MinY = BitConverter.ToDouble(ReadValue(br, 8), 0);
            gd.MinZ = BitConverter.ToDouble(ReadValue(br, 8), 0);
            gd.MaxX = BitConverter.ToDouble(ReadValue(br, 8), 0);
            gd.MaxY = BitConverter.ToDouble(ReadValue(br, 8), 0);
            gd.MaxZ = BitConverter.ToDouble(ReadValue(br, 8), 0);

            gd.LineX1 = new double[(gd.GridX + 1) * (gd.GridY + 1)];
            gd.LineY1 = new double[(gd.GridX + 1) * (gd.GridY + 1)];
            gd.LineZ1 = new double[(gd.GridX + 1) * (gd.GridY + 1)];
            gd.LineX2 = new double[(gd.GridX + 1) * (gd.GridY + 1)];
            gd.LineY2 = new double[(gd.GridX + 1) * (gd.GridY + 1)];
            gd.LineZ2 = new double[(gd.GridX + 1) * (gd.GridY + 1)];

            for (int i = 0; i < gd.LineX1.Length; i++)
            {
                gd.LineX1[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
                gd.LineY1[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
                gd.LineZ1[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
                gd.LineX2[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
                gd.LineY2[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
                gd.LineZ2[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
            }
            //List<double> DoubleList=new List<double>();
            gd.PointZcron = new double[gd.GridX * gd.GridY * gd.GridZ * 8];
            for (int i = 0; i < gd.GridX * gd.GridY * gd.GridZ * 8;i++ )
            {
                gd.PointZcron[i] = BitConverter.ToDouble(ReadValue(br, 8), 0);
            }
            gd.PointX = new double[gd.GridX * gd.GridY * gd.GridZ * 8];
            gd.PointY = new double[gd.GridX * gd.GridY * gd.GridZ * 8];
            gd.PointZ = new double[gd.GridX * gd.GridY * gd.GridZ * 8];
            for (int i = 0; i < gd.PointZcron.Length / 8; i++)
            {
                int XCount, YCount;
                XCount = (i % (gd.GridX * gd.GridY) % gd.GridX);
                YCount = (i % (gd.GridX * gd.GridY) / gd.GridX);
                int ZCount = i / (gd.GridX * gd.GridY);
                //1
                gd.PointZ[i * 8] = Convert.ToSingle(gd.PointZcron[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount]);
                gd.PointX[i * 8] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8]);
                gd.PointY[i * 8] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8]);
                //2
                gd.PointZ[i * 8 + 1] = Convert.ToSingle(gd.PointZcron[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount + 1]);
                gd.PointX[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 1]);
                gd.PointY[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 1]);
                //3
                gd.PointZ[i * 8 + 2] = Convert.ToSingle(gd.PointZcron[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2 + 1]); 
                gd.PointX[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 2]);
                gd.PointY[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 2]);
                //4
                gd.PointZ[i * 8 + 3] = Convert.ToSingle(gd.PointZcron[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2]);
                gd.PointX[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 3]);
                gd.PointY[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 3]);


                //5
                gd.PointZ[i * 8 + 4] = Convert.ToSingle(gd.PointZcron[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2]);
                gd.PointX[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 4]);
                gd.PointY[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 4]);
                //6
                gd.PointZ[i * 8 + 5] = Convert.ToSingle(gd.PointZcron[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2 + 1]);
                gd.PointX[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 5]);
                gd.PointY[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 5]);
                //7
                gd.PointZ[i * 8 + 6] = Convert.ToSingle(gd.PointZcron[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2 + 1]);
                gd.PointX[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 6]);
                gd.PointY[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 6]);
                //8
                gd.PointZ[i * 8 + 7] = Convert.ToSingle(gd.PointZcron[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2]);
                gd.PointX[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 7]);
                gd.PointY[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 7]);
            }
            int DGridDataCount = BitConverter.ToInt32(ReadValue(br,4), 0);
            for (int i = 0; i < DGridDataCount; i++)
            {
                DGridData dgd = new DGridData();
                dgd.Name = Encoding.Unicode.GetString(ReadValue(br, 16));
                dgd.X1 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.X2 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.Y1 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.Y2 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.Z1 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.Z2 = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.DX = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.DY = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.DZ = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.NWMAX = BitConverter.ToInt32(ReadValue(br, 4), 0);
                dgd.GLOBAL = Encoding.Unicode.GetString(ReadValue(br, 16));
                gd.DGridDataList.Add(dgd);

            }
            int DNameCount = BitConverter.ToInt32(ReadValue(br,4), 0);
            for (int i = 0; i < DNameCount; i++)
            {
                int DNameIDCount = BitConverter.ToInt32(ReadValue(br,4), 0);
                LGRName lgrn = new LGRName();
                for (int j = 0; j < DNameIDCount; j++)
                {
                    lgrn.ID.Add(BitConverter.ToInt32(ReadValue(br, 4), 0));
                }
                gd.DNameList.Add(lgrn);
            }
            //计算最大值最小值
            for (int i = 0; i < gd.PointX.Length; i++)
            {
                if (gd.MaxX < gd.PointX[i])
                {
                    gd.MaxX = gd.PointX[i];
                }
                if (gd.MaxY < gd.PointY[i])
                {
                    gd.MaxY = gd.PointY[i];
                }
                if (gd.MaxZ < gd.PointZ[i])
                {
                    gd.MaxZ = gd.PointZ[i];
                }

                if (gd.MinX > gd.PointX[i])
                {
                    gd.MinX = gd.PointX[i];
                }
                if (gd.MinY > gd.PointY[i])
                {
                    gd.MinY = gd.PointY[i];
                }
                if (gd.MinZ > gd.PointZ[i])
                {
                    gd.MinZ = gd.PointZ[i];
                }
            }            
            br.Close();
            GC.Collect();

            return gd;
        }
        /// <summary>
        /// 读二进制值
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="IntValue">读取位数</param>
        private byte[] ReadValue(BinaryReader br, int IntValue)
        {
            byte[] ByteValue = br.ReadBytes(IntValue);
            //Array.Reverse(ByteValue);
            return ByteValue;
        }
    }
}
