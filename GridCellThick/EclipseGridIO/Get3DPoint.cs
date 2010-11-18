using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    class Get3DPoint
    {
        /// <summary>
        /// 获取3D点的值，使用空间内直线方程（两点式）
        /// </summary>
        /// <param name="x1">x1</param>
        /// <param name="y1">y1</param>
        /// <param name="z1">z1</param>
        /// <param name="z2">z2</param>
        /// <param name="z">Z值</param>
        /// <returns></returns>
        public static double Get3DPointValue(double x1, double z1, double x2, double z2, double z)
        {
            // 直线过两点:(x1, y1, z1)及(x2, y2, z2)，则
            //
            //  			   x - x1   y - y1   z - z1
            // 空间直线方程为：------ = ------ = ------
            //  			   x2 -x1	y2 -y1   z2 -z1
            // 
            // 则，已知z求得x,y 如下：
            //     x = (z-z1)(x2-x1)/(z2-z1) + x1
            //     y = (z-z1)(y2-y1)/(z2-z1) + y1

            return (z - z1) * (x2 - x1) / (z2 - z1) + x1;
        }
    }
}
