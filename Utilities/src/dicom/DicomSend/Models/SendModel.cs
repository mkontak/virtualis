// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Utilities.Dicom.DicomSend.Enumerations;

namespace Viatronix.Utilities.Dicom.DicomSend.Models
{

  #region delegates

  /// <summary>
  /// Delegate to handle progress.
  /// </summary>
  /// <param name="sender"></param>
  /// <param name="progress"></param>
  /// <param name="totalProgress"></param>
  public delegate void ProgressEventHandler(object sender, int progress, int totalProgress);

  #endregion

  /// <summary>
  /// A model that can start and stop the SendImage process and capture output from it.
  /// </summary>
  public class SendModel
  {

    #region events
    
    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    public event ProgressEventHandler Progress;

    /// <summary>
    /// Notifies that the process has completed.
    /// </summary>
    public event EventHandler Completed;

    /// <summary>
    /// Notifies that the operation failed.
    /// </summary>
    public event EventHandler Failed;

    /// <summary>
    /// Notifies that the active state has changed.
    /// </summary>
    public event EventHandler ActiveChanged;

    #endregion

    #region fields

    /// <summary>
    /// The identifier for this send operation.
    /// </summary>
    private Guid m_id = Guid.NewGuid();

    /// <summary>
    /// The store object.
    /// </summary>
    private CStore m_store = null;

    /// <summary>
    /// The arguments provided to this model.
    /// </summary>
    private SendDicomArgs m_sendImageArgs = new SendDicomArgs();

    /// <summary>
    /// Notes whether or not the process is active.
    /// </summary>
    private bool m_active = false;

    /// <summary>
    /// Did the user abort the send.
    /// </summary>
    private bool m_aborted = false;

    /// <summary>
    /// Did an error occur.
    /// </summary>
    private bool m_errorOccurred = false;

    /// <summary>
    /// The error message.
    /// </summary>
    private string m_errorMessage = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets the Id.
    /// </summary>
    public Guid ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID


    /// <summary>
    /// Gets the arguments that will be provided to the SendImage process.
    /// </summary>
    public SendDicomArgs Arguments
    {
      get { return m_sendImageArgs; }
      set { m_sendImageArgs = value; }
    } // Arguments


    /// <summary>
    /// Gets or sets the active state.
    /// </summary>
    public bool Active
    {
      get { return m_active; }
      set { m_active = value; }
    } // Active


    /// <summary>
    /// Gets or sets whether an error occurred in the image send.  True if error, false otherwise.
    /// </summary>
    public bool Erroroccurred
    {
      get { return m_errorOccurred; }
      set { m_errorOccurred = value; }
    } // Erroroccurred


    /// <summary>
    /// Gets the error message.
    /// </summary>
    public string ErrorMessage
    {
      get { return m_errorMessage; }
      set { m_errorMessage = value; }
    } // ErrorMessage

    #endregion

    #region methods

    /// <summary>
    /// Starts the model's processing of DICOM.
    /// </summary>
    public void Start()
    {
      Active = true;
      if (ActiveChanged != null)
      {
        ActiveChanged(this, EventArgs.Empty);
      }

      Thread thread = new Thread(() => Run());
      thread.Start();
    } // Start()


    /// <summary>
    /// Aborts the model.
    /// </summary>
    public void Abort()
    {
      m_aborted = true;
      if (m_store != null && !Erroroccurred)
      {
        m_store.Abort();

        Erroroccurred = true;
        ErrorMessage = "The send was aborted by user.";

        if (Failed != null)
        {
          Failed(this, EventArgs.Empty);
        }
      }
    } // Abort()


    /// <summary>
    /// Executes the process to send the images in the provided directory.
    /// </summary>
    private void Run()
    {
      try
      {
        Active = true;
        if (ActiveChanged != null)
        {
          ActiveChanged(this, EventArgs.Empty);
        }

        if (m_aborted)
        {
          return;
        }

        m_store = CStore.Create(m_sendImageArgs.AETitle, m_sendImageArgs.CallingAETitle, m_sendImageArgs.Server, (uint)m_sendImageArgs.Port);
        m_store.Progress += new ClientEventHandler(OnStoreProgressHandler);
        m_store.Failed += new ClientEventHandler(OnStoreFailedHandler);

        if (!m_sendImageArgs.IsStoreAsMultiAssoc)
        {
          string[] files = Directory.GetFiles(m_sendImageArgs.Directory, m_sendImageArgs.Filter, SearchOption.AllDirectories);
          foreach (string file in files)
          {
            m_store.AddFile(file);
          }
        }
        else
        {
          string[] files = Directory.GetFiles(m_sendImageArgs.Directory, m_sendImageArgs.Filter, SearchOption.TopDirectoryOnly);
          foreach (string file in files)
          {
            m_store.AddFile(file);
          }
        }

        m_store.Start();
        m_store.Wait();

        if (!Erroroccurred)
        {
          if (Completed != null)
          {
            Completed(this, EventArgs.Empty);
          }
        }
      }
      catch (Exception e)
      {
        Erroroccurred = true;
        ErrorMessage = e.Message;
        if (Failed != null)
        {
          Failed(this, EventArgs.Empty);
        }
      }
      finally
      {
        Active = false;
        if (ActiveChanged != null)
        {
          ActiveChanged(this, EventArgs.Empty);
        }
      }
    } // Run()

    #endregion

    #region event handlers

    /// <summary>
    /// Handle's the CStore's Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreProgressHandler(object sender, ClientEventArgs args)
    {
      if (Progress != null)
      {
        Progress(this, args.Completed, args.Completed + args.Remaining);
      }
    } // OnStoreProgressHandler( sender, args )


    /// <summary>
    /// Handles the CStore's Failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreFailedHandler(object sender, ClientEventArgs args)
    {
      Log.Error(string.Format("Error captured from process.  Data: {0}", args.Message), "SendModel", "OnErrorDataReceivedHandler");

      Erroroccurred = true;
      ErrorMessage = args.Message;
      if (Failed != null)
      {
        Failed(this, EventArgs.Empty);
      }
    } // OnStoreFailedHandler( sender, args )

    #endregion

  } // class SendModel
} // namespace Viatronix.Utilities.Dicom.DicomSend.Models
