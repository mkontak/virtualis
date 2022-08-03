using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Windows.Media;
using System.IO;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Runtime.InteropServices;

namespace Viatronix.UI.Converters
{

  public class BitmapConverter : IValueConverter
  {


    #region static methods

    /// <summary> 
    /// Converts a <see cref="System.Drawing.Image"/> into a WPF <see cref="BitmapSource"/>. 
    /// </summary> 
    /// <param name="source">The source image.</param> 
    /// <returns>A BitmapSource</returns> 
    public static BitmapSource ToBitmapSource(System.Drawing.Image image)
    {
      System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(image);

      var bitmapSource = ToBitmapSource(bitmap);

      bitmap.Dispose();
      bitmap = null;

      return bitmapSource;
    }

    /// <summary> 
    /// Converts a <see cref="System.Drawing.Bitmap"/> into a WPF <see cref="BitmapSource"/>. 
    /// </summary> 
    /// <remarks>Uses GDI to do the conversion. Hence the call to the marshalled DeleteObject. 
    /// </remarks> 
    /// <param name="source">The source bitmap.</param> 
    /// <returns>A BitmapSource</returns> 
    public static BitmapSource ToBitmapSource(System.Drawing.Bitmap bitmap)
    {

      using (MemoryStream memoryStream = new MemoryStream())
      {
        try
        {
          // You need to specify the image format to fill the stream. 
          // I'm assuming it is PNG
          bitmap.Save(memoryStream, System.Drawing.Imaging.ImageFormat.Bmp);
          memoryStream.Seek(0, SeekOrigin.Begin);

          BitmapDecoder bitmapDecoder =
            BitmapDecoder.Create(memoryStream, BitmapCreateOptions.PreservePixelFormat, BitmapCacheOption.OnLoad);

          // This will disconnect the stream from the image completely...
          WriteableBitmap writable = new WriteableBitmap(bitmapDecoder.Frames.Single());

          writable.Freeze();

          return writable;
        }
        catch (Exception)
        {
          return null;
        }
      } // END ... Using MemoryStream
    }

    #endregion

    #region IValueConverter Members

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
      if (targetType != typeof(ImageSource))
        throw new InvalidOperationException("The target must be ImageSource or derived types");

      BitmapSource source = null;

      if (value != null)
      {
        if (value is System.Drawing.Bitmap)
        {
          source = ToBitmapSource((System.Drawing.Bitmap)value);
        }
        else if (value is BitmapSource)
        {
          source = (BitmapSource)value;
        }
        else
          throw new System.InvalidCastException("Unsupported conversion type");
      }

      return source;

    } // Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)

    public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
      throw new NotImplementedException();
    }

    #endregion
  }
}
