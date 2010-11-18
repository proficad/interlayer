using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;//线程

namespace GridCellThick
{
    public partial class TrackingBar : Form
    {
        #region 构造方法

        public TrackingBar()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 初始化,输入曲面文件名列表及追踪后保存的文件名列表和网格文件名
        /// </summary>
        /// <param name="FaceList">曲面文件名列表</param>
        /// <param name="SaveLise">追踪后保存的文件名列表</param>
        /// <param name="GridName">网格文件名</param>
        public TrackingBar(List<string> FaceList, List<string> SaveLise, string GridName)
        {
            InitializeComponent();
            FaceFileNameList = FaceList;
            SaveFileNameList = SaveLise;
            GridFileName = GridName;
        }

        #endregion

        #region 变量声明

        private List<string> FaceFileNameList = new List<string>();//曲面文件名列表
        private List<string> SaveFileNameList = new List<string>();//追踪后保存的文件名列表
        private string GridFileName = "";//网格文件名
        private bool _IsCancel = false;//是否点击取消
        private bool IsThreadingCloseForm = false;//是否是线程
        ThreadStart starter;//委托线程
        Thread t;//线程

        #endregion

        #region 属性

        /// <summary>
        /// 是否点击取消
        /// </summary>
        public bool IsCancel
        {
            get { return _IsCancel; }
        }
        

        #endregion

        #region 窗体Load

        private void TrackingBar_Load(object sender, EventArgs e)
        {
            t = new Thread(GridTracking);
            t.Start();
        }

        #endregion

        #region 线程执行网格套接方法

        private void GridTracking()
        {
            try
            {
                if (FaceFileNameList.Count > 0)
                {
                    SetBarMaxValue(1 + FaceFileNameList.Count * 6);
                    Track.GridTracking gt = new Track.GridTracking();
                    SetLabelText("正在读取网格数据...");
                    gt.ReadGridData(GridFileName);
                    step(1);
                    for (int i = 0; i < FaceFileNameList.Count; i++)
                    {
                        SetLabelText("正在读取曲面数据...");
                        gt.ReadFaceData(FaceFileNameList[i]);
                        step(i * 6 + 2);
                        SetLabelText("正在排除曲面以外网格数据...");
                        gt.GetInFaceDataList();
                        step(i * 6 + 3);
                        SetLabelText("正在创建网格八叉数...");
                        gt.CreateOctree();
                        step(i * 6 + 4);
                        SetLabelText("正在通过网格八叉数查找相交网格...");
                        gt.GetIndexList();
                        step(i * 6 + 5);
                        SetLabelText("正在生成追踪网格数据文件...");
                        gt.WriteData(SaveFileNameList[i]);
                        step(i * 6 + 6);
                        SetLabelText("正在清理缓存...");
                        gt.ClearData();
                        step(i * 6 + 7);
                    }
                    gt.ClearGridData();
                    IsThreadingCloseForm = true;
                    SetFormClose();
                }
            }
            catch //(Exception ee)
            {
                //MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 线程调用委托控制控件状态

        private delegate void BarValue(int i);//进度条Value的委托
        private delegate void BarMaxValue(int MaxValue);//进度条最大值
        private delegate void BarMinValue(int MinValue);//进度条最小值
        private delegate void LabelText(string TextValue);//文本框显示信息
        private delegate void ThisClose();//窗体关闭;
    
        /// <summary>
        /// 进度条Value
        /// </summary>
        /// <param name="i">Value</param>
        private void step(int i)
        {
            if (progressBar1.InvokeRequired)
            {
                BarValue bv = new BarValue(step);
                progressBar1.Invoke(bv, new object[] { i });
            }
            else
            {
                progressBar1.Value = i;
            }
        }
        /// <summary>
        /// 进度条最大值
        /// </summary>
        /// <param name="MaxValue">进度条最大值</param>
        private void SetBarMaxValue(int MaxValue)
        {
            if (progressBar1.InvokeRequired)
            {
                BarMaxValue bmv = new BarMaxValue(SetBarMaxValue);
                progressBar1.Invoke(bmv, new object[] { MaxValue });
            }
            else
            {
                progressBar1.Maximum = MaxValue;
            }
        }
        /// <summary>
        /// 进度条最小值
        /// </summary>
        /// <param name="MinValue">进度条最小值</param>
        private void SetBarMinValue(int MinValue)
        {
            if (progressBar1.InvokeRequired)
            {
                BarMinValue bmv = new BarMinValue(SetBarMinValue);
                progressBar1.Invoke(bmv, new object[] { MinValue });
            }
            else
            {
                progressBar1.Minimum = MinValue;
            }
        }
        /// <summary>
        /// 设置文本框显示信息
        /// </summary>
        /// <param name="TextValue">显示信息</param>
        private void SetLabelText(string TextValue)
        {
            if (label1.InvokeRequired)
            {
                LabelText lt = new LabelText(SetLabelText);
                label1.Invoke(lt, new object[] { TextValue });
            }
            else
            {
                label1.Text = TextValue;
            }
        }
        /// <summary>
        /// 关闭窗体
        /// </summary>
        private void SetFormClose()
        {
            if (this.InvokeRequired)
            {
                ThisClose tc = new ThisClose(SetFormClose);
                this.Invoke(tc);
            }
            else
            {
                this.Close();
            }
        }
        #endregion

        #region 取消

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        #endregion

        private void TrackingBar_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (t != null && t.IsAlive)
            {
                if (!IsThreadingCloseForm)
                {

                    t.Suspend();

                    if (MessageBox.Show("是否停止网格套接?", "确认", MessageBoxButtons.OKCancel) == DialogResult.OK)
                    {
                        _IsCancel = true;
                        t.Resume();
                        t.Abort();
                        e.Cancel = false;
                    }
                    else
                    {
                        t.Resume();
                        e.Cancel = true;
                    }
                }
            }

        }
    }
}
