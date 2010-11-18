using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RunGridCellThick
{
    /// <summary>
    /// 运行GridCellThick
    /// </summary>
	public class RunGridCellThick
	{
        /// <summary>
        /// 运行exe
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>是否成功</returns>
        [STAThread]
        public bool RunEXE(string FileName)
        {
            try
            {
                //Application.EnableVisualStyles();
                //Application.SetCompatibleTextRenderingDefault(false);
                //Application.Run(new GridCellThick.GridCell(FileName));
                GridCellThick.GridCell gc = new GridCellThick.GridCell(FileName);
                gc.ShowDialog();
                return true;
            }
            catch
            {
                return false;
            }
        }
	}
}
