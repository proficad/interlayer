using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Microsoft.DirectX.Direct3D;
using Microsoft.DirectX;
using System.Windows.Forms;
namespace Track
{
    /// <summary>
    /// 曲面中心点生成
    /// </summary>
    public class FacePointWrite
    {
        private Device device = null;

        /// <summary>
        /// 生成曲面加密点
        /// </summary>
        /// <param name="FaceName">曲面</param>
        /// <param name="SaveFileName"></param>
        /// <param name="lod"></param>
        /// <returns></returns>
        public bool FaceWrite(string FaceName, string SaveFileName, int lod)
        {
            try
            {
                PresentParameters presentParams = new PresentParameters();

                presentParams.Windowed = true;
                presentParams.SwapEffect = SwapEffect.Discard;
                presentParams.AutoDepthStencilFormat = DepthFormat.D16;
                presentParams.EnableAutoDepthStencil = true;

                // Create our device
                Form f1 = new Form();
                device = new Device(0, DeviceType.Hardware, f1, CreateFlags.SoftwareVertexProcessing, presentParams);


                //List<float> PointXList = new List<float>();
                //List<float> PointYList = new List<float>();
                //List<float> PointZList = new List<float>();
                FileStream fs = new FileStream(FaceName, FileMode.Open);
                BinaryReader br = new BinaryReader(fs, Encoding.Unicode);
                int PointCount = br.ReadInt32();
                CustomVertex.PositionOnly[] verts = new CustomVertex.PositionOnly[PointCount];
                for (int i = 0; i < PointCount; i++)
                {
                    //PointXList.Add(Convert.ToSingle(br.ReadDouble()));
                    //PointYList.Add(Convert.ToSingle(br.ReadDouble()));
                    //PointZList.Add(Convert.ToSingle(br.ReadDouble()));
                    verts[i].X = Convert.ToSingle(br.ReadDouble());
                    verts[i].Y = Convert.ToSingle(br.ReadDouble());
                    verts[i].Z = Convert.ToSingle(br.ReadDouble());
                }
                int IndexCount = br.ReadInt32();
                int[] IndexData = new int[IndexCount];
                for (int i = 0; i < IndexCount; i++)
                {
                    IndexData[i] = br.ReadInt32();
                }
                br.Close();
                Mesh tempmesh = new Mesh(IndexCount / 3, PointCount, MeshFlags.SystemMemory, CustomVertex.PositionOnly.Format, device);
                tempmesh.VertexBuffer.SetData(verts, 0, LockFlags.None);
                tempmesh.IndexBuffer.SetData(IndexData, 0, LockFlags.None);
                Mesh mesh = null;
                using (PatchMesh patch = PatchMesh.CreateNPatchMesh(tempmesh))
                {
                    int numberFaces = (int)(tempmesh.NumberFaces * Math.Pow(lod, 3));
                    int numberVerts = (int)(tempmesh.NumberVertices * Math.Pow(lod, 3));
                    mesh = new Mesh(numberFaces, numberVerts, MeshFlags.Managed | MeshFlags.Use32Bit, tempmesh.VertexFormat, device);
                    patch.Tessellate(lod, mesh);
                }

                return true;
            }
            catch
            {
                return false;
            }
        }
        /// <summary>
        /// 获取平均值
        /// </summary>
        private float GetAverageValue(float X1, float X2, float X3)
        {
            return Convert.ToSingle((X1 + X2 + X3) / 3);
        }
    }
}
