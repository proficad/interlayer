using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.SelectRegion
{
    /// <summary>
    /// 选定区域
    /// </summary>
    public class SelectRegion
    {
        #region 构造方法 

        /// <summary>
        /// 初始化数据
        /// </summary>
        public SelectRegion()
        {
            _PointList = new List<PointF>();//初始化点集合
        }

        #endregion

        #region 变量声明

        private List<PointF> _PointList;//区域点集合
        private bool _IsSelected = false;//是否选中
        private Color _PenColor = Color.Black;//画笔颜色
        private Color _FillColor = Color.White;//填充颜色
        private float _Scale = 1;//比例

        #endregion

        #region 属性

        /// <summary>
        /// 区域点集合
        /// </summary>
        public List<PointF> PointList
        {
            get { return _PointList; }
            set { _PointList = value; }
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
        /// 比例
        /// </summary>
        public float Scale
        {
            get { return _Scale; }
            set { _Scale = value; }
        }

        #endregion

        #region 画填充

        /// <summary>
        /// 画选定区域的填充和线
        /// </summary>
        /// <param name="g">Graphics对象</param>
        public void Draw(Graphics g)
        {
            Pen p = new Pen(_PenColor, 1f);//画笔
            Pen SelectPen = new Pen(_PenColor, 3f);//画笔
            if (_IsSelected)
            {
                //画线，一个点就不画了
                if (_PointList.Count > 1)
                {
                    for (int i = 0; i < _PointList.Count-1; i++)
                    {
                        g.DrawLine(SelectPen, _PointList[i].X * _Scale, _PointList[i].Y * _Scale, _PointList[i + 1].X * _Scale, _PointList[i + 1].Y * _Scale);
                    }
                    g.DrawLine(SelectPen, _PointList[0].X * _Scale, _PointList[0].Y * _Scale, _PointList[_PointList.Count - 1].X * _Scale, _PointList[_PointList.Count - 1].Y * _Scale);
                }

            }
            else
            { 
                //画填充
                if (_PointList.Count > 2)
                {
                    Color AlphaColor = Color.FromArgb(150, _FillColor);
                    SolidBrush br = new SolidBrush(AlphaColor);
                    PointF[] pdf = new PointF[_PointList.Count];
                    for (int i = 0; i < _PointList.Count; i++)
                    {
                        PointF PointTemp=new PointF();
                        PointTemp.X = _PointList[i].X * _Scale;
                        PointTemp.Y = _PointList[i].Y * _Scale;
                        pdf[i] = PointTemp;
                    }
                    g.FillPolygon(br, pdf);
                }
            }
        }

        #endregion

    }
}
