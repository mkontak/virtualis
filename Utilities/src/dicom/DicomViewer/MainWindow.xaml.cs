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
using System.Windows.Navigation;
using System.Windows.Shapes;
using Viatronix.Dicom.ViewModel;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using Viatronix.Dicom;
using System.Xml;
using GalaSoft.MvvmLight.Command;

namespace DicomViewer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

 
    public MainWindow()
    {

 
      InitializeComponent();

      GalaSoft.MvvmLight.Threading.DispatcherHelper.Initialize();

 
    }

    private void dicomViewer_Closing(object sender, CancelEventArgs e)
    {

    }

 
 
  }
}
