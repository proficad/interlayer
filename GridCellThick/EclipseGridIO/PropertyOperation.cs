using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace EclipseGridIO
{
    /// <summary>
    /// 属性计算类
    /// </summary>
    public class PropertyOperation
    {
        #region 变量声明

        private List<string> TrackFileNameList = new List<string>();//追踪后网格文件

        #endregion

        #region 调用方法

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

        #endregion

        /// <summary>
        /// 属性计算
        /// </summary>
        /// <param name="PropertyFileName1">属性1文件名</param>
        /// <param name="PropertyFileName2">属性2文件名</param>
        /// <param name="SaveFileName">计算结果保存文件名</param>
        /// <param name="Operator1">属性1与参数1的运算符</param>
        /// <param name="Operator2">属性2与参数2的运算符</param>
        /// <param name="Operator">属性1与属性2的运算符</param>
        /// <param name="ParameterValue1">参数1</param>
        /// <param name="ParameterValue2">参数2</param>
        /// <returns>是否成功</returns>
        public bool Operation(string PropertyFileName1, string PropertyFileName2, string SaveFileName, string Operator1, string Operator2, string Operator, float ParameterValue1, float ParameterValue2)
        {
            try
            {
                PropertyData pd1 = PropertyData.GetPropertyData(PropertyFileName1);
                PropertyData pd2 = PropertyData.GetPropertyData(PropertyFileName2);
                pd1.AddParameter(Operator1, ParameterValue1);
                pd2.AddParameter(Operator2, ParameterValue2);
                pd1.OperationProperty(Operator, pd2);
                pd1.SavePropertyData(SaveFileName);

                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 属性计算
        /// </summary>
        /// <param name="PropertyFileName">属性文件名</param>
        /// <param name="ParameterValue">参数</param>
        /// <param name="SaveFileName">计算结果保存文件名</param>
        /// <param name="Operator">运算符</param>
        /// <returns>是否成功</returns>
        public bool Operation(string PropertyFileName, float ParameterValue, string SaveFileName, string Operator)
        {
            try
            {
                PropertyData pd = PropertyData.GetPropertyData(PropertyFileName);
                pd.AddParameter(Operator, ParameterValue);
                pd.SavePropertyData(SaveFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// 替换保存属性
        /// </summary>
        /// <param name="OldPropertyFileName">原有属性名</param>
        /// <param name="NewPropertyFileName">新属性名</param>
        /// <param name="NewValue">替换的值</param>
        /// <returns>是否成功</returns>
        public bool SaveProperty(string OldPropertyFileName, string NewPropertyFileName, double NewValue)
        {
            try
            {
                if (TrackFileNameList != null && TrackFileNameList.Count > 0)
                {
                    PropertyData pd = PropertyData.GetPropertyData(OldPropertyFileName);
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
                            //跳跃坐标点
                            fs.Seek(4 * 3 * 8, SeekOrigin.Current);
                            int ChildrenCount = br.ReadInt32();
                            if (ChildrenCount > 0)
                            {
                                fs.Seek(ChildrenCount * (4 * 3 * 8+4), SeekOrigin.Current);
                            }
                        }
                        br.Close();
                    }
                    for (int i = 0; i < GridCellList.Count; i++)
                    {
                        pd.PropertyValue[GridCellList[i]] = Convert.ToSingle(NewValue);
                    }
                    pd.SavePropertyData(NewPropertyFileName);
                    return true;
                }
                return false;
            }
            catch
            {
                return false;
            }
        }
    }
}
