using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.DirectX;

namespace Track
{
    /// <summary>
    /// 点是否在线段内
    /// </summary>
    class PointInLine
    {
        /// <summary>
        /// 点是否在线段内
        /// </summary>
        /// <returns>是否在线段内</returns>
        public static bool IsPointInLine(double P1X, double P1Y, double P1Z, double P2X, double P2Y, double P2Z,
                                            double PX, double PY, double PZ)
        {
            Vector3 p1 = new Vector3(Convert.ToSingle(P1X), Convert.ToSingle(P1Y), Convert.ToSingle(P1Z));
            Vector3 p2 = new Vector3(Convert.ToSingle(P2X), Convert.ToSingle(P2Y), Convert.ToSingle(P2Z));
            Vector3 p = new Vector3(Convert.ToSingle(PX), Convert.ToSingle(PY), Convert.ToSingle(PZ));

            return true;

            ////if (Math.Abs((p - p1).Length() + (p - p2).Length() - (p1 - p2).Length()) < (p1 - p2).Length()/100)
            //if (Math.Abs((p - p1).Length() + (p - p2).Length() - (p1 - p2).Length()) <0.01)
            //{
            //    return true;
            //}
            //else
            //{
            //    return false;
            //}

            //判断目标点是否是端点
            if (double.Equals(p1.X,p.X) && double.Equals(p1.Y,p.Y) && double.Equals(p1.Z,p.Z))
            {
                return true;
            }
            if (double.Equals(p2.X,p.X) && double.Equals(p2.Y,p.Y) && double.Equals(p2.Z,p.Z))
            {
                return true;
            }
            double d = Math.Abs((p - p1).Length() + (p - p2).Length() - (p1 - p2).Length());

            if (d > 1e-6)//(d > 1e-6)
            {
                return false;
            }

            if (!double.Equals(p1.X, p2.X))
            {
                if (((p.X - p1.X)< 0) && ((p.X - p2.X)> 0)
                    || ((p.X - p1.X) > 0) && ((p.X - p2.X) < 0))
                {
                    return true;
                }
            }
            if (!double.Equals(p1.Y, p2.Y))
            {
                if (((p.Y - p1.Y) < 0) && ((p.Y - p2.Y) > 0)
                     || ((p.Y - p1.Y) > 0) && ((p.Y - p2.Y) < 0))
                {
                    return true;
                }
            }
            if (!double.Equals(p1.Z, p2.Z))
            {
                if (((p.Z - p1.Z) < 0) && ((p.Z - p2.Z) > 0)
                     || ((p.Z - p1.Z) > 0) && ((p.Z - p2.Z) < 0))
                {
                    return true;
                }
            }

            return false;

        }
    }
}
