using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace EclipseGridIO
{
    /// <summary>
    /// Eclipse文件输出
    /// </summary>
    public class EclipseWrite
    {
        #region 变量声明

        private bool _IsWriteGrid = true;//是否写入网格坐标数据
        private string _GridFileName = "";//网格数据文件名
        private bool _IsWriteProperty = true;//是否写入属性数据
        //private List<string> PropertyKeyList = new List<string>();//属性关键字
        private List<string> PropertyFileNameList = new List<string>();//属性文件列表
        private bool _IsWriteCarfin = true;//是否写入局部网格加密信息数据
        private bool _IsWriteCarfinProperty = true;//是否写入局部网格加密属性数据
        private bool _IsWriteCarfinMult = true;//是否写入局部网格加密传导率属性
        private bool _IsWriteMult = true;//是否写入追踪后面文件
        private List<string> TrackFileNameList = new List<string>();//追踪后网格文件
        
        #endregion

        #region 属性

        /// <summary>
        /// 是否写入网格坐标数据
        /// </summary>
        public bool IsWriteGrid
        {
            get { return _IsWriteGrid; }
            set { _IsWriteGrid = value; }
        }
        /// <summary>
        /// 是否写入属性数据
        /// </summary>
        public bool IsWriteProperty
        {
            get { return _IsWriteProperty; }
            set { _IsWriteProperty = value; }
        }
        /// <summary>
        /// 是否写入局部网格加密信息数据
        /// </summary>
        public bool IsWriteCarfin
        {
            get { return _IsWriteCarfin; }
            set { _IsWriteCarfin = value; }
        }
        /// <summary>
        /// 是否写入局部网格加密属性数据
        /// </summary>
        public bool IsWriteCarfinProperty
        {
            get { return _IsWriteCarfinProperty; }
            set { _IsWriteCarfinProperty = value; }
        }
        /// <summary>
        /// 网格文件名
        /// </summary>
        public string GridFileName
        {
            get { return _GridFileName; }
            set { _GridFileName = value; }
        }
        /// <summary>
        /// 是否写入传导率面
        /// </summary>
        public bool IsWriteMult
        {
            get { return _IsWriteMult; }
            set { _IsWriteMult = value; }
        }
        /// <summary>
        /// 是否写入局部网格加密传导率
        /// </summary>
        public bool IsWriteCarfinMult
        {
            get { return _IsWriteCarfinMult; }
            set { _IsWriteCarfinMult = value; }
        }

        #region c++调用c#属性方法

        /// <summary>
        /// 写入网格坐标数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteGridEnable()
        {
            try
            {
                _IsWriteGrid = true;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入网格坐标数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteGridDisable()
        {
            try
            {
                _IsWriteGrid = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 写入属性数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WritePropertyEnable()
        {
            try
            {
                _IsWriteProperty = true;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入属性数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WritePropertyDisable()
        {
            try
            {
                _IsWriteProperty = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 写入局部网格加密信息数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinEnable()
        {
            try
            {
                _IsWriteCarfin = true;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入局部网格加密信息数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinDisable()
        {
            try
            {
                _IsWriteCarfin = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 写入局部网格加密属性数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinPropertyEnable()
        {
            try
            {
                _IsWriteCarfinProperty = true;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入局部网格加密属性数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinPropertyDisable()
        {
            try
            {
                _IsWriteCarfinProperty = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 写入传导率面
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteMultEnable()
        {
            try
            {
                _IsWriteMult = true;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入传导率面
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteMultDisable()
        {
            try
            {
                _IsWriteMult = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 写入网格加密传导率
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinMultEnable()
        {
            try
            {
                _IsWriteCarfinMult = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 不写入网格加密传导率
        /// </summary>
        /// <returns>是否成功</returns>
        public bool WriteCarfinMultDisable()
        {
            try
            {
                _IsWriteCarfinMult = false;
                return true;
            }
            catch
            {
                return false;
            }
        }
        #endregion
       
        #endregion

        #region 属性文件

        /// <summary>
        /// 添加属性文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>是否成功</returns>
        public bool AddPropertyFileName(string FileName)
        {
            try
            {
                //PropertyKeyList.Add(Key);
                PropertyFileNameList.Add(FileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 清除属性文件
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearPropertyFileName()
        {
            try
            {
                //PropertyKeyList.Clear();
                PropertyFileNameList.Clear();
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 获取属性文件个数
        /// </summary>
        /// <returns>属性文件个数，错误返回-1</returns>
        public int GetPropertyFileNameCount()
        {
            try
            {
                return PropertyFileNameList.Count;
            }
            catch
            {
                return -1;
            }
        }
        /// <summary>
        /// 删除指定索引处属性文件
        /// </summary>
        /// <param name="PropertyKeyIndex">索引</param>
        /// <returns>是否成功</returns>
        public bool PropertyFileNameRemoveAt(int PropertyFileNameIndex)
        {
            try
            {
                //PropertyKeyList.RemoveAt(PropertyIndex);
                PropertyFileNameList.RemoveAt(PropertyFileNameIndex);
                return true;
            }
            catch
            {
                return false;
            }
        }
        ///// <summary>
        ///// 删除一个匹配属性关键字
        ///// </summary>
        ///// <param name="Key"></param>
        ///// <returns></returns>
        //public bool PropertyKeyRemove(string Key)
        //{
        //    try
        //    {
        //        PropertyKey.Remove(Key);
        //        return true;
        //    }
        //    catch
        //    {
        //        return false;
        //    }
        //}
        /// <summary>
        /// 获取属性文件某一项
        /// </summary>
        /// <param name="PropertyKeyIndex">属性关键字某一项索引</param>
        /// <returns>不成功返回字符串 "" </returns>
        public string GetPrivateFileNameItem(int PropertyFileNameIndex)
        {
            try
            {
                return PropertyFileNameList[PropertyFileNameIndex];
            }
            catch
            {
                return "";
            }
        }
        ///// <summary>
        ///// 获取属性文件名某一项
        ///// </summary>
        ///// <param name="PropertyFileNameIndex">属性文件名某一项索引</param>
        ///// <returns>不成功返回字符串 ""</returns>
        //private string GetProvateFileNameItem(int PropertyFileNameIndex)
        //{
        //    try
        //    {
        //        return PropertyFileNameList[PropertyFileNameIndex];
        //    }
        //    catch
        //    {
        //        return "";
        //    }
        //}
        #endregion

        #region 追踪文件

        /// <summary>
        /// 增加追踪文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>是否成功</returns>
        public bool AddTrackFileName(string FileName)
        {
            try
            {
                TrackFileNameList.Add(FileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 清除追踪文件
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearTrackFileName()
        {
            try
            {
                TrackFileNameList.Clear();
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 获取追踪文件个数
        /// </summary>
        /// <returns>追踪文件个数，错误返回-1</returns>
        public int GetTrackFileNameCount()
        {
            try
            {
                return TrackFileNameList.Count;
            }
            catch
            {
                return -1;
            }
        }
        /// <summary>
        /// 获取追踪文件某一项的值
        /// </summary>
        /// <param name="TrackFileNameIndex">索引</param>
        /// <returns>某一项的值，错误返回""</returns>
        public string GetTrackFileNameItem(int TrackFileNameIndex)
        {
            try
            {
                return TrackFileNameList[TrackFileNameIndex];
            }
            catch
            {
                return "";
            }
        }
        /// <summary>
        /// 移除追踪文件某一项
        /// </summary>
        /// <param name="TrackFileNameIndex">索引</param>
        /// <returns>是否成功</returns>
        public bool TrackFileNameRemoveAt(int TrackFileNameIndex)
        {
            try
            {
                TrackFileNameList.RemoveAt(TrackFileNameIndex);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 移除追踪文件某一项
        /// </summary>
        /// <param name="Item">某一项</param>
        /// <returns>是否成功</returns>
        public bool TrackFileNameRemove(string Item)
        {
            try
            {
                TrackFileNameList.Remove(Item);
                return true;
            }
            catch
            {
                return false;
            }
        }
        #endregion

        #region 输出Eclipse文件

        /// <summary>
        /// 输出Eclipse文件
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>是否成功</returns>
        public bool Write(string FileName)
        {
            try
            {
                FileStream fs = new FileStream(FileName, FileMode.Create);
                StreamWriter sw = new StreamWriter(fs, Encoding.Default);
                GridDataRead gdr = new GridDataRead();
                GridData gd = gdr.VCGridDataRead(_GridFileName);
                WriteHead(sw);//文件头
                WriteSpecgrid(sw, gd);//网格大小
                if (_IsWriteGrid)//网格坐标数据
                {
                    WriteGrid(sw, gd);
                }
                if (_IsWriteProperty)//属性
                {
                    WritePropertyData(sw);
                }
                if (_IsWriteMult)//面
                {
                    WriteMultData(sw);
                    //WriteMultTest(sw);
                }
                if (_IsWriteCarfin)//局部网格加密及网格加密属性、传导率
                {
                    WriteCarfin(sw, gd);   
                }
                //if (_IsWriteCarfinProperty)
                //{ 
                    
                //}
                sw.WriteLine("/");
                sw.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        #endregion

        #region 输出Eclipse文件头注释

        /// <summary>
        /// 输出Eclipse文件头注释
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        private void WriteHead(StreamWriter sw)
        {
            //写文件头（注释）
            sw.WriteLine("-- Format      : Generic ECLIPSE style (ASCII) grid geometry and properties (*.GRDECL)");
            //sw.WriteLine("-- Exported by : Interlayer  Excellent Co., Ltd.");
            sw.WriteLine("-- Exported by : Interlayer");
            sw.WriteLine("-- Date        : " + DateTime.Now.ToString());
            sw.WriteLine("-- Grid        : 3D grid");
            sw.WriteLine();
        }

        #endregion

        #region 输出Eclipse网格文件

        /// <summary>
        /// 输出Eclipse网格文件
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        /// <param name="gd">GridData</param>
        private void WriteGrid(StreamWriter sw,GridData gd)
        {
            #region COORD

            sw.WriteLine("COORD");
            for (int i = 0; i < gd.LineX1.Length; i++)
            {
                sw.WriteLine((gd.LineX1[i].ToString("#0.000000") + " " + gd.LineY1[i].ToString("#0.000000") + " " + gd.LineZ1[i].ToString("#0.000000")
                    + " " + gd.LineX2[i].ToString("#0.000000") + " " + gd.LineY2[i].ToString("#0.000000") + " " + gd.LineZ2[i].ToString("#0.000000")));
            }
            sw.WriteLine("/");
            sw.WriteLine();

            #endregion

            #region Zcorn

            sw.WriteLine("ZCORN");
            //string DataLine = "";
            for (int i = 0; i < gd.PointZcron.Length; i++)
            {
                if (i % 6 == 0)
                {
                    sw.Write("\n");
                    sw.Write(gd.PointZcron[i].ToString("#0.000000") + " ");
                }
                else
                {
                    sw.Write(gd.PointZcron[i].ToString("#0.000000") + " ");
                }
            }
            sw.Write("\n");
            sw.WriteLine("/");
            sw.WriteLine();

            #endregion
        }

        /// <summary>
        /// 写网格大小
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        /// <param name="gd">GridData</param>
        private void WriteSpecgrid(StreamWriter sw, GridData gd)
        {
            #region Specgrid

            sw.WriteLine("SPECGRID");
            sw.WriteLine(gd.GridX.ToString() + " " + gd.GridY.ToString() + " " + gd.GridZ.ToString() + " 1 F /");
            sw.WriteLine();

            #endregion
        }

        #endregion

        #region 输出Eclipse局部网格加密

        /// <summary>
        /// 输出Eclipse局部网格加密
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        /// <param name="gd">GridData</param>
        private void WriteCarfin(StreamWriter sw, GridData gd)
        {
            List<PropertyData> PropertyDataList = new List<PropertyData>();//属性列表
            if (_IsWriteCarfinProperty)
            {
                if (PropertyFileNameList != null && PropertyFileNameList.Count > 0)
                {
                    for (int i = 0; i < PropertyFileNameList.Count; i++)
                    {
                        PropertyDataList.Add(PropertyData.GetPropertyData(PropertyFileNameList[i]));
                    }
                }
            }

            #region Carfin

            if (gd.DNameList != null && gd.DNameList.Count > 0)
            {
                for (int i = 0; i < gd.DNameList.Count; i++)
                {
                    for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                    {
                        sw.WriteLine("CARFIN");
                        sw.WriteLine(gd.DGridDataList[gd.DNameList[i].ID[j]].Name + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].X1.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].X2.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Y1.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Y2.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Z1.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].Z2.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DX.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DY.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].DZ.ToString()
                            + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].NWMAX.ToString() + " " + gd.DGridDataList[gd.DNameList[i].ID[j]].GLOBAL);

                        #region 网格加密属性

                        if (_IsWriteCarfinProperty)//网格加密属性
                        {
                            for (int p = 0; p < PropertyDataList.Count; p++)
                            {
                                sw.WriteLine(PropertyDataList[i].PropertyName);//写属性名
                                for (int z = gd.DGridDataList[gd.DNameList[i].ID[j]].Z1 - 1; z < gd.DGridDataList[gd.DNameList[i].ID[j]].Z2; z++)
                                {
                                    for (int y = gd.DGridDataList[gd.DNameList[i].ID[j]].Y1 - 1; y < gd.DGridDataList[gd.DNameList[i].ID[j]].Y2; y++)
                                    {
                                        for (int x = gd.DGridDataList[gd.DNameList[i].ID[j]].X1 - 1;x< gd.DGridDataList[gd.DNameList[i].ID[j]].X2; x++)
                                        {
                                            int Count = gd.DGridDataList[gd.DNameList[i].ID[j]].DZ / (gd.DGridDataList[gd.DNameList[i].ID[j]].Z2 - gd.DGridDataList[gd.DNameList[i].ID[j]].Z1);
                                            Count = Count * gd.DGridDataList[gd.DNameList[i].ID[j]].DX / (gd.DGridDataList[gd.DNameList[i].ID[j]].X2 - gd.DGridDataList[gd.DNameList[i].ID[j]].X1);
                                            Count = Count * gd.DGridDataList[gd.DNameList[i].ID[j]].DY / (gd.DGridDataList[gd.DNameList[i].ID[j]].Y2 - gd.DGridDataList[gd.DNameList[i].ID[j]].Y1);
                                            for (int t = 0; t < Count; t++)
                                            { 
                                                if (t % 6 == 0)
                                                {
                                                    sw.Write("\n");
                                                    sw.Write(PropertyDataList[p].PropertyValue[PropertyDataList[p].PropertyX * PropertyDataList[p].PropertyY * z + PropertyDataList[p].PropertyX * y + x].ToString("#0.000000") + " ");
                                                }
                                                else
                                                {
                                                    sw.Write(PropertyDataList[p].PropertyValue[PropertyDataList[p].PropertyX * PropertyDataList[p].PropertyY * z + PropertyDataList[p].PropertyX * y + x].ToString("#0.000000") + " ");
                                                }
                                            }
                                            sw.Write("\n");
                                        }
                                    }
                                }
                                sw.WriteLine("/");
                            }

                        #endregion

                        }
                        sw.WriteLine("ENDFIN");
                    }
                }
                sw.WriteLine("/");
            }
            

            #endregion

            #region Amalgam

            if (gd.DNameList != null && gd.DNameList.Count > 0)
            {
                for (int i = 0; i < gd.DNameList.Count; i++)
                {
                    for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                    {
                        sw.Write(gd.DGridDataList[gd.DNameList[i].ID[j]].Name + " ");
                    }
                    sw.Write("\n");
                    if (gd.DNameList[i].ID.Count > 1)
                    {
                        sw.WriteLine("/");
                    }
                }
                sw.WriteLine();
            }

            #endregion
        }

        #endregion

        #region 输出Eclipse属性数据

        /// <summary>
        /// 输出Eclipse属性数据
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        private void WritePropertyData(StreamWriter sw)
        {
            if (PropertyFileNameList != null && PropertyFileNameList.Count > 0)
            {
                for (int i = 0; i < PropertyFileNameList.Count; i++)
                {
                    PropertyData pd = PropertyData.GetPropertyData(PropertyFileNameList[i]);
                    if (pd != null)
                    {
                        sw.WriteLine(pd.PropertyName);//写属性名
                        for (int j = 0; j < pd.PropertyValue.Count; j++)
                        {
                            if (j % 6 == 0)
                            {
                                sw.Write("\n");
                                sw.Write(pd.PropertyValue[j].ToString("#0.000000") + " ");
                            }
                            else
                            {
                                sw.Write(pd.PropertyValue[j].ToString("#0.000000") + " ");
                            }
                        }
                        sw.Write("\n");
                        sw.WriteLine("/");
                    }
                }
            }
        }

        #endregion

        #region 输出Eclipse面数据

        /// <summary>
        /// 输出Eclipse面数据
        /// </summary>
        /// <param name="sw">StreamWriter</param>
        private bool WriteMultData(StreamWriter sw)
        {
            try
            {
                if (TrackFileNameList != null && TrackFileNameList.Count > 0)
                {
                    int GridX = 0;
                    int GridY = 0;
                    int GridZ = 0;
                    List<int> MultX = new List<int>();
                    List<int> MultY = new List<int>();
                    List<int> MultZ = new List<int>();
                    List<int> MultX0 = new List<int>();
                    List<int> MultY0 = new List<int>();
                    List<int> MultZ0 = new List<int>();
                    for (int i = 0; i < TrackFileNameList.Count; i++)
                    {
                        FileStream fs = new FileStream(TrackFileNameList[i], FileMode.Open);
                        BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
                        GridX = br.ReadInt32();
                        GridY = br.ReadInt32();
                        GridZ = br.ReadInt32();
                        int Count = br.ReadInt32();

                        List<int> GridCellList = new List<int>();
                        for (int j = 0; j < Count; j++)
                        {
                            int I = br.ReadInt32();
                            int J = br.ReadInt32();
                            int K = br.ReadInt32();
                            GridCellList.Add(K * GridX * GridY + J * GridX + I);
                            //跳跃坐标点
                            fs.Seek(4 * 3 * 8, SeekOrigin.Current);
                            int ChildrenCount = br.ReadInt32();
                            if (ChildrenCount > 0)
                            {
                                fs.Seek(ChildrenCount * 4 * 3 * 8, SeekOrigin.Current);
                            }
                        }
                        br.Close();

                        GetMultItem(ref MultX, GridCellList, GridX, GridY, GridZ, "X");
                        GetMultItem(ref MultY, GridCellList, GridX, GridY, GridZ, "Y");
                        GetMultItem(ref MultZ, GridCellList, GridX, GridY, GridZ, "Z");
                        GetMultItem(ref MultX0, GridCellList, GridX, GridY, GridZ, "X-");
                        GetMultItem(ref MultY0, GridCellList, GridX, GridY, GridZ, "Y-");
                        GetMultItem(ref MultZ0, GridCellList, GridX, GridY, GridZ, "Z-");
                    }
                    WriteMult(sw, GridX * GridY * GridZ, MultX, "MultX");
                    WriteMult(sw, GridX * GridY * GridZ, MultX0, "MultX-");
                    WriteMult(sw, GridX * GridY * GridZ, MultY, "MultY");
                    WriteMult(sw, GridX * GridY * GridZ, MultY0, "MultY-");
                    WriteMult(sw, GridX * GridY * GridZ, MultZ, "MultZ");
                    WriteMult(sw, GridX * GridY * GridZ, MultZ0, "MultZ-");
                }
                return true;
            }
            catch
            {
                return false;
            }
        }
        private void GetMultItem(ref List<int> MultValue, List<int> XYZList,int GridX,int GridY,int GridZ,string Direction)
        {
            switch (Direction)
            {
                case "X":
                    for (int i = 0; i < GridZ; i++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            int Value = -1;
                            for (int k = 0; k < GridX; k++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    Value = k;
                                }
                            }
                            if (Value != -1 && Value != 0)
                            {
                                MultValue.Add(i * GridX * GridY + j * GridX + Value);
                            }
                        }
                    }
                    break;
                case "Y":
                    for (int i = 0; i < GridZ; i++)
                    {
                        for (int k = 0; k < GridX; k++)
                        {
                            int Value = -1;
                            int AddValue = -1;
                            bool IsAddDownItem = false;
                            int Count = 0;
                            for (int j = 0; j < GridY; j++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    if (Value == -1)
                                    {
                                        if (j != 0)
                                        { 
                                            AddValue = j;
                                            Count++;
                                            Value = j;
                                            MultValue.Add(i * GridX * GridY + (Value - 1) * GridX + k);
                                        }
                                    }
                                    else
                                    {
                                        Value = j;
                                    }
                                }
                                else
                                {

                                    if (Value != -1)
                                    {
                                        if (Value == AddValue)
                                        {
                                            MultValue.Add(i * GridX * GridY + Value * GridX + k);
                                        }
                                        else
                                        {
                                            if (j != GridY - 1)
                                            {
                                                bool falg = false;
                                                for (int t = AddValue; t <= Value; t++)
                                                {
                                                    if (XYZList.Contains(i * GridX * GridY + t * GridX + k + 1))
                                                    {
                                                        falg = true;
                                                        break;
                                                    }
                                                }
                                                if (!falg)
                                                {
                                                    MultValue.Add(i * GridX * GridY + Value * GridX + k);
                                                }
                                            }
                                            else
                                            {
                                                MultValue.Add(i * GridX * GridY + Value * GridX + k);
                                            }
                                        }
                                        Value = AddValue = -1;
                                    }

                                    //if (IsAddDownItem)
                                    //{
                                    //    continue;
                                    //}
                                    //if (Value != -1 && Value > AddValue)
                                    //{
                                    //    IsAddDownItem = true;
                                    //    if (j != GridY - 1)
                                    //    {
                                    //        bool falg = false;
                                    //        for (int t = AddValue; t < Value; t++)
                                    //        {
                                    //            if (XYZList.Contains(i * GridX * GridY + t * GridX + k + 1))
                                    //            {
                                    //                falg = true;
                                    //            }
                                    //        }
                                    //        if (!falg)
                                    //        {
                                    //            MultValue.Add(i * GridX * GridY + Value * GridX + k);
                                    //        }
                                    //    }
                                    //    else
                                    //    {
                                    //        MultValue.Add(i * GridX * GridY + Value * GridX + k);
                                    //    }
                                    //}
                                }
                            }
                            if (AddValue != -1 && AddValue != 0 && Count > 1)
                            {
                                MultValue.Add(i * GridX * GridY + Value * GridX + k);
                            }
                        }
                    }
                    break;
                case "Z":
                    for (int k = 0; k < GridX; k++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            int Value = -1;
                            int AddValue = -1;
                            int Count = 0;
                            for (int i = 0; i < GridZ; i++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                                {
                                    if (Value == -1)
                                    {
                                        if (i != 0)
                                        {
                                            AddValue = i;
                                            Count++;
                                            Value = i;
                                            MultValue.Add((Value - 1) * GridX * GridY + j * GridX + k);
                                        }
                                    }
                                    else
                                    {
                                        Value = i;
                                    }
                                }
                                else
                                {
                                    if (Value != -1)
                                    {
                                        if (Value == AddValue)
                                        {
                                            MultValue.Add(Value * GridX * GridY + j * GridX + k);
                                        }
                                        else
                                        {
                                            if (i != GridZ - 1)
                                            {
                                                bool falg = false;
                                                for (int t = AddValue; t <= Value; t++)
                                                {
                                                    if (XYZList.Contains(t * GridX * GridY + j * GridX + k + 1))
                                                    {
                                                        falg = true;
                                                    }
                                                }
                                                if (!falg)
                                                {
                                                    MultValue.Add(Value * GridX * GridY + j * GridX + k);
                                                }
                                            }
                                        }
                                        Value = AddValue = -1;
                                    }
                                }
                            }
                            if (AddValue != -1 && AddValue != 0 && Count > 1)
                            {
                                MultValue.Add(Value * GridX * GridY + j * GridX + k);
                            }
                            //int Value = -1;
                            //for (int i = 0; i < GridZ; i++)
                            //{
                            //    if (XYZList.Contains(i * GridX * GridY + j * GridX + k))
                            //    {
                            //        Value = i;
                            //    }
                            //}
                            //if (Value != -1 && Value != 0)
                            //{
                            //    MultValue.Add(Value * GridX * GridY + j * GridX + k);
                            //}
                        }
                    }
                    break;
                case "X-":
                    for (int i = 0; i < GridZ; i++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            if (XYZList.Contains(i * GridX * GridY + j * GridX))
                            {
                                MultValue.Add(i * GridX * GridY + j * GridX);
                            }
                        }
                    }
                        break;
                case "Y-":
                        for (int i = 0; i < GridZ; i++)
                        {
                            for (int k = 0; k < GridX; k++)
                            {
                                if (XYZList.Contains(i * GridX * GridY + k))
                                {
                                    MultValue.Add(i * GridX * GridY + k);
                                }
                            }
                        }
                    break;
                case "Z-":
                    for (int k = 0; k < GridX; k++)
                    {
                        for (int j = 0; j < GridY; j++)
                        {
                            if (XYZList.Contains(j * GridX + k))
                            {
                                MultValue.Add(j * GridX + k);
                            }
                        }
                    }
                    break;
            }
        }
        private void WriteMult(StreamWriter sw,int Count,List<int> MultValue,string MultKey)
        {
            sw.WriteLine(MultKey);
            for (int i = 0; i < Count; i++)
            {
                if (i % 6 == 0)
                {
                    sw.Write("\n");
                }
                if (MultValue.Contains(i))
                {
                    sw.Write("0\t");
                }
                else
                {
                    sw.Write("1\t");
                }
            }
            sw.Write("\n");
            sw.WriteLine("/");
        }
        private void WriteMultTest(StreamWriter sw)
        {
            try
            {
                if (TrackFileNameList != null && TrackFileNameList.Count > 0)
                {
                    List<int> GridCellList = new List<int>();
                    int CellCount = 0;
                    int GridX = 0;
                    int GridY = 0;
                    int GridZ = 0;
                    for (int i = 0; i < TrackFileNameList.Count; i++)
                    {
                        FileStream fs = new FileStream(TrackFileNameList[i], FileMode.Open);
                        BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
                        GridX = br.ReadInt32();
                        GridY = br.ReadInt32();
                        GridZ = br.ReadInt32();
                        CellCount = GridX * GridY * GridZ;
                        int Count = br.ReadInt32();

                        for (int j = 0; j < Count; j++)
                        {
                            int I = br.ReadInt32();
                            int J = br.ReadInt32();
                            int K = br.ReadInt32();
                            GridCellList.Add(K * GridX * GridY + J * GridX + I);
                            //YList.Add();
                            //ZList.Add();
                            //跳跃坐标点
                            fs.Seek(4 * 3 * 8, SeekOrigin.Current);
                            int ChildrenCount = br.ReadInt32();
                            if (ChildrenCount > 0)
                            {
                                fs.Seek(ChildrenCount * 4 * 3 * 8, SeekOrigin.Current);
                            }
                        }
                        br.Close();
                    }
                    sw.WriteLine("PORO");
                    for (int i = 0; i < CellCount; i++)
                    {
                        if (i % 6 == 0)
                        {
                            sw.Write("\n");
                        }
                        if (GridCellList.Contains(i))
                        {
                            sw.Write("0\t");
                        }
                        else
                        {
                            sw.Write("1\t");
                        }
                    }
                    sw.Write("\n");
                    sw.WriteLine("/");
                }
            }
            catch (Exception ee)
            {
                throw ee;
            }

        }
        #endregion

        #region 输出追踪后的中心点

        /// <summary>
        /// 输出追踪后的中心点
        /// </summary>
        /// <param name="SaveFileName">保存文件名</param>
        /// <param name="IsWriteThick">是否写入网格加密中心点</param>
        /// <param name="Number">参数</param>
        /// <returns>是否成功</returns>
        public bool WriteCenterPoint(string SaveFileName, bool IsWriteThick)
        {
            try
            {
                if (TrackFileNameList != null && TrackFileNameList.Count > 0)
                {
                    FileStream SaveFileFs = new FileStream(SaveFileName, FileMode.Create);
                    StreamWriter SaveFileSw = new StreamWriter(SaveFileFs, Encoding.Default);
                    for (int t = 0; t< TrackFileNameList.Count; t++)
                    {
                        FileStream OpenFileFs = new FileStream(TrackFileNameList[t], FileMode.Open);
                        BinaryReader OpenFileBr = new BinaryReader(OpenFileFs, Encoding.Unicode);
                        List<float> SaveFilePointX = new List<float>();
                        List<float> SaveFilePointY = new List<float>();
                        List<float> SaveFilePointZ = new List<float>();
                        OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃GridX,GridY,GridZ
                        int CellCount = OpenFileBr.ReadInt32();
                        for (int i = 0; i < CellCount; i++)
                        {
                            OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃I，J，K
                            List<float> PointX = new List<float>();
                            List<float> PointY = new List<float>();
                            List<float> PointZ = new List<float>();
                            for (int j = 0; j < 8; j++)
                            {
                                PointX.Add(OpenFileBr.ReadSingle());
                                PointY.Add(OpenFileBr.ReadSingle());
                                PointZ.Add(OpenFileBr.ReadSingle());
                            }
                            SaveFilePointX.Add(GetCenterPoint(PointX));
                            SaveFilePointY.Add(GetCenterPoint(PointY));
                            SaveFilePointZ.Add(GetCenterPoint(PointZ));
                            //读加密网格个数
                            int ChildrenCount = OpenFileBr.ReadInt32();
                            for (int j = 0; j < ChildrenCount; j++)
                            {
                                if (IsWriteThick)//是否写入加密网格坐标
                                {
                                    PointX.Clear();
                                    PointY.Clear();
                                    PointZ.Clear();
                                    for (int k = 0; k < 8; k++)
                                    {
                                        PointX.Add(OpenFileBr.ReadSingle());
                                        PointY.Add(OpenFileBr.ReadSingle());
                                        PointZ.Add(OpenFileBr.ReadSingle());
                                    }
                                    SaveFilePointX.Add(GetCenterPoint(PointX));
                                    SaveFilePointY.Add(GetCenterPoint(PointY));
                                    SaveFilePointZ.Add(GetCenterPoint(PointZ));
                                }
                                else
                                {
                                    OpenFileFs.Seek(12 * 8, SeekOrigin.Current);
                                }
                            }
                        }
                        OpenFileBr.Close();

                        for (int i = 0; i < SaveFilePointX.Count; i++)
                        {
                            SaveFileSw.WriteLine(SaveFilePointX[i] + "\t" + -SaveFilePointY[i] + "\t" + SaveFilePointZ[i] + "\t" + t.ToString());
                        }
                    }
                    SaveFileSw.Close();
                }
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 输出追踪后的中心点
        /// </summary>
        /// <param name="TrackFileName">曲面文件名</param>
        /// <param name="SaveFileName">保存文件名</param>
        /// <param name="IsWriteThick">是否写入网格加密中心点</param>
        /// <param name="Number">参数</param>
        /// <returns>是否成功</returns>
        public bool WriteCenterPoint(string TrackFileName, string SaveFileName, bool IsWriteThick, int Number)
        {
            try
            {
                FileStream SaveFileFs = new FileStream(SaveFileName, FileMode.Create);
                StreamWriter SaveFileSw = new StreamWriter(SaveFileFs, Encoding.Default);
                FileStream OpenFileFs = new FileStream(TrackFileName, FileMode.Open);
                BinaryReader OpenFileBr = new BinaryReader(OpenFileFs, Encoding.Unicode);
                List<float> SaveFilePointX = new List<float>();
                List<float> SaveFilePointY = new List<float>();
                List<float> SaveFilePointZ = new List<float>();
                OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃GridX,GridY,GridZ
                int CellCount = OpenFileBr.ReadInt32();
                for (int i = 0; i < CellCount; i++)
                {
                    OpenFileFs.Seek(12, SeekOrigin.Current);//跳跃I，J，K
                    List<float> PointX = new List<float>();
                    List<float> PointY = new List<float>();
                    List<float> PointZ = new List<float>();
                    for (int j = 0; j < 8; j++)
                    {
                        PointX.Add(OpenFileBr.ReadSingle());
                        PointY.Add(OpenFileBr.ReadSingle());
                        PointZ.Add(OpenFileBr.ReadSingle());
                    }
                    SaveFilePointX.Add(GetCenterPoint(PointX));
                    SaveFilePointY.Add(GetCenterPoint(PointY));
                    SaveFilePointZ.Add(GetCenterPoint(PointZ));
                    //读加密网格个数
                    int ChildrenCount = OpenFileBr.ReadInt32();
                    for (int j = 0; j < ChildrenCount; j++)
                    {
                        if (IsWriteThick)//是否写入加密网格坐标
                        {
                            PointX.Clear();
                            PointY.Clear();
                            PointZ.Clear();
                            for (int k = 0; k < 8; k++)
                            {
                                PointX.Add(OpenFileBr.ReadSingle());
                                PointY.Add(OpenFileBr.ReadSingle());
                                PointZ.Add(OpenFileBr.ReadSingle());
                            }
                            SaveFilePointX.Add(GetCenterPoint(PointX));
                            SaveFilePointY.Add(GetCenterPoint(PointY));
                            SaveFilePointZ.Add(GetCenterPoint(PointZ));
                        }
                        else
                        {
                            OpenFileFs.Seek(12 * 8, SeekOrigin.Current);
                        }
                    }
                }
                OpenFileBr.Close();
                for (int i = 0; i < SaveFilePointX.Count; i++)
                {
                    SaveFileSw.WriteLine(SaveFilePointX[i] + "\t" + -SaveFilePointY[i] + "\t" + SaveFilePointZ[i] + "\t" + Number);
                }

                SaveFileSw.Close();
                return true;
            }
            catch
            {
                return false;
            }

        }
        /// <summary>
        /// 获取中心点某个轴的值
        /// </summary>
        /// <param name="PointList">某个轴点坐标列表</param>
        /// <returns>中心点某个轴的值</returns>
        private float GetCenterPoint(List<float> PointList)
        {
            float Max = -999999999999;
            float Min = 999999999999;
            for (int i = 0; i < PointList.Count; i++)
            {
                if (Max < PointList[i])
                {
                    Max = PointList[i];
                }
                if (Min > PointList[i])
                {
                    Min = PointList[i];
                }
            }
            return Min + (Max - Min) / 2;
        }
        #endregion

        #region 输出曲面加密三角形文件

        /// <summary>
        /// 输出曲面加密文件
        /// </summary>
        /// <param name="SaveFileName">保存文件名</param>
        /// <param name="FaceFileName">曲面文件名</param>
        /// <param name="Depth">加密深度</param>
        /// <param name="Number">编号</param>
        /// <returns>是否成功</returns>
        public bool WriteTrianglePoint(string SaveFileName, string FaceFileName, int Depth, int Number)
        {
            try
            {
                #region  读取曲面

                FileStream OpenFileFs = new FileStream(FaceFileName, FileMode.Open);
                BinaryReader OpenFileBr = new BinaryReader(OpenFileFs, Encoding.Unicode);

                List<int> IndexList = new List<int>();
                List<float> PointX = new List<float>();
                List<float> PointY = new List<float>();
                List<float> PointZ = new List<float>();
                int PointCount = OpenFileBr.ReadInt32();

                for (int i = 0; i < PointCount; i++)
                {
                    PointX.Add(Convert.ToSingle(OpenFileBr.ReadDouble()));
                    PointY.Add(Convert.ToSingle(OpenFileBr.ReadDouble()));
                    PointZ.Add(Convert.ToSingle(OpenFileBr.ReadDouble()));
                }
                int IndexCount = OpenFileBr.ReadInt32();
                for (int i = 0; i < IndexCount; i++)
                {
                    IndexList.Add(OpenFileBr.ReadInt32());
                }
                OpenFileBr.Close();

                #endregion

                #region 计算细分

                List<float> XList = new List<float>();
                List<float> YList = new List<float>();
                List<float> ZList = new List<float>();

                for (int i = 0; i < IndexList.Count / 3; i++)
                {
                    float[] v1 = new float[3] { PointX[IndexList[i * 3]], PointY[IndexList[i * 3]], PointZ[IndexList[i * 3]] };
                    float[] v2 = new float[3] { PointX[IndexList[i * 3 + 1]], PointY[IndexList[i * 3 + 1]], PointZ[IndexList[i * 3 + 1]] };
                    float[] v3 = new float[3] { PointX[IndexList[i * 3 + 2]], PointY[IndexList[i * 3 + 2]], PointZ[IndexList[i * 3 + 2]] };
                    Subdivide(v1, v2, v3, Depth, XList, YList, ZList);
                }

                #endregion

                #region 写文件

                FileStream SaveFileFs = new FileStream(SaveFileName, FileMode.Create);
                StreamWriter SaveFileSw = new StreamWriter(SaveFileFs, Encoding.Default);
                for (int i = 0; i < XList.Count; i++)
                {
                    SaveFileSw.WriteLine(XList[i] + "\t" + -YList[i] + "\t" + ZList[i] + "\t" + Number.ToString());
                }

                SaveFileSw.Close();

                #endregion

                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// 递归计算三角形细分
        /// </summary>
        private void Subdivide(float[] v1, float[] v2, float[] v3,int depth, List<float> XList, List<float> YList, List<float> ZList)
        {
            float[] v12 = new float[3];
            float[] v23 = new float[3];
            float[] v31 = new float[3];

            if (depth == 0)
            {
                XList.Add(v1[0]);
                XList.Add(v2[0]);
                XList.Add(v3[0]);

                YList.Add(v1[1]);
                YList.Add(v2[1]);
                YList.Add(v3[1]);

                ZList.Add(v1[2]);
                ZList.Add(v2[2]);
                ZList.Add(v3[2]);

                return;
            }
            for (int i = 0; i < 3; i++)
            {
                v12[i] = (v1[i] + v2[i]) / 2;
                v23[i] = (v2[i] + v3[i]) / 2;
                v31[i] = (v3[i] + v1[i]) / 2;
            }
            Subdivide(v1, v12, v31, depth - 1, XList, YList, ZList);
            Subdivide(v2, v23, v12, depth - 1, XList, YList, ZList);
            Subdivide(v3, v31, v23, depth - 1, XList, YList, ZList);
            Subdivide(v12, v23, v31, depth - 1, XList, YList, ZList);
        }

        #endregion
    }
}
