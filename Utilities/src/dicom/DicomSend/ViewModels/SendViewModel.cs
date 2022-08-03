// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Windows.Media;
using System.Windows.Controls;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;

using Viatronix.UI;
using Viatronix.Utilities.Dicom.DicomSend.Models;
using Viatronix.Utilities.Dicom.DicomSend.Enumerations;

namespace Viatronix.Utilities.Dicom.DicomSend
{
  /// <summary>
  /// A view model for the Send object.
  /// </summary>
  public class SendViewModel : Viatronix.UI.ViewModels.ViewModelBase
  {

    #region events

    /// <summary>
    /// Notifies that the send has finished.
    /// </summary>
    public event EventHandler Finished;

    #endregion

    #region consts

    /// <summary>
    /// The images that constitute the pending animation.
    /// </summary>
    private readonly Uri[] m_pendingSources = new Uri[] 
    {
    }; // m_pendingSources

    #endregion

    #region fields

    /// <summary>
    /// The model we will be working against.
    /// </summary>
    private SendModel m_model = new SendModel();

    /// <summary>
    /// The status of the model.
    /// </summary>
    private SendStatusTypes m_status = SendStatusTypes.Pending;

    /// <summary>
    /// Indicatest the model's current image.
    /// </summary>
    private int m_current = 0;

    /// <summary>
    /// Indicates the model's total image count.
    /// </summary>
    private int m_total = 0;

    /// <summary>
    /// The progress.
    /// </summary>
    private float m_progress = 0.0F;

    /// <summary>
    /// An error code retrived from the send process.
    /// </summary>
    private string m_errorMessage = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public SendViewModel()
    {
      m_model.Progress  += new ProgressEventHandler(OnModelProgressHandler);
      m_model.Completed += new EventHandler(OnCompletedHandler);
      m_model.Failed    += new EventHandler(OnFailedHandler);
    } // SendViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the ID.
    /// </summary>
    public Guid ID
    {
      get { return m_model.ID; }
      set
      {
        m_model.ID = value;
        RaisePropertyChanged("ID");
      }
    } // ID


    /// <summary>
    /// Gets or sets the directory that the model is working on.
    /// </summary>
    public string Directory
    {
      get { return m_model.Arguments.Directory; }
      set
      {
        m_model.Arguments.Directory = value;
        RaisePropertyChanged("Directory");
      }
    } // Directory


    /// <summary>
    /// Gets the model's Current state.
    /// </summary>
    public int Current
    {
      get { return m_current; }
      private set
      {
        m_current = value;
        RaisePropertyChanged("Current");
      }
    } // Current


    /// <summary>
    /// Gets the model's TotalCount state.
    /// </summary>
    public int TotalCount
    {
      get { return m_total; }
      private set
      {
        m_total = value;
        RaisePropertyChanged("TotalCount");
      }
    } // TotalCount


    /// <summary>
    /// Gets or sets the progress of the operation.
    /// </summary>
    public float Progress
    {
      get { return m_progress; }
      private set
      {
        m_progress = value;
        RaisePropertyChanged("Progress");
      }
    } // Progress
    

    /// <summary>
    /// Gets or sets the error message of the send.
    /// </summary>
    public string Message
    {
      get { return m_errorMessage; }
      set
      {
        m_errorMessage = value;
        RaisePropertyChanged("Message");
      }
    } // Message


    /// <summary>
    /// Gets or sets the arguments provided to the SendDicom Model.
    /// </summary>
    public SendDicomArgs Arguments
    {
      get { return m_model.Arguments; }
      set { m_model.Arguments = value; }
    } // Arguments


    /// <summary>
    /// Gets or sets the status.
    /// </summary>
    public SendStatusTypes Status
    {
      get { return m_status; }
      set
      {
        m_status = value;
        RaisePropertyChanged("Status");
      }
    } // Status

    #endregion

    #region methods

    /// <summary>
    /// Starts the model's send to the server.
    /// </summary>
    public void Start()
    {
      m_model.Start();
    } // Start()


    /// <summary>
    /// Aborts the send.
    /// </summary>
    public void Abort()
    {
      m_model.Abort();
    } // Abort()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the model's Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="progress">The current progress.</param>
    /// <param name="totalProgress">The total progress.</param>
    private void OnModelProgressHandler(object sender, int progress, int totalProgress)
    {
      Current = progress;
      TotalCount = totalProgress;
      Message = string.Format("Sent {0} of {1}", progress, totalProgress);

      Progress = ((float)progress / (float)totalProgress) * 100.0F;
    } // OnModelProgressHandler( sender, progress, totalProgress )


    /// <summary>
    /// Handles the model's Completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnCompletedHandler(object sender, EventArgs args)
    {
      Status = SendStatusTypes.Completed;
      Message = string.Empty;

      if(Finished != null)
      {
        Finished(this, EventArgs.Empty);
      }
    } // OnCompletedHandler( sender, args )


    /// <summary>
    /// Handles the model's Failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnFailedHandler(object sender, EventArgs args)
    {
      Status = SendStatusTypes.Failed;
      Message = m_model.ErrorMessage;

      if (Finished != null)
      {
        Finished(this, EventArgs.Empty);
      }
    } // OnFailedHandler( sender, args )

    #endregion

  } // class SendViewModel
} // Viatronix.Utilities.Dicom.DicomSend
