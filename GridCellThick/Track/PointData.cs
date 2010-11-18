using System;
using System.Collections.Generic;
using System.Text;
using EclipseGridIO;
using Microsoft.DirectX;
namespace Track
{
    class PointData
    {
        #region 变量声明

        List<float> _PointX = new List<float>();
        List<float> _PointY = new List<float>();
        List<float> _PointZ = new List<float>();

        #endregion

        #region 属性

        /// <summary>
        /// X点集合
        /// </summary>
        public List<float> PointX
        {
            get { return _PointX; }
            set { _PointX = value; }
        }
        /// <summary>
        /// Y点集合
        /// </summary>
        public List<float> PointY
        {
            get { return _PointY; }
            set { _PointY = value; }
        }
        /// <summary>
        /// Z点集合
        /// </summary>
        public List<float> PointZ
        {
            get { return _PointZ; }
            set { _PointZ = value; }
        }

        #endregion

        #region 获取数据

        /// <summary>
        /// 获取
        /// </summary>
        /// <param name="f">曲面对象</param>
        /// <param name="gd">网格对象</param>
        public void GetPointData(Face f, GridData gd)
        {
            float GridMinLength = 999999999999;
            for (int i = 0; i < gd.PointX.Length / 8; i++)
            {
                Vector3 v1 = new Vector3(Convert.ToSingle(gd.PointX[i * 8]), Convert.ToSingle(gd.PointY[i * 8]), Convert.ToSingle(gd.PointZ[i * 8]));
                Vector3 v2 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 1]), Convert.ToSingle(gd.PointY[i * 8 + 1]), Convert.ToSingle(gd.PointZ[i * 8 + 1]));
                Vector3 v3 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 2]), Convert.ToSingle(gd.PointY[i * 8 + 2]), Convert.ToSingle(gd.PointZ[i * 8 + 2]));
                Vector3 v4 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 3]), Convert.ToSingle(gd.PointY[i * 8 + 3]), Convert.ToSingle(gd.PointZ[i * 8 + 3]));
                Vector3 v5 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 4]), Convert.ToSingle(gd.PointY[i * 8 + 4]), Convert.ToSingle(gd.PointZ[i * 8 + 4]));
                Vector3 v6 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 5]), Convert.ToSingle(gd.PointY[i * 8 + 5]), Convert.ToSingle(gd.PointZ[i * 8 + 5]));
                Vector3 v7 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 6]), Convert.ToSingle(gd.PointY[i * 8 + 6]), Convert.ToSingle(gd.PointZ[i * 8 + 6]));
                Vector3 v8 = new Vector3(Convert.ToSingle(gd.PointX[i * 8 + 7]), Convert.ToSingle(gd.PointY[i * 8 + 7]), Convert.ToSingle(gd.PointZ[i * 8 + 7]));
                //上面
                GridMinLength = Math.Min((v1 - v2).Length(), GridMinLength);
                GridMinLength = Math.Min((v1 - v4).Length(), GridMinLength);
                GridMinLength = Math.Min((v4 - v3).Length(), GridMinLength);
                GridMinLength = Math.Min((v3 - v2).Length(), GridMinLength);
                //下面
                GridMinLength = Math.Min((v5 - v6).Length(), GridMinLength);
                GridMinLength = Math.Min((v5 - v8).Length(), GridMinLength);
                GridMinLength = Math.Min((v8 - v7).Length(), GridMinLength);
                GridMinLength = Math.Min((v6 - v7).Length(), GridMinLength);
                //左面
                GridMinLength = Math.Min((v1 - v5).Length(), GridMinLength);
                GridMinLength = Math.Min((v4 - v8).Length(), GridMinLength);
                //右面
                GridMinLength = Math.Min((v1 - v2).Length(), GridMinLength);
                GridMinLength = Math.Min((v1 - v2).Length(), GridMinLength);
            }
            float FaceMaxLength = -99999999999;
            for (int i = 0; i < f.IndexCount / 3; i++)
            {
                Vector3 v1 = new Vector3(Convert.ToSingle(f.PointX[f.IndexData[i * 3]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3]]));
                Vector3 v2 = new Vector3(Convert.ToSingle(f.PointX[f.IndexData[i * 3 + 1]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3 + 1]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + 1]]));
                Vector3 v3 = new Vector3(Convert.ToSingle(f.PointX[f.IndexData[i * 3 + 2]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3 + 2]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + 2]]));

                FaceMaxLength = Math.Max((v1 - v2).Length(), FaceMaxLength);
                FaceMaxLength = Math.Max((v1 - v3).Length(), FaceMaxLength);
                FaceMaxLength = Math.Max((v2 - v3).Length(), FaceMaxLength);
            }

            int Depth = Convert.ToInt32(FaceMaxLength / GridMinLength);
            if (Depth > 1)
            {
                int i = 0;
                while (true)
                {
                    if (Depth <= Math.Pow(2, i))
                    {
                        Depth = i;
                        break;
                    }
                    else
                    {
                        i++;
                    }
                }
            }
            else
            {
                Depth = 0;
            }

            for (int i = 0; i < f.IndexCount / 3; i++)
            {
                float[] v1 = new float[3] { Convert.ToSingle(f.PointX[f.IndexData[i * 3]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3]]) };
                float[] v2 = new float[3] { Convert.ToSingle(f.PointX[f.IndexData[i * 3 + 1]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3 + 1]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + 1]]) };
                float[] v3 = new float[3] { Convert.ToSingle(f.PointX[f.IndexData[i * 3 + 2]]), Convert.ToSingle(f.PointY[f.IndexData[i * 3 + 2]]), Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + 2]]) };
                Subdivide(v1, v2, v3, Depth, _PointX, _PointY, _PointZ);
            }

        }
        /// <summary>
        /// 递归计算三角形细分
        /// </summary>
        private void Subdivide(float[] v1, float[] v2, float[] v3, int depth, List<float> XList, List<float> YList, List<float> ZList)
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
