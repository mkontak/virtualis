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
using System.Windows.Input;
using Viatronix.Dicom.MVVM.ViewModels;

namespace Viatronix.Dicom.MVVM.Converters
{

  public class ImageViewCursorConverter : IValueConverter
  {




    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {

      Cursor cursor = Cursors.Arrow;

      if (value != null)
      {
        if (value is Viatronix.Dicom.MVVM.ViewModels.Actions)
        {
          if ( (Actions)value == Actions.Panning )
            cursor = new System.Windows.Input.Cursor(Application.GetResourceStream(new Uri("pack://application:,,,/v3ddcmmvvm;component/Resources/pan.ico")).Stream);
          else if ((Actions)value == Actions.WindowLeveling)
            cursor = new System.Windows.Input.Cursor(Application.GetResourceStream(new Uri("pack://application:,,,/v3ddcmmvvm;component/Resources/windowlevel.cur")).Stream);
          else if ((Actions)value == Actions.Zooming)
            cursor = new System.Windows.Input.Cursor(Application.GetResourceStream(new Uri("pack://application:,,,/v3ddcmmvvm;component/Resources/zoom.cur")).Stream);
          else 
            cursor = Cursors.Arrow;
        }
      }

      return cursor;
    }


    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }

  }

}