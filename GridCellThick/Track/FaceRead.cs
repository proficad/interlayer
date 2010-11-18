using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Track
{
    class FaceRead
    {
        /// <summary>
        /// 读VC生成的曲面文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>Face类对象</returns>
        public Face VCFaceRead(string FileName)
        {
            FileStream fs = new FileStream(FileName, FileMode.Open);
            BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
            Face f = new Face();

            f.PointCount = br.ReadInt32();

            f.PointX = new double[f.PointCount];
            f.PointY = new double[f.PointCount];
            f.PointZ = new double[f.PointCount];

            for (int i = 0; i < f.PointCount; i++)
            {
                f.PointX[i] = br.ReadDouble();
                f.PointY[i] = br.ReadDouble();
                f.PointZ[i] = br.ReadDouble();
            }

            f.IndexCount = br.ReadInt32();

            f.IndexData = new int[f.IndexCount];

            for (int i = 0; i < f.IndexCount; i++)
            {
                f.IndexData[i] = br.ReadInt32();
            }

            br.Close();

            return f;
        }
    }
}
