using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Track
{
    /// <summary>
    /// 曲面
    /// </summary>
    public class Face
    {
        #region 变量声明

        private string _FaceName = "";//曲面名称
        private double[] _PointX;//曲面X点集合
        private double[] _PointY;//曲面Y点集合
        private double[] _PointZ;//曲面Z点集合
        private int[] _IndexData;//曲面点索引集合
        private int _PointCount;//曲面点个数
        private int _IndexCount;//曲面索引个数

        #endregion

        #region 属性

        /// <summary>
        /// 曲面名称
        /// </summary>
        public string FaceName
        {
            get { return _FaceName; }
            set { _FaceName = value; }
        }
        /// <summary>
        /// 曲面X点集合
        /// </summary>
        public double[] PointX
        {
            get { return _PointX; }
            set { _PointX = value; }
        }
        /// <summary>
        /// 曲面Y点集合
        /// </summary>
        public double[] PointY
        {
            get { return _PointY; }
            set { _PointY = value; }
        }
        /// <summary>
        /// 曲面Z点集合
        /// </summary>
        public double[] PointZ
        {
            get { return _PointZ; }
            set { _PointZ = value; }
        }
        /// <summary>
        /// 曲面索引集合
        /// </summary>
        public int[] IndexData
        {
            get { return _IndexData; }
            set { _IndexData = value; }
        }
        /// <summary>
        /// 曲面索引个数
        /// </summary>
        public int IndexCount
        {
            get { return _IndexCount; }
            set { _IndexCount = value; }
        }
        /// <summary>
        /// 曲面点个数
        /// </summary>
        public int PointCount
        {
            get { return _PointCount; }
            set { _PointCount = value; }
        }

        #endregion

        #region 方法



        #endregion
    }
}
