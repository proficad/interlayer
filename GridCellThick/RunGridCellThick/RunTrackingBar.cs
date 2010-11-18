using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RunGridCellThick
{
    public class RunTrackingBar
    {

        #region 变量声明

        private List<string> FaceFileNameList = new List<string>();//曲面文件名列表
        private List<string> SaveFileNameList = new List<string>();//追踪后保存的文件名列表
        private string GridFileName = "";//网格文件名

        #endregion


        [STAThread]
        public bool RunEXE()
        {
            try
            {
                GridCellThick.TrackingBar tb = new GridCellThick.TrackingBar(FaceFileNameList, SaveFileNameList, GridFileName);
                tb.ShowDialog();
                if (tb.IsCancel)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 添加追踪项
        /// </summary>
        /// <param name="FaceFileName">曲面文件名</param>
        /// <param name="SaveFileName">追踪后保存文件名</param>
        /// <returns>是否成功</returns>
        public bool ADDItem(string FaceFileName, string SaveFileName)
        {
            try
            {
                FaceFileNameList.Add(FaceFileName);
                SaveFileNameList.Add(SaveFileName);
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 清除追踪项
        /// </summary>
        /// <returns>是否成功</returns>
        public bool ClearItem()
        {
            try
            {
                FaceFileNameList.Clear();
                SaveFileNameList.Clear();
                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 设置网格文件名
        /// </summary>
        /// <param name="FileName">网格文件名</param>
        /// <returns>是否成功</returns>
        public bool SetGridFileName(string FileName)
        {
            try
            {
                GridFileName = FileName;
                return true;
            }
            catch
            {
                return false;
            }
        }
        
    }
}
