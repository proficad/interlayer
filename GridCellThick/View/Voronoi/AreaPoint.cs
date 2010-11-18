using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.Voronoi
{
    /// <summary>
    /// 面积点集合类
    /// </summary>
    public class AreaPoint
    {
        #region 变量声明

        private List<PointF> _PointList = new List<PointF>();//点集合
        private int _Index = 0;//索引
        private double _Area = 0;//面积

        #endregion

        #region 属性

        /// <summary>
        /// 点集合
        /// </summary>
        public List<PointF> PointList
        {
            get { return _PointList; }
            set { _PointList = value; }
        }

        /// <summary>
        /// 索引
        /// </summary>
        public int Index
        {
            get { return _Index; }
            set { _Index = value; }
        }

        /// <summary>
        /// 面积
        /// </summary>
        public double Area
        {
            get { return _Area; }
            set { _Area = value; }
        }

        #endregion
    }
}
