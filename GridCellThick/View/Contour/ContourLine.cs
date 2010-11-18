using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
namespace View.Contour
{
    /// <summary>
    ///  等值线类
    /// </summary>
    public class ContourLine
    {
        #region 变量声明

        PointF[] _LinePoints;//点集合
        double _LineValue = 0;//线的值

        #endregion

        #region 属性

        /// <summary>
        /// 点集合
        /// </summary>
        public PointF[] LinePoints
        {
            get { return _LinePoints; }
            set { _LinePoints = value; }
        }

        /// <summary>
        /// 线的值
        /// </summary>
        public double LineValue
        {
            get { return _LineValue; }
            set { _LineValue = value; }
        }

        #endregion

    }
}
