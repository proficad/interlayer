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
        public string ConvertUTF8toASCII(string text)
        {
            return System.Text.Encoding.ASCII.GetString(Encoding.UTF8.GetBytes(text));
        }
        public string ConvertUTF16toASCII(string text)
        {
            return System.Text.Encoding.UTF8.GetString(Encoding.UTF8.GetBytes(text));
        }
    }
}
