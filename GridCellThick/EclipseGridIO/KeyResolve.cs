using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace EclipseGridIO
{
    abstract class KeyResolve
    {
        /// <summary>
        /// 抽象类，解析方法
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="sr">流</param>
        public abstract void Resolve(StreamReader sr, GridData gd);
        /// <summary>
        /// 抽象类，写入方法
        /// </summary>
        /// <param name="sw">流</param>
        /// <param name="gd">GridData</param>
        public abstract void DataWrite(StreamWriter sw, GridData gd);
    }
}
