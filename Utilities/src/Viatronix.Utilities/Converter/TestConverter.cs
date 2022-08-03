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
using System.Globalization;
using System.Diagnostics;

namespace Viatronix.Utilities.Converter
{

  /// <summary>
  /// Implements a test convert that can be used to determine if evenet are being triggered
  /// </summary>
  public class TestConverter : IValueConverter
  {

    /// <summary>
    /// Performs the conversion
    /// </summary>
    /// <param name="value">Value to be converted</param>
    /// <param name="targetType">Target type</param>
    /// <param name="parameter">Parameter</param>
    /// <param name="culture">Culture</param>
    /// <returns>Converted parameter</returns>
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
      Debug.WriteLine("TestConverter.Convert(value := {0}, targetType := {1}, parameter := {2}, culture := {3})", value, targetType, parameter, culture);
      return value; // Put Breakpoint Here!!    
    } // Convert(object value, Type targetType, object parameter, CultureInfo culture)

    /// <summary>
    /// Convert callback
    /// </summary>
    /// <param name="value"></param>
    /// <param name="targetType"></param>
    /// <param name="parameter"></param>
    /// <param name="culture"></param>
    /// <returns></returns>
    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      Debug.WriteLine("TestConverter.ConvertBack(value := {0}, targetType := {1}, parameter := {2}, culture := {3})", value, targetType, parameter, culture);
      return value;
    } // ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)

  } // class TestConverter

} // namespace Viatronix.Utilities.Converter
