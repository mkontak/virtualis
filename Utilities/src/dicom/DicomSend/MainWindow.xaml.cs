// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

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

using Microsoft.Win32;

namespace Viatronix.Utilities.Dicom.DicomSend
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region fields

    /// <summary>
    /// A manager view model.
    /// </summary>
    private SendManagerViewModel m_manager = new SendManagerViewModel();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();
      DataContext = m_manager;
      m_manager.NoFilesToSend += new EventHandler(OnNoFilesToSendHandler);
    } // MainWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the manager's NoFilesToSend event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnNoFilesToSendHandler(object sender, EventArgs args)
    {
      MessageBox.Show("The directory or any children does not contain any files that conform to the provided filter, please verify directory contents.", "Nothing To Be Sent");
    } // OnNoFilesToSendHandler( sender, args )

    /// <summary>
    /// Handles the form closing event to abort the processes that are running.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFormClosingHandler(object sender, CancelEventArgs args)
    {
      m_manager.Abort();
      args.Cancel = false;
    } // OnFormClosingHandler( sender, args )

    #endregion

  } // class MainWindow
} // Viatronix.Utilities.Dicom.DicomSend
