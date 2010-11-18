using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace View.Well
{
    /// <summary>
    /// 画井类
    /// </summary>
    public class DrawWell
    {

        #region 画井

        /// <summary>
        /// 画井
        /// </summary>
        /// <param name="w">井</param>
        /// <param name="g">Graphics对象</param>
        public void Draw(Well w, Graphics g)
        {

            Pen p = new Pen(w.Color1, 1f);//画笔

            Font drawFont = new Font("Arial", w.WellCircle * w.Scale, GraphicsUnit.Pixel);//字体

            SolidBrush drawBrush = new SolidBrush(w.Color1);//字体颜色
            //对齐方式
            StringFormat drawFormat = new StringFormat();
            drawFormat.FormatFlags = StringFormatFlags.DisplayFormatControl;
            //是否在屏幕内
            if (w.IsVisible)
            {
                //画井名
                g.DrawString(w.WellName, drawFont, drawBrush, (w.X - w.Width) * w.Scale, (w.Y - w.Height) * w.Scale, drawFormat);
                
                //画井圈
                g.DrawEllipse(p, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                //画井
                if (w.WellType == "油井")
                {
                    SolidBrush br = new SolidBrush(w.Color2);
                    g.FillEllipse(br, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                }
                else
                {
                    if (w.WellType == "水井")
                    {
                        g.DrawEllipse(p, (w.X + w.Width / 4) * w.Scale, (w.Y + w.Height / 4) * w.Scale, (w.Width / 2) * w.Scale, (w.Height / 2) * w.Scale);
                        SolidBrush br = new SolidBrush(w.Color2);
                        g.FillEllipse(br, (w.X + w.Width / 4) * w.Scale, (w.Y + w.Height / 4) * w.Scale, (w.Width / 2) * w.Scale, (w.Height / 2) * w.Scale);
                    }
                    else
                    {
                        if (w.WellType == "报废井")
                        {
                            g.DrawLine(p, (w.X - w.WellCircle / 10) * w.Scale, (w.Y - w.WellCircle / 10) * w.Scale, (w.X + w.Width + w.WellCircle / 10) * w.Scale, (w.Y + w.Height + w.WellCircle / 10) * w.Scale);
                            g.DrawLine(p, (w.X + w.Width + w.WellCircle / 10) * w.Scale, (w.Y - w.WellCircle / 10) * w.Scale, (w.X - w.WellCircle / 10) * w.Scale, (w.Y + w.Height + w.WellCircle / 10) * w.Scale);
                            SolidBrush br = new SolidBrush(w.Color2);
                            g.FillEllipse(br, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);

                        }
                        else
                        {
                            if (w.WellType == "未投产井")
                            {
                                g.DrawRectangle(p, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                                SolidBrush br = new SolidBrush(w.Color2);
                                g.FillEllipse(br, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                            }
                            else
                            {
                                if (w.WellType == "虚拟井")
                                {
                                    SolidBrush br = new SolidBrush(Color.Purple);
                                    g.FillEllipse(br, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                                }
                            }
                        }
                    }
                }
                //画填充颜色
                if (w.IsSelected)
                {
                    Color AlphaColor = Color.FromArgb(150, Color.White);
                    SolidBrush br = new SolidBrush(AlphaColor);
                    g.FillEllipse(br, w.X * w.Scale, w.Y * w.Scale, w.Width * w.Scale, w.Height * w.Scale);
                }

            }
            else
            {
                return;
            }
        }

        #endregion
    }
}
