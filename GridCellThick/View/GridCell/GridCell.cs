using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
namespace View.GridCell
{
    /// <summary>
    /// 网格
    /// </summary>
    public class GridCell
    {

        #region 变量声明

        //X
        private float _X1;
        private float _X2;
        private float _X3;
        private float _X4;

        //Y
        private float _Y1;
        private float _Y2;
        private float _Y3;
        private float _Y4;

        //Z
        private float _Z1;
        private float _Z2;
        private float _Z3;
        private float _Z4;
        private float _ZValue;
        
        private int _NX;
        private int _NY;
        private int _NZ;

        private int _DX;
        private int _DY;
        private int _DZ;

        private Color _PenColor=Color.Black;//画笔颜色
        private Color _FillColor=Color.Red;//填充颜色
        private Color _DPenColor=Color.Blue;//加密线颜色
        private Color _DFillColor=Color.SkyBlue;//加密填充颜色
        private bool _IsSelected = false;//是否选中
        private bool _IsVisible = true;//是否显示
        private float _Scale = 1;//比例
        private bool _DIsSelected = false;//是否选中网格加密

        private string _DGridCellName ;//网格名字

        #endregion

        #region 属性

        /// <summary>
        /// 网格第一个X坐标
        /// </summary>
        public float X1
        {
            get { return _X1; }
            set { _X1 = value; }
        }
        /// <summary>
        /// 网格第二个X坐标
        /// </summary>
        public float X2
        {
            get { return _X2; }
            set { _X2 = value; }
        }
        /// <summary>
        /// 网格第三个X坐标
        /// </summary>
        public float X3
        {
            get { return _X3; }
            set { _X3 = value; }
        }
        /// <summary>
        /// 网格第四个X坐标
        /// </summary>
        public float X4
        {
            get { return _X4; }
            set { _X4 = value; }
        }


        /// <summary>
        /// 网格第一个Y坐标
        /// </summary>
        public float Y1
        {
            get { return _Y1; }
            set { _Y1 = value; }
        }
        /// <summary>
        /// 网格第二个Y坐标
        /// </summary>
        public float Y2
        {
            get { return _Y2; }
            set { _Y2 = value; }
        }
        /// <summary>
        /// 网格第三个X坐标
        /// </summary>
        public float Y3
        {
            get { return _Y3; }
            set { _Y3 = value; }
        }
        /// <summary>
        /// 网格第四个Y坐标
        /// </summary>
        public float Y4
        {
            get { return _Y4; }
            set { _Y4 = value; }
        }

        /// <summary>
        /// 网格第一个Z坐标
        /// </summary>
        public float Z1
        {
            get { return _Z1; }
            set { _Z1 = value; }
        }
        /// <summary>
        /// 网格第二个Z坐标
        /// </summary>
        public float Z2
        {
            get { return _Z2; }
            set { _Z2 = value; }
        }
        /// <summary>
        /// 网格第三个Z坐标
        /// </summary>
        public float Z3
        {
            get { return _Z3; }
            set { _Z3 = value; }
        }
        /// <summary>
        /// 网格第四个Z坐标
        /// </summary>
        public float Z4
        {
            get { return _Z4; }
            set { _Z4 = value; }
        }
        /// <summary>
        /// Z值的平均值
        /// </summary>
        public float ZValue
        {
            get { return _ZValue; }
            set { _ZValue = value; }
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
        /// 填充颜色
        /// </summary>
        public Color FillColor
        {
            get { return _FillColor; }
            set { _FillColor = value; }
        }
        /// <summary>
        /// 加密线颜色
        /// </summary>
        public Color DPenColor
        {
            get { return _DPenColor; }
            set { _DPenColor = value; }
        }
        /// <summary>
        /// 加密填充颜色
        /// </summary>
        public Color DFillColor
        {
            get { return _DFillColor; }
            set { _DFillColor = value; }
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
        /// 是否显示
        /// </summary>
        public bool IsVisible
        {
            get { return _IsVisible; }
            set { _IsVisible = value; }
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
        /// NX
        /// </summary>
        public int NX
        {
            get { return _NX; }
            set { _NX = value; }
        }
        /// <summary>
        /// NY
        /// </summary>
        public int NY
        {
            get { return _NY; }
            set { _NY = value; }
        }
        /// <summary>
        /// NZ
        /// </summary>
        public int NZ
        {
            get { return _NZ; }
            set { _NZ = value; }
        }
        /// <summary>
        /// X方向加密系数
        /// </summary>
        public int DX
        {
            get { return _DX; }
            set { _DX = value; }
        }
        /// <summary>
        /// Y方向加密系数
        /// </summary>
        public int DY
        {
            get { return _DY; }
            set { _DY = value; }
        }
        /// <summary>
        /// Z方向加密系数
        /// </summary>
        public int DZ
        {
            get { return _DZ; }
            set { _DZ = value; }
        }
        /// <summary>
        /// 是否选中网格加密
        /// </summary>
        public bool DIsSelected
        {
            get { return _DIsSelected; }
            set { _DIsSelected = value; }
        }
        /// <summary>
        /// 网格名称ID
        /// </summary>
        public string DGridCellName
        {
            get { return _DGridCellName; }
            set { _DGridCellName = value; }
        }
        #endregion

        #region 画网格

        /// <summary>
        /// 画网格
        /// </summary>
        /// <param name="g">Graphics 对象</param>
        public void Draw(Graphics g)
        {
            Pen p = new Pen(_PenColor, 1f);//画笔
            Pen dp = new Pen(_DPenColor, 1f);//加密画笔
            if (_DIsSelected)
            {
                dp.Color = _DFillColor;
            }
            //画填充
            if (_IsSelected)
            {
                Color AlphaColor = Color.FromArgb(150, _FillColor);
                SolidBrush br = new SolidBrush(AlphaColor);
                PointF[] pdf = new PointF[4];

                pdf[0].X = _X1 * _Scale;
                pdf[0].Y = _Y1 * _Scale;

                pdf[1].X = _X2 * _Scale;
                pdf[1].Y = _Y2 * _Scale;

                pdf[2].X = _X3 * _Scale;
                pdf[2].Y = _Y3 * _Scale;

                pdf[3].X = _X4 * _Scale;
                pdf[3].Y = _Y4 * _Scale;

                g.FillPolygon(br, pdf);

                if (_DX != 0 || _DY != 0 || _DZ != 0)
                {
                    AlphaColor = Color.FromArgb(150, _DFillColor);
                    br.Color = AlphaColor;
                    g.FillPolygon(br, pdf);
                }
                
            }
            else
            {
                Color AlphaColor = Color.FromArgb(255, Color.White);
                SolidBrush br = new SolidBrush(AlphaColor);
                if (_DX != 0 || _DY != 0 || _DZ != 0)
                {
                    if (_DIsSelected)
                    {
                        AlphaColor = Color.FromArgb(150, _DPenColor);
                    }
                    else
                    {
                        AlphaColor = Color.FromArgb(150, _DFillColor);
                    }
                    br.Color = AlphaColor;
                }
                
                PointF[] pdf = new PointF[4];

                pdf[0].X = _X1 * _Scale;
                pdf[0].Y = _Y1 * _Scale;

                pdf[1].X = _X2 * _Scale;
                pdf[1].Y = _Y2 * _Scale;

                pdf[2].X = _X3 * _Scale;
                pdf[2].Y = _Y3 * _Scale;

                pdf[3].X = _X4 * _Scale;
                pdf[3].Y = _Y4 * _Scale;

                g.FillPolygon(br, pdf);
            }
            //画轮廓
            if (_IsVisible)
            {
                g.DrawLine(p, _X1 * _Scale, _Y1 * _Scale, _X2 * _Scale, _Y2 * _Scale);
                g.DrawLine(p, _X2 * _Scale, _Y2 * _Scale, _X3 * _Scale, _Y3 * _Scale);
                g.DrawLine(p, _X3 * _Scale, _Y3 * _Scale, _X4 * _Scale, _Y4 * _Scale);
                g.DrawLine(p, _X4 * _Scale, _Y4 * _Scale, _X1 * _Scale, _Y1 * _Scale);
                
                for (int i = 0; i < _DX - 1; i++)
                {
                    PointF PTemp1 = new PointF();
                    PointF PTemp2 = new PointF();
                    PTemp1.X = (_X2 - _X1) / _DX * (i + 1) + _X1;
                    PTemp1.Y = (_Y2 - _Y1) / _DX * (i + 1) + _Y1;

                    PTemp2.X = (_X3 - _X4) / _DX * (i + 1) + _X4;
                    PTemp2.Y = (_Y3 - _Y4) / _DX * (i + 1) + _Y4;
                    g.DrawLine(dp, PTemp1.X * _Scale, PTemp1.Y * _Scale, PTemp2.X * _Scale, PTemp2.Y * _Scale);
                }
                for (int i = 0; i < _DY - 1; i++)
                {
                    PointF PTemp1 = new PointF();
                    PointF PTemp2 = new PointF();
                    PTemp1.X = (_X4 - _X1) / _DY * (i + 1) + _X1;
                    PTemp1.Y = (_Y4 - _Y1) / _DY * (i + 1) + _Y1;

                    PTemp2.X = (_X3 - _X2) / _DY * (i + 1) + _X2;
                    PTemp2.Y = (_Y3 - _Y2) / _DY * (i + 1) + _Y2;
                    g.DrawLine(dp, PTemp1.X * _Scale, PTemp1.Y * _Scale, PTemp2.X * _Scale, PTemp2.Y * _Scale);
                }
            }
        }

        #endregion

        #region 判断点是否在图形内

        /// <summary>
        /// 判断是否在图形内
        /// </summary>
        /// <param name="p">点</param>
        /// <returns>是否在图形内</returns>
        public bool IsInPath(PointF p)
        {
            PointF[] pdf = new PointF[4];

            pdf[0].X = _X1 * _Scale;
            pdf[0].Y = _Y1 * _Scale;

            pdf[1].X = _X2 * _Scale;
            pdf[1].Y = _Y2 * _Scale;

            pdf[2].X = _X3 * _Scale;
            pdf[2].Y = _Y3 * _Scale;

            pdf[3].X = _X4 * _Scale;
            pdf[3].Y = _Y4 * _Scale;

            GraphicsPath gp = new GraphicsPath();
            gp.AddPolygon(pdf);
            
            
            return gp.IsVisible(p);

        }
        /// <summary>
        /// 判断是否在图形内
        /// </summary>
        /// <param name="X">X坐标</param>
        /// <param name="Y">Y坐标</param>
        /// <returns>是否在图形内</returns>
        public bool IsInPath(float X,float Y)
        {
            PointF[] pdf = new PointF[4];

            pdf[0].X = _X1 * _Scale;
            pdf[0].Y = _Y1 * _Scale;

            pdf[1].X = _X2 * _Scale;
            pdf[1].Y = _Y2 * _Scale;

            pdf[2].X = _X3 * _Scale;
            pdf[2].Y = _Y3 * _Scale;

            pdf[3].X = _X4 * _Scale;
            pdf[3].Y = _Y4 * _Scale;

            GraphicsPath gp = new GraphicsPath();
            gp.AddPolygon(pdf);
            PointF p = new PointF(X, Y);
            return gp.IsVisible(p);
        }
        #endregion
    }
}
