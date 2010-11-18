using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using View;
using EclipseGridIO;
using System.Drawing;
namespace GridCellThick
{
    class AddGridCellData
    {
        private float _GridCellWidth = 0;//网格宽度
        private float _GridCellHeight = 0;//网格高度

        #region 添加网格数据

        /// <summary>
        /// 添加网格数据
        /// </summary>
        /// <param name="gd">GridData数据</param>
        /// <param name="LayerID">显示的层号</param>
        /// <returns>GridCell的List数据</returns>
        public List<View.GridCell.GridCell> AddGridDataInfo(GridData gd,int LayerID)
        {
            List<View.GridCell.GridCell> GridCellList = new List<View.GridCell.GridCell>();//单元格列表

            float XMax = -999999999;
            float YMax = -999999999;
            float XMin = 999999999;
            float YMin = 999999999;
            for (int i = 0; i < gd.GridX * gd.GridY; i++)
            {
                View.GridCell.GridCell gc = new View.GridCell.GridCell();

                gc.X1 = Convert.ToSingle(gd.PointX[i * 8 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.X2 = Convert.ToSingle(gd.PointX[i * 8 + 1 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.X3 = Convert.ToSingle(gd.PointX[i * 8 + 2 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.X4 = Convert.ToSingle(gd.PointX[i * 8 + 3 + LayerID * gd.GridX * gd.GridY * 8]);

                gc.Y1 = Convert.ToSingle(gd.PointY[i * 8 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Y2 = Convert.ToSingle(gd.PointY[i * 8 + 1 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Y3 = Convert.ToSingle(gd.PointY[i * 8 + 2 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Y4 = Convert.ToSingle(gd.PointY[i * 8 + 3 + LayerID * gd.GridX * gd.GridY * 8]);

                gc.Z1 = Convert.ToSingle(gd.PointZ[i * 8 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Z2 = Convert.ToSingle(gd.PointZ[i * 8 + 1 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Z3 = Convert.ToSingle(gd.PointZ[i * 8 + 2 + LayerID * gd.GridX * gd.GridY * 8]);
                gc.Z4 = Convert.ToSingle(gd.PointZ[i * 8 + 3 + LayerID * gd.GridX * gd.GridY * 8]);

                for (int t = 0; t < gd.DNameList.Count; t++)
                {
                    for (int j = 0; j < gd.DNameList[t].ID.Count; j++)
                    {
                        if (gd.DGridDataList[gd.DNameList[t].ID[j]].X1 <= (i % gd.GridX + 1) && (i % gd.GridX + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].X2)
                        {
                            if (gd.DGridDataList[gd.DNameList[t].ID[j]].Y1 <= (i / gd.GridX + 1) && (i / gd.GridX + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Y2)
                            {
                                if (gd.DGridDataList[gd.DNameList[t].ID[j]].Z1 <= (LayerID + 1) && (LayerID + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Z2)
                                {
                                    gc.DX = gd.DGridDataList[gd.DNameList[t].ID[j]].DX / ((gd.DGridDataList[gd.DNameList[t].ID[j]].X2 - gd.DGridDataList[gd.DNameList[t].ID[j]].X1) + 1);
                                    gc.DY = gd.DGridDataList[gd.DNameList[t].ID[j]].DY / ((gd.DGridDataList[gd.DNameList[t].ID[j]].Y2 - gd.DGridDataList[gd.DNameList[t].ID[j]].Y1) + 1);
                                    gc.DZ = gd.DGridDataList[gd.DNameList[t].ID[j]].DZ / ((gd.DGridDataList[gd.DNameList[t].ID[j]].Z2 - gd.DGridDataList[gd.DNameList[t].ID[j]].Z1) + 1);
                                    gc.DGridCellName = gd.DGridDataList[gd.DNameList[t].ID[0]].Name;
                                }
                            }
                        }
                    }
                }
                    //for (int i = Convert.ToInt32(StrList[5]) - 1; i < Convert.ToInt32(StrList[6]); i++)//Z方向循环
                    //{
                    //    for (int j = Convert.ToInt32(StrList[3]) - 1; j < Convert.ToInt32(StrList[4]); j++)//Y方向循环
                    //    {
                    //        for (int k = Convert.ToInt32(StrList[1]) - 1; k < Convert.ToInt32(StrList[2]); k++)//X方向
                    //        {
                    //            gd.DPointX[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[7]) / (Convert.ToInt32(StrList[2]) - Convert.ToInt32(StrList[1]) + 1);
                    //            gd.DPointY[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[8]) / (Convert.ToInt32(StrList[4]) - Convert.ToInt32(StrList[3]) + 1);
                    //            gd.DPointZ[gd.GridX * gd.GridY * i + gd.GridX * j + k] = Convert.ToInt32(StrList[9]) / (Convert.ToInt32(StrList[6]) - Convert.ToInt32(StrList[5]) + 1);
                    //        }
                    //    }
                    //}

                    //gc.DX = gd.DPointX[i + LayerID * gd.GridX * gd.GridY];
                    //gc.DY = gd.DPointY[i + LayerID * gd.GridX * gd.GridY];
                    //gc.DZ = gd.DPointZ[i + LayerID * gd.GridX * gd.GridY];

                gc.ZValue = (gc.Z1 + gc.Z2 + gc.Z3 + gc.Z4) / 4;
                gc.NX = i % gd.GridX;
                gc.NY = i / gd.GridX;
                gc.NZ = LayerID;
                GridCellList.Add(gc);

                //#region XMax

                //if (gc.X1 > XMax)
                //{
                //    XMax = gc.X1;
                //}
                //if (gc.X2 > XMax)
                //{
                //    XMax = gc.X2;
                //}
                //if (gc.X3 > XMax)
                //{
                //    XMax = gc.X3;
                //}
                //if (gc.X4 > XMax)
                //{
                //    XMax = gc.X4;
                //}

                //#endregion

                //#region YMax

                //if (gc.Y1 > YMax)
                //{
                //    YMax = gc.Y1;
                //}
                //if (gc.Y2 > YMax)
                //{
                //    YMax = gc.Y2;
                //}
                //if (gc.Y3 > YMax)
                //{
                //    YMax = gc.Y3;
                //}
                //if (gc.Y4 > YMax)
                //{
                //    YMax = gc.Y4;
                //}

                //#endregion

                //#region XMin

                //if (gc.X1 < XMin)
                //{
                //    XMin = gc.X1;
                //}
                //if (gc.X2 < XMin)
                //{
                //    XMin = gc.X2;
                //}
                //if (gc.X3 < XMin)
                //{
                //    XMin = gc.X3;
                //}
                //if (gc.X4 < XMin)
                //{
                //    XMin = gc.X4;
                //}

                //#endregion

                //#region YMin

                //if (gc.Y1 < YMin)
                //{
                //    YMin = gc.Y1;
                //}
                //if (gc.Y2 < YMin)
                //{
                //    YMin = gc.Y2;
                //}
                //if (gc.Y3 < YMin)
                //{
                //    YMin = gc.Y3;
                //}
                //if (gc.Y4 < YMin)
                //{
                //    YMin = gc.Y4;
                //}

                //#endregion
            }

            #region 设置网格

            float XTemp, YTemp;
            ////初始化图形宽度
            //if (XMin < 0)
            //{
            //    _GridCellWidth = XMax * 2 + XMax * 2 / 5;
            //    XTemp = XMax * 2 / 10;
            //}
            //else
            //{
            //    _GridCellWidth = (XMax - XMin) + (XMax - XMin) / 5;
            //    XTemp = (XMax - XMin) / 10;
            //}
            //if (YMin < 0)
            //{
            //    _GridCellHeight = YMax * 2 + YMax * 2 / 5;
            //    YTemp = YMax * 2 / 10;
            //}
            //else
            //{
            //    _GridCellHeight = (YMax - YMin) + (YMax - YMin) / 5;
            //    YTemp = (YMax - YMin) / 10;
            //}
            if (gd.MinX < 0)
            {
                _GridCellWidth = Convert.ToSingle(gd.MaxX * 2 + gd.MaxX * 2 / 5);
                XTemp =  Convert.ToSingle(gd.MaxX * 2 / 10);
            }
            else
            {
                _GridCellWidth =  Convert.ToSingle((gd.MaxX - gd.MinX) + (gd.MaxX - gd.MinX) / 5);
                XTemp =  Convert.ToSingle((gd.MaxX - gd.MinX) / 10);
            }
            if (gd.MinY < 0)
            {
                _GridCellHeight =  Convert.ToSingle(gd.MaxY * 2 + gd.MaxY * 2 / 5);
                YTemp =  Convert.ToSingle(gd.MaxY * 2 / 10);
            }
            else
            {
                _GridCellHeight =  Convert.ToSingle((gd.MaxY - gd.MinY) + (gd.MaxY - gd.MinY) / 5);
                YTemp = Convert.ToSingle((gd.MaxY - gd.MinY) / 10);
            }
            #endregion

            //转化坐标
            for (int i = 0; i < GridCellList.Count; i++)
            {
                GridCellList[i].X1 = GridCellList[i].X1 + XTemp;
                GridCellList[i].X2 = GridCellList[i].X2 + XTemp;
                GridCellList[i].X3 = GridCellList[i].X3 + XTemp;
                GridCellList[i].X4 = GridCellList[i].X4 + XTemp;

                GridCellList[i].Y1 = GridCellList[i].Y1 + YTemp;
                GridCellList[i].Y2 = GridCellList[i].Y2 + YTemp;
                GridCellList[i].Y3 = GridCellList[i].Y3 + YTemp;
                GridCellList[i].Y4 = GridCellList[i].Y4 + YTemp;
            }
            //GridCellList.Sort(SortZ);
            return GridCellList;
        }

        #endregion

        #region 加载加密网格名称

        /// <summary>
        /// 添加加密网格名称
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="LayerID">显示的层号</param>
        /// <returns></returns>
        public List<View.GridCell.DGridCellName> AddDGridCellNameInfo(GridData gd, int LayerID)
        {
            
            List<View.GridCell.DGridCellName> DGridCellNameList = new List<View.GridCell.DGridCellName>();

            for (int i = 0; i < gd.DNameList.Count; i++)
            {
                bool IsFind = false;//是否找到网格名称
                int MinX = 999999999;
                int MaxY = -999999999;
                for (int j = 0; j < gd.DNameList[i].ID.Count; j++)
                {
                    if (gd.DGridDataList[gd.DNameList[i].ID[j]].Z1 <= LayerID + 1 && LayerID + 1 <= gd.DGridDataList[gd.DNameList[i].ID[j]].Z2)
                    {
                        IsFind = true;
                        if (gd.DGridDataList[gd.DNameList[i].ID[j]].X1 < MinX)
                        {
                            MinX = gd.DGridDataList[gd.DNameList[i].ID[j]].X1;
                        }
                        if (gd.DGridDataList[gd.DNameList[i].ID[j]].X2 < MinX)
                        {
                            MinX = gd.DGridDataList[gd.DNameList[i].ID[j]].X2;
                        }

                        if (gd.DGridDataList[gd.DNameList[i].ID[j]].Y1 > MaxY)
                        {
                            MaxY = gd.DGridDataList[gd.DNameList[i].ID[j]].Y1;
                        }
                        if (gd.DGridDataList[gd.DNameList[i].ID[j]].Y2 > MaxY)
                        {
                            MaxY = gd.DGridDataList[gd.DNameList[i].ID[j]].Y2;
                        }
                    }
                }
                float XTemp, YTemp;
                if (gd.MinX < 0)
                {
                    XTemp = Convert.ToSingle(gd.MaxX * 2 / 10);
                }
                else
                {
                    XTemp = Convert.ToSingle((gd.MaxX - gd.MinX) / 10);
                }
                if (gd.MinY < 0)
                {
                    YTemp = Convert.ToSingle(gd.MaxY * 2 / 10);
                }
                else
                {
                    YTemp = Convert.ToSingle((gd.MaxY - gd.MinY) / 10);
                }
                if (gd.DNameList[i].ID.Count > 0)
                {
                    if (IsFind)
                    {
                        View.GridCell.DGridCellName dgcn = new View.GridCell.DGridCellName();
                        dgcn.DName = gd.DGridDataList[gd.DNameList[i].ID[0]].Name;
                        if (LayerID + 1 == gd.GridZ)
                        {
                            dgcn.X = Convert.ToSingle(gd.PointX[gd.GridX * gd.GridY * 8 * (LayerID-1) + MaxY * gd.GridX * 8 + (MinX - 1) * 8 + 3]) + XTemp;
                        }
                        else
                        {
                            dgcn.X = Convert.ToSingle(gd.PointX[gd.GridX * gd.GridY * 8 * LayerID + MaxY * gd.GridX * 8 + (MinX - 1) * 8 + 3]) + XTemp;
                        }
                        //dgcn.X = Convert.ToSingle(gd.PointX[gd.GridX * gd.GridY * 8 * LayerID + MaxY * gd.GridX * 8 + MinX * 8 + 3]);
                        //if (MaxY < gd.GridY)
                        //{
                        //    dgcn.Y = Convert.ToSingle(gd.PointY[gd.GridX * gd.GridY * 8 * LayerID + (MaxY + 1) * gd.GridX * 8 + MinX * 8]) + YTemp;
                        //    //dgcn.Y = Convert.ToSingle(gd.PointY[gd.GridX * gd.GridY * 8 * LayerID + (MaxY + 1) * gd.GridX * 8 + MinX * 8 + 3]) ;
                        //}
                        //else
                        //{
                        dgcn.Y = Convert.ToSingle(gd.PointY[gd.GridX * gd.GridY * 8 * LayerID + (MaxY - 1) * gd.GridX * 8 + (MinX - 1) * 8 + 3]) + YTemp;
                        //dgcn.Y = Convert.ToSingle(gd.PointY[gd.GridX * gd.GridY * 8 * LayerID + MaxY * gd.GridX * 8 + MinX * 8 + 3]);
                        //}
                        DGridCellNameList.Add(dgcn);
                    }
                }
            }
            return DGridCellNameList;

        }
        #endregion

        #region 添加网格加密选中数据

        /// <summary>
        /// 添加网格加密选中数据
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="GridCellList">网格单元列表</param>
        /// <param name="SelectedNameIndex">选中的网格名字列表索引</param>
        /// <returns></returns>
        public List<View.GridCell.GridCell> AddDGridCellInfo(GridData gd,List<View.GridCell.GridCell> GridCellList,int SelectedNameIndex)
        {
            
            for (int j = 0; j < GridCellList.Count; j++)
            {
                GridCellList[j].DIsSelected = false;
                for (int i = 0; i < gd.DNameList[SelectedNameIndex].ID.Count; i++)
                {
                    if (gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].X1 <= GridCellList[j].NX+1 && GridCellList[j].NX+1 <= gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].X2)
                    {
                        if (gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].Y1 <= GridCellList[j].NY+1 && GridCellList[j].NY + 1 <= gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].Y2)
                        {
                            if (gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].Z1 <= GridCellList[j].NZ+1 && GridCellList[j].NZ + 1 <= gd.DGridDataList[gd.DNameList[SelectedNameIndex].ID[i]].Z2)
                            {
                                GridCellList[j].DIsSelected = true;
                                break;
                            }
                        }
                    }
                }
            }

            return GridCellList;
        }

        #endregion

        #region 属性

        /// <summary>
        /// 网格宽度
        /// </summary>
        public float GridCellWidth
        {
            get { return _GridCellWidth; }
        }
        /// <summary>
        /// 网格高度
        /// </summary>
        public float GridCellHeight
        {
            get { return _GridCellHeight; }
        }
        
        #endregion
    }
}
