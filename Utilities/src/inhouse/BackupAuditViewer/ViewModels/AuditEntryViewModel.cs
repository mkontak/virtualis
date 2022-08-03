// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.BackupAuditViewer.ViewModels
{
  /// <summary>
  /// A jira audit entry record.
  /// </summary>
  public class AuditEntryViewModel : GalaSoft.MvvmLight.ViewModelBase
  {

    #region fields

    /// <summary>
    /// The type of action performed.
    /// </summary>
    private string m_type = string.Empty;

    /// <summary>
    /// The subtype of the action performed.
    /// </summary>
    private string m_subType = string.Empty;

    /// <summary>
    /// The server.
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// The system, JIRA/Subversion.
    /// </summary>
    private string m_system = string.Empty;

    /// <summary>
    /// The source.
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// The started date time.
    /// </summary>
    private DateTimeOffset m_started = DateTimeOffset.MinValue;

    /// <summary>
    /// The completed date time.
    /// </summary>
    private DateTimeOffset m_completed = DateTimeOffset.MaxValue;

    /// <summary>
    /// The time of the action.
    /// </summary>
    private string m_time = string.Empty;

    /// <summary>
    /// The affected file.
    /// </summary>
    private string m_file = string.Empty;

    /// <summary>
    /// The status.
    /// </summary>
    private string m_status = string.Empty;

    /// <summary>
    /// The message (indicates status).
    /// </summary>
    private string m_message = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the type.
    /// </summary>
    public string Type
    {
      get { return m_type; }
      set
      {
        m_type = value;
        RaisePropertyChanged("Type");
      }
    } // Type


    /// <summary>
    /// Gets or sets the subtype.
    /// </summary>
    public string SubType
    {
      get { return m_subType; }
      set
      {
        m_subType = value;
        RaisePropertyChanged("SubType");
      }
    } // SubType


    /// <summary>
    /// Gets or sets the server.
    /// </summary>
    public string Server
    {
      get { return m_server; }
      set
      {
        m_server = value;
        RaisePropertyChanged("Server");
      }
    } // Server


    /// <summary>
    /// Gets or sets the system.
    /// </summary>
    public string System
    {
      get { return m_system; }
      set
      {
        m_system = value;
        RaisePropertyChanged("System");
      }
    } // System


    /// <summary>
    /// Gets or sets the source.
    /// </summary>
    public string Source
    {
      get { return m_source; }
      set
      {
        m_source = value;
        RaisePropertyChanged("Source");
      }
    } // Source


    /// <summary>
    /// Gets or sets the started date.
    /// </summary>
    public DateTimeOffset Started
    {
      get { return m_started; }
      set
      {
        m_started = value;
        RaisePropertyChanged("Started");
      }
    } // Started


    /// <summary>
    /// Gets or sets the completed date.
    /// </summary>
    public DateTimeOffset Completed
    {
      get { return m_completed; }
      set
      {
        m_completed = value;
        RaisePropertyChanged("Completed");
      }
    } // Completed


    /// <summary>
    /// Gets or sets the time.
    /// </summary>
    public string Time
    {
      get { return m_time; }
      set
      {
        m_time = value;
        RaisePropertyChanged("Time");
      }
    } // Time


    /// <summary>
    /// Gets or sets the file.
    /// </summary>
    public string File
    {
      get { return m_file; }
      set
      {
        m_file = value;
        RaisePropertyChanged("File");
      }
    } // File


    /// <summary>
    /// Gets or sets the status.
    /// </summary>
    public string Status
    {
      get { return m_status; }
      set
      {
        m_status = value;
        RaisePropertyChanged("Status");
      }
    } // Status


    /// <summary>
    /// Gets or sets the message.
    /// </summary>
    public string Message
    {
      get { return m_message; }
      set
      {
        m_message = value;
        RaisePropertyChanged("Message");
      }
    } // Message

    #endregion

  } // class AuditEntryViewModel
} // namespace Viatronix.Utilities.BackupAuditViewer.ViewModels
