using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.DirectX;

namespace Track
{
    /// <summary>
    /// 检测直线与四边形相交类
    /// </summary>
    class LineQuadIntersect
    {

        #region 直线相交检测

        /// <summary>
        /// 判断空间内直线与四边形相交情况
        /// </summary>
        /// <returns>是否相交，如果相交返回True，如果不相交返回False</returns>
        public bool IntersectLineQuad(double PX, double PY, double PZ, double QX, double QY, double QZ, double AX, double AY, double AZ,
                                        double BX, double BY, double BZ, double CX, double CY, double CZ, double DX, double DY, double DZ,
                                            ref double X, ref double Y, ref double Z)
        {
            Vector3 p = new Vector3(Convert.ToSingle(PX), Convert.ToSingle(PY), Convert.ToSingle(PZ));
            Vector3 q = new Vector3(Convert.ToSingle(QX), Convert.ToSingle(QY), Convert.ToSingle(QZ));
            Vector3 a = new Vector3(Convert.ToSingle(AX), Convert.ToSingle(AY), Convert.ToSingle(AZ));
            Vector3 b = new Vector3(Convert.ToSingle(BX), Convert.ToSingle(BY), Convert.ToSingle(BZ));
            Vector3 c = new Vector3(Convert.ToSingle(CX), Convert.ToSingle(CY), Convert.ToSingle(CZ));
            Vector3 d = new Vector3(Convert.ToSingle(DX), Convert.ToSingle(DY), Convert.ToSingle(DZ));
            //return IntersectLineQuad(p, q, a, b, c, d, ref X, ref Y, ref Z);
            if (Intersect(a, b, c,p, q) || Intersect(d, a, c,p, q))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        /// <summary>
        /// 判断空间内直线与四边形相交情况
        /// </summary>
        /// <returns>是否相交，如果相交返回True，如果不相交返回False</returns>
        public bool IntersectLineQuad(Vector3 p, Vector3 q, Vector3 a, Vector3 b, Vector3 c, Vector3 d, ref double X, ref double Y, ref double Z)
        {
            Vector3 pq = q - p;
            Vector3 pa = a - p;
            Vector3 pb = b - p;
            Vector3 pc = c - p;
            Vector3 m = Vector3.Cross(pc, pq);
            Vector3 RetV = new Vector3();

            float v = Vector3.Dot(pa, m);
            if (v >= 0)
            {
                float u = -Vector3.Dot(pb, m);
                if (u < 0)
                {
                    return false;
                }
                float w = ScalarTriple(pq, pb, pa);
                if (w < 0)
                {
                    return false;
                }
                float denom = 1.0f / (u + v + w);
                u = denom * u;
                v = denom * v;
                w = denom * w;
                RetV = u * a + v * b + w * c;
                X = RetV.X;
                Y = RetV.Y;
                Z = RetV.Z;
            }
            else
            {
                Vector3 pd = d - p;
                float u = -Vector3.Dot(pd, m);
                if (u < 0)
                {
                    return false;
                }
                float w = ScalarTriple(pq, pa, pd);
                if (w < 0)
                {
                    return false;
                }
                v = -v;
                float denom = 1.0f / (u + v + w);
                u = denom * u;
                v = denom * v;
                w = denom * w;
                RetV = u * a + v * b + w * c;
                X = RetV.X;
                Y = RetV.Y;
                Z = RetV.Z;
            }
            return true;
        }

        #endregion

        /// <summary>
        /// 标量三重积
        /// </summary>
        /// <returns>标量三重积</returns>
        private float ScalarTriple(Vector3 u, Vector3 v, Vector3 w)
        {
            return Vector3.Dot(Vector3.Cross(u, v), w);
        }

        #region 三角形与线段相交检测

        public bool Intersect(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 _s, Vector3 _d)
        {
            Vector3 v12 = v2 - v1;
            Vector3 n12 = Vector3.Cross(v12, _d);
            float ds12 = Vector3.Dot((_s - v1), n12);
            float d312 = Vector3.Dot((v3 - v1), n12);
            if (d312 >= 0.0f)
            {
                if (ds12 < 0.0f)
                {
                    return false;
                }
                if (ds12 > d312)
                {
                    return false;
                }
            }
            else
            {
                if (ds12 > 0.0f)
                {
                    return false;
                }
                if (ds12 > d312)
                {
                    return false;
                }
            }
            Vector3 v23 = v3 - v2;
            Vector3 n23 = Vector3.Cross(v23, _d);
            float ds23 = Vector3.Dot((_s - v2), n23);
            float d123 = Vector3.Dot((v1 - v2), n23);
            if (d123 >= 0.0f)
            {
                if (ds23 < 0.0f)
                {
                    return false;
                }
                if (ds23 > d123)
                {
                    return false;
                }
            }
            else                     // d123 < 0
            {
                if (ds23 > 0.0f)
                {
                    return false;
                }
                if (ds23 < d123)
                {
                    return false;
                }
            }
            Vector3 v31 = v1 - v3;
            Vector3 n31 = Vector3.Cross(v31, _d);
            float ds31 = Vector3.Dot(_s - v3, n31);
            float d231 = Vector3.Dot((v2 - v3), n31);
            if (d231 >= 0.0f)
            {
                if (ds31 < 0.0f)
                {
                    return false;
                }
                if (ds31 > d231)
                {
                    return false;
                }
            }
            else                     // d231 < 0
            {
                if (ds31 > 0.0f)
                {
                    return false;
                }
                if (ds31 < d231)
                {
                    return false;
                }
            } 
            float r3;
            if (ds12 == 0.0f)
            {
                r3 = 0.0f;
            }
            else
            {
                if (d312 != 0.0f)
                {
                    r3 = ds12 / d312;
                }
                else
                {
                    return false;
                }
            }
            float r1;
            if (ds23 == 0.0f)
            {
                r1 = 0.0f;
            }
            else
            {
                if (d123 != 0.0f)
                {
                    r1 = ds23 / d123;
                }
                else
                {
                    return false;
                }
            }
            float r2;
            if (ds31 == 0.0f)
            {
                r2 = 0.0f;
            }
            else
            {
                if (d231 != 0.0f)
                {
                    r2 = ds31 / d231;
                }
                else
                {
                    return false; // the triangle and the line must be parallel intersection. 
                }
            }
            float total_r = (r1 + r2 + r3);
            if (total_r != 1.0f)
            {
                if (total_r == 0.0f)
                {
                    return false; // the triangle and the line must be parallel intersection.
                }
                float inv_total_r = 1.0f / total_r;
                r1 *= inv_total_r;
                r2 *= inv_total_r;
                r3 *= inv_total_r;
            } 
            Vector3 Vec3In=v1*r1+v2*r2+v3*r3;        //线段所在直线和三角形的交点坐标
            float d = Vector3.Dot((Vec3In - _s), _d);
            if (d < 0.0f)
            {
                return false;
            }
            if (d > (_s-_d).Length())
            {
                return false;
            }
            return true;
        }

        #endregion
    }
}
