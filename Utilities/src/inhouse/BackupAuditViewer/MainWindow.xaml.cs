// Copyright © 2011, Viatronix Inc., All Rights Reserved.
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

using Viatronix.Utilities.BackupAuditViewer.ViewModels;

namespace Viatronix.Utilities.BackupAuditViewer
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    #region fields

    /// <summary>
    /// Our datacontext.
    /// </summary>
    private ViewModels.AuditEntriesManagerViewModel m_managerVm = new ViewModels.AuditEntriesManagerViewModel();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindow()
    {
      InitializeComponent();
      
      DataContext = m_managerVm;
    } // MainWindow()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles tha window's loaded event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    public void OnWindowLoadedHandler(object sender, EventArgs args)
    {
      List<AuditEntryViewModel> entries = m_managerVm.GetDatabaseEntries();
      
      foreach(AuditEntryViewModel entry in entries)
      {
        m_managerVm.Entries.Add(entry);
      }
    } // OnWindowLoadedHandler( sender, args )

    #endregion

  } // class MainWindow
} // Viatronix.Utilities.BackupAuditViewer
