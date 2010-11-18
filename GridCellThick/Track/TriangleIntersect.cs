using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX;

namespace Track
{
    /// <summary>
    /// 判断空间内两个三角形是否相交
    /// </summary>
    class TriangleIntersect
    {
        /// <summary>
        /// 判断空间2个三角形是否相交
        /// </summary>
        /// <returns>是否相交，如果相交返回True，如果不相交返回False</returns>
        public bool IsTriangleIntersect(double X10, double Y10, double Z10, double X11, double Y11, double Z11,
            double X12, double Y12, double Z12, double X20, double Y20, double Z20, double X21, double Y21, double Z21,
            double X22, double Y22, double Z22)
        {
            Vector3 v10 = new Vector3(Convert.ToSingle(X10), Convert.ToSingle(Y10), Convert.ToSingle(Z10));
            Vector3 v11 = new Vector3(Convert.ToSingle(X11), Convert.ToSingle(Y11), Convert.ToSingle(Z11));
            Vector3 v12 = new Vector3(Convert.ToSingle(X12), Convert.ToSingle(Y12), Convert.ToSingle(Z12));
            Vector3 v20 = new Vector3(Convert.ToSingle(X20), Convert.ToSingle(Y20), Convert.ToSingle(Z20));
            Vector3 v21 = new Vector3(Convert.ToSingle(X21), Convert.ToSingle(Y21), Convert.ToSingle(Z21));
            Vector3 v22 = new Vector3(Convert.ToSingle(X22), Convert.ToSingle(Y22), Convert.ToSingle(Z22));
            return IsTriangleIntersect(v10, v11, v12, v20, v21, v22);
        }
        /// <summary>
        /// 判断空间2个三角形是否相交
        /// </summary>
        /// <returns>是否相交，如果相交返回True，如果不相交返回False</returns>
        public bool IsTriangleIntersect(Vector3 v10, Vector3 v11, Vector3 v12, Vector3 v20, Vector3 v21, Vector3 v22)
        {
            double d1 = GetMatrixValue(v10, v11, v12, v20);
            double d2 = GetMatrixValue(v10, v11, v12, v21);
            double d3 = GetMatrixValue(v10, v11, v12, v22);

            int Value = Test(d1, d2, d3, v10, v11, v12, v20, v21, v22);
            if (Value == 0)
            {
                return false;
            }
            else
            {
                if (Value == 1)
                {
                    return true;
                }
            }
            double d4 = GetMatrixValue(v20, v21, v22, v10);
            double d5 = GetMatrixValue(v20, v21, v22, v11);
            double d6 = GetMatrixValue(v20, v21, v22, v12);

            Value = Test(d4, d5, d6, v10, v11, v12, v20, v21, v22);

            if (Value == 0)
            {
                return false;
            }
            else
            {
                if (Value == 1)
                {
                    return true;
                }
            }
            return true;
            //把V10和V20换成在三角形一侧的点
            if (d1 * d2 > 0)//V20 V21两个点在三角形的一侧 V22点在三角形的另一侧
            {
                Vector3 V3Temp = v22;
                v22 = v20;
                v20 = V3Temp;

                /////////////
                V3Temp = v22;
                v22 = v21;
                v21 = v22;
                /////////////
            }
            else
            {
                if (d1 * d3 > 0)//V20 V22两个点在三角形的一侧 V21点在三角形的另一侧
                {
                    Vector3 V3Temp = v21;
                    v21 = v20;
                    v20 = V3Temp;

                    ///////////////
                    //V3Temp = v22;
                    //v22 = v21;
                    //v21 = v22;
                    ///////////////
                }
            }
            if (d4 * d5 > 0)//V10 V11两个点在三角形的一侧 V12点在三角形的另一侧
            {
                Vector3 V3Temp = v12;
                v12 = v10;
                v10 = V3Temp;
                ///////////////
                //V3Temp = v12;
                //v12 = v11;
                //v11 = v12;
                ///////////////
            }
            else
            {
                if (d4 * d6 > 0)//V10 V12两个点在三角形的一侧 V11点在三角形的另一侧
                {
                    Vector3 V3Temp = v11;
                    v11 = v10;
                    v10 = V3Temp;
                    ///////////////
                    //V3Temp = v12;
                    //v12 = v11;
                    //v11 = v12;
                    ///////////////
                }
            }

            //计算法线，然后置换V11,V12 和V21 V22
            Vector3 N1 = Vector3.Cross((v11 - v10), (v12 - v11));
            Vector3 N2 = Vector3.Cross((v21 - v20), (v22 - v21));

            N1.Normalize();
            N2.Normalize();

            float r = Vector3.Dot(N1, N2);

            //float r1 = Vector3.Dot(v20 - v10, N2);
            //float r2 = Vector3.Dot(v10 - v20, N1);
            if (r > 0)
            {
                Vector3 V3Temp = v22;
                v22 = v21;
                v21 = V3Temp;
            }
            //if (r2 > 0)
            //{
            //    Vector3 V3Temp = v12;
            //    v12 = v11;
            //    v11 = V3Temp;
            //}
            if (GetMatrixValue(v10, v11, v20, v21) <= 0 && GetMatrixValue(v10, v12, v22, v20) <= 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        /// <summary>
        /// 获取行列式的值
        /// </summary>
        /// <returns></returns>
        private double GetMatrixValue(Vector3 a, Vector3 b, Vector3 c, Vector3 d)
        {
            Microsoft.DirectX.Matrix m = new Microsoft.DirectX.Matrix();
            m.M11 = a.X;
            m.M12 = a.Y;
            m.M13 = -a.Z;
            m.M14 = 1;

            m.M21 = b.X;
            m.M22 = b.Y;
            m.M23 = -b.Z;
            m.M24 = 1;

            m.M31 = c.X;
            m.M32 = c.Y;
            m.M33 = -c.Z;
            m.M34 = 1;

            m.M41 = d.X;
            m.M42 = d.Y;
            m.M43 = -d.Z;
            m.M44 = 1;
            return m.Determinant;
        }


        /// <summary>
        /// 测试点是否通过 
        /// </summary>
        /// <returns>0 不相交 1 相交 2 继续判断</returns>
        private int Test(double d1, double d2, double d3, Vector3 v10, Vector3 v11, Vector3 v12, Vector3 v20, Vector3 v21, Vector3 v22)
        {

            //同号，且非0
            if ((d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0))
            {
                return 0;//不相交
            }
            //共面
            if (d1 == 0 && d2 == 0 && d3 == 0)
            {
                //投影到一个平面上做二维相交测试
                if (TriangleTriangle2D(v10, v11, v12, v20, v21, v22))
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                //其中为零的点在三角形平面内
                if ((d1 == 0 && d2 > 0 && d3 > 0) || (d1 == 0 && d2 < 0 && d3 < 0))//V20在三角形T1平面内
                {
                    if (PointTriangle2D(v10, v11, v12, v20))
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    if ((d2 == 0 && d1 > 0 && d3 > 0) || (d2 == 0 && d1 < 0 && d3 < 0))//V21在三角形T1平面内
                    {

                        if (PointTriangle2D(v10, v11, v12, v21))
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        if ((d3 == 0 && d1 > 0 && d2 > 0) || (d3 == 0 && d1 < 0 && d2 < 0))//V22在三角形T1平面内
                        {
                            if (PointTriangle2D(v10, v11, v12, v22))
                            {
                                return 1;
                            }
                            else
                            {
                                return 0;
                            }
                        }
                        else
                        {
                            if (d1 == 0 && d2 == 0)//v20和v21两个点都在三角形T1平面内
                            {
                                if (LineTriangle2D(v10, v11, v12, v20, v21))
                                {
                                    return 1;
                                }
                                else
                                {
                                    return 0;
                                }
                            }
                            else
                            {
                                if (d1 == 0 && d3 == 0)//v20和v22两个点都在三角形T1平面内
                                {
                                    if (LineTriangle2D(v10, v11, v12, v20, v22))
                                    {
                                        return 1;
                                    }
                                    else
                                    {
                                        return 0;
                                    }
                                }
                                else
                                {
                                    if (d2 == 0 && d3 == 0)//v21和v22两个点都在三角形T1平面内
                                    {
                                        if (LineTriangle2D(v10, v11, v12, v21, v22))
                                        {
                                            return 1;
                                        }
                                        else
                                        {
                                            return 0;
                                        }
                                    }
                                    else
                                    {
                                        return 2;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        /// <summary>
        /// 生成3D三角形投影到本身平面的矩阵
        /// </summary>
        /// <returns>投影到本身平面的矩阵</returns>
        Matrix Get3DTo2DMatrix(Vector3 v0, Vector3 v1, Vector3 v2)
        {
            Matrix mat;
            Vector3 v10 = v1 - v0;
            Vector3 v21 = v2 - v1;
            v10.Normalize();
            v21.Normalize();
            Vector3 vn = Vector3.Cross(v10, v21);
            vn.Normalize();
            mat = Matrix.LookAtLH(v0, v0 + vn, v10);
            return mat;
        }
        /// <summary>
        /// 测试2D点是否在三角形内
        /// </summary>
        private bool PointTriangle2D(Vector3 v10, Vector3 v11, Vector3 v12, Vector3 v)
        {
            Matrix m = Get3DTo2DMatrix(v10, v11, v12);
            Vector3 NewV10 = Vector3.TransformCoordinate(v10, m);
            Vector3 NewV11 = Vector3.TransformCoordinate(v11, m);
            Vector3 NewV12 = Vector3.TransformCoordinate(v12, m);
            Vector3 NewV = Vector3.TransformCoordinate(v, m);
            //判断面积是否相等
            if (Math.Abs(GetTriangleSpace(NewV10.X, NewV10.Y, NewV11.X, NewV11.Y, NewV12.X, NewV12.Y)) ==
                Math.Abs(GetTriangleSpace(NewV10.X, NewV10.Y, NewV11.X, NewV11.Y, NewV.X, NewV.Y)) +
                    Math.Abs(GetTriangleSpace(NewV10.X, NewV10.Y, NewV12.X, NewV12.Y, NewV.X, NewV.Y)) +
                        Math.Abs(GetTriangleSpace(NewV11.X, NewV11.Y, NewV12.X, NewV12.Y, NewV.X, NewV.Y)))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        /// <summary>
        /// 获取平面三角形面积
        /// </summary>
        /// <returns>三角形面积</returns>
        private double GetTriangleSpace(float x1, float y1, float x2, float y2, float x3, float y3)
        {
            return (x1 - x3) * (y2 - y3) - (y1 - y3) * (x2 - x3);
        }
        /// <summary>
        /// 测试2D线是否与三角形相交
        /// </summary>
        private bool LineTriangle2D(Vector3 v10, Vector3 v11, Vector3 v12, Vector3 LV1, Vector3 LV2)
        {
            Matrix m = Get3DTo2DMatrix(v10, v11, v12);
            Vector3 NewV10 = Vector3.TransformCoordinate(v10, m);
            Vector3 NewV11 = Vector3.TransformCoordinate(v11, m);
            Vector3 NewV12 = Vector3.TransformCoordinate(v12, m);
            Vector3 NewLV1 = Vector3.TransformCoordinate(LV1, m);
            Vector3 NewLV2 = Vector3.TransformCoordinate(LV2, m);
            //测试三角形三条边与线段是否有相交
            if (LineLine2D(NewV10.X, NewV10.Y, NewV11.X, NewV11.Y, NewLV1.X, NewLV1.Y, NewLV2.X, NewLV2.Y))
            {
                return true;
            }
            if (LineLine2D(NewV10.X, NewV10.Y, NewV12.X, NewV12.Y, NewLV1.X, NewLV1.Y, NewLV2.X, NewLV2.Y))
            {
                return true;
            }
            if (LineLine2D(NewV11.X, NewV11.Y, NewV12.X, NewV12.Y, NewLV1.X, NewLV1.Y, NewLV2.X, NewLV2.Y))
            {
                return true;
            }
            return false;
        }
        /// <summary>
        /// 测试2D三角形是否与三角形相交
        /// </summary>
        /// <returns></returns>
        private bool TriangleTriangle2D(Vector3 v10, Vector3 v11, Vector3 v12, Vector3 TV1, Vector3 TV2, Vector3 TV3)
        {
            if (LineTriangle2D(v10, v11, v12, TV1, TV2))
            {
                return true;
            }
            if (LineTriangle2D(v10, v11, v12, TV1, TV3))
            {
                return true;
            }
            if (LineTriangle2D(v10, v11, v12, TV2, TV3))
            {
                return true;
            }
            return false;
        }
        /// <summary>
        /// 测试平面内2线段是否相交
        /// </summary>
        /// <returns></returns>
        private bool LineLine2D(float X1, float Y1, float X2, float Y2, float LX1, float LY1, float LX2, float LY2)
        {
            double R = (double)((Y1 - LY1) * (LX2 - LX1) - (X1 - LX1) * (LY2 - LY1)) / (double)((X2 - X1) * (LY2 - LY1) - (Y2 - Y1) * (LX2 - LX1));
            double S = (double)((Y1 - LY1) * (X2 - X1) - (X1 - LX1) * (Y2 - Y1)) / (double)((X2 - X1) * (LY2 - LY1) - (Y2 - Y1) * (LX2 - LX1));
            if ((R >= 0 && R <= 1) && (S >= 0 && S <= 1))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
