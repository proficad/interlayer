using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Track
{
    class InFaceGridData
    {
        #region 变量声明

        private List<float> _PointX = new List<float>();//X的点集合，一个网格有8个点
        private List<float> _PointY = new List<float>();//Y的点集合，一个网格有8个点
        private List<float> _PointZ = new List<float>();//Z的点集合，一个网格有8个点

        private int _X;//I
        private int _Y;//J
        private int _Z;//K

        private int _DX;//网格加密X方向个数
        private int _DY;//网格加密Y方向个数
        private int _DZ;//网格加密Z方向个数

        private List<InFaceGridData> _ChildrenGridDataList=new List<InFaceGridData>();//子节点列表

        #endregion

        #region 属性

        /// <summary>
        /// X的点集合，一个网格有8个点
        /// </summary>
        public List<float> PointX
        {
            get { return _PointX; }
            set { _PointX = value; }
        }
        /// <summary>
        /// Y的点集合，一个网格有8个点
        /// </summary>
        public List<float> PointY
        {
            get { return _PointY; }
            set { _PointY = value; }
        }
        /// <summary>
        /// Z的点集合，一个网格有8个点
        /// </summary>
        public List<float> PointZ
        {
            get { return _PointZ; }
            set { _PointZ = value; }
        }
        /// <summary>
        /// I
        /// </summary>
        public int X
        {
            get { return _X; }
            set { _X = value; }
        }
        /// <summary>
        /// J
        /// </summary>
        public int Y
        {
            get { return _Y; }
            set { _Y = value; }
        }
        /// <summary>
        /// K
        /// </summary>
        public int Z
        {
            get { return _Z; }
            set { _Z = value; }
        }
        /// <summary>
        /// 网格加密X方向个数
        /// </summary>
        public int DX
        {
            get { return _DX; }
            set { _DX = value; }
        }
        /// <summary>
        /// 网格加密Y方向个数
        /// </summary>
        public int DY
        {
            get { return _DY; }
            set { _DY = value; }
        }
        /// <summary>
        /// 网格加密Z方向个数
        /// </summary>
        public int DZ
        {
            get { return _DZ; }
            set { _DZ = value; }
        }
        /// <summary>
        /// 子节点列表
        /// </summary>
        public List<InFaceGridData> ChildrenGridDataList
        {
            get { return _ChildrenGridDataList; }
            set { _ChildrenGridDataList = value; }
        }
        #endregion
    }
}
