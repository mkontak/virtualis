﻿using System;
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

using Viatronix.Utilities.Licensing.LicenseInstaller.ViewModels;

namespace Viatronix.Utilities.Licensing.LicenseInstaller
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();

      DataContext = new InstallerViewModel();
    } // MainWindow()

    #endregion

  } // class MainWindow
} // namespace Viatronix.Utilities.Licensing.LicenseInstaller