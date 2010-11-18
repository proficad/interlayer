namespace View
{
    partial class Draw
    {
        /// <summary> 
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.ultraScrollBar1 = new Infragistics.Win.UltraWinScrollBar.UltraScrollBar();
            this.ultraScrollBar2 = new Infragistics.Win.UltraWinScrollBar.UltraScrollBar();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.ultraScrollBar1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ultraScrollBar2)).BeginInit();
            this.SuspendLayout();
            // 
            // ultraScrollBar1
            // 
            this.ultraScrollBar1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.ultraScrollBar1.Location = new System.Drawing.Point(0, 528);
            this.ultraScrollBar1.Name = "ultraScrollBar1";
            this.ultraScrollBar1.NonAutoSize = new System.Drawing.Size(80, 16);
            this.ultraScrollBar1.Size = new System.Drawing.Size(827, 16);
            this.ultraScrollBar1.TabIndex = 0;
            this.ultraScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.ultraScrollBar1_Scroll);
            // 
            // ultraScrollBar2
            // 
            this.ultraScrollBar2.Dock = System.Windows.Forms.DockStyle.Right;
            this.ultraScrollBar2.Location = new System.Drawing.Point(811, 0);
            this.ultraScrollBar2.Name = "ultraScrollBar2";
            this.ultraScrollBar2.NonAutoSize = new System.Drawing.Size(80, 16);
            this.ultraScrollBar2.ScrollBarInfo.Enabled = true;
            this.ultraScrollBar2.ScrollBarInfo.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.ultraScrollBar2.ScrollBarInfo.SetFocusOnClick = true;
            this.ultraScrollBar2.Size = new System.Drawing.Size(16, 528);
            this.ultraScrollBar2.TabIndex = 1;
            this.ultraScrollBar2.Scroll += new System.Windows.Forms.ScrollEventHandler(this.ultraScrollBar2_Scroll);
            // 
            // Draw
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.ultraScrollBar2);
            this.Controls.Add(this.ultraScrollBar1);
            this.DoubleBuffered = true;
            this.Name = "Draw";
            this.Size = new System.Drawing.Size(827, 544);
            this.Load += new System.EventHandler(this.Draw_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Draw_Paint);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Draw_MouseMove);
            this.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.Draw_MouseDoubleClick);
            this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.Draw_MouseClick);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Draw_MouseDown);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Draw_MouseUp);
            ((System.ComponentModel.ISupportInitialize)(this.ultraScrollBar1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ultraScrollBar2)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private Infragistics.Win.UltraWinScrollBar.UltraScrollBar ultraScrollBar1;
        private Infragistics.Win.UltraWinScrollBar.UltraScrollBar ultraScrollBar2;
        private System.Windows.Forms.ToolTip toolTip1;


    }
}
