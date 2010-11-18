using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    /// <summary>
    /// 加密网格数据，一个DGridData是一个数据项，具有一个名字，多个加密项目
    /// </summary>
    public class DGridData
    {
        private string _Name = "";//加密网格的总名字
        //private List<string> _DName;//加密网格中包含的加密网格
        private int _X1;//X方向网格
        private int _X2;//X方向网格
        private int _Y1;//Y方向网格
        private int _Y2;//Y方向网格
        private int _Z1;//Z方向网格
        private int _Z2;//Z方向网格
        private int _DX;//X方向网格细分个数
        private int _DY;//Y方向网格细分个数
        private int _DZ;//Z方向网格细分个数

        private int _NWMAX ;//XXX
        private string _GLOBAL = "";//XX

        #region 属性

        /// <summary>
        /// 网格加密总名字
        /// </summary>
        public string Name
        {
            get { return _Name; }
            set { _Name = value; }
        }

        public int NWMAX
        {
            get { return _NWMAX; }
            set { _NWMAX = value; }
        }
        public string GLOBAL
        {
            get { return _GLOBAL; }
            set { _GLOBAL = value; }
        }
        ///// <summary>
        ///// 网格加密总名字下面的名字列表
        ///// </summary>
        //public List<string> DName
        //{
        //    get { return _DName; }
        //    set { _DName = value; }
        //}

        #region X，Y，Z方向

        /// <summary>
        /// 网格加密X方向起始
        /// </summary>
        public int X1
        {
            get { return _X1; }
            set { _X1 = value; }
        }
        /// <summary>
        /// 网格加密X方向终止
        /// </summary>
        public int X2
        {
            get { return _X2; }
            set { _X2 = value; }
        }
        /// <summary>
        /// 网格加密Y方向起始
        /// </summary>
        public int Y1
        {
            get { return _Y1; }
            set { _Y1 = value; }
        }
        /// <summary>
        /// 网格加密Y方向终止
        /// </summary>
        public int Y2
        {
            get { return _Y2; }
            set { _Y2 = value; }
        }
        /// <summary>
        /// 网格加密Z方向起始
        /// </summary>
        public int Z1
        {
            get { return _Z1; }
            set { _Z1 = value; }
        }
        /// <summary>
        /// 网格加密Z方向终止
        /// </summary>
        public int Z2
        {
            get { return _Z2; }
            set { _Z2 = value; }
        }
        
        #endregion

        /// <summary>
        /// X方向网格细分个数
        /// </summary>
        public int DX
        {
            get { return _DX; }
            set { _DX = value; }
        }
        /// <summary>
        /// Y方向网格细分个数
        /// </summary>
        public int DY
        {
            get { return _DY; }
            set { _DY = value; }
        }
        /// <summary>
        /// Z方向网格细分个数
        /// </summary>
        public int DZ
        {
            get { return _DZ; }
            set { _DZ = value; }
        }

        #endregion
    }
}
