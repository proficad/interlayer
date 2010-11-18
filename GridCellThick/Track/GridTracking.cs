using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using EclipseGridIO;
using Microsoft.DirectX;

using OctreeSearch;//八叉树 octree

namespace Track
{
    public class GridTracking
    {
        #region 变量声明

        TriangleIntersect1 ti = new TriangleIntersect1();//空间三角形碰撞检测对象
        List<string> FaceNameList = new List<string>();//曲面文件名列表
        GridData _Grid = new GridData();//网格数据
        Face _Face = new Face();//曲面数据
        Octree _octree = null;//八叉树
        List<InFaceGridData> _GridList = new List<InFaceGridData>();//排除曲面以外的网格数据
        double GridCellValue = 0;//网格大小
        List<int> _IndexList = new List<int>();//查找到的网格数据索引
        #endregion

        #region 添加&清除曲面文件名

        /// <summary>
        /// 添加曲面文件名
        /// </summary>
        /// <param name="FaceName">曲面文件名</param>
        /// <returns>是否成功</returns>
        public bool AddFaceName(string FaceName)
        {
            try
            {
                FaceNameList.Add(FaceName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 清除曲面文件名
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearFaceName()
        {
            try
            {
                FaceNameList.Clear();
                return true;
            }
            catch
            {
                return false;
            }
        }
        #endregion

        #region 追踪

        /// <summary>
        /// 采用点方式追踪
        /// </summary>
        /// <param name="PointFileName">点文件名</param>
        /// <param name="GridFileName">网格文件名</param>
        /// <param name="SaveFileName">追踪之后的文件名</param>
        /// <returns>是否成功</returns>
        public bool UsePointTracking(string FaceFileName, string GridFileName, string SaveFileName)
        {
            try
            {
                GridIO gio = new GridIO();
                GridData gd = new GridData();
                FaceRead fr = new FaceRead();
                int GridX;
                int GridY;
                int GridZ;
                gd = gio.ReadGridDataFile(GridFileName);//读取网格文件
                GridX = gd.GridX;
                GridY = gd.GridY;
                GridZ = gd.GridZ;
                Face f = new Face();
                f = fr.VCFaceRead(FaceFileName);//读取曲面文件
                List<InFaceGridData> InFaceGridDataList = GetInFaceDataList(gd, f);//排除在曲面空间外的网格
                PointData pd = new PointData();//曲面加密点数据
                pd.GetPointData(f, gd);//获取曲面三角形加密点
                gd = null;//清除网格数据
                f = null;
                GC.Collect();
                double GridInterval = 0;
                Octree GridOctree = GetGridDataOctree(InFaceGridDataList, GridX, GridY, GridZ, ref GridInterval);//获取网格节点八叉数

                List<int> InFaceDataIndexList = Calculation(pd, InFaceGridDataList, GridOctree);//通过八叉数查找相交网格

                InFaceGridDataWrite ifgdw = new InFaceGridDataWrite();
                ifgdw.Write(SaveFileName, InFaceGridDataList, InFaceDataIndexList, GridX, GridY, GridZ);
                GC.Collect();
                return true;
            }
            catch
            {
                return false;
            }
        }

        #region  线程调用网格套接方法

        /// <summary>
        /// 读取网格文件
        /// </summary>
        /// <param name="GridFileName">网格文件名</param>
        /// <returns>是否成功</returns>
        public bool ReadGridData(string GridFileName)
        {
            try
            {
                GridIO gio = new GridIO();
                _Grid = gio.ReadGridDataFile(GridFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 读取曲面数据
        /// </summary>
        /// <param name="FaceFileName">曲面文件名</param>
        /// <returns>是否成功</returns>
        public bool ReadFaceData(string FaceFileName)
        {
            try
            {
                FaceRead fr = new FaceRead();
                _Face = fr.VCFaceRead(FaceFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 排除在曲面空间外的网格
        /// </summary>
        /// <returns>是否成功</returns>
        public bool GetInFaceDataList()
        {
            try
            {
                _GridList.Clear();
                _GridList = GetInFaceDataList(_Grid, _Face);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 创建八叉数
        /// </summary>
        /// <returns>是否成功</returns>
        public bool CreateOctree()
        {
            try
            {
                _octree = null;
                GC.Collect();
                _octree = GetGridDataOctree(_GridList, _Grid.GridX, _Grid.GridY, _Grid.GridZ, ref GridCellValue);//获取网格节点八叉数
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 通过八叉树查找相交网格
        /// </summary>
        /// <returns>是否成功</returns>
        public bool GetIndexList()
        {
            try
            {
                _IndexList.Clear();
                _IndexList = Calculation(_Face, _GridList, _octree, GridCellValue);
                return true;
            }
            catch
            {
                return false;
            }
        }
        
        /// <summary>
        /// 生成网格数据文件
        /// </summary>
        /// <param name="SaveFileName">保存文件名</param>
        /// <returns>是否成功</returns>
        public bool WriteData(string SaveFileName)
        {
            try
            {
                InFaceGridDataWrite ifgdw = new InFaceGridDataWrite();
                ifgdw.Write(SaveFileName, _GridList, _IndexList, _Grid.GridX, _Grid.GridY, _Grid.GridZ);
                return true;
            }
            catch
            {
                return false;
            }

        }
        /// <summary>
        /// 清理数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearData()
        {
            try
            {
                _Face = null;
                _octree = null;
                _GridList.Clear();
                _IndexList.Clear();
                GC.Collect();
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 清理网格数据
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearGridData()
        {
            try
            {
                _Grid = null;
                GC.Collect();
                return true;
            }
            catch
            {
                return false;
            }
        }

        #endregion

        /// <summary>
        /// 追踪
        /// </summary>
        /// <param name="FaceFileName">曲面文件名</param>
        /// <param name="GridFileName">网格文件名</param>
        /// <param name="SaveFileName">追踪之后的文件名</param>
        /// <returns>是否成功</returns>
        public bool Tracking(string FaceFileName, string GridFileName, string SaveFileName)
        {
            try
            {
                GridIO gio = new GridIO();
                GridData gd = new GridData();
                FaceRead fr = new FaceRead();
                int GridX;
                int GridY;
                int GridZ;

                gd = gio.ReadGridDataFile(GridFileName);//读取网格文件
                GridX = gd.GridX;
                GridY = gd.GridY;
                GridZ = gd.GridZ;

                Face f = new Face();

                f = fr.VCFaceRead(FaceFileName);//读取曲面文件

                List<InFaceGridData> InFaceGridDataList = GetInFaceDataList(gd, f);//排除在曲面空间外的网格

                gd = null;//清除网格数据
                GC.Collect();

                double GridInterval = 0;
                Octree GridOctree = GetGridDataOctree(InFaceGridDataList, GridX, GridY, GridZ, ref GridInterval);//获取网格节点八叉数

                List<int> InFaceDataIndexList = Calculation(f, InFaceGridDataList, GridOctree, GridInterval);//通过八叉数查找相交网格

                InFaceGridDataWrite ifgdw = new InFaceGridDataWrite();
                ifgdw.Write(SaveFileName, InFaceGridDataList, InFaceDataIndexList, GridX, GridY, GridZ);

                GC.Collect();

                return true;
            }
            catch
            {
                return false;
            }
        }


        /// <summary>
        /// 追踪
        /// </summary>
        /// <param name="GridFileName">网格文件名</param>
        /// <param name="SaveFileName">追踪之后的文件名</param>
        /// <returns>是否成功</returns>
        public bool Tracking(string GridFileName, string SaveFileName)
        {
            try
            {
                GridIO gio = new GridIO();
                GridData gd = new GridData();
                FaceRead fr = new FaceRead();
                int GridX;
                int GridY;
                int GridZ;

                gd = gio.ReadGridDataFile(GridFileName);//读取网格文件
                GridX = gd.GridX;
                GridY = gd.GridY;
                GridZ = gd.GridZ;

                for (int i = 0; i < gd.PointY.Length; i++)
                {
                    gd.PointY[i] = -gd.PointY[i];
                    gd.PointZ[i] = -gd.PointZ[i];
                }
                List<InFaceGridData> SaveFileInFaceGridDataList = new List<InFaceGridData>();
                for (int i = 0; i > FaceNameList.Count; i++)
                {
                    Face f = new Face();

                    f = fr.VCFaceRead(FaceNameList[i]);//读取曲面文件

                    List<InFaceGridData> InFaceGridDataList = GetInFaceDataList(gd, f);//排除在曲面空间外的网格

                    gd = null;//清除网格数据
                    GC.Collect();

                    double GridInterval = 0;

                    Octree GridOctree = GetGridDataOctree(InFaceGridDataList, GridX, GridY, GridZ, ref GridInterval);//获取网格节点八叉数

                    List<int> InFaceDataIndexList = Calculation(f, InFaceGridDataList, GridOctree, GridInterval);//通过八叉数查找相交网格

                    for (int j = 0; j < InFaceDataIndexList.Count; j++)
                    {
                        SaveFileInFaceGridDataList.Add(InFaceGridDataList[InFaceDataIndexList[i]]);
                    }
                }
                InFaceGridDataWrite ifgdw = new InFaceGridDataWrite();
                ifgdw.Write(SaveFileName, SaveFileInFaceGridDataList, GridX, GridY, GridZ);
                GC.Collect();
                return true;
            }
            catch
            {
                return false;
            }
        }

        #endregion

        #region 追踪计算

        private List<int> Calculation(PointData pd, List<InFaceGridData> InFaceGridDataList, Octree GridOctree)
        {
            List<int> InFaceGridDataListIndex = new List<int>();
            for (int i = 0; i < pd.PointX.Count; i++)
            {
                int FindGridIndex = (int)GridOctree.GetNode(pd.PointX[i], pd.PointY[i], pd.PointZ[i]);

                //if (FindGridIndex != null)
                //{
                //    InFaceGridDataListIndex.Add(FindGridIndex);
                //}
                //System.Collections.ArrayList FindGridList = GridOctree.GetNodes(pd.PointX[i], pd.PointY[i], pd.PointZ[i],0);
                //for (int j = 0; j < FindGridList.Count; j++)
                //{
                bool falg = false;
                for (int k = 0; k < InFaceGridDataListIndex.Count; k++)
                {
                    if (InFaceGridDataListIndex[k] == FindGridIndex)
                    {
                        falg = true;
                        break;
                    }
                }
                if (!falg)
                {
                    if (PointGridIntersect(InFaceGridDataList[FindGridIndex].PointX,
                       InFaceGridDataList[FindGridIndex].PointY,
                       InFaceGridDataList[FindGridIndex].PointZ,
                       pd.PointX[i], pd.PointY[i], pd.PointZ[i]))
                    {
                        //查找到
                        InFaceGridDataListIndex.Add(FindGridIndex);
                    }
                }
                //}
            }
            return InFaceGridDataListIndex;

        }

        /// <summary>
        /// 追踪计算
        /// </summary>
        /// <param name="f">曲面数据对象</param>
        /// <param name="InFaceGridDataList">在曲面范围内的网格对象列表</param>
        /// <param name="GridOctree">网格节点八叉树</param>
        /// <param name="GridInterval">网格最大距离</param>
        /// <returns>追踪后的网格索引</returns>
        private List<int> Calculation(Face f, List<InFaceGridData> InFaceGridDataList, Octree GridOctree, double GridInterval)
        {

            List<int> InFaceGridDataListIndex = new List<int>();
            for (int i = 0; i < f.IndexData.Length / 3; i++)
            {
                float MaxX = -9999999999999;
                float MaxY = -9999999999999;
                float MaxZ = -9999999999999;

                float MinX = 9999999999999;
                float MinY = 9999999999999;
                float MinZ = 9999999999999;

                for (int j = 0; j < 3; j++)
                {
                    if (MaxX < f.PointX[f.IndexData[i * 3 + j]])
                    {
                        MaxX = Convert.ToSingle(f.PointX[f.IndexData[i * 3 + j]]);
                    }
                    if (MinX > f.PointX[f.IndexData[i * 3 + j]])
                    {
                        MinX = Convert.ToSingle(f.PointX[f.IndexData[i * 3 + j]]);
                    }
                    if (MaxY < f.PointY[f.IndexData[i * 3 + j]])
                    {
                        MaxY = Convert.ToSingle(f.PointY[f.IndexData[i * 3 + j]]);
                    }
                    if (MinY > f.PointY[f.IndexData[i * 3 + j]])
                    {
                        MinY = Convert.ToSingle(f.PointY[f.IndexData[i * 3 + j]]);
                    }
                    if (MaxZ < f.PointZ[f.IndexData[i * 3 + j]])
                    {
                        MaxZ = Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + j]]);
                    }
                    if (MinZ > f.PointZ[f.IndexData[i * 3 + j]])
                    {
                        MinZ = Convert.ToSingle(f.PointZ[f.IndexData[i * 3 + j]]);
                    }
                }
                //三角形最大范围，求三角形的X，Y，Z点最大距离
                float Max = -9999999999999;
                float Min = 9999999999999;
                Max = MaxX;
                Min = MinX;
                if (MaxY - MinY > Max - Min)
                {
                    Max = MaxY;
                    Min = MinY;
                }
                if (MaxZ - MinZ > Max - Min)
                {
                    Max = MaxZ;
                    Min = MinZ;
                }
                System.Collections.ArrayList FindGridList = GridOctree.GetNodes((MaxX - MinX) / 2 + MinX, (MaxY - MinY) / 2 + MinY, (MaxZ - MinZ) / 2 + MinZ, Max - Min + GridInterval * 2);
                for (int j = 0; j < FindGridList.Count; j++)
                {
                    bool falg = false;
                    for (int k = 0; k < InFaceGridDataListIndex.Count; k++)
                    {
                        if (InFaceGridDataListIndex[k] == Convert.ToInt32(FindGridList[j]) && InFaceGridDataList[InFaceGridDataListIndex[k]].DX == 0 && InFaceGridDataList[InFaceGridDataListIndex[k]].DY == 0 && InFaceGridDataList[InFaceGridDataListIndex[k]].DZ == 0)
                        {
                            falg = true;
                            break;
                        }
                    }
                    //if (!falg)
                    //{
                    //    if (Test(InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointX,
                    //        InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointY,
                    //        InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointZ,
                    //        f.PointX[f.IndexData[i * 3]], f.PointY[f.IndexData[i * 3]], f.PointZ[f.IndexData[i * 3]],
                    //        f.PointX[f.IndexData[i * 3 + 1]], f.PointY[f.IndexData[i * 3 + 1]], f.PointZ[f.IndexData[i * 3 + 1]],
                    //        f.PointX[f.IndexData[i * 3 + 2]], f.PointY[f.IndexData[i * 3 + 2]], f.PointZ[f.IndexData[i * 3 + 2]]))
                    //    {
                    //        //检查网格内部加密网格的相交情况
                    //        GridCellThickIntersectTest(InFaceGridDataList[Convert.ToInt32(FindGridList[j])], f.PointX[f.IndexData[i * 3]],
                    //            f.PointY[f.IndexData[i * 3]], f.PointZ[f.IndexData[i * 3]],
                    //        f.PointX[f.IndexData[i * 3 + 1]], f.PointY[f.IndexData[i * 3 + 1]], f.PointZ[f.IndexData[i * 3 + 1]],
                    //        f.PointX[f.IndexData[i * 3 + 2]], f.PointY[f.IndexData[i * 3 + 2]], f.PointZ[f.IndexData[i * 3 + 2]]);

                    //        //查找到
                    //        InFaceGridDataListIndex.Add(Convert.ToInt32(FindGridList[j]));
                    //    }
                    //}
                    /////////////////////////////////////////////////////////////////////////////////////
                    if (!falg)
                    {
                        if (GridIntersect(InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointX,
                            InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointY,
                            InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointZ,
                            f.PointX[f.IndexData[i * 3]], f.PointY[f.IndexData[i * 3]], f.PointZ[f.IndexData[i * 3]],
                            f.PointX[f.IndexData[i * 3 + 1]], f.PointY[f.IndexData[i * 3 + 1]], f.PointZ[f.IndexData[i * 3 + 1]],
                            f.PointX[f.IndexData[i * 3 + 2]], f.PointY[f.IndexData[i * 3 + 2]], f.PointZ[f.IndexData[i * 3 + 2]]))
                        {
                            //检查网格内部加密网格的相交情况
                            GridCellThickIntersect(InFaceGridDataList[Convert.ToInt32(FindGridList[j])],
                            f.PointX[f.IndexData[i * 3]],f.PointY[f.IndexData[i * 3]], f.PointZ[f.IndexData[i * 3]],
                            f.PointX[f.IndexData[i * 3 + 1]], f.PointY[f.IndexData[i * 3 + 1]], f.PointZ[f.IndexData[i * 3 + 1]],
                            f.PointX[f.IndexData[i * 3 + 2]], f.PointY[f.IndexData[i * 3 + 2]], f.PointZ[f.IndexData[i * 3 + 2]]);

                            falg = false;
                            for (int k = 0; k < InFaceGridDataListIndex.Count; k++)
                            {
                                if (InFaceGridDataListIndex[k] == Convert.ToInt32(FindGridList[j]))
                                {
                                    falg = true;
                                    break;
                                }
                            }
                            if (!falg)
                            {
                                //查找到
                                InFaceGridDataListIndex.Add(Convert.ToInt32(FindGridList[j]));
                            }

                            ////移除八叉树节点
                            //GridOctree.RemoveNode((InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointX[1] - InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointX[0]) / 2 + InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointX[0],
                            //(InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointY[0] - InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointY[4]) / 2 + InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointY[4],
                            //(InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointZ[0] - InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointZ[3]) / 2 + InFaceGridDataList[Convert.ToInt32(FindGridList[j])].PointZ[3], Convert.ToInt32(FindGridList[j]));
                        }
                    }
                    /////////////////////////////////////////////////////////////////////////////////////
                }
            }
            return InFaceGridDataListIndex;
        }

        #endregion

        #region 获取网格八叉树

        /// <summary>
        /// 获取网格八叉树
        /// </summary>
        /// <param name="InFaceGridDataList">在曲面范围内的网格对象列表</param>
        /// <param name="GridX">I方向个数</param>
        /// <param name="GridY">J方向个数</param>
        /// <param name="GridZ">K方向个数</param>
        /// <param name="GridInterval">网格最大距离</param>
        /// <returns>八叉树</returns>
        private Octree GetGridDataOctree(List<InFaceGridData> InFaceGridDataList, int GridX, int GridY, int GridZ, ref double GridInterval)
        {
            double MaxX = -9999999999999;
            double MaxY = -9999999999999;
            double MaxZ = -9999999999999;

            double MinX = 9999999999999;
            double MinY = 9999999999999;
            double MinZ = 9999999999999;

            for (int i = 0; i < InFaceGridDataList.Count; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (MaxX < InFaceGridDataList[i].PointX[j])
                    {
                        MaxX = InFaceGridDataList[i].PointX[j];
                    }
                    if (MinX > InFaceGridDataList[i].PointX[j])
                    {
                        MinX = InFaceGridDataList[i].PointX[j];
                    }
                    if (MaxY < InFaceGridDataList[i].PointY[j])
                    {
                        MaxY = InFaceGridDataList[i].PointY[j];
                    }
                    if (MinY > InFaceGridDataList[i].PointY[j])
                    {
                        MinY = InFaceGridDataList[i].PointY[j];
                    }
                    if (MaxZ < InFaceGridDataList[i].PointZ[j])
                    {
                        MaxZ = InFaceGridDataList[i].PointZ[j];
                    }
                    if (MinZ > InFaceGridDataList[i].PointZ[j])
                    {
                        MinZ = InFaceGridDataList[i].PointZ[j];
                    }
                }
            }
            GridInterval = Math.Abs(MaxX - MinX) / GridX / 2;
            if (Math.Abs(MaxY - MinX) / GridY / 2 > GridInterval)
            {
                GridInterval = Math.Abs(MaxY - MinX) / GridY / 2;
            }
            if (Math.Abs(MaxZ - MinZ) / GridZ / 2 > GridInterval)
            {
                GridInterval = Math.Abs(MaxY - MinX) / GridZ / 2;
            }
            Octree GridOctree = new Octree(Convert.ToSingle(MaxX), Convert.ToSingle(MinX),
            Convert.ToSingle(MaxY), Convert.ToSingle(MinY),
            Convert.ToSingle(MaxZ), Convert.ToSingle(MinZ), InFaceGridDataList.Count);
            for (int i = 0; i < InFaceGridDataList.Count; i++)
            {
                GridOctree.AddNode((InFaceGridDataList[i].PointX[1] - InFaceGridDataList[i].PointX[0]) / 2 + InFaceGridDataList[i].PointX[0],
                    (InFaceGridDataList[i].PointY[0] - InFaceGridDataList[i].PointY[4]) / 2 + InFaceGridDataList[i].PointY[4],
                    (InFaceGridDataList[i].PointZ[0] - InFaceGridDataList[i].PointZ[3]) / 2 + InFaceGridDataList[i].PointZ[3], i);
            }
            return GridOctree;
        }

        #endregion

        #region 检测单元格与三角形相交


        private bool PointGridIntersect(List<float> GridXList, List<float> GridYList, List<float> GridZList,
            float PX1, float PY1, float PZ1)
        {
            Vector3 v1 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v2 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v3 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v4 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);

            Vector3 v5 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v6 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v7 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);
            Vector3 v8 = new Vector3(GridXList[0], GridYList[0], GridZList[0]);

            #region 顶点

            Vector3[] GridVector = new Vector3[24];
            Vector3[] GridNormal = new Vector3[24];
            //顶面
            GridVector[0] = v1;
            GridNormal[0] = new Vector3(0, 0, 0);
            GridVector[1] = v2;
            GridNormal[1] = new Vector3(0, 0, 0);
            GridVector[2] = v3;
            GridNormal[2] = new Vector3(0, 0, 0);
            GridVector[3] = v4;
            GridNormal[3] = new Vector3(0, 0, 0);
            //左面
            GridVector[4] = v2;
            GridNormal[4] = new Vector3(0, 0, 0);
            GridVector[5] = v1;
            GridNormal[5] = new Vector3(0, 0, 0);
            GridVector[6] = v5;
            GridNormal[6] = new Vector3(0, 0, 0);
            GridVector[7] = v6;
            GridNormal[7] = new Vector3(0, 0, 0);
            //右面
            GridVector[8] = v4;
            GridNormal[8] = new Vector3(0, 0, 0);
            GridVector[9] = v3;
            GridNormal[9] = new Vector3(0, 0, 0);
            GridVector[10] = v7;
            GridNormal[10] = new Vector3(0, 0, 0);
            GridVector[11] = v8;
            GridNormal[11] = new Vector3(0, 0, 0);
            //前面
            GridVector[12] = v1;
            GridNormal[12] = new Vector3(0, 0, 0);
            GridVector[13] = v4;
            GridNormal[13] = new Vector3(0, 0, 0);
            GridVector[14] = v8;
            GridNormal[14] = new Vector3(0, 0, 0);
            GridVector[15] = v5;
            GridNormal[15] = new Vector3(0, 0, 0);
            //后面
            GridVector[16] = v2;
            GridNormal[16] = new Vector3(0, 0, 0);
            GridVector[17] = v6;
            GridNormal[17] = new Vector3(0, 0, 0);
            GridVector[18] = v7;
            GridNormal[18] = new Vector3(0, 0, 0);
            GridVector[19] = v3;
            GridNormal[19] = new Vector3(0, 0, 0);
            //底面
            GridVector[20] = v6;
            GridNormal[20] = new Vector3(0, 0, 0);
            GridVector[21] = v5;
            GridNormal[21] = new Vector3(0, 0, 0);
            GridVector[22] = v8;
            GridNormal[22] = new Vector3(0, 0, 0);
            GridVector[23] = v7;
            GridNormal[23] = new Vector3(0, 0, 0);

            #endregion

            #region 索引

            int[] IndexData = new int[36];
            //顶面0-3
            IndexData[0] = 0;
            IndexData[1] = 1;
            IndexData[2] = 2;
            IndexData[3] = 3;
            IndexData[4] = 0;
            IndexData[5] = 2;

            //左面4-7
            IndexData[6] = 4;
            IndexData[7] = 5;
            IndexData[8] = 6;
            IndexData[9] = 7;
            IndexData[10] = 4;
            IndexData[11] = 6;

            //右面8-11
            IndexData[12] = 8;
            IndexData[13] = 9;
            IndexData[14] = 10;
            IndexData[15] = 11;
            IndexData[16] = 8;
            IndexData[17] = 10;

            //前面12-15
            IndexData[18] = 12;
            IndexData[19] = 13;
            IndexData[20] = 14;
            IndexData[21] = 15;
            IndexData[22] = 12;
            IndexData[23] = 14;

            //后面16-19
            IndexData[24] = 16;
            IndexData[25] = 17;
            IndexData[26] = 18;
            IndexData[27] = 19;
            IndexData[28] = 16;
            IndexData[29] = 18;

            //底面20-23
            IndexData[30] = 20;
            IndexData[31] = 21;
            IndexData[32] = 22;
            IndexData[33] = 23;
            IndexData[34] = 20;
            IndexData[35] = 22;

            #endregion

            //计算法线
            for (int j = 0; j < IndexData.Length / 3; j++)
            {
                Vector3 firstVec = GridVector[IndexData[j * 3 + 1]] - GridVector[IndexData[j * 3]];
                Vector3 secondVec = GridVector[IndexData[j * 3 + 2]] - GridVector[IndexData[j * 3]];
                Vector3 normal = Vector3.Cross(secondVec, firstVec);

                GridNormal[IndexData[j * 3]] = GridNormal[IndexData[j * 3]] + normal;
                GridNormal[IndexData[j * 3 + 1]] = GridNormal[IndexData[j * 3 + 1]] + normal;
                GridNormal[IndexData[j * 3 + 2]] = GridNormal[IndexData[j * 3 + 2]] + normal;
            }
            for (int i = 0; i < IndexData.Length / 3; i++)
            {
                if (Vector3.Dot(new Vector3(PX1, PY1, PZ1), GridNormal[IndexData[i * 3]]) >= 0)
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 检测单个单元格是否相交
        /// </summary>
        /// <param name="GridXList">单元格X坐标列表</param>
        /// <param name="GridYList">单元格Y坐标列表</param>
        /// <param name="GridZList">单元格Z坐标列表</param>
        /// <param name="TX1">曲面三角形X1点坐标</param>
        /// <param name="TY1">曲面三角形Y1点坐标</param>
        /// <param name="TZ1">曲面三角形Z1点坐标</param>
        /// <param name="TX2">曲面三角形X2点坐标</param>
        /// <param name="TY2">曲面三角形Y2点坐标</param>
        /// <param name="TZ2">曲面三角形Z2点坐标</param>
        /// <param name="TX3">曲面三角形X3点坐标</param>
        /// <param name="TY3">曲面三角形Y2点坐标</param>
        /// <param name="TZ3">曲面三角形Z3点坐标</param>
        /// <returns>是否相交</returns>
        private bool GridIntersect(
            List<float> GridXList, List<float> GridYList, List<float> GridZList,
            double TX1, double TY1, double TZ1,
            double TX2, double TY2, double TZ2,
            double TX3, double TY3, double TZ3)
        {
            //上面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[0], GridYList[0], GridZList[0],
                        GridXList[1], GridYList[1], GridZList[1],
                        GridXList[2], GridYList[2], GridZList[2]))
            {
                return true;
            }

            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[0], GridYList[0], GridZList[0],
                        GridXList[2], GridYList[2], GridZList[2],
                        GridXList[3], GridYList[3], GridZList[3]))
            {
                return true;
            }
            //前面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[3], GridYList[3], GridZList[3],
                        GridXList[2], GridYList[2], GridZList[2],
                        GridXList[6], GridYList[6], GridZList[6]))
            {
                return true;
            }

            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[3], GridYList[3], GridZList[3],
                        GridXList[6], GridYList[6], GridZList[6],
                        GridXList[7], GridYList[7], GridZList[7]))
            {
                return true;
            }
            //左面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[3], GridYList[3], GridZList[3],
                        GridXList[0], GridYList[0], GridZList[0],
                        GridXList[4], GridYList[4], GridZList[4]))
            {
                return true;
            }
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[3], GridYList[3], GridZList[3],
                        GridXList[4], GridYList[4], GridZList[4],
                        GridXList[7], GridYList[7], GridZList[7]))
            {
                return true;
            }
            //右面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[2], GridYList[2], GridZList[2],
                        GridXList[1], GridYList[1], GridZList[1],
                        GridXList[5], GridYList[5], GridZList[5]))
            {
                return true;
            }
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[2], GridYList[2], GridZList[2],
                        GridXList[5], GridYList[5], GridZList[5],
                        GridXList[6], GridYList[6], GridZList[6]))
            {
                return true;
            }
            //后面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[0], GridYList[0], GridZList[0],
                        GridXList[1], GridYList[1], GridZList[1],
                        GridXList[5], GridYList[5], GridZList[5]))
            {
                return true;
            }
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[0], GridYList[0], GridZList[0],
                        GridXList[5], GridYList[5], GridZList[5],
                        GridXList[4], GridYList[4], GridZList[4]))
            {
                return true;
            }
            //底面
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[5], GridYList[5], GridZList[5],
                        GridXList[6], GridYList[6], GridZList[6],
                        GridXList[7], GridYList[7], GridZList[7]))
            {
                return true;
            }
            if (ti.IsTriangleIntersect(
                        TX1, TY1, TZ1,
                        TX2, TY2, TZ2,
                        TX3, TY3, TZ3,
                        GridXList[5], GridYList[5], GridZList[5],
                        GridXList[7], GridYList[7], GridZList[7],
                        GridXList[4], GridYList[4], GridZList[4]))
            {
                return true;
            }
            return false;
        }

        private bool Test(List<float> GridXList, List<float> GridYList, List<float> GridZList,
            double TX1, double TY1, double TZ1,
            double TX2, double TY2, double TZ2,
            double TX3, double TY3, double TZ3)
        {
            if (Test(GridXList, GridYList, GridZList, TX1, TY1, TZ1, TX2, TY2, TZ2))
            {
                return true;
            }
            if (Test(GridXList, GridYList, GridZList, TX1, TY1, TZ1, TX3, TY3, TZ3))
            {
                return true;
            }
            if (Test(GridXList, GridYList, GridZList, TX2, TY2, TZ2, TX3, TY3, TZ3))
            {
                return true;
            }
            return false;

        }
        private bool Test(List<float> GridXList, List<float> GridYList, List<float> GridZList,
            double TX1, double TY1, double TZ1,
            double TX2, double TY2, double TZ2)
        {
            LineQuadIntersect lqi = new LineQuadIntersect();
            double X = 0, Y = 0, Z = 0;

            //上面  3,2,1,0
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[3], GridYList[3], GridZList[3],
                GridXList[2], GridYList[2], GridZList[2], GridXList[1], GridYList[1], GridZList[1],
                GridXList[0], GridYList[0], GridZList[0], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }
            }
            //前面 3,7,6,2
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[3], GridYList[3], GridZList[3],
                GridXList[7], GridYList[7], GridZList[7], GridXList[6], GridYList[6], GridZList[6],
                GridXList[2], GridYList[2], GridZList[2], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }

            }
            //左面 4,7,3,0
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[4], GridYList[4], GridZList[4],
                GridXList[7], GridYList[7], GridZList[7], GridXList[3], GridYList[3], GridZList[3],
                GridXList[0], GridYList[0], GridZList[0], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }
            }
            //右面 2,6,5,1
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[2], GridYList[2], GridZList[2],
                GridXList[6], GridYList[6], GridZList[6], GridXList[5], GridYList[5], GridZList[5],
                GridXList[1], GridYList[1], GridZList[1], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }
            }
            //后面 1,5,4,0
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[1], GridYList[1], GridZList[1],
                GridXList[5], GridYList[5], GridZList[5], GridXList[4], GridYList[4], GridZList[4],
                GridXList[0], GridYList[0], GridZList[0], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }
            }
            //下面 5,6,7,4
            if (lqi.IntersectLineQuad(TX1, TY1, TZ1, TX2, TY2, TZ2, GridXList[5], GridYList[5], GridZList[5],
                GridXList[6], GridYList[6], GridZList[6], GridXList[7], GridYList[7], GridZList[7],
                GridXList[4], GridYList[4], GridZList[4], ref X, ref Y, ref Z))
            {
                if (PointInLine.IsPointInLine(TX1, TY1, TZ1, TX2, TY2, TZ2, X, Y, Z))
                {
                    return true;
                }
            }
            return false;
        }

        #endregion

        #region 排除在范围外的网格

        private List<InFaceGridData> GetInFaceDataList(GridData gd, Face f)
        {
            double MaxX = -9999999999999;
            double MaxY = -9999999999999;
            double MaxZ = -9999999999999;

            double MinX = 9999999999999;
            double MinY = 9999999999999;
            double MinZ = 9999999999999;

            List<InFaceGridData> InFaceDataList = new List<InFaceGridData>();
            for (int i = 0; i < f.PointX.Length; i++)
            {
                if (MaxX < f.PointX[i])
                {
                    MaxX = f.PointX[i];
                }
                if (MinX > f.PointX[i])
                {
                    MinX = f.PointX[i];
                }
                if (MaxY < f.PointY[i])
                {
                    MaxY = f.PointY[i];
                }
                if (MinY > f.PointY[i])
                {
                    MinY = f.PointY[i];
                }
                if (MaxZ < f.PointZ[i])
                {
                    MaxZ = f.PointZ[i];
                }
                if (MinZ > f.PointZ[i])
                {
                    MinZ = f.PointZ[i];
                }
            }
            for (int i = 0; i < gd.PointX.Length / 8; i++)
            {
                bool falg = false;
                for (int j = 0; j < 8; j++)
                {
                    if (gd.PointX[i * 8 + j] >= MinX && gd.PointX[i * 8 + j] <= MaxX && gd.PointZ[i * 8 + j] >= MinZ && gd.PointZ[i * 8 + j] <= MaxZ && gd.PointY[i * 8 + j] >= MinY && gd.PointY[i * 8 + j] <= MaxY)
                    {
                        falg = true;
                        break;
                    }
                }
                //在范围内
                if (falg)
                {
                    InFaceGridData ifgd = new InFaceGridData();
                    for (int j = 0; j < 8; j++)
                    {
                        ifgd.PointX.Add(Convert.ToSingle(gd.PointX[i * 8 + j]));
                        ifgd.PointY.Add(Convert.ToSingle(gd.PointY[i * 8 + j]));
                        ifgd.PointZ.Add(Convert.ToSingle(gd.PointZ[i * 8 + j]));
                    }
                    ifgd.X = (i % (gd.GridX * gd.GridY) % gd.GridX);
                    ifgd.Y = (i % (gd.GridX * gd.GridY) / gd.GridX);
                    ifgd.Z = i / (gd.GridX * gd.GridY);

                    for (int t = 0; t < gd.DNameList.Count; t++)
                    {
                        for (int j = 0; j < gd.DNameList[t].ID.Count; j++)
                        {
                            if (gd.DGridDataList[gd.DNameList[t].ID[j]].X1 <= (ifgd.X + 1) && (ifgd.X + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].X2)
                            {
                                if (gd.DGridDataList[gd.DNameList[t].ID[j]].Y1 <= (ifgd.Y + 1) && (ifgd.Y + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Y2)
                                {
                                    if (gd.DGridDataList[gd.DNameList[t].ID[j]].Z1 <= (ifgd.Z + 1) && (ifgd.Z + 1) <= gd.DGridDataList[gd.DNameList[t].ID[j]].Z2)
                                    {
                                        ifgd.DX = gd.DGridDataList[gd.DNameList[t].ID[j]].DX / ((gd.DGridDataList[gd.DNameList[t].ID[j]].X2 - gd.DGridDataList[gd.DNameList[t].ID[j]].X1) + 1);
                                        ifgd.DY = gd.DGridDataList[gd.DNameList[t].ID[j]].DY / ((gd.DGridDataList[gd.DNameList[t].ID[j]].Y2 - gd.DGridDataList[gd.DNameList[t].ID[j]].Y1) + 1);
                                        ifgd.DZ = gd.DGridDataList[gd.DNameList[t].ID[j]].DZ / ((gd.DGridDataList[gd.DNameList[t].ID[j]].Z2 - gd.DGridDataList[gd.DNameList[t].ID[j]].Z1) + 1);
                                    }
                                }
                            }
                        }
                    }
                    InFaceDataList.Add(ifgd);
                }

            }
            return InFaceDataList;
        }

        #endregion

        #region 检测加密网格相交情况

        /// <summary>
        /// 检测网格内部加密网格相交状况
        /// </summary>
        /// <param name="InFaceGridDataList">相交的网格</param>
        private void GridCellThickIntersect(InFaceGridData InFaceGridDataThick, double TX1, double TY1, double TZ1, double TX2, double TY2, double TZ2, double TX3, double TY3, double TZ3)
        {
            if (InFaceGridDataThick.DX == 0 && InFaceGridDataThick.DY == 0 && InFaceGridDataThick.DZ == 0)
            {
                return;
            }

            #region 求加密网格坐标

            double XLength_1_0 = (InFaceGridDataThick.PointX[1] - InFaceGridDataThick.PointX[0]) / InFaceGridDataThick.DX;
            double XLength_2_3 = (InFaceGridDataThick.PointX[2] - InFaceGridDataThick.PointX[3]) / InFaceGridDataThick.DX;
            double XLength_5_4 = (InFaceGridDataThick.PointX[5] - InFaceGridDataThick.PointX[4]) / InFaceGridDataThick.DX;
            double XLength_6_7 = (InFaceGridDataThick.PointX[6] - InFaceGridDataThick.PointX[7]) / InFaceGridDataThick.DX;

            double YLength_0_3 = (InFaceGridDataThick.PointY[0] - InFaceGridDataThick.PointY[3]) / InFaceGridDataThick.DY;
            double YLength_1_2 = (InFaceGridDataThick.PointY[1] - InFaceGridDataThick.PointY[2]) / InFaceGridDataThick.DY;
            double YLength_4_7 = (InFaceGridDataThick.PointY[4] - InFaceGridDataThick.PointY[7]) / InFaceGridDataThick.DY;
            double YLength_5_6 = (InFaceGridDataThick.PointY[5] - InFaceGridDataThick.PointY[6]) / InFaceGridDataThick.DY;

            double ZLength_0_4 = (InFaceGridDataThick.PointZ[4] - InFaceGridDataThick.PointZ[0]) / InFaceGridDataThick.DZ;
            double ZLength_1_5 = (InFaceGridDataThick.PointZ[5] - InFaceGridDataThick.PointZ[1]) / InFaceGridDataThick.DZ;
            double ZLength_3_7 = (InFaceGridDataThick.PointZ[7] - InFaceGridDataThick.PointZ[3]) / InFaceGridDataThick.DZ;
            double ZLength_2_6 = (InFaceGridDataThick.PointZ[6] - InFaceGridDataThick.PointZ[2]) / InFaceGridDataThick.DZ;


            for (int i = 0; i < InFaceGridDataThick.DX * InFaceGridDataThick.DY * InFaceGridDataThick.DZ; i++)
            {
                List<float> PointXList = new List<float>();
                List<float> PointYList = new List<float>();
                List<float> PointZList = new List<float>();
                int x = (i % (InFaceGridDataThick.DX * InFaceGridDataThick.DY) % InFaceGridDataThick.DX);
                int y = (i % (InFaceGridDataThick.DX * InFaceGridDataThick.DY) / InFaceGridDataThick.DX);
                int z = i / (InFaceGridDataThick.DX * InFaceGridDataThick.DY);
                //上面

                //0
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[0] + x * XLength_1_0));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[0] - y * YLength_0_3));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[0] + z * ZLength_0_4));
                //1
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[0] + (x + 1) * XLength_1_0));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[1] - y * YLength_1_2));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[1] + z * ZLength_1_5));

                //2
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[3] + (x + 1) * XLength_2_3));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[1] - (y + 1) * YLength_1_2));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[2] + z * ZLength_2_6));
                //3
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[3] + x * XLength_2_3));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[0] - (y + 1) * YLength_0_3));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[3] + z * ZLength_3_7));

                //下面

                //4
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[4] + x * XLength_5_4));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[4] - y * YLength_4_7));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[0] + (z + 1) * ZLength_0_4));
                //5
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[4] + (x + 1) * XLength_5_4));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[5] - y * YLength_5_6));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[1] + (z + 1) * ZLength_1_5));
                //6
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[7] + (x + 1) * XLength_6_7));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[5] - (y + 1) * YLength_5_6));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[2] + (z + 1) * ZLength_2_6));
                //7
                PointXList.Add(Convert.ToSingle(InFaceGridDataThick.PointX[7] + x * XLength_6_7));
                PointYList.Add(Convert.ToSingle(InFaceGridDataThick.PointY[4] - (y + 1) * YLength_4_7));
                PointZList.Add(Convert.ToSingle(InFaceGridDataThick.PointZ[3] + (z + 1) * ZLength_3_7));

                //if (InFaceGridDataThick.X == 3 && InFaceGridDataThick.Y == 2 && InFaceGridDataThick.Z == 9)
                //{
                    if (GridIntersect(PointXList, PointYList, PointZList, TX1, TY1, TZ1, TX2, TY2, TZ2, TX3, TY3, TZ3))
                    //if(i==0||i==1||i==2||i==3||i==4||i==5||i==6||i==7)
                    {
                        InFaceGridData ifgd = new InFaceGridData(); //InFaceGridDataThick
                        ifgd.PointX = PointXList;
                        ifgd.PointY = PointYList;
                        ifgd.PointZ = PointZList;
                        ifgd.X = InFaceGridDataThick.X;
                        ifgd.Y = InFaceGridDataThick.Y;
                        ifgd.Z = InFaceGridDataThick.Z;
                        ifgd.DX = 0;
                        ifgd.DY = 0;
                        ifgd.DZ = 0;
                        InFaceGridDataThick.ChildrenGridDataList.Add(ifgd);
                    }
                //}
            }

            #endregion
        }
        private void GridCellThickIntersectTest(InFaceGridData InFaceGridDataThick, double TX1, double TY1, double TZ1, double TX2, double TY2, double TZ2, double TX3, double TY3, double TZ3)
        {
            if (InFaceGridDataThick.DX == 0 && InFaceGridDataThick.DY == 0 && InFaceGridDataThick.DZ == 0)
            {
                return;
            }

            #region 求加密网格坐标

            float XLength_1_0 = (InFaceGridDataThick.PointX[1] - InFaceGridDataThick.PointX[0]) / InFaceGridDataThick.DX;
            float XLength_2_3 = (InFaceGridDataThick.PointX[2] - InFaceGridDataThick.PointX[3]) / InFaceGridDataThick.DX;
            float XLength_5_4 = (InFaceGridDataThick.PointX[5] - InFaceGridDataThick.PointX[4]) / InFaceGridDataThick.DX;
            float XLength_6_7 = (InFaceGridDataThick.PointX[6] - InFaceGridDataThick.PointX[7]) / InFaceGridDataThick.DX;

            float YLength_0_3 = (InFaceGridDataThick.PointY[0] - InFaceGridDataThick.PointY[3]) / InFaceGridDataThick.DY;
            float YLength_1_2 = (InFaceGridDataThick.PointY[1] - InFaceGridDataThick.PointY[2]) / InFaceGridDataThick.DY;
            float YLength_4_7 = (InFaceGridDataThick.PointY[4] - InFaceGridDataThick.PointY[7]) / InFaceGridDataThick.DY;
            float YLength_5_6 = (InFaceGridDataThick.PointY[5] - InFaceGridDataThick.PointY[6]) / InFaceGridDataThick.DY;

            float ZLength_0_4 = (InFaceGridDataThick.PointZ[4] - InFaceGridDataThick.PointZ[0]) / InFaceGridDataThick.DZ;
            float ZLength_1_5 = (InFaceGridDataThick.PointZ[5] - InFaceGridDataThick.PointZ[1]) / InFaceGridDataThick.DZ;
            float ZLength_3_7 = (InFaceGridDataThick.PointZ[7] - InFaceGridDataThick.PointZ[3]) / InFaceGridDataThick.DZ;
            float ZLength_2_6 = (InFaceGridDataThick.PointZ[6] - InFaceGridDataThick.PointZ[2]) / InFaceGridDataThick.DZ;

            for (int i = 0; i < InFaceGridDataThick.DX * InFaceGridDataThick.DY * InFaceGridDataThick.DZ; i++)
            {
                List<float> PointXList = new List<float>();
                List<float> PointYList = new List<float>();
                List<float> PointZList = new List<float>();
                int x = (i % (InFaceGridDataThick.DX * InFaceGridDataThick.DY) % InFaceGridDataThick.DX);
                int y = (i % (InFaceGridDataThick.DX * InFaceGridDataThick.DY) / InFaceGridDataThick.DX);
                int z = i / (InFaceGridDataThick.DX * InFaceGridDataThick.DY);
                //上面

                //0
                PointXList.Add(InFaceGridDataThick.PointX[0] + x * XLength_1_0);
                PointYList.Add(InFaceGridDataThick.PointY[0] - y * YLength_0_3);
                //PointZList.Add(InFaceGridDataThick.PointZ[0] + z * ZLength_0_4);
                PointZList.Add(InFaceGridDataThick.PointZ[0] + z * ZLength_0_4);
                //1
                PointXList.Add(InFaceGridDataThick.PointX[0] + (x + 1) * XLength_1_0);
                PointYList.Add(InFaceGridDataThick.PointY[1] - y * YLength_1_2);
                //PointZList.Add(InFaceGridDataThick.PointZ[1] - z * ZLength_1_5);
                PointZList.Add(InFaceGridDataThick.PointZ[1] + z * ZLength_1_5);

                //2
                PointXList.Add(InFaceGridDataThick.PointX[3] + (x + 1) * XLength_2_3);
                PointYList.Add(InFaceGridDataThick.PointY[1] - (y + 1) * YLength_1_2);
                //PointZList.Add(InFaceGridDataThick.PointZ[2] - z * ZLength_2_6);
                PointZList.Add(InFaceGridDataThick.PointZ[2] + z * ZLength_2_6);
                //3
                PointXList.Add(InFaceGridDataThick.PointX[3] + x * XLength_2_3);
                PointYList.Add(InFaceGridDataThick.PointY[0] - (y + 1) * YLength_0_3);
                //PointZList.Add(InFaceGridDataThick.PointZ[3] + z * ZLength_3_7);
                PointZList.Add(InFaceGridDataThick.PointZ[3] + z * ZLength_3_7);

                //下面

                //4
                PointXList.Add(InFaceGridDataThick.PointX[4] + x * XLength_5_4);
                PointYList.Add(InFaceGridDataThick.PointY[4] - y * YLength_4_7);
                //PointZList.Add(InFaceGridDataThick.PointZ[0] - (z + 1) * ZLength_0_4);
                PointZList.Add(InFaceGridDataThick.PointZ[0] + (z + 1) * ZLength_0_4);
                //5
                PointXList.Add(InFaceGridDataThick.PointX[4] + (x + 1) * XLength_5_4);
                PointYList.Add(InFaceGridDataThick.PointY[5] - y * YLength_5_6);
                //PointZList.Add(InFaceGridDataThick.PointZ[1] - (z + 1) * ZLength_1_5);
                PointZList.Add(InFaceGridDataThick.PointZ[1] + (z + 1) * ZLength_1_5);
                //6
                PointXList.Add(InFaceGridDataThick.PointX[7] + (x + 1) * XLength_6_7);
                PointYList.Add(InFaceGridDataThick.PointY[5] - (y + 1) * YLength_5_6);
                //PointZList.Add(InFaceGridDataThick.PointZ[2] - (z + 1) * ZLength_2_6);
                PointZList.Add(InFaceGridDataThick.PointZ[2] + (z + 1) * ZLength_2_6);
                //7
                PointXList.Add(InFaceGridDataThick.PointX[7] + x * XLength_6_7);
                PointYList.Add(InFaceGridDataThick.PointY[4] - (y + 1) * YLength_4_7);
                //PointZList.Add(InFaceGridDataThick.PointZ[3] - (z + 1) * ZLength_3_7);
                PointZList.Add(InFaceGridDataThick.PointZ[3] + (z + 1) * ZLength_3_7);

                if (Test(PointXList, PointYList, PointZList, TX1, TY1, TZ1, TX2, TY2, TZ2, TX3, TY3, TZ3))
                {
                    InFaceGridData ifgd = new InFaceGridData(); //InFaceGridDataThick
                    ifgd.PointX = PointXList;
                    ifgd.PointY = PointYList;
                    ifgd.PointZ = PointZList;
                    ifgd.X = InFaceGridDataThick.X;
                    ifgd.Y = InFaceGridDataThick.Y;
                    ifgd.Z = InFaceGridDataThick.Z;
                    ifgd.DX = 0;
                    ifgd.DY = 0;
                    ifgd.DZ = 0;
                    InFaceGridDataThick.ChildrenGridDataList.Add(ifgd);
                }
            }

            #endregion
        }
        #endregion

        /// <summary>
        /// 重载排序方法
        /// </summary>
        /// <param name="gc1">GridCell1</param>
        /// <param name="gc2">GridCell2</param>
        /// <returns>比较结果</returns>
        static int SortZ(int a, int b)
        {
            if (a > b)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
    }
}
