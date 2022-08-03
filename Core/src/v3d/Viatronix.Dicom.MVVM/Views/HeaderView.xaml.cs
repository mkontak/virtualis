using System;
using System.ComponentModel;
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
using System.Xml;
using Viatronix.Dicom.MVVM.ViewModels;

namespace Viatronix.Dicom.MVVM.Views
{
  /// <summary>
  /// Interaction logic for HeaderView.xaml
  /// </summary>
  public partial class HeaderView : UserControl
  {
    public HeaderView()
    {
      InitializeComponent();
    
    }


    private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
    {
  
    }

  }
}
