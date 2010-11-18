using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace EclipseGridIO
{
    class EclipseZcorn : KeyResolve
    {
        /* 
                         (1)*---------------------*(2)
                           /|   				 /|
                          / |   				/ |
                         /  |   			   /  |
                        /   |   			  /   |
                    (4)*---------------------*(3) |
                       |	|   			 |    |
                       |	|   			 |    |
                       |	|   			 |    |
                       | [5]*---------------------*[6]
                       |   /				 |   /
                       |  / 				 |  /
                       | /  				 | /
                       |/   				 |/
                    [8]*---------------------*[7]
      
                    */

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
            //gd.PointX = new double[DoubleList.Count];
            //gd.PointY = new double[DoubleList.Count];
            //gd.PointZ = new double[DoubleList.Count];
            gd.PointZcron = new double[DoubleList.Count];
            for (int i = 0; i < DoubleList.Count / 8; i++)
            {
                int XCount, YCount;
                XCount = (i % (gd.GridX * gd.GridY) % gd.GridX);
                YCount = (i % (gd.GridX * gd.GridY) / gd.GridX);
                int ZCount = i / (gd.GridX * gd.GridY);

                for (int j = 0; j < 8; j++)
                {
                    gd.PointZcron[i * 8 + j] = DoubleList[i * 8 + j];    
                }
                ////1
                //gd.PointZ[i * 8] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount]);//Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount + 1][0]);
                //gd.PointX[i * 8] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 ]);
                //gd.PointY[i * 8] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 ]);
                ////2
                //gd.PointZ[i * 8 + 1] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount + 1]);//Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 1]);
                //gd.PointY[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 1]);
                ////3
                //gd.PointZ[i * 8 + 2] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2 + 1]); //Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 2]);
                //gd.PointY[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 2]);
                ////4
                //gd.PointZ[i * 8 + 3] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2]);
                //gd.PointX[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8+3]);
                //gd.PointY[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8+3]);

               
                ////5
                //gd.PointZ[i * 8 + 4] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2]);// Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 4]);
                //gd.PointY[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 4]);
                ////6
                //gd.PointZ[i * 8 + 5] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2 + 1]);//Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 5]);
                //gd.PointY[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 5]);
                ////7
                //gd.PointZ[i * 8 + 6] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2 + 1]);// Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 6]);
                //gd.PointY[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 6]);
                ////8
                //gd.PointZ[i * 8 + 7] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2]);//Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 7]);
                //gd.PointY[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 7]);

                ////1
                //gd.PointZ[i * 8] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2]);
                //gd.PointX[i * 8] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8]);
                //gd.PointY[i * 8] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8]);
                ////2
                //gd.PointZ[i * 8 + 1] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount]);//Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount + 1][0]);
                //gd.PointX[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 1]);
                //gd.PointY[i * 8 + 1] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 1]);
                ////3
                //gd.PointZ[i * 8 + 2] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + 2 * XCount + 1]);//Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 2]);
                //gd.PointY[i * 8 + 2] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 2]);
                ////4
                //gd.PointZ[i * 8 + 3] = Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2 + 1]); //Convert.ToSingle(DoubleList[8 * gd.GridX * gd.GridY * ZCount + 4 * YCount * gd.GridX + gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 3]);
                //gd.PointY[i * 8 + 3] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 3]);
                ////5
                //gd.PointZ[i * 8 + 4] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2]);//Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 4]);
                //gd.PointY[i * 8 + 4] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount], gd.PointZ[i * 8 + 4]);
                ////6
                //gd.PointZ[i * 8 + 5] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2]);// Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineX2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 5]);
                //gd.PointY[i * 8 + 5] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount], gd.LineZ1[(gd.GridX + 1) * YCount + XCount], gd.LineY2[(gd.GridX + 1) * YCount + XCount], gd.LineZ2[(gd.GridX + 1) * YCount + XCount], gd.PointZ[i * 8 + 5]);
                ////7
                //gd.PointZ[i * 8 + 6] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + 2 * YCount * gd.GridX * 2 + XCount * 2 + 1]);//Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2][0]);
                //gd.PointX[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineX2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 6]);
                //gd.PointY[i * 8 + 6] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ1[(gd.GridX + 1) * YCount + XCount + 1], gd.LineY2[(gd.GridX + 1) * YCount + XCount + 1], gd.LineZ2[(gd.GridX + 1) * YCount + XCount + 1], gd.PointZ[i * 8 + 6]);
                ////8
                //gd.PointZ[i * 8 + 7] = Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2 + 1]);// Convert.ToSingle(DoubleList[(2 * ZCount + 1) * (gd.GridX * gd.GridY * 4) + (2 * YCount + 1) * gd.GridX * 2 + XCount * 2 + 1][0]);
                //gd.PointX[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineX1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineX2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 7]);
                //gd.PointY[i * 8 + 7] = Get3DPoint.Get3DPointValue(gd.LineY1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ1[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineY2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.LineZ2[(gd.GridX + 1) * (YCount + 1) + XCount + 1], gd.PointZ[i * 8 + 7]);


                ////计算最大值最小值
                //for (int j = 0; j < 8; j++)
                //{
                //    if (gd.MaxX < gd.PointX[i * 8 + j])
                //    {
                //        gd.MaxX = gd.PointX[i * 8 + j];
                //    }
                //    if (gd.MaxY < gd.PointY[i * 8 + j])
                //    {
                //        gd.MaxY = gd.PointY[i * 8 + j];
                //    }
                //    if (gd.MaxZ < gd.PointZ[i * 8 + j])
                //    {
                //        gd.MaxZ = gd.PointZ[i * 8 + j];
                //    }

                //    if (gd.MinX > gd.PointX[i * 8 + j])
                //    {
                //        gd.MinX = gd.PointX[i * 8 + j];
                //    }
                //    if (gd.MinY > gd.PointY[i * 8 + j])
                //    {
                //        gd.MinY = gd.PointY[i * 8 + j];
                //    }
                //    if (gd.MinZ > gd.PointZ[i * 8 + j])
                //    {
                //        gd.MinZ = gd.PointZ[i * 8 + j];
                //    }
                //}
            }
        }

        public override void DataWrite(System.IO.StreamWriter sw, GridData gd)
        {
            sw.WriteLine("ZCRON");
            //string DataLine = "";
            for (int i = 0; i < gd.PointZcron.Length; i++)
            {
                if (i % 6 == 0)
                {
                    sw.Write("\n");
                }
                else
                {
                    sw.Write(gd.PointZcron[i].ToString("#0.000000") + " ");     
                }
            }
            sw.Write("\n");
            sw.WriteLine("/");
            sw.WriteLine();
        }
    }
}
