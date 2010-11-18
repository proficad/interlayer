using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace EclipseGridIO
{
    /// <summary>
    /// 属性数据
    /// </summary>
    public class PropertyData
    {
        #region 变量声明

        string _PropertyName = "";//属性关键字
        int _PropertyX = 0;//I
        int _PropertyY = 0;//J
        int _PropertyZ = 0;//K
        float _Max = -99999999999;//最大值
        float _Min = 99999999999;//最小值
        List<float> _PropertyValue = new List<float>();//属性值列表
        int _IsShow = 1;//是否显示
        #endregion

        #region 属性

        /// <summary>
        /// 属性关键字
        /// </summary>
        public string PropertyName
        {
            get { return _PropertyName; }
            set { _PropertyName = value; }
        }
        /// <summary>
        /// I
        /// </summary>
        public int PropertyX
        {
            get { return _PropertyX; }
            set { _PropertyX = value; }
        }
        /// <summary>
        /// J
        /// </summary>
        public int PropertyY
        {
            get { return _PropertyY; }
            set { _PropertyY = value; }
        }
        /// <summary>
        /// K
        /// </summary>
        public int PropertyZ
        {
            get { return _PropertyZ; }
            set { _PropertyZ = value; }
        }
        /// <summary>
        /// 最大值
        /// </summary>
        public float Max
        {
            get { return _Max; }
            set { _Max = value; }
        }
        /// <summary>
        /// 最小值
        /// </summary>
        public float Min
        {
            get { return _Min; }
            set { _Min = value; }
        }
        /// <summary>
        /// 属性值列表
        /// </summary>
        public List<float> PropertyValue
        {
            get { return _PropertyValue; }
            set { _PropertyValue = value; }
        }
        /// <summary>
        /// 是否显示
        /// </summary>
        public int IsShow
        {
            get { return _IsShow; }
            set { _IsShow = value; }
        }

        #endregion

        #region 读取属性

        /// <summary>
        /// 读取属性
        /// </summary>
        /// <param name="FileName">文件名</param>
        /// <returns>PropertyData</returns>
        public static PropertyData GetPropertyData(string FileName)
        {
            try
            {
                FileStream fs = new FileStream(FileName, FileMode.Open);
                BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
                PropertyData pd = new PropertyData();
                pd.PropertyName = Encoding.ASCII.GetString((br.ReadBytes(8))).Trim();
                pd.IsShow = br.ReadInt32();//是否显示
                pd.Min = Convert.ToSingle(br.ReadDouble());
                pd.Max = Convert.ToSingle(br.ReadDouble());
                pd.PropertyX = br.ReadInt32();
                pd.PropertyY = br.ReadInt32();
                pd.PropertyZ = br.ReadInt32();
                int ValueCount = br.ReadInt32();
                for (int i = 0; i < ValueCount; i++)
                {
                    pd.PropertyValue.Add(Convert.ToSingle(br.ReadDouble()));
                }
                br.Close();
                return pd;
            }
            catch
            {
                return null;
            }
        }

        #endregion

        #region 属性存储

        /// <summary>
        /// 属性存储
        /// </summary>
        /// <param name="SaveFileName">保存文件名</param>
        /// <param name="pd">属性类</param>
        /// <returns>是否成功</returns>
        public bool SavePropertyData(string SaveFileName)
        {
            try
            {
                FileStream fs = new FileStream(SaveFileName, FileMode.Create);
                BinaryWriter bw = new BinaryWriter(fs, Encoding.Unicode);
                WriteValue(bw, _PropertyName);//关键字
                WriteValue(bw, _IsShow);//是否显示
                WriteValue(bw, _Min);//最小值
                WriteValue(bw, _Max);//最大值
                WriteValue(bw, _PropertyX);//I
                WriteValue(bw, _PropertyY);//J
                WriteValue(bw, _PropertyZ);//K
                WriteValue(bw, _PropertyValue.Count);
                for (int i = 0; i < _PropertyValue.Count; i++)
                {
                    WriteValue(bw, _PropertyValue[i]);    
                }
                bw.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        #endregion

        #region 写二进制

        /// <summary>
        /// 写二进制字符，如果不够8位，补齐，如果超过，截断
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="str">字符</param>
        private void WriteValue(BinaryWriter bw, string str)
        {
            if (str.Length > 8)
            {
                str = str.Substring(0, 8);
            }
            else
            {
                int KongCount = 8 - str.Length;
                for (int i = 0; i < KongCount; i++)
                {
                    str = str + " ";
                }
            }
            byte[] ByteValue = Encoding.ASCII.GetBytes(str);
            bw.Write(ByteValue);
        }
        /// <summary>
        /// 写二进制，float转换成double写入文件
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="FloatValue">float</param>
        private void WriteValue(BinaryWriter bw, float FloatValue)
        {
            double DoubleValue = Convert.ToDouble(FloatValue);
            byte[] ByteValue = BitConverter.GetBytes(DoubleValue);
            bw.Write(ByteValue);
        }
        /// <summary>
        /// 写二进制值
        /// </summary>
        /// <param name="bw">二进制流</param>
        /// <param name="IntValue">值</param>
        private void WriteValue(BinaryWriter bw, int IntValue)
        {
            byte[] ByteValue = BitConverter.GetBytes(IntValue);
            bw.Write(ByteValue);
        }

        #endregion

        #region 计算属性系数

        /// <summary>
        ///  增加参数
        /// </summary>
        /// <param name="Operator">运算符</param>
        /// <param name="ParameterValue">参数</param>
        public void AddParameter(string Operator, float ParameterValue)
        {
            switch (Operator)
            { 
                case "+":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] + ParameterValue;
                    }
                    MinMax();
                    break;
                case "-":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] - ParameterValue;
                    }
                    MinMax();
                    break;
                case "*":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] * ParameterValue;
                    }
                    MinMax();
                    break;
                case "/":
                    if (ParameterValue != 0)
                    {
                        for (int i = 0; i < _PropertyValue.Count; i++)
                        {
                            _PropertyValue[i] = _PropertyValue[i] / ParameterValue;
                        }
                    }
                    MinMax();
                    break;
            }
        }

        #endregion

        #region 属性计算

        /// <summary>
        /// 属性计算，将指定的属性与当前属性进行运算
        /// </summary>
        /// <param name="Operator">运算符</param>
        /// <param name="pd">属性</param>
        public void OperationProperty(string Operator, PropertyData pd)
        {
            switch (Operator)
            { 
                case "+":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] + pd._PropertyValue[i];
                    }
                    MinMax();
                    break;
                case "-":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] - pd._PropertyValue[i];
                    }
                    MinMax();
                    break;
                case "*":
                    for (int i = 0; i < _PropertyValue.Count; i++)
                    {
                        _PropertyValue[i] = _PropertyValue[i] * pd._PropertyValue[i];
                    }
                    MinMax();
                    break;
            }
        }

        #endregion

        #region 属性修正

        /// <summary>
        /// 属性使用平均值修正
        /// </summary>
        /// <param name="IsLayerOnly">是否只修正单层数据</param>
        public void Average(bool IsLayerOnly)
        {
            if (!IsLayerOnly)
            {
                for (int i = 0; i < _PropertyZ - 1; i++)
                {
                    for (int j = 0; j < _PropertyY - 1; j++)
                    {
                        for (int k = 0; k < _PropertyX - 1; k++)
                        {
                            _PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k] =
                                (_PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k] +
                                _PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k + 1] +
                                _PropertyValue[i * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k + 1] +
                                _PropertyValue[i * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k] +
                                _PropertyValue[(i + 1) * _PropertyX * _PropertyY + j * _PropertyX + k] +
                                _PropertyValue[(i + 1) * _PropertyX * _PropertyY + j * _PropertyX + k + 1] +
                                _PropertyValue[(i + 1) * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k + 1] +
                                _PropertyValue[(i + 1) * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k]) / 8;
                        }
                    }
                }
            }
            else
            { 
                for (int i = 0; i < _PropertyZ - 1; i++)
                {
                    for (int j = 0; j < _PropertyY - 1; j++)
                    {
                        for (int k = 0; k < _PropertyX - 1; k++)
                        {
                            _PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k] =
                                (_PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k] +
                                _PropertyValue[i * _PropertyX * _PropertyY + j * _PropertyX + k + 1] +
                                _PropertyValue[i * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k + 1] +
                                _PropertyValue[i * _PropertyX * _PropertyY + (j + 1) * _PropertyX + k]) / 4;
                        }
                    }
                }
            }
            MinMax();
        }
        #endregion

        #region 求最大值最小值

        private void MinMax()
        {
            _Max = -99999999999;
            _Min = 99999999999;
            for (int i = 0; i < _PropertyValue.Count; i++)
            {
                if (_Max < _PropertyValue[i])
                {
                    _Max = _PropertyValue[i];
                }
                if (_Min > _PropertyValue[i])
                {
                    _Min = _PropertyValue[i];
                }
            }
        }

        #endregion
    }
}
