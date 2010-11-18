using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using EclipseGridIO;
namespace GridCellThick
{
    public partial class AddDGridCellName : Form
    {
        #region 构造方法

        /// <summary>
        /// 启动添加网格
        /// </summary>
        /// <param name="Layer">可选层列表</param>
        public AddDGridCellName(List<int> Layer)
        {
            InitializeComponent();
            LayerList = Layer;
        }

        #endregion
        
        #region 变量声明

        List<int> LayerList = new List<int>();//可选层列表
        bool _IsClick = false;//是否点击了确定
        List<int> _SeleceedLayerList = new List<int>();//选中的层
        //int _DX;//NX方向细分个数
        //int _DY;//NY方向细分个数
        //int _DZ;//NZ方向细分个数
        //string _DName = "";//网格加密名称
        #endregion

        #region 属性

        /// <summary>
        /// 是否点击了确定
        /// </summary>
        public bool IsClick
        {
            get { return _IsClick; }
        }
        /// <summary>
        /// 选中的层
        /// </summary>
        public List<int> SeleceedLayerList
        {
            get { return _SeleceedLayerList; }
        }
        /// <summary>
        /// NX方向细分个数
        /// </summary>
        public int DX
        {
            get { return Convert.ToInt32(textBox2.Text.Trim()); }
        }
        /// <summary>
        /// NY方向细分个数
        /// </summary>
        public int DY
        {
            get { return Convert.ToInt32(textBox3.Text.Trim()); }
        }
        /// <summary>
        /// NZ方向细分个数
        /// </summary>
        public int DZ
        {
            get { return Convert.ToInt32(textBox4.Text.Trim()); }
        }
        /// <summary>
        /// 网格加密名称
        /// </summary>
        public string DName
        {
            get { return textBox1.Text.Trim(); }
        }
        /// <summary>
        /// 选中的层
        /// </summary>
        public List<int> SelectedLayerList
        {
            get
            {
                return _SeleceedLayerList;
            }
        }
        #endregion

        #region 窗体Load

        private void AddDGridCellName_Load(object sender, EventArgs e)
        {
            if (LayerList != null && LayerList.Count > 0)
            {
                for (int i = 0; i < LayerList.Count; i++)
                {
                    listBox1.Items.Add(LayerList[i]);
                }
            }
        }

        #endregion

        #region 确定&取消

        //确定
        private void button1_Click(object sender, EventArgs e)
        {
            _IsClick = true;
            _SeleceedLayerList.Clear();
            for (int i = 0; i < listBox1.SelectedItems.Count; i++)
            {
                _SeleceedLayerList.Add(Convert.ToInt32(listBox1.SelectedItems[i]) - 1);
            }
            this.Close();
        }

        //取消
        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        #endregion

        #region TextChanged

        //Name
        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            CheckInPutValue();
        }
        //NX
        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            CheckInPutValue();
        }
        //NY
        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            CheckInPutValue();
        }
        //NZ
        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            CheckInPutValue();
        }

        #endregion

        #region SelectedIndexChanged

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //MessageBox.Show("我变形了。");
            CheckInPutValue();
        }

        #endregion

        #region 检查

        /// <summary>
        /// 检查输入数据
        /// </summary>
        private void CheckInPutValue()
        {
            bool CheckValue = true;
            if (textBox1.Text.Trim() == "")
            {
                CheckValue = false;
            }
            try
            {
                int DXValue = Convert.ToInt32(textBox2.Text.Trim());
                if (DXValue < 1)
                {
                    CheckValue = false;
                }
            }
            catch
            {
                CheckValue = false;
            }

            try
            {
                int DYValue = Convert.ToInt32(textBox3.Text.Trim());
                if (DYValue < 1)
                {
                    CheckValue = false;
                }
            }
            catch
            {
                CheckValue = false;
            }

            try
            {
                int DZValue = Convert.ToInt32(textBox4.Text.Trim());
                if (DZValue < 1)
                {
                    CheckValue = false;
                }
            }
            catch
            {
                CheckValue = false;
            }
            if (listBox1.Items == null || listBox1.SelectedItems.Count == 0)
            {
                CheckValue = false;
            }
            if (CheckValue)
            {
                button1.Enabled = true;
            }
            else
            {
                button1.Enabled = false;
            }
        }

        #endregion
    }
}
