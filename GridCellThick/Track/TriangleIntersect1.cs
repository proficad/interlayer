using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX;

namespace Track
{
    class TriangleIntersect1
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
            Vector3[] E0 = new Vector3[3];
            E0[0] = v11 - v10;
            E0[1] = v12 - v11;
            E0[2] = v10 - v12;
            
            Vector3 N0 = Vector3.Cross(E0[0], E0[1]);
            N0.Normalize();
            float N0dT0V0 = Vector3.Dot(N0, v10);
            float min1=0, max1=0;
            ProjectOntoAxis(ref v20, ref v21, ref v22, ref N0, ref min1, ref max1);
            if (N0dT0V0 < min1 || N0dT0V0 > max1)
            {
                return false;
            }
            Vector3[] E1 = new Vector3[3];
            E1[0] = v21 - v20;
            E1[1] = v22 - v21;
            E1[2] = v20 - v22;
            Vector3 N1 = Vector3.Cross(E1[0], E1[1]);
            N1.Normalize();
            Vector3 dir;
            float min0=0, max0=0;
            int i0, i1;
            Vector3 N0xN1 = Vector3.Cross(N0, N1);
            N0xN1.Normalize();
            if (Vector3.Dot(N0xN1, N0xN1) >= 1e-6)
            {
                float N1dT1V0 = Vector3.Dot(N1, v20);
                ProjectOntoAxis(ref v10, ref v11, ref v12, ref N1, ref min0, ref max0);
                if (N1dT1V0 < min0 || N1dT1V0 > max0)
                {
                    return false;
                }
                for (i1 = 0; i1 < 3; i1++)
                {
                    for (i0 = 0; i0 < 3; i0++)
                    {
                        dir = Vector3.Cross(E0[i0], E1[i1]);
                        dir.Normalize();
                        ProjectOntoAxis(ref v10, ref v11, ref v12, ref dir, ref min0, ref max0);
                        ProjectOntoAxis(ref v20, ref v21, ref v22, ref dir, ref min1, ref max1);
                        if (max0 < min1 || max1 < min0)
                        {
                            return false;
                        }
                    }
                }
            }
            else
            {
                for (i0 = 0; i0 < 3; i0++)
                {
                    dir = Vector3.Cross(N0, E0[i0]);
                    dir.Normalize();
                    ProjectOntoAxis(ref v10, ref v11, ref v12, ref dir, ref min0,ref  max0);
                    ProjectOntoAxis(ref v20, ref v21, ref v22, ref dir, ref min1,ref  max1);
                    if (max0 < min1 || max1 < min0)
                    {
                        return false;
                    }
                }
                for (i1 = 0; i1 < 3; i1++)
                {
                    dir = Vector3.Cross(N1, E1[i1]);
                    dir.Normalize();
                    ProjectOntoAxis(ref v10, ref v11, ref v12, ref dir, ref min0, ref max0);
                    ProjectOntoAxis(ref v20, ref v21, ref v22, ref dir, ref min1, ref max1);
                    if (max0 < min1 || max1 < min0)
                    {
                        return false;
                    }
                }
            }
            return true;
        }
        private void ProjectOntoAxis(ref Vector3 v10, ref Vector3 v11, ref Vector3 v12, ref Vector3 axis, ref float fmin, ref float fmax)
        {
            float dot0 = Vector3.Dot(axis, v10);
            float dot1 = Vector3.Dot(axis, v11);
            float dot2 = Vector3.Dot(axis, v12);
            fmin = dot0;
            fmax = fmin;
            if (dot1 < fmin)
            {
                fmin = dot1;
            }
            else if (dot1 > fmax)
            {
                fmax = dot1;
            }

            if (dot2 < fmin)
            {
                fmin = dot2;
            }
            else if (dot2 > fmax)
            {
                fmax = dot2;
            }
        }
    }
}
