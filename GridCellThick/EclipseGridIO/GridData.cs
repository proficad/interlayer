using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    /// <summary>
    /// 网格数据类
    /// </summary>
    public class GridData
    {
        #region 变量声明

        private bool _IsConerPointGrid = true;//是否是角点网格
        
        //方向数
        private int _GridX = 0;//X方向数
        private int _GridY = 0;//Y方向数
        private int _GridZ = 0;//Z方向数
        //最大值&最小值
        private double _MinX = 99999999999;//X最小值
        private double _MinY = 99999999999;//Y最小值
        private double _MinZ = 99999999999;//Z最小值
        private double _MaxX = -99999999999;//X最大值
        private double _MaxY = -99999999999;//Y最大值
        private double _MaxZ = -99999999999;//Z最大值
        //网格点
        private double[] _PointX;//X点，一个网格8个点
        private double[] _PointY;//Y点，一个网格8个点
        private double[] _PointZ;//Z点，一个网格8个点
        private int[] _DPointX;//网格的加密X方向个数
        private int[] _DPointY;//网格的加密Y方向个数
        private int[] _DPointZ;//网格的加密Z方向个数
        //线
        private double[] _LineX1;
        private double[] _LineX2;
        private double[] _LineY1;
        private double[] _LineY2;
        private double[] _LineZ1;
        private double[] _LineZ2;

        private double[] _PointZcron;//原始点深度
        private List<LGRName> _DNameList =new List<LGRName>();//分组列表
        private List<DGridData> _DGridDataList = new List<DGridData>();//加密网格列表

        #endregion

        #region 属性

        /// <summary>
        /// 是否是角点网格
        /// </summary>
        public bool IsConerPointGrid
        {
            get { return _IsConerPointGrid; }
            set { _IsConerPointGrid = value; }
        }
        /// <summary>
        /// 加密网格列表
        /// </summary>
        public List<DGridData> DGridDataList
        {
            get { return _DGridDataList; }
            set { _DGridDataList = value; }
        }
        /// <summary>
        /// 加密网格名字列表
        /// </summary>
        public List<LGRName> DNameList
        {
            get { return _DNameList; }
            set { _DNameList = value; }
        }

        #region  方向数

        /// <summary>
        /// X方向数
        /// </summary>
        public int GridX
        {
            get { return _GridX; }
            set { _GridX = value; }
        }
        /// <summary>
        /// Y方向数
        /// </summary>
        public int GridY
        {
            get { return _GridY; }
            set { _GridY = value; }
        }
        /// <summary>
        /// Z方向数
        /// </summary>
        public int GridZ
        {
            get { return _GridZ; }
            set { _GridZ = value; }
        }

        #endregion

        #region 最大值&最小值

        /// <summary>
        /// X最小值
        /// </summary>
        public double MinX
        {
            get { return _MinX; }
            set { _MinX = value; }
        }
        /// <summary>
        /// Y最小值
        /// </summary>
        public double MinY
        {
            get { return _MinY; }
            set { _MinY = value; }
        }
        /// <summary>
        /// Z最小值
        /// </summary>
        public double MinZ
        {
            get { return _MinZ; }
            set { _MinZ = value; }
        }
        /// <summary>
        /// X最大值
        /// </summary>
        public double MaxX
        {
            get { return _MaxX; }
            set { _MaxX = value; }
        }
        /// <summary>
        /// Y最大值
        /// </summary>
        public double MaxY
        {
            get { return _MaxY; }
            set { _MaxY = value; }
        }
        /// <summary>
        /// Z最大值
        /// </summary>
        public double MaxZ
        {
            get { return _MaxZ; }
            set { _MaxZ = value; }
        }

        #endregion

        #region 网格点

        /// <summary>
        /// X点，一个网格8个点
        /// </summary>
        public double[] PointX
        {
            get { return _PointX; }
            set { _PointX = value; }
        }
        /// <summary>
        /// Y点，一个网格8个点
        /// </summary>
        public double[] PointY
        {
            get { return _PointY; }
            set { _PointY = value; }
        }
        /// <summary>
        /// Z点，一个网格8个点
        /// </summary>
        public double[] PointZ
        {
            get { return _PointZ; }
            set { _PointZ = value; }
        }
        /// <summary>
        /// 网格的加密X方向个数
        /// </summary>
        public int[] DPointX
        {
            get { return _DPointX; }
            set { _DPointX = value; }
        }
        /// <summary>
        /// 网格的加密Y方向个数
        /// </summary>
        public int[] DPointY
        {
            get { return _DPointY; }
            set { _DPointY = value; }
        }
        /// <summary>
        /// 网格的加密Z方向个数
        /// </summary>
        public int[] DPointZ
        {
            get { return _DPointZ; }
            set { _DPointZ = value; }
        }

        /// <summary>
        /// 原始点深度
        /// </summary>
        public double[] PointZcron
        {
            get { return _PointZcron; }
            set { _PointZcron = value; }
        }

        #endregion

        #region 网格线

        /// <summary>
        /// 线,X1
        /// </summary>
        public double[] LineX1
        {
            get { return _LineX1; }
            set { _LineX1 = value; }
        }
        /// <summary>
        /// 线,X2
        /// </summary>
        public double[] LineX2
        {
            get { return _LineX2; }
            set { _LineX2 = value; }
        }
        /// <summary>
        /// 线,Y1
        /// </summary>
        public double[] LineY1
        {
            get { return _LineY1; }
            set { _LineY1 = value; }
        }
        /// <summary>
        /// 线,Y2
        /// </summary>
        public double[] LineY2
        {
            get { return _LineY2; }
            set { _LineY2 = value; }
        }
        /// <summary>
        /// 线,Z1
        /// </summary>
        public double[] LineZ1
        {
            get { return _LineZ1; }
            set { _LineZ1 = value; }
        }
        /// <summary>
        /// 线,Z2
        /// </summary>
        public double[] LineZ2
        {
            get { return _LineZ2; }
            set { _LineZ2 = value; }
        }

        #endregion

        #endregion

    }
}
