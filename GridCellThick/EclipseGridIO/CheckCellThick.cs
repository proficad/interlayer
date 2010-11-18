using System;
using System.Collections.Generic;
using System.Text;

namespace EclipseGridIO
{
    public class CheckCellThick
    {
        /// <summary>
        /// 检查是否有单元格加密过
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="NX">NX</param>
        /// <param name="NY">NY</param>
        /// <param name="NZ">NZ</param>
        /// <returns>是否加密</returns>
        public static bool IsCheckCellThick(GridData gd, int NX, int NY, int NZ)
        {
            for (int t = 0; t < gd.DNameList.Count; t++)
            {
                for (int j = 0; j < gd.DNameList[t].ID.Count; j++)
                {
                    if (gd.DGridDataList[gd.DNameList[t].ID[j]].X1 <= (NX + 1) && (NX + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].X2)
                    {
                        if (gd.DGridDataList[gd.DNameList[t].ID[j]].Y1 <= (NY + 1) && (NY + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Y2)
                        {
                            if (gd.DGridDataList[gd.DNameList[t].ID[j]].Z1 <= (NZ + 1) && (NZ + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Z2)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
    }
}
