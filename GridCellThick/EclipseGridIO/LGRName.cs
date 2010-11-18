using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EclipseGridIO
{
    public class LGRName
    {
        private List<string> _Name = new List<string>();
        private List<int> _ID = new List<int>();
        /// <summary>
        /// LGRName名字列表
        /// </summary>
        public List<string> Name
        {
            get { return _Name; }
            set { _Name = value; }
        }
        /// <summary>
        /// LGRName名字索引
        /// </summary>
        public List<int> ID
        {
            get { return _ID; }
            set { _ID = value; }
        }
    }
}
