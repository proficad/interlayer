using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Data;
using System.Reflection;

namespace EclipseGridIO
{
    /// <summary>
    /// 处理Eclipse数据类
    /// </summary>
    class EclipseData
    {
        public EclipseData()
        {

        }
        #region 变量声明

        ReadKey rk = new ReadKey();

        #endregion

        /// <summary>
        /// 解析带加密网格的Eclipse数据
        /// </summary>
        /// <param name="FileName">文件路径</param>
        /// <returns>网格数据类</returns>
        public GridData ReadEclipseGrid(string FileName)
        {
            StreamReader sr = null;
            try
            {
                FileStream fs = new FileStream(FileName, FileMode.Open);
                sr = new StreamReader(fs, Encoding.Default);
                GridData gd = new GridData();
                string DataLine = "";
                
                DataTable dt = rk.ReadKeyToDataTable(0);
                DataColumn[] dc = new DataColumn[1];
                dc[0] = dt.Columns[0];
                dt.PrimaryKey = dc;
                KeyContext kc = new KeyContext();
                while ((DataLine = sr.ReadLine()) != null)
                {
                    DataRow dr = dt.Rows.Find(DataLine.Trim().ToUpper());
                    if (dr != null)
                    {
                        kc.SetKeyClass((KeyResolve)Assembly.Load("EclipseGridIO").CreateInstance("EclipseGridIO." + dr["class"].ToString().Trim(), false, BindingFlags.Default, null, null, null, null));
                        kc.Resolve(sr, gd);
                        GC.Collect();
                    }
                }
               
                return gd;
            }
            catch (Exception ee)
            {
                throw ee;
            }
            finally
            {
                sr.Close();
                GC.Collect();
            }
        }
        /// <summary>
        /// 生成带网格加密的Eclipse格式数据
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="FileName">保存文件名</param>
        /// <returns>是否执行成功</returns>
        public bool WriteEclipseGrid(GridData gd, string FileName)
        {
            StreamWriter sw = null;
            try
            {
                FileStream fs = new FileStream(FileName, FileMode.Create);
                sw = new StreamWriter(fs, Encoding.Default);
                DataTable dt = rk.ReadKeyToDataTable("Write");
                KeyContext kc = new KeyContext();
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    kc.SetKeyClass((KeyResolve)Assembly.Load("EclipseGridIO").CreateInstance("EclipseGridIO." + dt.Rows[i]["class"].ToString().Trim(), false, BindingFlags.Default, null, null, null, null));
                    kc.DataWrite(sw, gd);
                }
                return true;
            }
            catch
            {
                return false;
            }
            finally
            {
                sw.Close();
                GC.Collect();
            }
        }
    }
}
