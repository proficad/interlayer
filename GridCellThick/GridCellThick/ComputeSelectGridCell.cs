using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using EclipseGridIO;

namespace GridCellThick
{
    class ComputeSelectGridCell
    {
        #region 计算网格加密

        /// <summary>
        /// 计算网格加密
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="NX">NX方向加密个数</param>
        /// <param name="NY">NY方向加密个数</param>
        /// <param name="NZ">NZ方向加密个数</param>
        /// <param name="LayerList">选中层列表</param>
        /// <param name="Name">加密名字</param>
        /// <param name="SelectedGridCell">带有选中网格的列表</param>
        public static void ComputeGridCell(GridData gd, List<View.GridCell.GridCell> SelectedGridCell, int NX, int NY, int NZ, List<int> LayerList, string Name)
        {
            if (SelectedGridCell.Count != gd.GridX * gd.GridY)
            {
                Exception ee = new Exception("网格个数不匹配");
                throw ee;
            }
            List<DGridData> DGridDataList = new List<DGridData>();

            int DGridDataListCount = gd.DGridDataList.Count;

            #region 把每一层，每一行都拆分成加密格式 

            for (int i = 0; i < gd.GridY; i++)
            {
                int StartX = -1;
                int EndX = -1;
                for (int j = 0; j < gd.GridX; j++)
                {
                    if (SelectedGridCell[i * gd.GridX + j].IsSelected)
                    {
                        if (StartX == -1)
                        {
                            StartX = j;
                            EndX = j;
                        }
                        else
                        {
                            if (j - EndX == 1)
                            {
                                EndX = j;
                            }
                        }
                    }
                    else
                    {
                        if (StartX != -1)
                        {
                            DGridData dgd = new DGridData();
                            dgd.X1 = StartX + 1;
                            dgd.X2 = EndX + 1;
                            dgd.Y1 = i + 1;
                            dgd.Y2 = i + 1;
                            dgd.DX = NX * (dgd.X2 - dgd.X1 + 1);
                            dgd.DY = NY * (dgd.Y2 - dgd.Y2 + 1);
                            DGridDataList.Add(dgd);
                            StartX = EndX = -1;
                        }
                    }
                }
                if (EndX == gd.GridX - 1)
                {
                    DGridData dgd = new DGridData();
                    dgd.X1 = StartX + 1;
                    dgd.X2 = EndX + 1;
                    dgd.Y1 = i + 1;
                    dgd.Y2 = i + 1;
                    dgd.DX = NX * (dgd.X2 - dgd.X1 + 1);
                    dgd.DY = NY * (dgd.Y2 - dgd.Y2 + 1);
                    DGridDataList.Add(dgd);
                }
            }
            #endregion

            #region 判断层是否连续，如果不连续，则存储

            int StarZ = -1;
            int EndZ = -1;

            for (int i = 0; i < LayerList.Count; i++)
            {
                if (StarZ == -1)
                {
                    StarZ = LayerList[i];
                    EndZ = LayerList[i];
                }
                else
                {
                    if (LayerList[i] - EndZ == 1)
                    {
                        EndZ = LayerList[i];
                    }
                    else
                    {
                        for (int j = 0; j < DGridDataList.Count; j++)
                        {
                            DGridData dgd = new DGridData();
                            dgd.X1 = DGridDataList[j].X1;
                            dgd.X2 = DGridDataList[j].X2;
                            dgd.Y1 = DGridDataList[j].Y1;
                            dgd.Y2 = DGridDataList[j].Y2;
                            dgd.Z1 = StarZ + 1;
                            dgd.Z2 = EndZ + 1;
                            dgd.DX = DGridDataList[j].DX;
                            dgd.DY = DGridDataList[j].DY;
                            dgd.DZ = (dgd.Z2 - dgd.Z1 + 1) * NZ;
                            dgd.NWMAX = 1;
                            dgd.GLOBAL = "GLOBAL";
                            gd.DGridDataList.Add(dgd);
                        }
                        StarZ = EndZ = -1;
                    }
                }
            }
            if (EndZ == LayerList[LayerList.Count- 1] )
            {
                for (int j = 0; j < DGridDataList.Count; j++)
                {
                    DGridData dgd = new DGridData();
                    dgd.X1 = DGridDataList[j].X1;
                    dgd.X2 = DGridDataList[j].X2;
                    dgd.Y1 = DGridDataList[j].Y1;
                    dgd.Y2 = DGridDataList[j].Y2;
                    dgd.Z1 = StarZ + 1;
                    dgd.Z2 = EndZ + 1;
                    dgd.DX = DGridDataList[j].DX;
                    dgd.DY = DGridDataList[j].DY;
                    dgd.DZ = (dgd.Z2 - dgd.Z1 + 1) * NZ;
                    dgd.NWMAX = 1;
                    dgd.GLOBAL = "GLOBAL";
                    gd.DGridDataList.Add(dgd);
                }
            }

            #endregion

            #region 添加名字

            LGRName lgrn = new LGRName();
            int NameLength = (gd.DGridDataList.Count - DGridDataListCount).ToString().Length;
            if ((gd.DGridDataList.Count - DGridDataListCount).ToString().Length + Name.Length > 9)
            {
                Name.Substring(0, 8 - NameLength);
            }
            else
            {
                NameLength = 8 - Name.Length;
            }
            for (int i = DGridDataListCount; i < gd.DGridDataList.Count; i++)
            {
                lgrn.ID.Add(i);

                //使用随机文件名来进行生成无重复名字
                gd.DGridDataList[i].Name = Name + System.IO.Path.GetRandomFileName().Substring(0, NameLength);
                
                //重名状况没有考虑，一会再写

                //if (!CheckLGRName(gd.DGridDataList, Name + (i - DGridDataListCount).ToString()))
                //{
                //    gd.DGridDataList[i].Name = Name + (i - DGridDataListCount).ToString();
                //}
                //else
                //{ 
                //    //gd.DGridDataList[i].Name=
                //}
            }
            gd.DNameList.Add(lgrn);

            #endregion
        }

        #endregion

        #region 检测是否有重复名字

        /// <summary>
        /// 检测是否有重复名字
        /// </summary>
        /// <param name="DGridDataList">DGridDataList</param>
        /// <param name="DName">名字</param>
        /// <returns>是否有重复的</returns>
        private static bool CheckLGRName(List<DGridData> DGridDataList,string DName)
        {
            for (int i = 0; i < DGridDataList.Count; i++)
            {
                if (DGridDataList[i].Name == DName)
                {
                    return true;
                }
            }
            return false;
        }

        #endregion
    }
}

