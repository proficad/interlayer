using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    /// <summary>
    /// 柔化属性类
    /// </summary>
    public class PropertyAverage
    {
        /// <summary>
        /// 柔化属性
        /// </summary>
        /// <param name="PropertyFileName">属性文件名</param>
        /// <param name="SaveFileName">保存文件名</param>
        /// <returns>是否成功</returns>
        public bool Average(string PropertyFileName, string SaveFileName)
        {
            try
            {
                PropertyData pd = PropertyData.GetPropertyData(PropertyFileName);
                pd.Average(false);
                pd.SavePropertyData(SaveFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 柔化属性单层
        /// </summary>
        /// <param name="PropertyFileName">属性文件名</param>
        /// <param name="SaveFileName">保存文件名</param>
        /// <returns>是否成功</returns>
        public bool AverageLayerOnly(string PropertyFileName, string SaveFileName)
        {
            try
            {
                PropertyData pd = PropertyData.GetPropertyData(PropertyFileName);
                pd.Average(true);
                pd.SavePropertyData(SaveFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
    }
}
