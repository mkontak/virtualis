using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;


namespace Viatronix.UI
{
    public static class Utilities
    {


      /// <summary>
      /// Converts the Stsrem.Drawing bitmap to a media image
      /// </summary>
      /// <param name="source"></param>
      /// <returns></returns>
      public static System.Windows.Media.Imaging.BitmapSource ToBitmapSource(System.Drawing.Bitmap source)
      {
          return System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(source.GetHbitmap(), IntPtr.Zero, Int32Rect.Empty,
              System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions());
      }

 
    }
}
