using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    /// <summary>
    /// 网格文件读取、存储类
    /// </summary>
    public class GridIO
    {
        /// <summary>
        /// 读取网格文件，并存成VC++可读的二进制文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <param name="SaveFileName">保存的文件名</param>
        /// <returns>是否成功</returns>
        public bool LoadEclipseGridToFile(string FileName,string SaveFileName)
        {
            try
            {
                EclipseData ed = new EclipseData();
                GridData gd = ed.ReadEclipseGrid(FileName);
                GridDataWrite dw = new GridDataWrite();
                dw.GridDataWriteToVC(gd, SaveFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 读取VC++可读格式的二进制文件到GridData中
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>是否成功</returns>
        public GridData ReadGridDataFile(string FileName)
        {
            try
            {
                GridDataRead gdr = new GridDataRead();
                GridData gd = gdr.VCGridDataRead(FileName);
                return gd;
            }
            catch
            {
                return null;
            }
        }
        /// <summary>
        /// 生成Eclipse格式数据文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <param name="gd">GridData</param>
        /// <returns>是否成功</returns>
        public bool WriteGridDataFile(GridData gd,string FileName)
        {
            try
            {
                GridDataWrite gdw = new GridDataWrite();
                gdw.GridDataWriteToVC(gd, FileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 测试
        /// </summary>
        /// <param name="a">A</param>
        /// <param name="b">B</param>
        /// <returns>返回执行行数</returns>
        public int GetTest(int a, int b)
        {
            return 0;
        }
    }
}
