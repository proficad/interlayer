using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    class EclipseHead:KeyResolve
    {
        public override void Resolve(System.IO.StreamReader sr, GridData gd)
        {
            throw new NotImplementedException();
        }

        public override void DataWrite(System.IO.StreamWriter sw, GridData gd)
        {
            //写文件头（注释）
            sw.WriteLine("-- Format      : Generic ECLIPSE style (ASCII) grid geometry and properties (*.GRDECL)");
            //sw.WriteLine("-- Exported by : Interlayer  Excellent Co., Ltd.");
            sw.WriteLine("-- Exported by : Interlayer");
            sw.WriteLine("-- Date        : " + DateTime.Now.ToString());
            sw.WriteLine("-- Grid        : 3D grid");
            sw.WriteLine();
        }
    }
}
