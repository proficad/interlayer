using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using View.Voronoi;
using View.Well;
using System.Drawing.Imaging;
namespace View
{
    public partial class Draw : UserControl
    {

        #region 构造方法

        /// <summary>
        /// 构造方法
        /// </summary>
        public Draw()
        {
            InitializeComponent();
            this.MouseWheel += new MouseEventHandler(Form1_MouseWheel);//注册鼠标滚轮事件
            _ColorDt.Columns.Add("颜色", typeof(Color));
            _ColorDt.Columns.Add("值", typeof(double));
        }

        #endregion

        #region 变量定义

        //井位图变量
        private float _scale = 1f;//显示比例
        private float _XMax = -99999999;//最大X值
        private float _YMax = -99999999;//最大Y值
        private float _XMin = 99999999f;//最小X值
        private float _YMin = 99999999f;//最小Y值
        private float x = 0;//进度条偏移X值
        private float y = 0;//进度条偏移Y值
        private List<View.Well.Well> _lst = new List<View.Well.Well>();//井列表
        private List<View.GridCell.GridCell> _GridCellList = new List<View.GridCell.GridCell>();//网格列表
        private List<View.GridCell.DGridCellName> _DGridCellNameList = new List<View.GridCell.DGridCellName>();//加密网格名字列表
        
        //控件属性定义
        private float _DrawWidth = 0;//宽度
        private float _DrawHeight = 0;//高度

        //鼠标选择
        private bool isDrag = false;//是否鼠标开始拖拽
        private Rectangle theRectangle = new Rectangle(new Point(0, 0), new Size(0, 0));//鼠标拖拽矩形框
        private Point startPoint;//拖拽屏幕开始点
        private Point endPoint;//拖拽屏幕结束点
        private Point StarP;//拖拽开始点
        private Point EndP;//拖拽结束点
        private bool _IsHand = false;//是否开启鼠标拖动

        //Voronoi图
        private Delaynay _D_TIN = new Delaynay(); //Voronoi图核心功能类
        private List<PointF> _VoronoiList = new List<PointF>();//Voronoi图数据点
        private List<AreaPoint> _AreaPointList = new List<AreaPoint>();//面积点
        private bool _IsVoronoiDrag = false;//是否开始

        //显示图形
        private bool _IsWellVisible = true;//是否显示井位图
        private bool _IsVoronoiVisible = false;//是否显示Voronoi图
        private bool _IsTriangleVisible = false;//是否显示三角网络
        private bool _IsEdgeVisible = false;//是否显示三角网络边缘
        private bool _IsContourVisible = true;//是否显示等值线
        private bool _IsContourBackColorVisible = true;//是否显示等值线色带
        private bool _IsGridCelVisible = false;//是否显示网格

        //等值图
        private double _ValueMax = -99999999;//等值线最大值
        private double _ValueMin = 99999999;//等值线最小值
        private List<Contour.ContourLine> _LineList = new List<View.Contour.ContourLine>();//等值线集合
        private List<double> _LineValueList = new List<double>();//等值线分隔集合

        //色带
        private List<Contour.ContourColorLine> _LineColorList = new List<View.Contour.ContourColorLine>();//色带点集合
        private DataTable _ColorDt = new DataTable();//颜色列表

        #endregion

        #region 属性

        /// <summary>
        /// 井列表
        /// </summary>
        public List<View.Well.Well> lst
        {
            get { return _lst; }
            set { _lst = value; }
        }
        /// <summary>
        /// 网格列表
        /// </summary>
        public List<View.GridCell.GridCell> GridCellList
        {
            get { return _GridCellList; }
            set { _GridCellList = value; }
        }
        /// <summary>
        /// 网格加密名称列表
        /// </summary>
        public List<View.GridCell.DGridCellName> DGridCellNameList
        {
            get { return _DGridCellNameList; }
            set { _DGridCellNameList = value; }
        }
        /// <summary>
        /// 最大X值
        /// </summary>
        public float XMax
        {
            get { return _XMax; }
            set { _XMax = value; }
        }
        /// <summary>
        /// 最大Y值
        /// </summary>
        public float YMax
        {
            get { return _YMax; }
            set { _YMax = value; }
        }
        /// <summary>
        /// 最小X值
        /// </summary>
        public float XMin
        {
            get { return _XMin; }
            set { _XMin = value; }
        }
        /// <summary>
        /// 最小Y值
        /// </summary>
        public float YMin
        {
            get { return _YMin; }
            set { _YMin = value; }
        }
        /// <summary>
        /// Voronoi图核心功能类
        /// </summary>
        public Delaynay D_TIN
        {
            get { return _D_TIN; }
            set { _D_TIN = value; }
        }

        /// <summary>
        /// 是否显示井位图
        /// </summary>
        public bool IsWellVisible
        {
            get { return _IsWellVisible; }
            set { _IsWellVisible = value; }
        }
        /// <summary>
        /// 是否显示Voronoi图
        /// </summary>
        public bool IsVoronoiVisible
        {
            get { return _IsVoronoiVisible; }
            set { _IsVoronoiVisible = value; }
        }
        /// <summary>
        /// 是否显示三角网络
        /// </summary>
        public bool IsTriangleVisible
        {
            get { return _IsTriangleVisible; }
            set { _IsTriangleVisible = value; }
        }
        /// <summary>
        /// 是否显示三角网络边缘
        /// </summary>
        public bool IsEdgeVisible
        {
            get { return _IsEdgeVisible; }
            set { _IsEdgeVisible = value; }
        }
        /// <summary>
        /// 是否显示等值线
        /// </summary>
        public bool IsContourVisible
        {
            get { return _IsContourVisible; }
            set { _IsContourVisible = value; }
        }
        /// <summary>
        /// 是否显示色带
        /// </summary>
        public bool IsContourBackColorVisible
        {
            get { return _IsContourBackColorVisible; }
            set { _IsContourBackColorVisible = value; }
        }
        /// <summary>
        /// 是否显示网格
        /// </summary>
        public bool IsGridCellVisible
        {
            get { return _IsGridCelVisible; }
            set { _IsGridCelVisible = value; }
        }
        /// <summary>
        /// 等值线最大值
        /// </summary>
        public double ValueMax
        {
            get { return _ValueMax; }
            set { _ValueMax = value; }
        }
        /// <summary>
        /// 等值线最小值
        /// </summary>
        public double ValueMin
        {
            get { return _ValueMin; }
            set { _ValueMin = value; }
        }
        /// <summary>
        /// 色带边缘点集合
        /// </summary>
        public List<Contour.ContourColorLine> LineColorList
        {
            get { return _LineColorList; }
            set { _LineColorList = value; }
        }
        /// <summary>
        /// 等值线集合
        /// </summary>
        public List<Contour.ContourLine> LineList
        {
            get { return _LineList; }
            set { _LineList = value; }
        }
        public List<double> LineValueList
        {
            get { return _LineValueList; }
            set { _LineValueList = value; }
        }
        /// <summary>
        /// 颜色列表
        /// </summary>
        public DataTable ColorDt
        {
            get { return _ColorDt; }
            set { _ColorDt = value; }
        }
        /// <summary>
        /// 鼠标拖动
        /// </summary>
        public bool IsHand
        {
            get { return _IsHand; }
            set { _IsHand = value; }
        }
        /// <summary>
        /// 图形宽度
        /// </summary>
        public float DrawWidth
        {
            get { return _DrawWidth; }
            set { _DrawWidth = value; }
        }
        /// <summary>
        /// 图形高度
        /// </summary>
        public float DrawHeight
        {
            get { return _DrawHeight; }
            set { _DrawHeight = value; }
        }
        /// <summary>
        /// 移动x
        /// </summary>
        public float MoveX
        {
            get { return x; }
            set { x = value; ultraScrollBar1.Value = -Convert.ToInt32(value); }
        }
        /// <summary>
        /// 移动y
        /// </summary>
        public float MoveY
        {
            get { return y; }
            set { y = value; ultraScrollBar2.Value = -Convert.ToInt32(value); }
        }
        #endregion

        #region 初始化滚动条

        /// <summary>
        /// 初始化进度条
        /// </summary>
        public void ScrollBarLoad()
        {
            try
            {
                //加载进度条值
                if (Convert.ToInt32(_DrawWidth * _scale) - this.Width < 0)
                {
                    ultraScrollBar1.Maximum = this.Width;
                }
                else
                {
                    ultraScrollBar1.Maximum = Convert.ToInt32(_DrawWidth * _scale) - this.Width + Convert.ToInt32(_DrawWidth );
                }
            }
            catch { }
            try
            {
                if (Convert.ToInt32(_DrawHeight * _scale) - this.Height < 0)
                {
                    ultraScrollBar2.Maximum = this.Height;
                }
                else
                {
                    ultraScrollBar2.Maximum = Convert.ToInt32(_DrawHeight * _scale) - this.Height + Convert.ToInt32(_DrawHeight );
                }
            }
            catch { }
        }

        #endregion

        #region 放大/缩小图形

        /// <summary>
        /// 放大/缩小图形
        /// </summary>
        /// <param name="b">增加/减少</param>
        public void SetScale(bool b)
        {
            if (b)
            {
                //_scale = _scale + 0.2f;//增加比例
                _scale = _scale * 1.1f;//增加比例
                //加载进度条
                double sb1 = Convert.ToDouble(ultraScrollBar1.Value) / Convert.ToDouble(ultraScrollBar1.Maximum);
                double sb2 = Convert.ToDouble(ultraScrollBar2.Value) / Convert.ToDouble(ultraScrollBar2.Maximum);
                ultraScrollBar1.Maximum = Convert.ToInt32(_DrawWidth * _scale);
                ultraScrollBar2.Maximum = Convert.ToInt32(_DrawHeight * _scale);
                ultraScrollBar1.Value = Convert.ToInt32(ultraScrollBar1.Maximum * sb1);
                ultraScrollBar2.Value = Convert.ToInt32(ultraScrollBar2.Maximum * sb2);
                x = -ultraScrollBar1.Value;
                y = -ultraScrollBar2.Value;
                this.Invalidate();//刷新


            }
            else
            {
                if (_scale * 0.9f > 0.01)
                {
                    //_scale = _scale - 0.2f;//减少比例
                    _scale = _scale * 0.9f;//减少比例
                    //加载进度条
                    double sb1 = Convert.ToDouble(ultraScrollBar1.Value) / Convert.ToDouble(ultraScrollBar1.Maximum);
                    double sb2 = Convert.ToDouble(ultraScrollBar2.Value) / Convert.ToDouble(ultraScrollBar2.Maximum);
                    ultraScrollBar1.Maximum = Convert.ToInt32(_DrawWidth * _scale);
                    ultraScrollBar2.Maximum = Convert.ToInt32(_DrawHeight * _scale);
                    ultraScrollBar1.Value = Convert.ToInt32(ultraScrollBar1.Maximum * sb1);
                    ultraScrollBar2.Value = Convert.ToInt32(ultraScrollBar2.Maximum * sb2);

                    x = -ultraScrollBar1.Value;
                    y = -ultraScrollBar2.Value;
                    this.Invalidate();//刷新

                }

            }
        }

        #endregion

        #region 事件

        #region 重绘

        //重绘
        private void Draw_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;//消除锯齿
                e.Graphics.TranslateTransform(x, y);//变换坐标系

                #region 网格

                if (_IsGridCelVisible)
                {
                    ShowGridCell(e.Graphics, false);//画网格
                }

                #endregion

                #region 等值线

                ////色带
                //if (_IsContourBackColorVisible)
                //{
                //    ShowContourBackColor(e.Graphics);
                //}
                ////等值线
                //if (_IsContourVisible)
                //{
                //    ShowContour(e.Graphics);
                //}

                #endregion

                #region 画Voronoi图

                if (_IsTriangleVisible)
                {
                    ShowTriangle(e.Graphics);//三角网络
                }
                if (_IsEdgeVisible)
                {
                    ShowConvex(e.Graphics);//三角网络边缘
                }
                if (_IsVoronoiVisible)
                {
                    ShowVoronoi(e.Graphics);//画Voronoi图
                    CalculationArea();
                }

                #endregion

                #region 井位图

                if (_IsWellVisible)
                {
                    ShowWell(e.Graphics,false);//画井位图
                }

                #endregion


                //画外面的那个黑框
                Pen p = new Pen(Color.Black, 10);
                e.Graphics.DrawRectangle(p, 0, 0, _DrawWidth * _scale, _DrawHeight * _scale);
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }

        }

        #endregion

        #region 窗体Load

        //窗体Load
        private void Draw_Load(object sender, EventArgs e)
        {
            
        }

        #endregion

        #region 鼠标滚轮

        //鼠标滚轮事件
        private void Form1_MouseWheel(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.Delta > 0)
                {
                    SetScale(true);
                }
                else
                {
                    SetScale(false);
                }
            }
            catch
            {

            }
        }

        #endregion

        #region 滚动条移动

        private void ultraScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            //移动
            x = -e.NewValue;//更新偏移坐标X
            this.Invalidate();//刷新
        }

        private void ultraScrollBar2_Scroll(object sender, ScrollEventArgs e)
        {
            //移动
            y = -e.NewValue;//更新偏移坐标Y
            this.Invalidate();//刷新
        }
        
        #endregion

        #region 鼠标按下

        //鼠标按下
        private void Draw_MouseDown(object sender, MouseEventArgs e)
        {
            if (_IsHand)//是否拖动
            {
                Control control = (Control)sender;
                StarP = new Point(e.X, e.Y);//开始窗体坐标
                isDrag = true;//开始拖动
            }
            else
            {
                //是否按下左键
                if (e.Button == MouseButtons.Left)
                {
                    isDrag = true;//开始拖拽
                    Control control = (Control)sender;
                    startPoint = control.PointToScreen(new Point(e.X, e.Y));//开始屏幕坐标
                    endPoint = control.PointToScreen(new Point(e.X, e.Y));//结束屏幕坐标
                    StarP = new Point(e.X, e.Y);//开始窗体坐标
                }
            }
        }

        #endregion

        #region 鼠标移动
        
        //鼠标移动
        private void Draw_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDrag)//是否拖拽
            {
                if (_IsHand)//是否拖动
                {
                    int IntervalX, IntervalY;
                    IntervalX = (StarP.X - e.X);
                    IntervalY = (StarP.Y - e.Y);

                    if ((ultraScrollBar1.Value + IntervalX) >= ultraScrollBar1.Minimum && (ultraScrollBar1.Value + IntervalX) <= ultraScrollBar1.Maximum)
                    {
                        ultraScrollBar1.Value = ultraScrollBar1.Value + IntervalX;
                        x = -ultraScrollBar1.Value;
                    }
                    if ((ultraScrollBar2.Value + IntervalY) >= ultraScrollBar2.Minimum && (ultraScrollBar2.Value + IntervalY) <= ultraScrollBar2.Maximum)
                    {
                        ultraScrollBar2.Value = ultraScrollBar2.Value + IntervalY;
                        y = -ultraScrollBar2.Value;
                    }
                    this.Invalidate();
                    StarP.X = e.X;
                    StarP.Y = e.Y;
                }
                else
                {
                    ControlPaint.DrawReversibleFrame(theRectangle, this.BackColor, FrameStyle.Dashed);//清除虚线框
                    //改变虚线框坐标
                    endPoint = this.PointToScreen(new Point(e.X, e.Y));
                    int width = endPoint.X - startPoint.X;
                    int height = endPoint.Y - startPoint.Y;
                    theRectangle = new Rectangle(startPoint.X, startPoint.Y, width, height);//虚线框存储
                    ControlPaint.DrawReversibleFrame(theRectangle, this.BackColor, FrameStyle.Dashed);//重新绘制虚线框
                }
            }
            //是否显示网格
            if (_IsGridCelVisible)
            {
                //判断是否在点击范围内
                for (int i = 0; i < _GridCellList.Count; i++)
                {
                    if (_GridCellList[i].IsVisible)
                    {
                        if (_GridCellList[i].IsInPath((e.X - x), (e.Y - y)))
                        {
                            if (_GridCellList[i].DX != 0 && _GridCellList[i].DY != 0 && _GridCellList[i].DZ != 0)
                            {
                                toolTip1.Show(GridCellList[i].DGridCellName, this, e.X + 10, e.Y + 20);
                                break;
                            }
                            else
                            {
                                toolTip1.Hide(this);
                                break;
                            }
                        }
                    }
                }
            }
        }

        #endregion

        #region 鼠标抬起

        //鼠标抬起
        private void Draw_MouseUp(object sender, MouseEventArgs e)
        {
            if (isDrag)
            {
                if (_IsHand)//是否拖动
                {
                    isDrag = false;//拖拽结束
                }
                else
                {
                    EndP = new Point(e.X, e.Y);

                    if (isDrag)
                    {
                        //开始和结束坐标点转换
                        if (StarP.X > EndP.X)
                        {
                            Int32 temp = StarP.X;
                            StarP.X = EndP.X;
                            EndP.X = temp;
                        }
                        if (StarP.Y > EndP.Y)
                        {
                            Int32 temp = StarP.Y;
                            StarP.Y = EndP.Y;
                            EndP.Y = temp;
                        }

                        isDrag = false;//拖拽结束
                        ControlPaint.DrawReversibleFrame(theRectangle, this.BackColor, FrameStyle.Dashed);//清楚虚线框
                        if (_IsWellVisible)
                        {
                            foreach (Well.Well w in _lst)
                            {
                                //是否显示
                                if (w.IsVisible)
                                {
                                    if ((w.X * w.Scale + x >= StarP.X && w.X * w.Scale + x <= EndP.X) && (w.Y * w.Scale + y >= StarP.Y && w.Y * w.Scale + y <= EndP.Y))
                                    {
                                        w.IsSelected = true;
                                    }
                                }
                            }
                        }
                        if (_IsGridCelVisible)
                        {
                            for (int i = 0; i < _GridCellList.Count; i++)
                            {
                                if (_GridCellList[i].IsVisible)
                                {
                                    if ((_GridCellList[i].X1 * _scale + x >= StarP.X && _GridCellList[i].X1 * _scale + x <= EndP.X) && (_GridCellList[i].Y1 * _scale + y >= StarP.Y && _GridCellList[i].Y1 * _scale + y <= EndP.Y)
                                        && (_GridCellList[i].X2 * _scale + x >= StarP.X && _GridCellList[i].X2 * _scale + x <= EndP.X) && (_GridCellList[i].Y2 * _scale + y >= StarP.Y && _GridCellList[i].Y2 * _scale + y <= EndP.Y)
                                        && (_GridCellList[i].X3 * _scale + x >= StarP.X && _GridCellList[i].X3 * _scale + x <= EndP.X) && (_GridCellList[i].Y3 * _scale + y >= StarP.Y && _GridCellList[i].Y3 * _scale + y <= EndP.Y)
                                        && (_GridCellList[i].X4 * _scale + x >= StarP.X && _GridCellList[i].X4 * _scale + x <= EndP.X) && (_GridCellList[i].Y4 * _scale + y >= StarP.Y && _GridCellList[i].Y4 * _scale + y <= EndP.Y))
                                    {
                                        if (_GridCellList[i].DX == 0 && _GridCellList[i].DY == 0 && _GridCellList[i].DZ == 0)
                                        {
                                            _GridCellList[i].IsSelected = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            this.Invalidate();//刷新
            //还原变量
            StarP.X = 0;
            StarP.Y = 0;
            EndP.X = 0;
            EndP.Y = 0;
            theRectangle = new Rectangle(0, 0, 0, 0);
        }
        #endregion

        #region 鼠标点击

        private void Draw_MouseClick(object sender, MouseEventArgs e)
        {
            bool IsInvalidate = false;

            if (!_IsHand)//是否是选择状态
            {
                EndP = new Point(e.X, e.Y);
                if (StarP == EndP)
                {
                    //是否显示井
                    if (_IsWellVisible)
                    {
                        //判断是否在点击范围内
                        for (int i = 0; i < _lst.Count; i++)
                        {
                            if (_lst[i].IsVisible)
                            {
                                if ((_lst[i].X * _lst[i].Scale + x) <= e.X && ((_lst[i].X + _lst[i].Width) * _lst[i].Scale + x) >= e.X && (_lst[i].Y * _lst[i].Scale + y) <= e.Y && ((_lst[i].Y + _lst[i].Height) * _lst[i].Scale + y) >= e.Y)
                                {
                                    //选中和不选中变换
                                    _lst[i].IsSelected = !_lst[i].IsSelected;
                                    IsInvalidate = true;
                                    break;
                                }
                            }
                        }
                    }
                    //是否显示网格
                    if (_IsGridCelVisible)
                    {
                        //判断是否在点击范围内
                        for (int i = 0; i < _GridCellList.Count; i++)
                        {
                            if (_GridCellList[i].IsVisible)
                            {
                                if (_GridCellList[i].IsInPath((e.X - x), (e.Y - y)))
                                {
                                    if (_GridCellList[i].DX == 0 && _GridCellList[i].DY == 0 && _GridCellList[i].DZ == 0)
                                    {
                                        _GridCellList[i].IsSelected = !_GridCellList[i].IsSelected;
                                        IsInvalidate = true;
                                        break;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (IsInvalidate)
                    {
                        this.Invalidate();
                    }
                }
            }
        }

        #endregion

        #endregion

        #region 画井位图

        /// <summary>
        /// 画井位图
        /// </summary>
        /// <param name="g">Graphics</param>
        /// <param name="IsSavePacture">是否保存图片（保存图片时，井全显示）</param>
        private void ShowWell(Graphics g,bool IsSavePacture)
        {
            DrawWell dw = new DrawWell();
            if (IsSavePacture)
            {
                foreach (Well.Well w in _lst)
                {
                    w.Scale = _scale;//显示比例
                    w.IsVisible = true;
                    w.IsSelected = false;
                    try
                    {
                        dw.Draw(w, g);//画井
                    }
                    catch
                    { }
                }
            }
            else
            {
                foreach (Well.Well w in _lst)
                {
                    w.Scale = _scale;//显示比例
                    //是否在屏幕内
                    if ((w.X * w.Scale + x >= 0 && w.X * w.Scale + x <= this.Width) && (w.Y * w.Scale + y >= 0 && w.Y * w.Scale + y <= this.Height))
                    {
                        w.IsVisible = true;
                    }
                    else
                    {
                        w.IsVisible = false;
                    }
                    try
                    {
                        dw.Draw(w, g);//画井
                    }
                    catch
                    { }
                }
            }
        }


        #endregion

        #region Voronoi图及三角网络

        #region 刷新Voronoi图盒子大小

        /// <summary>
        /// 刷新Voronoi图盒子大小
        /// </summary>
        /// <param name="XRight">X坐标</param>
        /// <param name="YBottom">Y坐标</param>
        private void SetVoronoiBoxSize(float XRight, float YBottom)
        {
            D_TIN.DS.BBOX.XLeft = 0;
            D_TIN.DS.BBOX.YTop = 0;
            D_TIN.DS.BBOX.XRight = Convert.ToInt64(XRight);
            D_TIN.DS.BBOX.YBottom = Convert.ToInt64(YBottom);
        }

        #endregion

        #region 增加Voronoi图中的点
        /// <summary>
        /// 增加Voronoi图中的点
        /// </summary>
        /// <param name="PointX">X</param>
        /// <param name="PointY">Y</param>
        private void AddVoronoiPoint(float PointX,float PointY)
        {
            //检查是否有重复点
            for (int i = 0; i < _D_TIN.DS.VerticesNum; i++)
            {
                if ((float)PointX == _D_TIN.DS.Vertex[i].x && (float)PointY == _D_TIN.DS.Vertex[i].y)
                    return;  //若该点已有则不再加入
            }
            //加点            
            _D_TIN.DS.Vertex[_D_TIN.DS.VerticesNum].x = Convert.ToInt64(PointX);
            _D_TIN.DS.Vertex[_D_TIN.DS.VerticesNum].y = Convert.ToInt64(PointY);
            _D_TIN.DS.Vertex[_D_TIN.DS.VerticesNum].ID = D_TIN.DS.VerticesNum;
            _D_TIN.DS.VerticesNum++;
            this.Invalidate();
        }

        #endregion

        #region 设置Voronoi图中的数据

        //设置Voronoi图中的数据
        public void SetVoronoiData()
        {
            //设置Voronoi图的数据
            _D_TIN.DS = new DataStruct();
            SetVoronoiBoxSize(_DrawWidth, _DrawHeight);//设置盒子大小
            for (int i = 0; i < _lst.Count; i++)
            {
                AddVoronoiPoint(_lst[i].X, _lst[i].Y);
            }
        }


        #endregion

        #region 设置Voronoi图

        /// <summary>
        /// 设置Voronoi图
        /// </summary>
        public void SetVoronoi()
        {
            D_TIN.CalculateBC();    //求出每个三角形的圆心
            _VoronoiList=D_TIN.CreateVoronoi();//画Voronoi图
        }

        #endregion

        #region 画Voronoi图

        /// <summary>
        /// 画Voronoi图
        /// </summary>
        /// <param name="g">Graphics</param>
        private void ShowVoronoi(Graphics g)
        {
            for (int i = 0; i < _AreaPointList.Count; i++)
            {
                PointF[] ps = new PointF[_AreaPointList[i].PointList.Count];
                for (int j = 0; j < _AreaPointList[i].PointList.Count; j++)
                {
                    ps[j].X = _AreaPointList[i].PointList[j].X * _scale;
                    ps[j].Y = _AreaPointList[i].PointList[j].Y * _scale;
                }
                if (ps.Length > 0)
                {
                    g.FillPolygon(Brushes.Red, ps);
                }
            }
            for (int i = 0; i < _VoronoiList.Count; i = i + 2)
            {
                PointF p1 = new PointF(_VoronoiList[i].X * _scale, _VoronoiList[i].Y * _scale);
                PointF p2 = new PointF(_VoronoiList[i + 1].X * _scale, _VoronoiList[i + 1].Y * _scale);
                g.DrawLine(new Pen(Color.SkyBlue, 2), p1, p2);
            }
            
        }

        #endregion

        #region 设置三角网络

        /// <summary>
        /// 设置三角网络
        /// </summary>
        /// <param name="g"></param>
        public void SetTriangle()
        {
            if (_D_TIN.DS.VerticesNum > 2)  //构建三角网
                _D_TIN.CreateTIN();
        }

        #endregion

        #region 画三角网络

        /// <summary>
        /// 画三角网格
        /// </summary>
        /// <param name="g">Graphics</param>
        private void ShowTriangle(Graphics g)
        {
            

            float WellX = 0;
            float WellY = 0;
            if (_lst.Count > 0)
            {
                WellX = _lst[0].Width * _scale / 2;
                WellY = _lst[0].Height * _scale / 2;
            }
            //输出三角形
            for (int i = 0; i < _D_TIN.DS.TriangleNum; i++)
            {
                PointF point1 = new PointF(Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V1Index].x) * _scale + WellX, Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V1Index].y) * _scale + WellY);
                PointF point2 = new PointF(Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V2Index].x) * _scale + WellX, Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V2Index].y) * _scale + WellY);
                PointF point3 = new PointF(Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V3Index].x) * _scale + WellX, Convert.ToSingle(D_TIN.DS.Vertex[D_TIN.DS.Triangle[i].V3Index].y) * _scale + WellY);

                Pen p = new Pen(Color.Orange, 0.5f);
                //if ((point1.X + x >= 0 && point1.X + x <= this.Width) && (point1.Y + y >= 0 && point1.Y + y <= this.Height))
                //{
                //    g.DrawLine(p, point1, point2);
                //    g.DrawLine(p, point1, point3);
                //}
                //if ((point2.X + x >= 0 && point2.X + x <= this.Width) && (point2.Y + y >= 0 && point2.Y + y <= this.Height))
                //{
                //    g.DrawLine(p, point2, point3);
                //}
                g.DrawLine(p, point1, point2);
                g.DrawLine(p, point1, point3);
                g.DrawLine(p, point2, point3);
            }
        }
        
        #endregion

        #region 画三角网络的轮廓

        /// <summary>
        /// 画三角网络的轮廓
        /// </summary>
        /// <param name="g">Graphics</param>
        private void ShowConvex(Graphics g)
        {
            D_TIN.CreateHull();
            
            float WellX = 0;
            float WellY = 0;
            if (_lst.Count > 0)
            {
                WellX = _lst[0].Width * _scale / 2;
                WellY = _lst[0].Height * _scale / 2;
            }
            //PointF[] Points = new PointF[D_TIN.HullPoint.Count];
            for (int i = 0; i < D_TIN.HullPoint.Count; i++)
            {
                g.DrawLine(Pens.Red, D_TIN.DS.Vertex[D_TIN.HullPoint[i]].x * _scale + WellX, D_TIN.DS.Vertex[D_TIN.HullPoint[i]].y * _scale + WellY,
                 D_TIN.DS.Vertex[D_TIN.HullPoint[(i + 1) % D_TIN.HullPoint.Count]].x * _scale + WellX,
                 D_TIN.DS.Vertex[D_TIN.HullPoint[(i + 1) % D_TIN.HullPoint.Count]].y * _scale + WellY);
                //Points[i] = new PointF(D_TIN.DS.Vertex[D_TIN.HullPoint[i]].x * _scale + WellX, D_TIN.DS.Vertex[D_TIN.HullPoint[i]].y * _scale + WellY);
            }
            //g.FillPolygon(new SolidBrush(Color.Red), Points);
        }

        #endregion

        #endregion

        #region  等值线及色带

        //#region 装载等值线

        ///// <summary>
        ///// 装载等值线数据
        ///// </summary>
        //public void SetContour()
        //{
        //    if (_lst != null && _lst.Count != 0)
        //    {
        //        axContourOCX1.FreeData();
        //        //设置算法
        //        axContourOCX1.InitialRandomKrigingOK(-1, 100, 0.2f, 3, -1);
        //        double Max = -99999999;
        //        double Min = 99999999;

        //        #region 判断类别及加点

        //        for (int i = 0; i < _lst.Count; i++)
        //        {

        //            axContourOCX1.AddPointRandom(_lst[i].X, _lst[i].Y, _lst[i].WellData);
        //            if (_lst[i].WellData > Max)
        //            {
        //                Max = _lst[i].WellData;
        //            }
        //            if (_lst[i].WellData < Min)
        //            {
        //                Min = _lst[i].WellData;
        //            }
        //        }

        //        #endregion

        //        //最大最小值赋值
        //        _ValueMin = Min;
        //        _ValueMax = Max;
        //        if (_LineValueList != null && _LineValueList.Count > 0)
        //        {
        //            SetStep(_LineValueList);
        //        }
        //        else
        //        {
        //            SetStep(Min, Max, 10);//设置步长
        //        }
        //        axContourOCX1.CalculateRandom();//生成等值线
        //        int Succeed = 0;
        //        axContourOCX1.ConvertToPolygon(ref Succeed);// 将等值线转化为等值面
        //        if (Succeed <= 0)
        //        {
        //            axContourOCX1.FreeData();
        //            return;
        //        }
        //        //获取等值线
        //        Int32 LineCount = 0;
        //        axContourOCX1.GetLineCount(ref LineCount);
        //        //清除线记录
        //        if (LineCount > 0)
        //        {
        //            _LineList.Clear();
        //        }
        //        for (int j = 0; j < LineCount; j++)
        //        {
        //            int PointCount = 0;
        //            axContourOCX1.GetCtrlPointCount(j, ref PointCount); // 得到等值线的中控制点的个数
        //            PointF[] PointTemp = new PointF[PointCount];
        //            float TempZ = 0;
        //            axContourOCX1.GetLineValue(j, ref TempZ);
        //            for (int k = 0; k < PointCount; k++)
        //            {
        //                double X = 0, Y = 0, Z = 0;
        //                axContourOCX1.GetCtrlPoint(j, k, ref X, ref Y, ref Z);

        //                PointTemp[k].X = Convert.ToSingle(X);
        //                PointTemp[k].Y = Convert.ToSingle(Y);
        //            }
        //            Contour.ContourLine cl = new View.Contour.ContourLine();//等值线类
        //            cl.LineValue = TempZ;
        //            cl.LinePoints = (PointTemp);
        //            _LineList.Add(cl);
        //        }
        //    }
        //}
        ///// <summary>
        ///// 设置步长
        ///// </summary>
        ///// <param name="Min">最小值</param>
        ///// <param name="Max">最大值</param>
        ///// <param name="StepCount">步长个数</param>
        //private void SetStep(double Min, double Max, int StepCount)
        //{
        //    _LineValueList.Clear();
        //    double Step = Math.Abs(Max - Min) / StepCount;
        //    for (int i = 0; i < StepCount; i++)
        //    {
        //        double temp = Min + Step * i;
        //        axContourOCX1.AddCustomedStep(Convert.ToSingle(temp));
        //        _LineValueList.Add(temp);
        //    }
        //}
        ///// <summary>
        ///// 设置步长
        ///// </summary>
        ///// <param name="ValueList">等值线步长</param>
        //private void SetStep(List<double> ValueList)
        //{
        //    for (int i = 0; i < ValueList.Count; i++)
        //    {
        //        axContourOCX1.AddCustomedStep(Convert.ToSingle(ValueList[i]));
        //    }
        //}
        //#endregion

        //#region 画等值线

        ///// <summary>
        ///// 画等值线
        ///// </summary>
        ///// <param name="g">Graphics</param>
        //private void ShowContour(Graphics g)
        //{
        //    for (int i = 0; i < _LineList.Count; i++)
        //    {
        //        PointF[] PointTemp = new PointF[_LineList[i].LinePoints.Length];
        //        _LineList[i].LinePoints.CopyTo(PointTemp, 0);
        //        for (int j = 0; j < PointTemp.Length; j++)
        //        {
        //            PointTemp[j].X = PointTemp[j].X * _scale;
        //            PointTemp[j].Y = PointTemp[j].Y * _scale;
        //        }
        //        Pen p = new Pen(Color.Black, 1);
        //        g.DrawLines(p, PointTemp);
        //    }
        //}

        //#endregion

        //#region 设置色带

        ///// <summary>
        ///// 设置色带
        ///// </summary>
        ///// <param name="g">Graphics</param>
        //public void SetContourBackColor()
        //{

        //    DataTable ValueSortDt = new DataTable();
        //    ValueSortDt.Columns.Add("索引", typeof(Int32));
        //    ValueSortDt.Columns.Add("值", typeof(float));
        //    ValueSortDt.Columns.Add("面积", typeof(double));
        //    ValueSortDt.Columns.Add("百分比", typeof(double));
        //    int PolygonCount = 0;//等值面个数
        //    axContourOCX1.GetPolygonCount(ref PolygonCount); //得到等值面个数
        //    for (int i = 0; i < PolygonCount; i++)
        //    {
        //        int PointCount = 0;
        //        float MinValue = 0, MaxValue = 0, AreaWithoutHoles = 0, AreaWithHoles = 0;
        //        //得到一个等值面的多边形的控制点数
        //        axContourOCX1.GetPolygonPointCountValueArea(i, ref PointCount, ref MinValue, ref MaxValue, ref AreaWithoutHoles, ref AreaWithHoles);

        //        if (MinValue < -900000000)
        //            MinValue = Convert.ToSingle(ValueMin);
        //        if (MaxValue > 900000000)
        //            MaxValue = Convert.ToSingle(ValueMax);
        //        //获取点集合
        //        double[] PointX = new double[PointCount];//点X坐标集合
        //        double[] PointY = new double[PointCount];//点Y坐标集合
        //        for (int j = 0; j < PointCount; j++)
        //        {
        //            float X = 0, Y = 0;
        //            axContourOCX1.GetPolygonPointXY(i, j, ref X, ref Y);
        //            PointX[j] = X;
        //            PointY[j] = Y;
        //        }
        //        DataRow dr = ValueSortDt.NewRow();
        //        dr[0] = i;
        //        dr[1] = MinValue;//值
        //        dr[3] = (MinValue - ValueMin) / (ValueMax - ValueMin);//百分比
        //        dr[2] = AoArea(PointCount, PointX, PointY);//计算面积
        //        ValueSortDt.Rows.Add(dr);
        //    }
        //    //排序，避免小圈被遮挡住的情况
        //    ValueSortDt.DefaultView.Sort = "面积 desc";
        //    ValueSortDt = ValueSortDt.DefaultView.ToTable();
        //    //清除色带数据
        //    if (ValueSortDt.Rows.Count > 0)
        //    {
        //        _LineColorList.Clear();
        //    }
        //    for (int i = 0; i < ValueSortDt.Rows.Count; i++)
        //    {
        //        int PointCount = 0;
        //        float MinValue = 0, MaxValue = 0, AreaWithoutHoles = 0, AreaWithHoles = 0;
        //        //得到一个等值面的多边形的控制点数
        //        axContourOCX1.GetPolygonPointCountValueArea(Convert.ToInt32(ValueSortDt.Rows[i][0]), ref PointCount, ref MinValue, ref MaxValue, ref AreaWithoutHoles, ref AreaWithHoles);

        //        PointF[] PointTemp = new PointF[PointCount];//点的集合
        //        for (int j = 0; j < PointCount; j++)
        //        {
        //            float X = 0, Y = 0;
        //            axContourOCX1.GetPolygonPointXY(Convert.ToInt32(ValueSortDt.Rows[i][0]), j, ref X, ref Y);
        //            PointTemp[j].X = Convert.ToSingle(X);
        //            PointTemp[j].Y = Convert.ToSingle(Y);
        //        }
        //        Contour.ContourColorLine ccl = new View.Contour.ContourColorLine();
        //        ccl.LinePoints = PointTemp;
        //        ccl.LineValue = Convert.ToDouble(ValueSortDt.Rows[i][1]);
        //        ccl.LinePercentageValue = Convert.ToDouble(ValueSortDt.Rows[i][3]);
        //        _LineColorList.Add(ccl);
        //    }
        //}
        ///// <summary>
        ///// 多边形面积计算(不论凸凹)
        ///// </summary>
        ///// <param name="count">坐标点数</param>
        ///// <param name="X">坐标的横坐标集合</param>
        ///// <param name="Y">坐标的纵坐标集合</param>
        ///// <returns></returns>
        //private double AoArea(int count, double[] X, double[] Y)
        //{
        //    double area;
        //    if (count < 3)
        //        return 0;
        //    area = Y[0] * (X[count - 1] - X[1]);
        //    for (int i = 1; i < count; i++)
        //        area += Y[i] * (X[(i - 1)] - X[(i + 1) % count]);
        //    return Math.Abs(area / 2);
        //}

        //#endregion

        //#region 画色带

        ///// <summary>
        ///// 画色带
        ///// </summary>
        ///// <param name="g">Graphics</param>
        //private void ShowContourBackColor(Graphics g)
        //{
        //    if (_ColorDt.Rows.Count <= 0)
        //    {
        //        for (int i = 0; i < _LineColorList.Count; i++)
        //        {
        //            PointF[] PointTemp = new PointF[_LineColorList[i].LinePoints.Length];
        //            _LineColorList[i].LinePoints.CopyTo(PointTemp, 0);
        //            for (int j = 0; j < PointTemp.Length; j++)
        //            {
        //                PointTemp[j].X = PointTemp[j].X * _scale;
        //                PointTemp[j].Y = PointTemp[j].Y * _scale;
        //            }
        //            SolidBrush br = new SolidBrush(Color.FromArgb(Convert.ToInt32(_LineColorList[i].LinePercentageValue * 250), Convert.ToInt32(_LineColorList[i].LinePercentageValue * 250), Convert.ToInt32(_LineColorList[i].LinePercentageValue * 250)));
        //            g.FillPolygon(br, PointTemp);
        //        }
        //    }
        //    else
        //    {
        //        for (int i = 0; i < _LineColorList.Count; i++)
        //        {
        //            PointF[] PointTemp = new PointF[_LineColorList[i].LinePoints.Length];
        //            _LineColorList[i].LinePoints.CopyTo(PointTemp, 0);
        //            for (int j = 0; j < PointTemp.Length; j++)
        //            {
        //                PointTemp[j].X = PointTemp[j].X * _scale;
        //                PointTemp[j].Y = PointTemp[j].Y * _scale;
        //            }
        //            SolidBrush br = new SolidBrush(GetColor(_LineColorList[i].LineValue));
        //            g.FillPolygon(br, PointTemp);

        //        }
        //    }
        //}

        ///// <summary>
        ///// 获取颜色
        ///// </summary>
        ///// <param name="Value">值</param>
        ///// <returns>颜色</returns>
        //private Color GetColor(double Value)
        //{
        //    int R = 0, G = 0, B = 0;
        //    double RStep, GStep, BStep;
        //    int temp = 0;
        //    //double Step = _ValueMax - _ValueMin / 100;
        //    while (temp < _ColorDt.Rows.Count - 1)
        //    {
        //        if (Value >= Convert.ToDouble(_ColorDt.Rows[temp][1]) && Value <= Convert.ToDouble(_ColorDt.Rows[temp + 1][1]))
        //        {
        //            double t = Convert.ToDouble(_ColorDt.Rows[temp + 1][1]) - Convert.ToDouble(_ColorDt.Rows[temp][1]);
        //            if (t == 0) { t = 1; }
        //            RStep = (((Color)(_ColorDt.Rows[temp + 1][0])).R - ((Color)(_ColorDt.Rows[temp][0])).R) / t ;
        //            GStep = (((Color)(_ColorDt.Rows[temp + 1][0])).G - ((Color)(_ColorDt.Rows[temp][0])).G) / t ;
        //            BStep = (((Color)(_ColorDt.Rows[temp + 1][0])).B - ((Color)(_ColorDt.Rows[temp][0])).B) / t ;

        //            R = Convert.ToInt32((Value - Convert.ToDouble(_ColorDt.Rows[temp][1])) * RStep + ((Color)(_ColorDt.Rows[temp][0])).R);
        //            G = Convert.ToInt32((Value - Convert.ToDouble(_ColorDt.Rows[temp][1])) * GStep + ((Color)(_ColorDt.Rows[temp][0])).G);
        //            B = Convert.ToInt32((Value - Convert.ToDouble(_ColorDt.Rows[temp][1])) * BStep + ((Color)(_ColorDt.Rows[temp][0])).B);
        //            if (R < 0) { R = 0; }
        //            if (G < 0) { G = 0; }
        //            if (B < 0) { B = 0; }
        //            if (R > 255) { R = 255; }
        //            if (G > 255) { G = 255; }
        //            if (B > 255) { B = 255; }
        //            break;
        //        }
        //        temp++;
        //    }
        //    return Color.FromArgb(R, G, B);
        //}

        //#endregion

        #endregion

        #region 网格

        /// <summary>
        /// 画网格
        /// </summary>
        /// <param name="g">Graphics 对象</param>
        /// <param name="IsSavePacture">是否保存图片</param>
        private void ShowGridCell(Graphics g, bool IsSavePacture)
        {
            if (IsSavePacture)
            {
                for (int i = 0; i < _GridCellList.Count; i++)
                {
                    _GridCellList[i].Scale = _scale;//比例
                    _GridCellList[i].IsVisible = true;
                    _GridCellList[i].IsSelected = false;
                    try
                    {
                        _GridCellList[i].Draw(g);
                    }
                    catch
                    { }
                }
            }
            else
            {
                for (int i = 0; i < _GridCellList.Count; i++)
                {
                    ////是否在屏幕内
                    if ((_GridCellList[i].X1 * _scale + x >= 0 && _GridCellList[i].X1 * _scale + x <= this.Width) && (_GridCellList[i].Y1 * _scale + y >= 0 && _GridCellList[i].Y1 * _scale + y <= this.Height)
                        || (_GridCellList[i].X2 * _scale + x >= 0 && _GridCellList[i].X2 * _scale + x <= this.Width) && (_GridCellList[i].Y2 * _scale + y >= 0 && _GridCellList[i].Y2 * _scale + y <= this.Height)
                        || (_GridCellList[i].X3 * _scale + x >= 0 && _GridCellList[i].X3 * _scale + x <= this.Width) && (_GridCellList[i].Y3 * _scale + y >= 0 && _GridCellList[i].Y3 * _scale + y <= this.Height)
                        || (_GridCellList[i].X4 * _scale + x >= 0 && _GridCellList[i].X4 * _scale + x <= this.Width) && (_GridCellList[i].Y4 * _scale + y >= 0 && _GridCellList[i].Y4 * _scale + y <= this.Height)
                        )
                    {
                        _GridCellList[i].IsVisible = true;
                    }
                    else
                    {
                        _GridCellList[i].IsVisible = false;
                    }
                    try
                    {
                        _GridCellList[i].Scale = _scale;
                        _GridCellList[i].Draw(g);
                    }
                    catch
                    { }
                }
            }
            //画网格加密名称
            for (int i = 0; i < _DGridCellNameList.Count; i++)
            {
                _DGridCellNameList[i].Scale = _scale;//比例
                _DGridCellNameList[i].IsSelected = false;
                try
                {
                    _DGridCellNameList[i].Draw(g);
                }
                catch
                { }
            }
        }

        #endregion

        #region 计算面积

        /// <summary>
        /// 计算多边形面积
        /// </summary>
        public void CalculationArea()
        {
            _AreaPointList.Clear();
            for (int i = 0; i < _lst.Count; i++)
            {
                if (_lst[i].IsSelected)
                {
                    AreaPoint ap = new AreaPoint();
                    ap.Index = i;
                    _AreaPointList.Add(ap);
                }
            }
            _D_TIN.CreateVoronoi(ref _AreaPointList);
            for (int i = 0; i < _AreaPointList.Count; i++)
            {
                if (_AreaPointList[i].PointList.Count > 0)
                {
                    ConvexHull ch = new ConvexHull();
                    ch.Points = _AreaPointList[i].PointList;
                    ch.GetConvexHull();
                    _AreaPointList[i].PointList = ch.HullPoints;
                }
            }
            ////计算面积
            //for (int i = 0; i < _AreaPointList.Count; i++)
            //{
            //    double[] AreaX = new double[_AreaPointList[i].PointList.Count];
            //    double[] AreaY = new double[_AreaPointList[i].PointList.Count];
            //    for (int j = 0; j < _AreaPointList[i].PointList.Count; j++)
            //    {
            //        AreaX[j] = _AreaPointList[i].PointList[j].X;
            //        AreaY[j] = _AreaPointList[i].PointList[j].Y;
            //    }
            //    _AreaPointList[i].Area = AoArea(_AreaPointList[i].PointList.Count, AreaX, AreaY);
            //    _lst[_AreaPointList[i].Index].WellName = _lst[_AreaPointList[i].Index].WellName + " " + _AreaPointList[i].Area.ToString();
            //}
        }

        #endregion
        
        #region 保存图片

        /// <summary>
        /// 保存图片
        /// </summary>
        /// <param name="FileName">图片保存路径</param>
        /// <param name="PictureType">图片类型</param>
        public void SavePicture(string FileName,System.Drawing.Imaging.ImageFormat PictureType)
        {
            
            Bitmap SaveBitMap = new Bitmap(Convert.ToInt32(_DrawWidth), Convert.ToInt32(_DrawHeight));
            Graphics g = Graphics.FromImage(SaveBitMap);
            float TempS = _scale;
            _scale = 1;
            g.FillRectangle(Brushes.White, 0, 0, (_DrawWidth) * _scale, (_DrawHeight) * _scale);//背景色
            g.SmoothingMode = SmoothingMode.AntiAlias;//消除锯齿

            #region 画网格

            if (_IsGridCelVisible)
            {
                ShowGridCell(g, true);
            }

            #endregion

            #region 等值线

            ////色带
            //if (_IsContourBackColorVisible)
            //{
            //    ShowContourBackColor(g);
            //}
            ////等值线
            //if (_IsContourVisible)
            //{
            //    ShowContour(g);
            //}

            #endregion

            #region 画Voronoi图

            if (_IsTriangleVisible)
            {
                ShowTriangle(g);//三角网络
            }
            if (_IsEdgeVisible)
            {
                ShowConvex(g);//三角网络边缘
            }
            if (_IsVoronoiVisible)
            {
                ShowVoronoi(g);//画Voronoi图
            }

            #endregion

            #region 井位图

            if (_IsWellVisible)
            {
                ShowWell(g,true);//画井位图
            }

            #endregion

            //画外面的那个黑框
            Pen p = new Pen(Color.Black, 10);
            g.DrawRectangle(p, 0, 0, (_DrawWidth) * _scale, (_DrawHeight) * _scale);
            _scale = TempS;
            
            SaveBitMap.Save(FileName, PictureType);
        }

        #endregion

        #region 双击

        private void Draw_MouseDoubleClick(object sender, MouseEventArgs e)
        {

        }

        #endregion
    }
}
