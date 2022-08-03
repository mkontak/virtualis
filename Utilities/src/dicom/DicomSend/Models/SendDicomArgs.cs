// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Utilities.Dicom.DicomSend.Enumerations;

namespace Viatronix.Utilities.Dicom.DicomSend.Models
{
  /// <summary>
  /// Arguments for sending dicom.
  /// </summary>
  public class SendDicomArgs : ICloneable
  {

    #region fields

    /// <summary>
    /// The directory to send.
    /// </summary>
    private string m_directory = ConfigurationManager.AppSettings["DefaultDirectory"];

    /// <summary>
    /// The sender's AETitle.
    /// </summary>
    private string m_aeTitle = ConfigurationManager.AppSettings["AETitle"];

    /// <summary>
    /// The calling AE Title.
    /// </summary>
    private string m_callingAETitle = ConfigurationManager.AppSettings["Calling_AETitle"];

    /// <summary>
    /// The server we are sending to.
    /// </summary>
    private string m_server = ConfigurationManager.AppSettings["Server"];

    /// <summary>
    /// The port.
    /// </summary>
    private int m_port = Convert.ToInt32(ConfigurationManager.AppSettings["Port"]);

    /// <summary>
    /// The extension filter of files to send.
    /// </summary>
    private string m_filter = ConfigurationManager.AppSettings["Filter"];

    /// <summary>
    /// The transfer syntax protocol.
    /// </summary>
    private TransferSyntaxTypes m_transferSyntax = TransferSyntaxTypes.Both;

    /// <summary>
    /// Store as multiple associations?
    /// </summary>
    private bool m_isStoreAsMultiAssoc = Convert.ToBoolean(ConfigurationManager.AppSettings["StoreAsMultiAssoc"]);

    /// <summary>
    /// How many associations to open if sending as multiple associations.
    /// </summary>
    private int m_multiAssocThreads = Convert.ToInt32(ConfigurationManager.AppSettings["MultiAssocThreads"]);

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the directory.
    /// </summary>
    public string Directory
    {
      get { return m_directory; }
      set { m_directory = value; }
    } // Directory


    /// <summary>
    /// Gets or sets the AETitle.
    /// </summary>
    public string AETitle
    {
      get { return m_aeTitle; }
      set { m_aeTitle = value; }
    } // AETitle


    /// <summary>
    /// Gets or sets the calling AE Title.
    /// </summary>
    public string CallingAETitle
    {
      get { return m_callingAETitle; }
      set { m_callingAETitle = value; }
    } // CallingAETitle


    /// <summary>
    /// Gets or sets the Server.
    /// </summary>
    public string Server
    {
      get { return m_server; }
      set { m_server = value; }
    } // Server


    /// <summary>
    /// Gets or sets the Port.
    /// </summary>
    public int Port
    {
      get { return m_port; }
      set { m_port = value; }
    } // Port


    /// <summary>
    /// The filter of valid dicom files to send.
    /// </summary>
    public string Filter
    {
      get { return m_filter; }
      set { m_filter = value; }
    } // Pattern


    /// <summary>
    /// Gets or sets the transfer syntax protocol.
    /// </summary>
    public TransferSyntaxTypes TransferSyntax
    {
      get { return m_transferSyntax; }
      set { m_transferSyntax = value; }
    } // TransferSyntax


    /// <summary>
    /// Gets or sets if we are to store as multiple associations.
    /// </summary>
    public bool IsStoreAsMultiAssoc
    {
      get { return m_isStoreAsMultiAssoc; }
      set { m_isStoreAsMultiAssoc = value; }
    } // StoreAsMultipleAssoc


    /// <summary>
    /// Gets or sets the number of multi association threads.
    /// </summary>
    public int MultiAssocThreads
    {
      get { return m_multiAssocThreads; }
      set { m_multiAssocThreads = value; }
    } // MultiAssocThreads

    #endregion

    #region methods

    /// <summary>
    /// Clones this instance.
    /// </summary>
    /// <returns>A clone of this instance.</returns>
    public object Clone()
    {
      SendDicomArgs args = new SendDicomArgs();

      args.Directory = Directory;
      args.AETitle = AETitle;
      args.CallingAETitle = CallingAETitle;
      args.Filter = Filter;
      args.Port = Port;
      args.Server = Server;
      args.IsStoreAsMultiAssoc = IsStoreAsMultiAssoc;
      args.MultiAssocThreads = MultiAssocThreads;
      args.TransferSyntax = TransferSyntax;

      return args;
    } // Clone()

    #endregion

  } // class SendDicomArgs
} // namespace Viatronix.Utilities.Dicom.DicomSend.Models
