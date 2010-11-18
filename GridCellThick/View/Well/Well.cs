using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
namespace View.Well
{
    /// <summary>
    /// 井
    /// </summary>
    public class Well:WellObj
    {
        #region 变量声明

        private String _WellName;//井名
        private float _Height;//高度
        private float _Width;//宽度
        private String _WellType;//井别
        private float _WellCircle = 1;
        private double _ActualX = 0;//实际X坐标
        private double _ActualY = 0;//实际X坐标
        private double _WellData = 0;//数据
        private int _WellIndex = 0;//井序号

        #endregion

        #region 属性

        /// <summary>
        /// 井名
        /// </summary>
        public String WellName
        {
            get { return _WellName; }
            set { _WellName = value; }
        }
        
        /// <summary>
        /// 高度
        /// </summary>
        public float Height
        {
            get { return _Height; }
            set { _Height = value; }
        }
        /// <summary>
        /// 宽度
        /// </summary>
        public float Width
        {
            get { return _Width; }
            set { _Width = value; }
        }
        /// <summary>
        /// 井别
        /// </summary>
        public String WellType
        {
            get { return _WellType; }
            set { _WellType = value; }
        }
        /// <summary>
        /// 井圈大小
        /// </summary>
        public float WellCircle
        {
            get { return _WellCircle; }
            set { _WellCircle = value; }
        }
        /// <summary>
        /// 实际X坐标
        /// </summary>
        public double ActualX
        {
            get { return _ActualX; }
            set { _ActualX = value; }
        }
        /// <summary>
        /// 实际Y坐标
        /// </summary>
        public double ActualY
        {
            get { return _ActualY; }
            set { _ActualY = value; }
        }
        /// <summary>
        /// 井数据
        /// </summary>
        public double WellData
        {
            get { return _WellData; }
            set { _WellData = value; }
        }
        /// <summary>
        /// 井序号
        /// </summary>
        public int WellIndex
        {
            get { return _WellIndex; }
            set { _WellIndex = value; }
        }

        #endregion

    }
}
