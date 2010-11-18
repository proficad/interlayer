using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace EclipseGridIO
{
    /// <summary>
    /// 读取关键字类
    /// </summary>
    class ReadKey
    {
        /// <summary>
        /// 读取关键字
        /// </summary>
        /// <returns>关键字列表</returns>
        public DataTable ReadKeyToDataTable(int TableID)
        {
            DataSet ds = new DataSet();
            ds.ReadXml(AppDomain.CurrentDomain.BaseDirectory + "Key.Xml");
            DataTable dt = new DataTable();
            dt = ds.Tables[TableID];
            return dt;
        }
        /// <summary>
        /// 读取关键字
        /// </summary>
        /// <returns>关键字列表</returns>
        public DataTable ReadKeyToDataTable(string TableName)
        {
            DataSet ds = new DataSet();
            ds.ReadXml(AppDomain.CurrentDomain.BaseDirectory + "Key.Xml");
            DataTable dt = new DataTable();
            dt = ds.Tables[TableName];
            return dt;
        }
    }
}
