using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace DicomViewer.Windows
{
  /// <summary>
  /// Interaction logic for HeaderWindow.xaml
  /// </summary>
  public partial class HeaderWindow : Window
  {
    public HeaderWindow()
    {
      InitializeComponent();

      GalaSoft.MvvmLight.Threading.DispatcherHelper.Initialize();


    }
  }
}
