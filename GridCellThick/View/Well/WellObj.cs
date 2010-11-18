using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
namespace View.Well
{
    /// <summary>
    /// 井基类
    /// </summary>
    public class WellObj
    {
        private float _X;//X坐标
        private float _Y;//Y坐标
        private Color _Color1 = Color.Black;//前景色
        private Color _Color2;//背景色
        private bool _IsVisible = true;//是否可见
        private bool _IsSelected = false;//是否选中
        private float _Scale = 0.1f;//显示比例

        /// <summary>
        /// X坐标
        /// </summary>
        public float X
        {
            get { return _X; }
            set { _X = value; }
        }
        /// <summary>
        /// Y坐标
        /// </summary>
        public float Y
        {
            get { return _Y; }
            set { _Y = value; }
        }
        /// <summary>
        /// 前景色
        /// </summary>
        public Color Color1
        {
            get { return _Color1; }
            set { _Color1 = value; }
        }
        /// <summary>
        /// 背景色
        /// </summary>
        public Color Color2
        {
            get { return _Color2; }
            set { _Color2 = value; }
        }
        /// <summary>
        /// 是否可见
        /// </summary>
        public bool IsVisible
        {
            get { return _IsVisible; }
            set { _IsVisible = value; }
        }
        /// <summary>
        /// 是否选中
        /// </summary>
        public bool IsSelected
        {
            get { return _IsSelected; }
            set { _IsSelected = value; }
        }
        /// <summary>
        /// 显示比例
        /// </summary>
        public float Scale
        {
            get { return _Scale; }
            set { _Scale = value; }
        }
    }
}
