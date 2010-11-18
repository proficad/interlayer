using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.Voronoi
{
    /// <summary>
    /// 获取线段交点类
    /// </summary>
    public class CrossPoint
    {
        /// <summary>
        /// 判断两条线段是否相交
        /// </summary>
        /// <param name="PointFirst1">线段1点1</param>
        /// <param name="PointFirst2">线段1点2</param>
        /// <param name="PointSecond1">线段2点2</param>
        /// <param name="PointSecond2">线段2点2</param>
        /// <returns>是否相交</returns>
        public static bool IsLineSegmentCross(PointF PointFirst1, PointF PointFirst2, PointF PointSecond1, PointF PointSecond2)
        {   
            //每个线段的两点都在另一个线段的左右不同侧，则能断定线段相交
            //公式对于向量(x1,y1)->(x2,y2),判断点(x3,y3)在向量的左边,右边,还是线上.
            //p=x1(y3-y2)+x2(y1-y3)+x3(y2-y1).p<0 左侧,	p=0 线上, p>0 右侧
            long Linep1, Linep2;
            //判断PointFirst1和PointFirst2是否在PointSecond1->PointSecond2两侧
            Linep1 = Convert.ToInt64(PointFirst1.X * (PointSecond1.Y - PointFirst2.Y) +
                PointFirst2.X * (PointFirst1.Y - PointSecond1.Y) +
                PointSecond1.X * (PointFirst2.Y - PointFirst1.Y));
            Linep2 = Convert.ToInt64(PointFirst1.X * (PointSecond2.Y - PointFirst2.Y) +
                PointFirst2.X * (PointFirst1.Y - PointSecond2.Y) +
                PointSecond2.X * (PointFirst2.Y - PointFirst1.Y));
            if (((Linep1 ^ Linep2) >= 0) && !(Linep1 == 0 && Linep2 == 0))//符号位异或为0:PointSecond1和PointSecond2在PointFirst1->PointFirst2同侧
            {
                return false;
            }
            //判断PointFirst1和PointFirst2是否在PointSecond1->PointSecond2两侧
            Linep1 = Convert.ToInt64(PointSecond1.X * (PointFirst1.Y - PointSecond2.Y) +
                PointSecond2.X * (PointSecond1.Y - PointFirst1.Y) +
                PointFirst1.X * (PointSecond2.Y - PointSecond1.Y));
            Linep2 = Convert.ToInt64(PointSecond1.X * (PointFirst2.Y - PointSecond2.Y) +
                PointSecond2.X * (PointSecond1.Y - PointFirst2.Y) +
                PointFirst2.X * (PointSecond2.Y - PointSecond1.Y));
            if (((Linep1 ^ Linep2) >= 0) && !(Linep1 == 0 && Linep2 == 0))//符号位异或为0:PointFirst1和PointFirst2在PointSecond1->PointSecond2同侧
            {
                return false;
            }
            //否则判为相交
            return true;
        }
        
        //求两直线交点，前提是两条直线必须有交点
        //在相交的情况下，可以应付各种情况(垂直、系数等)


        /// <summary>
        /// 获取两线段交点
        /// </summary>
        /// <param name="p1">线段1点1</param>
        /// <param name="p2">线段1点2</param>
        /// <param name="q1">线段2点1</param>
        /// <param name="q2">线段2点2</param>
        /// <returns></returns>
        public static PointF GetCrossPoint(PointF p1, PointF p2, PointF q1, PointF q2)
        {
            /*根据两点式化为标准式，进而求线性方程组*/
            PointF crossPoint = new PointF();
            double tempLeft, tempRight;
            //求x坐标
            tempLeft = (q2.X - q1.X) * (p1.Y - p2.Y) - (p2.X - p1.X) * (q1.Y - q2.Y);
            tempRight = (p1.Y - q1.Y) * (p2.X - p1.X) * (q2.X - q1.X) + q1.X * (q2.Y - q1.Y) * (p2.X - p1.X) - p1.X * (p2.Y - p1.Y) * (q2.X - q1.X);
            crossPoint.X = (float)((double)tempRight / (double)tempLeft);
            //求Y坐标	
            tempLeft = (p1.X - p2.X) * (q2.Y - q1.Y) - (p2.Y - p1.Y) * (q1.X - q2.X);
            tempRight = p2.Y * (p1.X - p2.X) * (q2.Y - q1.Y) + (q2.X - p2.X) * (q2.Y - q1.Y) * (p1.Y - p2.Y) - q2.Y * (q1.X - q2.X) * (p2.Y - p1.Y);
            crossPoint.Y = (float)((double)tempRight / (double)tempLeft);
            return crossPoint;
        }
    }
}
