using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    /// <summary>
    /// 数据归一化，方便显示
    /// </summary>
    public class GridDataNormalized
    {
        /// <summary>
        /// 网格归一化，方便显示
        /// </summary>
        /// <param name="gd">网格数据</param>
        /// <returns>归一化的网格数据</returns>
        public static GridData Normalized(GridData gd)
        {
            for (int i = 0; i < gd.PointX.Length; i++)
            {
                gd.PointX[i] = gd.PointX[i] - gd.MinX;
                gd.PointY[i] = gd.PointY[i] - gd.MinY;
                gd.PointZ[i] = gd.PointZ[i] - gd.MinZ;
            }
            gd.MinX = 999999999;
            gd.MinY = 999999999;
            gd.MinZ = 999999999;
            gd.MaxX = -999999999;
            gd.MaxY = -999999999;
            gd.MaxZ = -999999999;
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
            return gd;
        }
        /// <summary>
        /// 网格归一化，方便显示
        /// </summary>
        /// <param name="gd">网格数据</param>
        /// <returns>归一化的网格数据</returns>
        public static GridData DNormalized(GridData gd)
        {
            for (int i = 0; i < gd.PointX.Length; i++)
            {
                gd.PointX[i] = gd.PointX[i] + gd.MinX;
                gd.PointY[i] = gd.PointY[i] + gd.MinY;
                gd.PointZ[i] = gd.PointZ[i] + gd.MinZ;
            }
            gd.MinX = 999999999;
            gd.MinY = 999999999;
            gd.MinZ = 999999999;
            gd.MaxX = -999999999;
            gd.MaxY = -999999999;
            gd.MaxZ = -999999999;
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
            return gd;
        }
    }
}
