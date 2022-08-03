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
using Viatronix.Dicom;
using System.Globalization;

namespace Viatronix.Dicom.MVVM.Converters
{

  /// <summary>
  /// Allows the element tag to be indented based on the level
  /// 
  /// example:
  ///     (0020,0020)   Level = 0 then no indent
  ///     (0010,0010)   Level = 1 then indent of 2 spaces
  ///     
  /// </summary>
  public class ElementTagConverter : IMultiValueConverter
  {



    /// <summary>
    /// Converts the dicom headers tag to have an indent of 2 spaces times the level indicator
    /// </summary>
    /// <param name="values"></param>
    /// <param name="targetType"></param>
    /// <param name="parameter"></param>
    /// <param name="culture"></param>
    /// <returns></returns>
    public object Convert(object [] values, Type targetType, object parameter, CultureInfo culture)
    {

      if (values.Length > 0 )
      {
        string ptag = (values[0] != null ? (string)values[0] : "");
        UInt32 level = (values[1] != null ? System.Convert.ToUInt32(values[1]) : 0);

        string tag = "";
        for (int i = 0; i < level; tag = tag + "   ", i++) ;
        tag = tag + ptag;

        return tag;

      }
      return null;
    } // Convert()


    /// <summary>
    /// ConvertBack to the original (Not Impelemented)
    /// </summary>
    /// <param name="value"></param>
    /// <param name="targetType"></param>
    /// <param name="parameter"></param>
    /// <param name="culture"></param>
    /// <returns></returns>
    public object [] ConvertBack(object value, Type [] targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    } // ConvertBack

  } // class ElementTagConverter

} // namespace Viatronix.Dicom.MVVM.Converters
