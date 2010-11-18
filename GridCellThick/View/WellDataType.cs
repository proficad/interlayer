using System;
using System.Collections.Generic;
using System.Text;

namespace View
{
    /// <summary>
    /// 井数据类型
    /// </summary>
    public enum WellDataType
    {
        /// <summary>
        /// 砂岩厚度
        /// </summary>
        SandThick=0,
        /// <summary>
        /// 有效厚度
        /// </summary>
        EffectiveThickness=1,
        /// <summary>
        /// 渗透率
        /// </summary>
        Permeability=2,
        /// <summary>
        /// 孔隙度
        /// </summary>
        Porosity=3,
        /// <summary>
        /// 束缚水饱和度
        /// </summary>
        BoundWaterSaturation=4,
        /// <summary>
        /// 目前含水饱和度
        /// </summary>
        WaterSaturation=5
    }
}
