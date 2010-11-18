using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace View.D3D
{
    public partial class Display : UserControl
    {

        #region 构造方法

        /// <summary>
        /// 构造方法
        /// </summary>
        /// <param name="X">X点集合</param>
        /// <param name="Y">Y点集合</param>
        /// <param name="Z">Z点集合</param>
        /// <param name="NX">NX</param>
        /// <param name="NY">NY</param>
        /// <param name="NZ">NZ</param>
        /// <param name="dt">井号</param>
        public Display(float[] X,float[] Y,float[] Z,int NX,int NY,int NZ,DataTable dt)
        //public Display(float[] X,float[] Y,float[] Z,int NX,int NY,int NZ)
        {
            InitializeComponent();
            this.MouseWheel += new MouseEventHandler(Form1_MouseWheel);//注册鼠标滚轮事件
            XCount = NX;
            YCount = NY;
            ZCount = NZ;
            PointX = X;
            PointY = Y;
            PointZ = Z;
            WindowWidth = this.Width;
            WindowHeight = this.Height;
            WellDt = dt;
        }
        
        #endregion

        #region 变量声明

        Device device = null;//绘图设备

        VertexBuffer[] DisplayVertexBuffer = null;//面缓冲

        VertexBuffer[] LineVertexBuffer = null;//边缘缓冲

        IndexBuffer indexBuffer = null;//索引

        //Material[] ColorMaterial = null;//环境光

        Matrix m = Matrix.Zero;//旋转矩镇

        float Distance = -10000;//视野距离

        bool _IsSolid = true;//是否填充

        int XCount, YCount, ZCount;//NX,NY,NZ

        bool _IsLine = true;//是否画线

        bool MouseClickFalg = false;//鼠标是否按下

        Point StartPoint;//鼠标按下开始点

        float[] PointX, PointY, PointZ;//点数据

        float _XCoefficient = 1, _YCoefficient = 1, _ZCoefficient = 1;

        int WindowWidth, WindowHeight;//宽、高

        List<int> _Layer=new List<int>();//显示的层

        DataTable _ColorDt = null;//颜色DataTable

        double[,] _PropertyData;//属性
        double _PropertyMax;//属性最大值
        double _PropertyMin;//属性最小值

        float _Max;
        float _Min;

        DataTable WellDt;//井号

        Mesh[] WellMesh;//井号mesh

        VertexBuffer WellVertexBuffer;//井轨迹

        Material[] WellTypeMaterial;//井别采制

        bool _IsWell=false;//是否画井


        ///////////////////////////////////////////////

        //private List<double> _LineValueList = new List<double>();//等值线分隔集合
        //private List<Contour.ContourLine> _LineList = new List<View.Contour.ContourLine>();//等值线集合
        //VertexBuffer[] ContourLineBuffer;//等值线缓冲

        //////////////////////////////////////////////

        #endregion

        #region 属性

        /// <summary>
        /// 是否填充
        /// </summary>
        public bool IsSolid
        {
            get { return _IsSolid; }
            set { _IsSolid = value; }
        }

        /// <summary>
        /// 是否画线
        /// </summary>
        public bool IsLine
        {
            get { return _IsLine; }
            set { _IsLine = value; }
        }
        /// <summary>
        /// X系数
        /// </summary>
        public float XCoefficient
        {
            get { return _XCoefficient; }
            set { _XCoefficient = value; }
        }
        /// <summary>
        /// Y系数
        /// </summary>
        public float YCoefficient
        {
            get { return _YCoefficient; }
            set { _YCoefficient = value; }
        }
        /// <summary>
        /// Z系数
        /// </summary>
        public float ZCoefficient
        {
            get { return _ZCoefficient; }
            set { _ZCoefficient = value; }
        }
        /// <summary>
        /// 显示的层
        /// </summary>
        public List<int> Layer
        {
            get { return _Layer; }
            set { _Layer = value; }
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
        /// 属性数据
        /// </summary>
        public double[,] PropertyData
        {
            get { return _PropertyData; }
            set { _PropertyData = value; }
        }
        /// <summary>
        /// 属性最大值
        /// </summary>
        public double PropertyMax
        {
            get { return _PropertyMax; }
            set { _PropertyMax = value; }
        }
        /// <summary>
        /// 属性最小值
        /// </summary>
        public double PropertyMin
        {
            get { return _PropertyMin; }
            set { _PropertyMin = value; }
        }
        /// <summary>
        /// 最大值
        /// </summary>
        public float Max
        {
            get { return _Max; }
            set { _Max = value; }
        }
        /// <summary>
        /// 最小值
        /// </summary>
        public float Min
        {
            get { return _Min; }
            set { _Min = value; }
        }

        /// <summary>
        /// 是否画井
        /// </summary>
        public bool IsWell
        {
            get { return _IsWell; }
            set { _IsWell = value; }
        }

        #endregion

        #region 初始化图形

        /// <summary>
        /// 初始化绘制
        /// </summary>
        /// <returns></returns>
        public bool InitializeGraphics()
        {
            try
            {
                Distance = -(_Max-_Min)* 2;
                // Now let's setup our D3D stuff
                PresentParameters presentParams = new PresentParameters();
                presentParams.Windowed = true;
                presentParams.SwapEffect = SwapEffect.Discard;
                presentParams.EnableAutoDepthStencil = true;
                presentParams.AutoDepthStencilFormat = DepthFormat.D24X8;
                presentParams.PresentationInterval = PresentInterval.Immediate;
                
                device = new Device(0, DeviceType.Hardware, this, CreateFlags.SoftwareVertexProcessing, presentParams);
                device.RenderState.ZBufferEnable = true;
                device.RenderState.ZBufferFunction = Compare.LessEqual;
                CreateRectangleData(XCount, YCount, ZCount);
                m = device.Transform.World;//赋值当前坐标给m矩阵
                device.DeviceReset += new System.EventHandler(this.OnResetDevice);
                device.DeviceResizing += new CancelEventHandler(this.CancelResize);
                this.OnResetDevice(device, null);
                return true;
            }
            catch (DirectXException)
            {
                return false;
            }
        }
        /// <summary>
        /// 重置设备
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public void OnResetDevice(object sender, EventArgs e)
        {
            Device dev = (Device)sender;
            dev.RenderState.CullMode = Cull.None;

        }
        /// <summary>
        /// 尺寸大小改变
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CancelResize(object sender, CancelEventArgs e)
        {
            e.Cancel = true;
            
            Render(0, 0);
        }
        #endregion

        #region 绘制图形

        /// <summary>
        /// 绘制图形
        /// </summary>
        /// <param name="x">X轴旋转量</param>
        /// <param name="y">Y轴旋转量</param>
        public void Render(int x, int y)
        {
            if (device == null)
                return;

            device.Clear(ClearFlags.Target | ClearFlags.ZBuffer, System.Drawing.Color.SkyBlue, 1.0f, 0);
            //device.RenderState.Lighting = false;//光照
            device.RenderState.Lighting = true;//光照
            SetupLights();
            SetupCamera(x, y);
            device.BeginScene();

            #region 画模型

            for (int i = 0; i < ZCount; i++)
            //for (int i = 0; i < 20; i++)
            {
                if (_Layer.Contains(i))
                {
                    //轮廓
                    device.RenderState.FillMode = FillMode.Solid;
                    device.Indices = indexBuffer;
                    if (_IsSolid)
                    {
                        device.SetStreamSource(0, DisplayVertexBuffer[i], 0);
                        device.VertexFormat = CustomVertex.PositionNormalColored.Format;
                        device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, 24 * XCount * YCount, 0, 12 * XCount * YCount);
                        //for (int j = 0; j < XCount * YCount; j++)
                        //{
                        //    device.DrawPrimitives(PrimitiveType.TriangleList, j * 36, 12);
                        //}
                    }
                    if (_IsLine)
                    {
                        Material LineMaterial = new Material();
                        LineMaterial.Ambient = Color.White;
                        LineMaterial.Diffuse = Color.White;
                        LineMaterial.Emissive = Color.Black;
                        LineMaterial.Specular = Color.White;
                        device.VertexFormat = CustomVertex.PositionColored.Format;
                        device.SetStreamSource(0, LineVertexBuffer[i], 0);
                        device.DrawPrimitives(PrimitiveType.LineList, 0, 12 * XCount * YCount);
                    }
                }
            }

            #endregion

            #region 画井

            if (_IsWell)
            {
                if (WellMesh != null)
                {
                    for (int i = 0; i < WellMesh.Length; i++)
                    {
                        device.Material = WellTypeMaterial[i];
                        WellMesh[i].DrawSubset(0);
                        device.VertexFormat = CustomVertex.PositionNormal.Format;
                        device.SetStreamSource(0, WellVertexBuffer, 0);
                        device.DrawPrimitives(PrimitiveType.LineList, i * 2, 1);
                    }
                }
                //if (ContourLineBuffer != null)
                //{
                //    for (int i = 0; i < ContourLineBuffer.Length; i++)
                //    {
                //        device.VertexFormat = CustomVertex.PositionNormal.Format;
                //        device.SetStreamSource(0, ContourLineBuffer[i], 0);
                //        device.DrawPrimitives(PrimitiveType.LineStrip, 0, _LineList[i].LinePoints.Length - 1);
                //    }
                //}
            }
            #endregion

            device.EndScene();
            try
            {
                device.Present();
            }
            catch
            {
                InitializeGraphics();
            }
        }

        /// <summary>
        /// 设置光源
        /// </summary>
        private void SetupLights()
        {
            ////点光源
            //device.Lights[0].Type = LightType.Point;
            ////device.Lights[0].Type = LightType.Spot;
            //device.Lights[0].Position = new Vector3(20000, -20000, 20000);
            ////device.Lights[0].Direction = new Vector3(0.0f, -1.0f, 1.0f);
            //device.Lights[0].Range = 100000.0f;//照耀距离
            //device.Lights[0].Attenuation1 = 0.05f;//强度
            ////device.Lights[1].OuterConeAngle = 45f;
            //device.Lights[0].Enabled = true; //打开灯光

            ////点光源
            //device.Lights[0].Type = LightType.Point;
            ////device.Lights[0].Type = LightType.Spot;
            //device.Lights[0].Position = new Vector3((_Max - _Min), -(_Max - _Min) * 2, _Max - _Min);
            ////device.Lights[0].Direction = new Vector3(0.0f, -1.0f, 1.0f);
            //device.Lights[0].Range = (_Max - _Min) * 4;//照耀距离
            //device.Lights[0].Attenuation1 = 0.1f;//强度
            ////device.Lights[1].OuterConeAngle = 45f;
            //device.Lights[0].Enabled = true; //打开灯光

            //平行光
            device.Lights[0].Type = LightType.Directional;
            device.Lights[0].Direction = new Vector3(1, -1, 1);
            device.Lights[0].Enabled = true;


        }

        //重载绘制函数
        protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
        {
            this.Render(0, 0); // Render on painting
        }
        /// <summary>
        /// 设置摄像机的位置
        /// </summary>
        private void SetupCamera(int x, int y)
        {
            if (MouseClickFalg)
            {
                Matrix MTemp = Matrix.MultiplyTranspose(m, Matrix.RotationX(-y * (float)Math.PI / 460));
                
                MTemp = Matrix.MultiplyTranspose(MTemp, Matrix.RotationY(x * (float)Math.PI / 460));
                device.Transform.World = MTemp;
            }
            //设置摄像机的位置，它在z轴上Distance处，看着原点，y轴为正方向
            device.Transform.View = Matrix.LookAtLH(new Vector3(0.0f, 0.0f, Distance), new Vector3(0.0f, 0.0f, 0.0f), new Vector3(0.0f, 1.0f, 0.0f));

            //设置摄像机的视界，角度为度，看的最近为,看的最远处为.不再这个视界中的影像都不会被显示
            device.Transform.Projection = Matrix.PerspectiveFovLH((float)Math.PI / 4, WindowWidth / WindowHeight, (float)(Math.Abs(Distance) / 500), Math.Abs(Distance) + 50000f);
        }
        #endregion

        #region 鼠标事件

        //鼠标滚轮事件
        private void Form1_MouseWheel(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.Delta > 0)
                {
                    if (Distance < -5)
                    {
                        Distance = Distance + 500;
                    }

                    //放大
                }
                else
                {
                    Distance = Distance - 500;
                    //缩小
                }
                Render(0, 0);
            }
            catch
            {

            }
        }

        //鼠标按下
        private void Display_MouseDown(object sender, MouseEventArgs e)
        {
            MouseClickFalg = true;
            StartPoint.X = e.X;
            StartPoint.Y = e.Y;
        }
        //鼠标移动
        private void Display_MouseMove(object sender, MouseEventArgs e)
        {
            if (MouseClickFalg)
            {
                Render(e.X - StartPoint.X, e.Y - StartPoint.Y);
            }
        }
        //鼠标抬起
        private void Display_MouseUp(object sender, MouseEventArgs e)
        {
            MouseClickFalg = false;
            m = device.Transform.World;
        }
        #endregion

        #region 生成立方体顶点数据

        /// <summary>
        /// 生成立方体顶点数据
        /// </summary>
        /// <param name="XCount">立方体X方向个数</param>
        /// <param name="YCount">立方体Y方向个数</param>
        /// <param name="ZCount">立方体Z方向个数</param>
        private void CreateRectangleData(int XCount, int YCount, int ZCount)
        {
            if (DisplayVertexBuffer != null)
            {
                for (int i = 0; i < DisplayVertexBuffer.Length; i++)
                {
                    DisplayVertexBuffer[i].Dispose();
                }
            }
            if (LineVertexBuffer != null)
            {
                for (int i = 0; i < LineVertexBuffer.Length; i++)
                {
                    LineVertexBuffer[i].Dispose();
                }
            }
            if (indexBuffer != null)
            {
                indexBuffer.Dispose();
            }

            #region 索引

            indexBuffer = new IndexBuffer(typeof(int), 36 * XCount * YCount, device, Usage.WriteOnly, Pool.Default);
            int[] IndexData = new int[36 * XCount * YCount];
            for (int t36 = 0; t36 < XCount * YCount; t36++)
            {
                //顶面0-3
                IndexData[t36 * 36 + 0] = t36 * 24 + 0;
                IndexData[t36 * 36 + 1] = t36 * 24 + 1;
                IndexData[t36 * 36 + 2] = t36 * 24 + 2;
                IndexData[t36 * 36 + 3] = t36 * 24 + 3;
                IndexData[t36 * 36 + 4] = t36 * 24 + 0;
                IndexData[t36 * 36 + 5] = t36 * 24 + 2;

                //左面4-7
                IndexData[t36 * 36 + 6] = t36 * 24 + 4;
                IndexData[t36 * 36 + 7] = t36 * 24 + 5;
                IndexData[t36 * 36 + 8] = t36 * 24 + 6;
                IndexData[t36 * 36 + 9] = t36 * 24 + 7;
                IndexData[t36 * 36 + 10] = t36 * 24 + 4;
                IndexData[t36 * 36 + 11] = t36 * 24 + 6;

                //右面8-11
                IndexData[t36 * 36 + 12] = t36 * 24 + 8;
                IndexData[t36 * 36 + 13] = t36 * 24 + 9;
                IndexData[t36 * 36 + 14] = t36 * 24 + 10;
                IndexData[t36 * 36 + 15] = t36 * 24 + 11;
                IndexData[t36 * 36 + 16] = t36 * 24 + 8;
                IndexData[t36 * 36 + 17] = t36 * 24 + 10;

                //前面12-15
                IndexData[t36 * 36 + 18] = t36 * 24 + 12;
                IndexData[t36 * 36 + 19] = t36 * 24 + 13;
                IndexData[t36 * 36 + 20] = t36 * 24 + 14;
                IndexData[t36 * 36 + 21] = t36 * 24 + 15;
                IndexData[t36 * 36 + 22] = t36 * 24 + 12;
                IndexData[t36 * 36 + 23] = t36 * 24 + 14;

                //后面16-19
                IndexData[t36 * 36 + 24] = t36 * 24 + 16;
                IndexData[t36 * 36 + 25] = t36 * 24 + 17;
                IndexData[t36 * 36 + 26] = t36 * 24 + 18;
                IndexData[t36 * 36 + 27] = t36 * 24 + 19;
                IndexData[t36 * 36 + 28] = t36 * 24 + 16;
                IndexData[t36 * 36 + 29] = t36 * 24 + 18;

                //底面20-23
                IndexData[t36 * 36 + 30] = t36 * 24 + 20;
                IndexData[t36 * 36 + 31] = t36 * 24 + 21;
                IndexData[t36 * 36 + 32] = t36 * 24 + 22;
                IndexData[t36 * 36 + 33] = t36 * 24 + 23;
                IndexData[t36 * 36 + 34] = t36 * 24 + 20;
                IndexData[t36 * 36 + 35] = t36 * 24 + 22;
            }
            indexBuffer.SetData(IndexData, 0, LockFlags.None);
            GC.Collect();
            #endregion

            //ColorMaterial = null;
            //ColorMaterial = new Material[XCount * YCount * ZCount];
            DisplayVertexBuffer = new VertexBuffer[ZCount];
            LineVertexBuffer = new VertexBuffer[ZCount];
            for (int i = 0; i < ZCount; i++)
            //for (int i = 0; i < 20; i++)
            {
                //初始化要贴图的矩形框顶点数据
                DisplayVertexBuffer[i] = new VertexBuffer(typeof(CustomVertex.PositionNormalColored), XCount * YCount * 24, device, 0, CustomVertex.PositionNormalColored.Format, Pool.Default);
                LineVertexBuffer[i] = new VertexBuffer(typeof(CustomVertex.PositionColored), XCount * YCount * 24, device, 0, CustomVertex.PositionColored.Format, Pool.Default);
                //构造个点，以构成贴图的矩形表面
                CustomVertex.PositionNormalColored[] verts = (CustomVertex.PositionNormalColored[])DisplayVertexBuffer[i].Lock(0, 0);
                CustomVertex.PositionColored[] v = (CustomVertex.PositionColored[])LineVertexBuffer[i].Lock(0, 0);
                for (int t36 = 0; t36 < XCount * YCount; t36++)
                {
                    #region 顶点赋值

                    //顶面4个点
                    Vector3 v1 = new Vector3(PointX[(XCount * YCount * i + t36) * 8] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8]) * _YCoefficient;
                    Vector3 v2 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 1] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 1] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 1]) * _YCoefficient;
                    Vector3 v3 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 2] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 2] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 2]) * _YCoefficient;
                    Vector3 v4 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 3] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 3] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 3]) * _YCoefficient;
                    //底面4个点

                    Vector3 v5 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 4] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 4] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 4]) * _YCoefficient;
                    Vector3 v6 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 5] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 5] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 5]) * _YCoefficient;
                    Vector3 v7 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 6] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 6] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 6]) * _YCoefficient;
                    Vector3 v8 = new Vector3(PointX[(XCount * YCount * i + t36) * 8 + 7] * _XCoefficient, PointZ[(XCount * YCount * i + t36) * 8 + 7] * _ZCoefficient, PointY[(XCount * YCount * i + t36) * 8 + 7]) * _YCoefficient;

                    #region 无索引版

                    ////顶面
                    //verts[t36 * 36 + 0].Position = v1;
                    //verts[t36 * 36 + 0].Normal = Vector3.Cross((v2 - v1), (v3 - v1));
                    //verts[t36 * 36 + 1].Position = v2;
                    //verts[t36 * 36 + 1].Normal = Vector3.Cross((v3 - v2), (v1 - v2));
                    //verts[t36 * 36 + 2].Position = v3;
                    //verts[t36 * 36 + 2].Normal = Vector3.Cross((v1 - v3), (v2 - v3));

                    //verts[t36 * 36 + 3].Position = v4;
                    //verts[t36 * 36 + 3].Normal = Vector3.Cross((v1 - v4), (v3 - v4));
                    //verts[t36 * 36 + 4].Position = v1;
                    //verts[t36 * 36 + 4].Normal = Vector3.Cross((v3 - v1), (v4 - v1));
                    //verts[t36 * 36 + 5].Position = v3;
                    //verts[t36 * 36 + 5].Normal = Vector3.Cross((v4 - v3), (v1 - v3));



                    ////右侧面
                    //verts[t36 * 36 + 6].Position = v4;
                    //verts[t36 * 36 + 6].Normal = Vector3.Cross((v3 - v4), (v7 - v4));
                    //verts[t36 * 36 + 7].Position = v3;
                    //verts[t36 * 36 + 7].Normal = Vector3.Cross(v7 - v3, v4 - v3);
                    //verts[t36 * 36 + 8].Position = v7;
                    //verts[t36 * 36 + 8].Normal = Vector3.Cross(v4 - v7, v3 - v7);

                    //verts[t36 * 36 + 9].Position = v8;
                    //verts[t36 * 36 + 9].Normal = Vector3.Cross(v4 - v8, v7 - v8);
                    //verts[t36 * 36 + 10].Position = v4;
                    //verts[t36 * 36 + 10].Normal = Vector3.Cross(v7 - v4, v8 - v4);
                    //verts[t36 * 36 + 11].Position = v7;
                    //verts[t36 * 36 + 11].Normal = Vector3.Cross(v8 - v7, v4 - v7);

                    ////左侧面
                    //verts[t36 * 36 + 12].Position = v1;
                    //verts[t36 * 36 + 12].Normal = Vector3.Cross(v6 - v1, v2 - v1);
                    //verts[t36 * 36 + 13].Position = v2;
                    //verts[t36 * 36 + 13].Normal = Vector3.Cross(v1 - v2, v6 - v2);
                    //verts[t36 * 36 + 14].Position = v6;
                    //verts[t36 * 36 + 14].Normal = Vector3.Cross(v2 - v6, v1 - v6);

                    //verts[t36 * 36 + 15].Position = v5;
                    //verts[t36 * 36 + 15].Normal = Vector3.Cross(v6 - v5, v1 - v5);
                    //verts[t36 * 36 + 16].Position = v1;
                    //verts[t36 * 36 + 16].Normal = Vector3.Cross(v5 - v1, v6 - v1);
                    //verts[t36 * 36 + 17].Position = v6;
                    //verts[t36 * 36 + 17].Normal = Vector3.Cross(v1 - v6, v5 - v6);

                    ////前面
                    //verts[t36 * 36 + 18].Position = v5;
                    //verts[t36 * 36 + 18].Normal = Vector3.Cross(v1 - v5, v4 - v5);
                    //verts[t36 * 36 + 19].Position = v1;
                    //verts[t36 * 36 + 19].Normal = Vector3.Cross(v4 - v1, v5 - v1);
                    //verts[t36 * 36 + 20].Position = v4;
                    //verts[t36 * 36 + 20].Normal = Vector3.Cross(v5 - v4, v1 - v4);

                    //verts[t36 * 36 + 21].Position = v8;
                    //verts[t36 * 36 + 21].Normal = Vector3.Cross(v5 - v8, v4 - v8);
                    //verts[t36 * 36 + 22].Position = v5;
                    //verts[t36 * 36 + 22].Normal = Vector3.Cross(v4 - v5, v8 - v5);
                    //verts[t36 * 36 + 23].Position = v4;
                    //verts[t36 * 36 + 23].Normal = Vector3.Cross(v8 - v4, v5 - v4);

                    ////后面
                    //verts[t36 * 36 + 24].Position = v6;
                    //verts[t36 * 36 + 24].Normal = Vector3.Cross(v3 - v6, v2 - v6);
                    //verts[t36 * 36 + 25].Position = v2;
                    //verts[t36 * 36 + 25].Normal = Vector3.Cross(v6 - v2, v3 - v2);
                    //verts[t36 * 36 + 26].Position = v3;
                    //verts[t36 * 36 + 26].Normal = Vector3.Cross(v2 - v3, v6 - v3);

                    //verts[t36 * 36 + 27].Position = v7;
                    //verts[t36 * 36 + 27].Normal = Vector3.Cross(v3 - v7, v6 - v7);
                    //verts[t36 * 36 + 28].Position = v6;
                    //verts[t36 * 36 + 28].Normal = Vector3.Cross(v7 - v6, v3 - v6);
                    //verts[t36 * 36 + 29].Position = v3;
                    //verts[t36 * 36 + 29].Normal = Vector3.Cross(v6 - v3, v7 - v3);

                    ////底面
                    //verts[t36 * 36 + 30].Position = v5;
                    //verts[t36 * 36 + 30].Normal = Vector3.Cross(v7 - v5, v6 - v5);
                    //verts[t36 * 36 + 31].Position = v6;
                    //verts[t36 * 36 + 31].Normal = Vector3.Cross(v5 - v6, v7 - v6);
                    //verts[t36 * 36 + 32].Position = v7;
                    //verts[t36 * 36 + 32].Normal = Vector3.Cross(v6 - v7, v5 - v7);

                    //verts[t36 * 36 + 33].Position = v8;
                    //verts[t36 * 36 + 33].Normal = Vector3.Cross(v7 - v8, v5 - v8);
                    //verts[t36 * 36 + 34].Position = v5;
                    //verts[t36 * 36 + 34].Normal = Vector3.Cross(v8 - v5, v7 - v5);
                    //verts[t36 * 36 + 35].Position = v7;
                    //verts[t36 * 36 + 35].Normal = Vector3.Cross(v5 - v7, v8 - v7);

                    #endregion

                    #region 有索引版

                    //顶面
                    verts[t36 * 24 + 0].Position = v1;
                    verts[t36 * 24 + 0].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 1].Position = v2;
                    verts[t36 * 24 + 1].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 2].Position = v3;
                    verts[t36 * 24 + 2].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 3].Position = v4;
                    verts[t36 * 24 + 3].Normal = new Vector3(0, 0, 0);
                    //左面
                    verts[t36 * 24 + 4].Position = v2;
                    verts[t36 * 24 + 4].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 5].Position = v1;
                    verts[t36 * 24 + 5].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 6].Position = v5;
                    verts[t36 * 24 + 6].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 7].Position = v6;
                    verts[t36 * 24 + 7].Normal = new Vector3(0, 0, 0);
                    //右面
                    verts[t36 * 24 + 8].Position = v4;
                    verts[t36 * 24 + 8].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 9].Position = v3;
                    verts[t36 * 24 + 9].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 10].Position = v7;
                    verts[t36 * 24 + 10].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 11].Position = v8;
                    verts[t36 * 24 + 11].Normal = new Vector3(0, 0, 0);
                    //前面
                    verts[t36 * 24 + 12].Position = v1;
                    verts[t36 * 24 + 12].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 13].Position = v4;
                    verts[t36 * 24 + 13].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 14].Position = v8;
                    verts[t36 * 24 + 14].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 15].Position = v5;
                    verts[t36 * 24 + 15].Normal = new Vector3(0, 0, 0);
                    //后面
                    verts[t36 * 24 + 16].Position = v2;
                    verts[t36 * 24 + 16].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 17].Position = v6;
                    verts[t36 * 24 + 17].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 18].Position = v7;
                    verts[t36 * 24 + 18].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 19].Position = v3;
                    verts[t36 * 24 + 19].Normal = new Vector3(0, 0, 0);
                    //底面
                    verts[t36 * 24 + 20].Position = v6;
                    verts[t36 * 24 + 20].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 21].Position = v5;
                    verts[t36 * 24 + 21].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 22].Position = v8;
                    verts[t36 * 24 + 22].Normal = new Vector3(0, 0, 0);
                    verts[t36 * 24 + 23].Position = v7;
                    verts[t36 * 24 + 23].Normal = new Vector3(0, 0, 0);

                    #endregion

                    //线
                    v[t36 * 24].Position = v1;
                    v[t36 * 24 + 1].Position = v2;

                    v[t36 * 24 + 2].Position = v2;
                    v[t36 * 24 + 3].Position = v3;

                    v[t36 * 24 + 4].Position = v3;
                    v[t36 * 24 + 5].Position = v4;

                    v[t36 * 24 + 6].Position = v4;
                    v[t36 * 24 + 7].Position = v1;

                    v[t36 * 24 + 8].Position = v5;
                    v[t36 * 24 + 9].Position = v6;

                    v[t36 * 24 + 10].Position = v6;
                    v[t36 * 24 + 11].Position = v7;

                    v[t36 * 24 + 12].Position = v7;
                    v[t36 * 24 + 13].Position = v8;

                    v[t36 * 24 + 14].Position = v8;
                    v[t36 * 24 + 15].Position = v5;

                    v[t36 * 24 + 16].Position = v1;
                    v[t36 * 24 + 17].Position = v5;

                    v[t36 * 24 + 18].Position = v2;
                    v[t36 * 24 + 19].Position = v6;

                    v[t36 * 24 + 20].Position = v3;
                    v[t36 * 24 + 21].Position = v7;

                    v[t36 * 24 + 22].Position = v4;
                    v[t36 * 24 + 23].Position = v8;


                    #endregion

                    #region 颜色

                    //填充颜色
                    if (_ColorDt != null && _ColorDt.Rows.Count >= 2 && _PropertyData != null && _PropertyData.GetLength(0) == ZCount && _PropertyData.GetLength(1) == XCount * YCount)
                    {
                        Color c = GetPropertyColor(i, t36);
                        //ColorMaterial[i * XCount * YCount + t36].Ambient = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Diffuse = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Specular = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Emissive = c;
                        for (int t = 0; t < 24; t++)
                        {   
                            verts[t36 * 24 + t].Color = c.ToArgb();
                        }
                    }
                    else
                    {
                        //ColorMaterial[i * XCount * YCount + t36].Ambient = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Diffuse = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Specular = Color.White;
                        //ColorMaterial[i * XCount * YCount + t36].Emissive = Color.Red;
                        for (int t = 0; t < 24; t++)
                        {
                            verts[t36 * 24 + t].Color = Color.YellowGreen.ToArgb();
                        }
                    }
                    //线颜色
                    for (int t = 0; t < 24; t++)
                    {
                        v[t36 * 24 + t].Color = Color.Black.ToArgb();
                    }

                    #endregion
                }
                //计算法线
                for (int j = 0; j < IndexData.Length / 3; j++)
                {
                    Vector3 firstVec = verts[IndexData[j * 3 + 1]].Position - verts[IndexData[j * 3]].Position;
                    Vector3 secondVec = verts[IndexData[j * 3 + 2]].Position - verts[IndexData[j * 3]].Position;
                    Vector3 normal = Vector3.Cross(secondVec, firstVec);
                    normal.Normalize();

                    verts[IndexData[j * 3]].Normal = Vector3.Normalize(verts[IndexData[j * 3]].Normal + normal);
                    verts[IndexData[j * 3 + 1]].Normal = Vector3.Normalize(verts[IndexData[j * 3 + 1]].Normal + normal);
                    verts[IndexData[j * 3 + 2]].Normal = Vector3.Normalize(verts[IndexData[j * 3 + 2]].Normal + normal);
                }
                DisplayVertexBuffer[i].Unlock();
                LineVertexBuffer[i].Unlock();
                GC.Collect();
            }
            GC.Collect();
        }
        
        #endregion

        #region 获取颜色

        /// <summary>
        /// 获取属性颜色
        /// </summary>
        /// <param name="z">纵向第几个</param>
        /// <param name="x">横向第几个</param>
        /// <returns></returns>
        private Color GetPropertyColor(int z,int x)
        {
            double RStep, GStep, BStep;
            int R, G, B;
            double t = (_PropertyData[z, x] - _PropertyMin) / (_PropertyMax - _PropertyMin) * 100;

            for (int i = 0; i < _ColorDt.Rows.Count - 1; i++)
            {
                if (t >= Convert.ToDouble(_ColorDt.Rows[i][1]) && t <= Convert.ToDouble(_ColorDt.Rows[i + 1][1]))
                {
                    double temp = Convert.ToDouble(_ColorDt.Rows[i + 1][1]) - Convert.ToDouble(_ColorDt.Rows[i][1]);
                    {
                        if (temp == 0)
                        {
                            temp = 1;
                        }
                        RStep = Convert.ToDouble(((Color)_ColorDt.Rows[i + 1][0]).R - ((Color)_ColorDt.Rows[i][0]).R) / temp;
                        GStep = Convert.ToDouble(((Color)_ColorDt.Rows[i + 1][0]).G - ((Color)_ColorDt.Rows[i][0]).G) / temp;
                        BStep = Convert.ToDouble(((Color)_ColorDt.Rows[i + 1][0]).B - ((Color)_ColorDt.Rows[i][0]).B) / temp;

                        R = Convert.ToInt32((t - Convert.ToDouble(_ColorDt.Rows[i][1]) - 1) * RStep + ((Color)_ColorDt.Rows[i][0]).R);
                        G = Convert.ToInt32((t - Convert.ToDouble(_ColorDt.Rows[i][1]) - 1) * GStep + ((Color)_ColorDt.Rows[i][0]).G);
                        B = Convert.ToInt32((t - Convert.ToDouble(_ColorDt.Rows[i][1]) - 1) * BStep + ((Color)_ColorDt.Rows[i][0]).B);
                        if (R < 0) { R = 0; }
                        if (G < 0) { G = 0; }
                        if (B < 0) { B = 0; }
                        if (R > 255) { R = 255; }
                        if (G > 255) { G = 255; }
                        if (B > 255) { B = 255; }
                        return Color.FromArgb(R, G, B);
                    }
                }
            }

            return Color.Black;
        }

        #endregion

        #region 改变Z轴比例

        /// <summary>
        /// 改变Z轴比例
        /// </summary>
        /// <param name="ZFalg">True=增加 False=减少</param>
        public void ChangeZ(bool ZFalg)
        {
            if (ZFalg)
            {
                if (DisplayVertexBuffer != null)
                {
                    for (int i = 0; i < DisplayVertexBuffer.Length; i++)
                    {
                        CustomVertex.PositionNormalColored[] DisplayPointData = (CustomVertex.PositionNormalColored[])DisplayVertexBuffer[i].Lock(0, 0);
                        for (int j = 0; j < DisplayPointData.Length; j++)
                        {
                            DisplayPointData[j].Y = DisplayPointData[j].Y * 2;
                        }
                        DisplayVertexBuffer[i].Unlock();
                    }
                }
                if (LineVertexBuffer != null)
                {
                    for (int i = 0; i < LineVertexBuffer.Length; i++)
                    {
                        CustomVertex.PositionColored[] LinePointData = (CustomVertex.PositionColored[])LineVertexBuffer[i].Lock(0, 0);
                        for (int j = 0; j < LinePointData.Length; j++)
                        {
                            LinePointData[j].Y = LinePointData[j].Y * 2;
                        }
                        LineVertexBuffer[i].Unlock();
                    }
                }
                if (WellVertexBuffer != null)
                {
                    CustomVertex.PositionNormal[] TextLineData = (CustomVertex.PositionNormal[])WellVertexBuffer.Lock(0, 0);
                    for (int i = 0; i < TextLineData.Length; i++)
                    {
                        TextLineData[i].Y = TextLineData[i].Y * 2;
                    }
                    WellVertexBuffer.Unlock();
                }
                if (WellMesh != null)
                {
                    for (int i = 0; i < WellMesh.Length; i++)
                    {
                        CustomVertex.PositionNormal[] TextPointData = (CustomVertex.PositionNormal[])WellMesh[i].LockVertexBuffer(typeof(CustomVertex.PositionNormal), 0, WellMesh[i].NumberVertices);
                        for (int j = 0; j < TextPointData.Length; j++)
                        {
                            TextPointData[j].Y = TextPointData[j].Y * 2;
                        }
                        WellMesh[i].UnlockVertexBuffer();
                    }
                }
            }
            else
            {
                if (DisplayVertexBuffer != null)
                {
                    for (int i = 0; i < DisplayVertexBuffer.Length; i++)
                    {
                        CustomVertex.PositionNormalColored[] DisplayPointData = (CustomVertex.PositionNormalColored[])DisplayVertexBuffer[i].Lock(0, 0);
                        for (int j = 0; j < DisplayPointData.Length; j++)
                        {
                            DisplayPointData[j].Y = DisplayPointData[j].Y / 2;
                        }
                        DisplayVertexBuffer[i].Unlock();
                    }
                }
                if (LineVertexBuffer != null)
                {
                    for (int i = 0; i < LineVertexBuffer.Length; i++)
                    {
                        CustomVertex.PositionColored[] LinePointData = (CustomVertex.PositionColored[])LineVertexBuffer[i].Lock(0, 0);
                        for (int j = 0; j < LinePointData.Length; j++)
                        {
                            LinePointData[j].Y = LinePointData[j].Y / 2;
                        }
                        LineVertexBuffer[i].Unlock();
                    }
                }
                if (WellVertexBuffer != null)
                {
                    CustomVertex.PositionNormal[] TextLineData = (CustomVertex.PositionNormal[])WellVertexBuffer.Lock(0, 0);
                    for (int i = 0; i < TextLineData.Length; i++)
                    {
                        TextLineData[i].Y = TextLineData[i].Y / 2;
                    }
                    WellVertexBuffer.Unlock();
                }
                if (WellMesh != null)
                {
                    for (int i = 0; i < WellMesh.Length; i++)
                    {
                        CustomVertex.PositionNormal[] TextPointData = (CustomVertex.PositionNormal[])WellMesh[i].LockVertexBuffer(typeof(CustomVertex.PositionNormal), 0, WellMesh[i].NumberVertices);
                        for (int j = 0; j < TextPointData.Length; j++)
                        {
                            TextPointData[j].Y = TextPointData[j].Y / 2;
                        }
                        WellMesh[i].UnlockVertexBuffer();
                    }
                }
            }
        }

        #endregion

        #region 生成井号顶点数据

        public void CreateWellDataPublic()
        {
            CreateWellData(WellDt);
        }

        private void CreateWellData(DataTable dt)
        {
            if (dt != null && dt.Rows.Count > 0)
            {
                if (WellMesh != null)
                {
                    for (int i = 0; i < WellMesh.Length; i++)
                    {
                        WellMesh[i].Dispose();
                    }
                }
                WellMesh = new Mesh[dt.Rows.Count];
                WellTypeMaterial = new Material[dt.Rows.Count];
                WellVertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionNormal), dt.Rows.Count * 2, device, 0, CustomVertex.PositionNormal.Format, Pool.Default);
                CustomVertex.PositionNormal[] v = (CustomVertex.PositionNormal[])WellVertexBuffer.Lock(0, 0);
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    System.Drawing.Font localFont = new System.Drawing.Font("Arial", 14, FontStyle.Regular);
                    WellMesh[i] = Mesh.TextFromFont(device, localFont, dt.Rows[i]["井号"].ToString(), 1, 0.2f);
                    SetWellMesh(WellMesh[i], Convert.ToSingle(dt.Rows[i]["X"])*_XCoefficient, Convert.ToSingle(dt.Rows[i]["Y2"])*_ZCoefficient, Convert.ToSingle(dt.Rows[i]["Z"])*_YCoefficient);
                    switch (dt.Rows[i]["井别"].ToString().Trim())
                    {
                        case "油井":
                            WellTypeMaterial[i].Ambient = WellTypeMaterial[i].Diffuse = Color.Red;
                            break;
                        case "水井":
                            WellTypeMaterial[i].Ambient = WellTypeMaterial[i].Diffuse = Color.Blue;
                            break;
                        default:
                            WellTypeMaterial[i].Ambient = WellTypeMaterial[i].Diffuse = Color.Black;
                            break;
                    }
                    v[i * 2].Position = new Vector3(Convert.ToSingle(dt.Rows[i]["X"]) * _XCoefficient, Convert.ToSingle(dt.Rows[i]["Y1"]) * _ZCoefficient, Convert.ToSingle(dt.Rows[i]["Z"]) * _YCoefficient);
                    v[i * 2].Normal = new Vector3(Convert.ToSingle(dt.Rows[i]["X"]) * _XCoefficient, Convert.ToSingle(dt.Rows[i]["Y1"]) * _ZCoefficient, Convert.ToSingle(dt.Rows[i]["Z"]) * _YCoefficient);
                    v[i * 2 + 1].Position = new Vector3(Convert.ToSingle(dt.Rows[i]["X"]) * _XCoefficient, Convert.ToSingle(dt.Rows[i]["Y2"]) * _ZCoefficient, Convert.ToSingle(dt.Rows[i]["Z"]) * _YCoefficient);
                    v[i * 2 + 1].Normal = new Vector3(Convert.ToSingle(dt.Rows[i]["X"]) * _XCoefficient, Convert.ToSingle(dt.Rows[i]["Y2"]) * _ZCoefficient, Convert.ToSingle(dt.Rows[i]["Z"]) * _YCoefficient);
                }
                WellVertexBuffer.Unlock();
            }
        }
        private void SetWellMesh(Mesh TextMesh, float x, float y, float z)
        {
            CustomVertex.PositionNormal[] TextPointData = (CustomVertex.PositionNormal[])TextMesh.LockVertexBuffer(typeof(CustomVertex.PositionNormal), 0, TextMesh.NumberVertices);
            for (int i = 0; i < TextPointData.Length; i++)
            {
                TextPointData[i].X = TextPointData[i].X*20 + x;
                TextPointData[i].Y = TextPointData[i].Y*20 + y;
                TextPointData[i].Z = TextPointData[i].Z*20 + z;
                TextPointData[i].Nx = TextPointData[i].Nx*20 + x;
                TextPointData[i].Ny = TextPointData[i].Ny*20 + y;
                TextPointData[i].Nz = TextPointData[i].Nz*20 + z;
            }
            TextMesh.UnlockVertexBuffer();
        }
        #endregion

        #region 装载等值线

        ///// <summary>
        ///// 装载等值线数据
        ///// </summary>
        //public void SetContour()
        //{
        //    float YValue = 0;

        //    if (WellDt != null && WellDt.Rows.Count != 0)
        //    {
        //        axContourOCX1.FreeData();
        //        //设置算法
        //        axContourOCX1.InitialRandomKrigingOK(-1, 100, 0.2f, 3, -1);
        //        double Max = -99999999;
        //        double Min = 99999999;

        //        #region 判断类别及加点

        //        for (int i = 0; i < WellDt.Rows.Count; i++)
        //        {
        //            if (WellDt.Rows[i]["数据"] != DBNull.Value)
        //            {
        //                axContourOCX1.AddPointRandom(Convert.ToDouble(WellDt.Rows[i]["X"]), Convert.ToDouble(WellDt.Rows[i]["Z"]), Convert.ToDouble(WellDt.Rows[i]["数据"]));
        //                YValue = Convert.ToSingle(WellDt.Rows[i]["Y2"]);
        //                if (Convert.ToDouble(WellDt.Rows[i]["数据"]) > Max)
        //                {
        //                    Max = Convert.ToDouble(WellDt.Rows[i]["数据"]);
        //                }
        //                if (Convert.ToDouble(WellDt.Rows[i]["数据"]) < Min)
        //                {
        //                    Min = Convert.ToDouble(WellDt.Rows[i]["数据"]);
        //                }
        //            }
        //        }

        //        #endregion

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
        //        ContourLineBuffer = new VertexBuffer[_LineList.Count];
        //        for (int i = 0; i < _LineList.Count; i++)
        //        {
        //            ContourLineBuffer[i] = new VertexBuffer(typeof(CustomVertex.PositionNormal), _LineList[i].LinePoints.Length, device, 0, CustomVertex.PositionNormal.Format, Pool.Default);
        //            CustomVertex.PositionNormal[] v = (CustomVertex.PositionNormal[])ContourLineBuffer[i].Lock(0, 0);
        //            for (int j = 0; j < _LineList[i].LinePoints.Length; j++)
        //            {
        //                v[j].Position = new Vector3(_LineList[i].LinePoints[j].X, YValue * _ZCoefficient, _LineList[i].LinePoints[j].Y); 
        //            }
        //            ContourLineBuffer[i].Unlock();
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
        #endregion

    }
}
