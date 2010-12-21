using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using EclipseGridIO;
using Track;

namespace GridCellThick
{
    public partial class GridCell : Form
    {
        #region 构造方法,初始化网格文件名

        /// <summary>
        /// 网格文件名
        /// </summary>
        /// <param name="FileName">文件名</param>
        public GridCell(string FileName)
        {
            InitializeComponent();
            try
            {
                dr = new View.Draw();//图形控件
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
            GridFileName = FileName;
        }
        public GridCell()
        {
            InitializeComponent();
            try
            {
                dr = new View.Draw();//图形控件
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 变量声明

        View.Draw dr;
        AddGridCellData agd = new AddGridCellData();
        GridData gd = null;
        GridIO gio = new GridIO();
        Track.GridTracking gt = new Track.GridTracking();
        double MinX = 999999999;
        double MinY = 999999999;
        double MinZ = 999999999;
        double MaxX = -999999999;
        double MaxY = -999999999;
        double MaxZ = -999999999;
        bool IsSave = false;
        string GridFileName = "";
        #endregion

        #region 窗体Load

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                EclipseGridIO.EclipseWrite ew = new EclipseWrite();
                ew.IsWriteMult = true;
                ew.GridFileName = @"C:\Users\Whl\Desktop\测试\{DFBFDD6C-175D-4BE6-8EA5-9C89CB3527EE}.mod";

                ew.AddTrackFileName(@"C:\Users\Whl\Desktop\测试\{3ABE3327-C8C4-4C9A-8FC8-4E2DE3DE5947}");

                ew.AddTrackFileName(@"C:\Users\Whl\Desktop\测试\{32FD4FD7-ADE6-484C-97B4-1307C7F54399}");

                ew.AddTrackFileName(@"C:\Users\Whl\Desktop\测试\{909E3693-6BED-4D58-9D88-746080FA2093}");

                ew.AddTrackFileName(@"C:\Users\Whl\Desktop\测试\{C2855C04-C4A7-4ECE-A873-D3FA67BAAEB5}");

                ew.AddPropertyFileName(@"C:\Users\Whl\Desktop\测试\{694632F1-88CD-41B0-8B28-AADBB09B0657}.grd");

                ew.AddPropertyFileName(@"C:\Users\Whl\Desktop\测试\{D03C4F77-3373-406E-BDD8-EBFF77DBD4A9}.grd");

                ew.AddPropertyFileName(@"C:\Users\Whl\Desktop\测试\{D3F4CB41-2B7C-49A3-B13F-FCA277960731}.grd");

                ew.AddPropertyFileName(@"C:\Users\Whl\Desktop\测试\{FA81A763-23BB-49A6-BDC7-6B98D9E173B5}.grd");

                ew.ChangePropertyName = @"C:\Users\Whl\Desktop\测试\{FA81A763-23BB-49A6-BDC7-6B98D9E173B5}.grd";
                ew.IsWriteCarfinProperty = false;
                ew.IsWriteProperty = false;
                ew.IsWriteCarfinChangeProperty = false;
                ew.ChangePropertyValue = 250;
                ew.Write(@"C:\Users\Whl\Desktop\测试\123");

                gd = gio.ReadGridDataFile(GridFileName);
                if (gd != null)
                {
                    MinX = gd.MinX;
                    MaxX = gd.MaxX;
                    MinY = gd.MinY;
                    MaxY = gd.MaxY;
                    MinZ = gd.MinZ;
                    MaxZ = gd.MaxZ;
                    gd = GridDataNormalized.Normalized(gd);
                    //增加网格加密选项 
                    if (gd.DNameList != null && gd.DNameList.Count > 0)
                    {
                        for (int i = 0; i < gd.DNameList.Count; i++)
                        {
                            if (gd.DNameList[i].ID.Count > 0)
                            {
                                toolStripComboBox2.Items.Add(gd.DGridDataList[gd.DNameList[i].ID[0]].Name);
                            }
                        }
                    }
                    //增加层选项 
                    if (gd.GridZ > 0)
                    {
                        for (int i = 0; i < gd.GridZ; i++)
                        {
                            toolStripComboBox1.Items.Add(i + 1);
                        }
                        toolStripComboBox1.SelectedItem = 1;
                    }

                    dr.DrawHeight = agd.GridCellHeight;
                    dr.DrawWidth = agd.GridCellWidth;
                    dr.IsGridCellVisible = true;
                    dr.ScrollBarLoad();//初始化进度条
                    dr.MoveX = Convert.ToSingle(-(gd.MaxX - gd.MinX) / 2);
                    dr.MoveY = Convert.ToSingle(-(gd.MaxY - gd.MinY) / 2);
                    dr.Dock = DockStyle.Fill;//位置
                    panel1.Controls.Add(dr);
                    this.Cursor = Cursors.Hand;
                    dr.IsHand = true;
                    toolStripButton6.Checked = true;
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 选择层

        private void toolStripComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (gd != null)
                {
                    if (toolStripComboBox1.SelectedItem != null)
                    {
                        List<View.GridCell.GridCell> GridCellList = agd.AddGridDataInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                        List<View.GridCell.DGridCellName> DGridCellNameList = agd.AddDGridCellNameInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                        if (DGridCellNameList != null && DGridCellNameList.Count > 0)
                        {
                            if (toolStripComboBox2.SelectedItem == null)
                            {
                                toolStripComboBox2.SelectedItem = DGridCellNameList[0].DName;
                            }
                            string SelectedItemName = toolStripComboBox2.SelectedItem.ToString();
                            bool SelectedInDGridCellNameList=false;
                            for (int i = 0; i < DGridCellNameList.Count; i++)
                            {
                                if (SelectedItemName == DGridCellNameList[i].DName)
                                {
                                    SelectedInDGridCellNameList = true;
                                    break;
                                }
                            }
                            int SelectedIndex;
                            if (SelectedInDGridCellNameList)
                            {
                                SelectedIndex = toolStripComboBox2.SelectedIndex;
                            }
                            else
                            {
                                toolStripComboBox2.SelectedItem = DGridCellNameList[0].DName;
                                SelectedIndex = toolStripComboBox2.SelectedIndex;
                            }
                            if (SelectedIndex >= 0 && SelectedIndex < gd.DNameList.Count)
                            {
                                GridCellList = agd.AddDGridCellInfo(gd, GridCellList, SelectedIndex);
                            }
                        }
                        else
                        {
                            toolStripComboBox2.SelectedItem = null;
                        }
                        dr.GridCellList = GridCellList;
                        dr.DGridCellNameList = DGridCellNameList;
                        dr.Invalidate();
                    }
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 选择&拖拽

        //选择
        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            try
            {
                if (toolStripButton1.Checked == true)
                {
                    toolStripButton6.Checked = false;
                    dr.IsHand = false;
                    this.Cursor = Cursors.Default;
                }
                else
                {
                    toolStripButton1.Checked = true;
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        //拖拽
        private void toolStripButton6_Click(object sender, EventArgs e)
        {
            try
            {
                if (toolStripButton6.Checked == true)
                {
                    toolStripButton1.Checked = false;
                    dr.IsHand = true;
                    this.Cursor = Cursors.Hand;
                }
                else
                {
                    //dr.IsHand = false;
                    //this.Cursor = Cursors.Default;
                    toolStripButton6.Checked = true;
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }
        #endregion

        #region 放大&缩小

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            try
            {
                dr.SetScale(true);
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        private void toolStripButton4_Click(object sender, EventArgs e)
        {
            try
            {
                dr.SetScale(false);
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 取消

        private void toolStripButton7_Click(object sender, EventArgs e)
        {
            try
            {
                if (dr.GridCellList != null)
                {
                    for (int i = 0; i < dr.GridCellList.Count; i++)
                    {
                        dr.GridCellList[i].IsSelected = false;
                    }
                    dr.Invalidate();
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (dr.GridCellList != null)
                {
                    for (int i = 0; i < dr.GridCellList.Count; i++)
                    {
                        dr.GridCellList[i].IsSelected = false;
                    }
                    dr.Invalidate();
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 选择网格加密

        private void toolStripComboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (gd != null)
                {
                    if (toolStripComboBox2.SelectedItem != null)
                    {
                        int MinZ = 999999999;
                        int MaxZ = -999999999;
                        int SelectedIndex = toolStripComboBox2.SelectedIndex;
                        for (int i = 0; i < gd.DNameList[SelectedIndex].ID.Count; i++)
                        {
                            if (gd.DGridDataList[gd.DNameList[SelectedIndex].ID[i]].Z1 < MinZ)
                            {
                                MinZ = gd.DGridDataList[gd.DNameList[SelectedIndex].ID[i]].Z1;
                            }
                            if (gd.DGridDataList[gd.DNameList[SelectedIndex].ID[i]].Z2 > MaxZ)
                            {
                                MaxZ = gd.DGridDataList[gd.DNameList[SelectedIndex].ID[i]].Z2;
                            }
                        }
                        if (toolStripComboBox1.SelectedItem != null)
                        {
                            if (!(Convert.ToInt32(toolStripComboBox1.SelectedItem) > MinZ&&Convert.ToInt32(toolStripComboBox1.SelectedItem)<MaxZ))
                            {
                                toolStripComboBox1.SelectedItem = MinZ;
                                List<View.GridCell.GridCell> GridCellList = agd.AddGridDataInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                List<View.GridCell.DGridCellName> DGridCellNameList = agd.AddDGridCellNameInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                GridCellList = agd.AddDGridCellInfo(gd, GridCellList, SelectedIndex);
                                dr.GridCellList = GridCellList;
                                dr.DGridCellNameList = DGridCellNameList;
                                dr.Invalidate();
                            }
                            else
                            {
                                if (SelectedIndex >= 0 && SelectedIndex < gd.DNameList.Count)
                                {
                                    List<View.GridCell.GridCell> GridCellList = agd.AddGridDataInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                    List<View.GridCell.DGridCellName> DGridCellNameList = agd.AddDGridCellNameInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                    GridCellList = agd.AddDGridCellInfo(gd, GridCellList, SelectedIndex);
                                    dr.GridCellList = GridCellList;
                                    dr.DGridCellNameList = DGridCellNameList;
                                    dr.Invalidate();
                                }
                            }
                        }

                    }
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion

        #region 网格加密编辑

        //删除网格加密
        private void toolStripButton8_Click(object sender, EventArgs e)
        {
            try
            {
                if (gd != null)
                {
                    if (toolStripComboBox2.SelectedItem != null)
                    {
                        if (MessageBox.Show("是否删除网格加密!", "确认", MessageBoxButtons.YesNo) == DialogResult.Yes)
                        {
                            int SelectedIndex = toolStripComboBox2.SelectedIndex;
                            gd.DNameList.RemoveAt(SelectedIndex);
                            toolStripComboBox2.Items.RemoveAt(SelectedIndex);
                            if (toolStripComboBox1.SelectedItem != null)
                            {
                                List<View.GridCell.GridCell> GridCellList = agd.AddGridDataInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                List<View.GridCell.DGridCellName> DGridCellNameList = agd.AddDGridCellNameInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                if (DGridCellNameList != null && DGridCellNameList.Count > 0)
                                {
                                    toolStripComboBox2.SelectedItem = DGridCellNameList[0].DName;
                                }
                                else
                                {
                                    toolStripComboBox2.SelectedItem = null;
                                }
                                dr.GridCellList = GridCellList;
                                dr.DGridCellNameList = DGridCellNameList;
                                dr.Invalidate();
                            }
                            IsSave = true;
                        }
                    }
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        //添加网格加密
        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            try
            {
                bool falg = true;
                int IsSelectedCount = 0;
                for (int i = 0; i < dr.GridCellList.Count; i++)
                {
                    if ((dr.GridCellList[i].IsSelected))
                    {
                        IsSelectedCount++;
                        if (dr.GridCellList[i].DX != 0 || dr.GridCellList[i].DY != 0 || dr.GridCellList[i].DZ != 0)
                        {
                            falg = false;
                        }
                    }
                }
                if (falg)
                {
                    if (IsSelectedCount > 0)
                    {
                        List<int> LayerList = new List<int>();
                        List<View.GridCell.GridCell> SelectedGridList = new List<View.GridCell.GridCell>();
                        for (int i = 0; i < dr.GridCellList.Count; i++)
                        {
                            if (dr.GridCellList[i].IsSelected)
                            {
                                SelectedGridList.Add(dr.GridCellList[i]);
                            }
                        }
                        for (int i = 0; i < gd.GridZ; i++)
                        {
                            bool IsNoThick = true;
                            for (int j = 0; j < SelectedGridList.Count; j++)
                            {
                                if (CheckCellThick.IsCheckCellThick(gd, SelectedGridList[j].NX, SelectedGridList[j].NY, i))
                                {
                                    IsNoThick = false;
                                }
                            }
                            if (IsNoThick)
                            {
                                LayerList.Add(i+1);
                            }
                        }
                        if (LayerList.Count > 0)
                        {
                            AddDGridCellName adcn = new AddDGridCellName(LayerList);
                            adcn.ShowDialog();
                            if (adcn.IsClick)//点击了确定
                            {
                                IsSave = true;
                                if (adcn.DName.Length > 0 && adcn.DName.Length < 5)
                                {
                                    ComputeSelectGridCell.ComputeGridCell(gd, dr.GridCellList, adcn.DX, adcn.DY, adcn.DZ, adcn.SeleceedLayerList, adcn.DName);
                                    if (gd.DNameList[gd.DNameList.Count - 1].ID.Count > 0)
                                    {
                                        toolStripComboBox2.Items.Add(gd.DGridDataList[gd.DNameList[gd.DNameList.Count - 1].ID[0]].Name);
                                        int SeleceedLayer = toolStripComboBox1.SelectedIndex;
                                        toolStripComboBox2.SelectedItem = gd.DGridDataList[gd.DNameList[gd.DNameList.Count - 1].ID[0]].Name;
                                        if (SeleceedLayer == toolStripComboBox1.SelectedIndex)
                                        {
                                            List<View.GridCell.GridCell> GridCellList = agd.AddGridDataInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                            List<View.GridCell.DGridCellName> DGridCellNameList = agd.AddDGridCellNameInfo(gd, Convert.ToInt32(toolStripComboBox1.SelectedItem) - 1);
                                            GridCellList = agd.AddDGridCellInfo(gd, GridCellList, toolStripComboBox2.SelectedIndex);
                                            dr.GridCellList = GridCellList;
                                            dr.DGridCellNameList = DGridCellNameList;
                                            dr.Invalidate();
                                        }
                                    }
                                    
                                }
                                else
                                {
                                    MessageBox.Show("加密名字不够或者太长！");
                                }
                            }
                        }
                        else
                        {
                            MessageBox.Show("模型中所选区域没有层复合加密条件！");
                        }
                    }
                    else
                    {
                        MessageBox.Show("请选择网格！");
                    }
                }
                else
                {
                    MessageBox.Show("所选网格中包含已加密的网格！");
                }
            }
            catch (Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }
        #endregion

        #region 保存

        private void toolStripButton9_Click(object sender, EventArgs e)
        {
            //List<string> FaceFileNameList = new List<string>();
            //FaceFileNameList.Add(@"d:\数据\face");
            //List<string> SaveFileNameList = new List<string>();
            //SaveFileNameList.Add(@"d:\数据\TrackFile");

            //TrackingBar tb = new TrackingBar(FaceFileNameList, SaveFileNameList, @"d:\数据\grid");
            //tb.ShowDialog();
            if (IsSave)
            {
                if (gd != null)
                {
                    IsSave = false;
                    gd.MinX = MinX;
                    gd.MaxX = MaxX;
                    gd.MinY = MinY;
                    gd.MaxY = MaxY;
                    gd.MinZ = MinZ;
                    gd.MaxZ = MaxZ;
                    GridDataNormalized.DNormalized(gd);
                    gio.WriteGridDataFile(gd, GridFileName);
                    GridDataNormalized.Normalized(gd);
                }
            }
            //saveFileDialog1.Filter = "Generic ECLIPSE style (ASCII) grid geometry and properties (*.GRDECL)|(*.GRDECL)";
            //if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            //{
            //    bool IsTrue= gio.WriteGridDataFile(gd,saveFileDialog1.FileName);
            //    if (IsTrue)
            //    {
            //        MessageBox.Show("网格文件已经成功导出到：" + saveFileDialog1.FileName + " 目录下！");
            //    }
            //    else
            //    {
            //        MessageBox.Show("网格文件导出失败！");
            //    }
            //}
        }

        #endregion

        #region 窗体关闭

        private void GridCell_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (IsSave)
                {
                    DialogResult dg = MessageBox.Show("是否保存更改？", "保存", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Exclamation);
                    if (dg == DialogResult.Yes)
                    {
                        if (gd != null)
                        {
                            IsSave = false;
                            gd.MinX = MinX;
                            gd.MaxX = MaxX;
                            gd.MinY = MinY;
                            gd.MaxY = MaxY;
                            gd.MinZ = MinZ;
                            gd.MaxZ = MaxZ;
                            GridDataNormalized.DNormalized(gd);
                            gio.WriteGridDataFile(gd, GridFileName);
                            GridDataNormalized.Normalized(gd);
                        }
                    }
                    else
                    {
                        if (dg == DialogResult.Cancel)
                        {
                            e.Cancel = true;
                        }
                    }
                }

            }
            catch(Exception ee)
            {
                MessageBox.Show(ee.Message);
            }
        }

        #endregion
    }
}
