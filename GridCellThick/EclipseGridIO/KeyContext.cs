using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace EclipseGridIO
{
    class KeyContext
    {
        KeyResolve kr;
        /// <summary>
        /// 设置实现的类是哪个
        /// </summary>
        /// <param name="kr">KeyResolve类对象</param>
        public void SetKeyClass(KeyResolve kr)
        {
            this.kr = kr;
        }
        /// <summary>
        /// 解析，使用KeyResolve类的Resolve方法
        /// </summary>
        /// <param name="gd">GridData</param>
        /// <param name="sr">流</param>
        public void Resolve(StreamReader sr, GridData gd)
        {
            kr.Resolve(sr,gd);
        }
        /// <summary>
        /// 写入，使用KeyResolve类的DataWrite方法
        /// </summary>
        /// <param name="sw">流</param>
        /// <param name="gd">GridData</param>
        public void DataWrite(StreamWriter sw, GridData gd)
        {
            kr.DataWrite(sw, gd);
        }
    }
}
