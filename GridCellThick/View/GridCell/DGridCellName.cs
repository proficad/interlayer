using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.GridCell
{
    /// <summary>
    /// 加密网格名称
    /// </summary>
    public class DGridCellName
    {
        #region 变量声明

        private float _X;//名字X坐标
        private float _Y;//名字Y坐标

        private Color _PenColor = Color.Black;//画笔颜色
        private Color _SelectPenColor = Color.Red;//选中画笔颜色
        private float _Scale = 1;//比例

        private string _DName = "";//名称

        private float _Height=10;//高度
        private float _Width=10;//宽度

        private bool _IsSelected = false;//是否选中

        private int _FontSize = 10;//字体大小

        #endregion

        #region 属性

        /// <summary>
        /// 名字坐标X
        /// </summary>
        public float X
        {
            get { return _X; }
            set { _X = value; }
        }
        /// <summary>
        /// 名字坐标Y
        /// </summary>
        public float Y
        {
            get { return _Y; }
            set { _Y = value; }
        }
        /// <summary>
        /// 比例
        /// </summary>
        public float Scale
        {
            get { return _Scale; }
            set { _Scale = value; }
        }
        /// <summary>
        /// 画笔颜色
        /// </summary>
        public Color PenColor
        {
            get { return _PenColor; }
            set { _PenColor = value; }
        }
        /// <summary>
        /// 选中画笔颜色
        /// </summary>
        public Color SelectPenColor
        {
            get { return _SelectPenColor; }
            set { _SelectPenColor = value; }
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
        /// 加密网格名字
        /// </summary>
        public string DName
        {
            get { return _DName; }
            set { _DName = value; }
        }
        /// <summary>
        /// 是否选中
        /// </summary>
        public bool IsSelected
        {
            get { return _IsSelected; }
            set { _IsSelected = value; }
        }

        #endregion

        #region 方法

        /// <summary>
        /// 画名字
        /// </summary>
        /// <param name="g">Graphics对象</param>
        public void Draw(Graphics g)
        {
            Font DrawFont = new Font("Arial", 10 * _Scale, GraphicsUnit.Pixel);
            SolidBrush DrawBrush = new SolidBrush(_PenColor);
            SolidBrush DrawSelectBrush = new SolidBrush(_SelectPenColor);
            //对齐方式
            StringFormat drawFormat = new StringFormat();
            drawFormat.FormatFlags = StringFormatFlags.DirectionRightToLeft;
            if (_IsSelected)
            {
                g.DrawString(_DName, DrawFont, DrawSelectBrush, _X * _Scale, _Y * _Scale);
            }
            else
            {
                g.DrawString(_DName, DrawFont, DrawBrush, _X * _Scale, _Y * _Scale);
            }
        }

        #endregion
    }
}
