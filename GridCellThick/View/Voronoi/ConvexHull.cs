using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.Voronoi
{
    public class ConvexHull
    {
        #region 变量及属性

        /// <summary>
        /// 原始点列
        /// </summary>
        private List<PointF> points;

        public List<PointF> Points
        {
            set
            {
                points = value;
            }
        }

        /// <summary>
        /// 最终的凸壳点列
        /// </summary>
        private List<PointF> hullPoints;

        public List<PointF> HullPoints
        {
            get
            {
                return hullPoints;
            }
        }

        /// <summary>
        /// 初始凸壳点列
        /// </summary>
        private List<PointF> initHulls;

        /// <summary>
        ///经过处理的点结构
        /// </summary>
        struct VecPoint
        {
            /// <summary>
            /// 点坐标
            /// </summary>
            PointF point;

            /// <summary>
            /// 是否被使用
            /// </summary>
            bool isUsed;

            /// <summary>
            /// 点索引
            /// </summary>
            int index;
            public PointF Point
            {
                get { return point; }
                set { point = value; }
            }

            public bool IsUsed
            {
                get { return isUsed; }
                set { isUsed = value; }
            }

            public int Index
            {
                get { return index; }
                set { index = value; }
            }
        }

        /// <summary>
        /// 经过处理的点列
        /// </summary>
        private List<VecPoint> vecPoints;

        #endregion

        #region 私有方法


        /// <summary>
        /// 生成初始凸壳，将x坐标最大最小和y坐标最大最小的点作为初始凸壳
        /// </summary>
        private void GetInitHull()
        {
            vecPoints = new List<VecPoint>();
            for (int i = 0; i < points.Count; i++)
            {
                VecPoint vecPoint = new VecPoint();
                vecPoint.Index = i;
                vecPoint.IsUsed = false;
                vecPoint.Point = points[i];
                vecPoints.Add(vecPoint);
            }

            float minX;
            float minY;
            float maxX;
            float maxY;
            int minXIndex = 0;
            int maxXIndex = 0;
            int minYIndex = 0;
            int maxYIndex = 0;
            minX = vecPoints[0].Point.X;
            maxX = minX;
            minY = vecPoints[0].Point.Y;
            maxY = minY;

            //找到x最大最小点，和y最大最小点
            for (int i = 1; i < vecPoints.Count; i++)
            {
                if (minX > vecPoints[i].Point.X)
                {
                    minX = vecPoints[i].Point.X;
                    minXIndex = i;
                }

                if (maxX < vecPoints[i].Point.X)
                {
                    maxX = vecPoints[i].Point.X;
                    maxXIndex = i;
                }

                if (minY > vecPoints[i].Point.Y)
                {
                    minY = vecPoints[i].Point.Y;
                    minYIndex = i;
                }
                if (maxY < vecPoints[i].Point.Y)
                {
                    maxY = vecPoints[i].Point.Y;
                    maxYIndex = i;
                }
            }

            hullPoints = new List<PointF>();

            //将这几个点以逆时针的顺序加入到凸壳点列中
            if (vecPoints[minXIndex].IsUsed == false)
            {
                VecPoint vecPoint = vecPoints[minXIndex];
                hullPoints.Add(vecPoint.Point);
                vecPoint.IsUsed = true;
                vecPoints[minXIndex] = vecPoint;
            }

            if (vecPoints[minYIndex].IsUsed == false)
            {
                VecPoint vecPoint = vecPoints[minYIndex];
                hullPoints.Add(vecPoint.Point);
                vecPoint.IsUsed = true;
                vecPoints[minYIndex] = vecPoint;
            }
            if (vecPoints[maxXIndex].IsUsed == false)
            {
                VecPoint vecPoint = vecPoints[maxXIndex];
                hullPoints.Add(vecPoint.Point);
                vecPoint.IsUsed = true;
                vecPoints[maxXIndex] = vecPoint;
            }
            if (vecPoints[maxYIndex].IsUsed == false)
            {
                VecPoint vecPoint = vecPoints[maxYIndex];
                hullPoints.Add(vecPoint.Point);
                vecPoint.IsUsed = true;
                vecPoints[maxYIndex] = vecPoint;
            }

            initHulls = new List<PointF>();
            for (int i = 0; i < hullPoints.Count; i++)
            {
                initHulls.Add(hullPoints[i]);
            }
        }

        /// <summary>
        /// 寻找两个点之间的凸点，采用递归算法
        /// </summary>
        /// <param name="point1"></param>
        /// <param name="point2"></param>
        private void Convex(PointF point1, PointF point2)
        {
            float distance = -1;
            int currentIndex = -1;
            for (int i = 0; i < vecPoints.Count; i++)
            {
                VecPoint vecPoint = vecPoints[i];

                if (vecPoint.IsUsed)
                {
                    continue;
                }
                else
                {
                    int position = Position(point1, point2, vecPoint.Point);
                    if (position == -1)
                    {//在线的左侧，继续搜索
                        continue;
                    }
                    else if (position == 0)
                    {//在线段上，则记下与线的距离
                        if (distance == -1)
                        {
                            distance = 0;
                            currentIndex = i;
                        }
                    }
                    else if (position == 2)
                    {//在线段延长线上，继续搜索
                        continue;
                    }
                    else
                    {//在线的右侧，记录下与线段距离最远的那个点
                        float dis = Distance(point1, point2, vecPoint.Point);
                        if (distance < dis)
                        {
                            distance = dis;
                            currentIndex = i;
                        }
                    }
                }
            }

            if (distance == -1)
            {
                return;
            }
            else
            {
                int index = 0;
                for (int i = 0; i < hullPoints.Count; i++)
                {
                    if (point2.Equals(hullPoints[i]))
                    {
                        index = i;
                    }
                }

                hullPoints.Insert(index, vecPoints[currentIndex].Point);
                VecPoint vecPoint = vecPoints[currentIndex];
                vecPoint.IsUsed = true;
                vecPoints[currentIndex] = vecPoint;
                Convex(point1, vecPoints[currentIndex].Point);
                Convex(vecPoints[currentIndex].Point, point2);
            }

        }

        /// <summary>
        /// 点c与 点a,b组成的线段的位置关系
        /// 采用的算法为面积法。1表示在其右侧，-1表示在其左侧，0表示在线段上，2表示在线段的延长线上
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="c"></param>
        /// <returns></returns>
        private int Position(PointF a, PointF b, PointF c)
        {
            float S = (c.X - a.X) * (b.Y - a.Y) - (b.X - a.X) * (c.Y - a.Y);

            if (S == 0)
            {
                if (c.X >= Math.Min(a.X, b.X) && c.X <= Math.Max(a.X, b.X) && c.Y >= Math.Min(a.Y, b.Y) && c.Y <= Math.Max(a.Y, b.Y))
                {
                    return 0;
                }
                else
                {
                    return 2;
                }
            }
            else if (S > 0)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }

        /// <summary>
        /// 点c 与 点a,b组成的直线的距离
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="c"></param>
        /// <returns></returns>
        private float Distance(PointF a, PointF b, PointF c)
        {
            float top = (b.Y - a.Y) * c.X + (a.X - b.X) * c.Y + a.Y * (b.X - a.X) - (b.Y - a.Y) * a.X;
            float bottom = (float)Math.Sqrt((double)((b.Y - a.Y) * (b.Y - a.Y) + (b.X - a.X) * (b.X - a.X)));
            return Math.Abs(top) / bottom;
        }



        #endregion

        #region 公有方法

        public ConvexHull() { }

        public ConvexHull(List<PointF> pointList)
        {
            points = pointList;
        }


        /// <summary>
        /// 生成凸壳
        /// </summary>
        public void GetConvexHull()
        {
            GetInitHull();
            for (int i = 0; i < initHulls.Count - 1; i++)
            {
                Convex(initHulls[i], initHulls[i + 1]);
            }
            if (initHulls.Count >= 2)
            {
                Convex(initHulls[initHulls.Count - 1], initHulls[0]);
            }
        }

        #endregion
    }
}
