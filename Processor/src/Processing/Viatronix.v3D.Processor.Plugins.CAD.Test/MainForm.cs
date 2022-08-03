// $Id: MainForm.cs,v 1.9 2006/12/21 20:14:50 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


#region using

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Viatronix.v3D.Logging;

#endregion


namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  /// <summary>
  /// The Main form
  /// </summary>
  public partial class CADConsoleForm : Viatronix.v3D.UI.Dialog
  {
    private delegate void StartProcess();

    #region fields

    private StartProcess StartProcessDelagate = null;


    /// <summary>
    /// Args object
    /// </summary>
    private EnvrionmentArgs m_args = new EnvrionmentArgs();

    /// <summary>
    /// available appIds collection
    /// </summary>
    private System.Collections.ArrayList m_availableAppIds = new System.Collections.ArrayList();

    /// <summary>
    /// Process thread object
    /// </summary>
    private System.Threading.Thread m_processThread = null;

    /// <summary>
    /// Pulse generator
    /// </summary>
    private Viatronix.v3D.UI.PulseGenerator m_pulseGenerator = null;

    /// <summary>
    /// stage finished event handler
    /// </summary>
    ProcessEventHandler m_stageFinishedEventHandler;

    /// <summary>
    /// process started event handler
    /// </summary>
    ProcessEventHandler m_processStartEventHandler;

    /// <summary>
    /// process aborted event handler
    /// </summary>
    ProcessEventHandler m_processAbortedEventHandler;

    /// <summary>
    /// process failed event handler
    /// </summary>
    ProcessEventHandler m_processFailedEventHandler;

    /// <summary>
    /// process progress event handler
    /// </summary>
    ProcessEventHandler m_processProgressEventHandler;

    /// <summary>
    /// Enables UI event handler
    /// </summary>
    EventHandler m_enableUIEventHandler;

    /// <summary>
    /// Process object
    /// </summary>
    V3DProcess m_process = new V3DProcess();

    
    /// <summary>
    /// Started flag
    /// </summary>
    private bool m_bStarted = false;

#endregion

    #region property


    #endregion

    #region methods
    /// <summary>
    /// Constructor
    /// </summary>
    public CADConsoleForm()
    {
      InitializeComponent();

       StartProcessDelagate = new StartProcess ( this.StartProcessing );

      //Create Logger
      string logFolder = Viatronix.v3D.Utilities.Registry.SystemSettings.LogDirectory;
      string logFile = System.IO.Path.Combine(logFolder, "Viatronix.v3D.Processor.Plugins.CAD.Test");
      Logger.CreateDefaultLogger(logFile, LogLevel.Highest);

      //attach event handlers
      m_processStartEventHandler    = new ProcessEventHandler(this.OnProcessStartHandler);
      m_processAbortedEventHandler  = new ProcessEventHandler(this.OnProcessAbortedHandler);
      m_processFailedEventHandler   = new ProcessEventHandler(this.OnProcessFailedHandler);
      m_processProgressEventHandler = new ProcessEventHandler(this.OnProcessProgressHandler);
      m_stageFinishedEventHandler   = new ProcessEventHandler(this.OnStageFinshedHandler);
      m_enableUIEventHandler        = new EventHandler(this.OnEnableUIButtons);

      m_process.Started   += m_processStartEventHandler;
      m_process.Failed    += m_processFailedEventHandler;
      m_process.Progress  += m_processProgressEventHandler;
      m_process.Aborted   += m_processAbortedEventHandler;
      m_process.StageFinished += m_stageFinishedEventHandler;
      m_process.AllProcessFinished += m_enableUIEventHandler;


      m_progressbar.Visible = false;

      Configuration.ReadFromAppConfig(m_args);
      m_availableAppIds = Configuration.GetAllAppIds();

      //TODO: read it from the config file
      m_destinationFolder.Text = m_args.WorkingFolder;

      m_dicomFolder.Text = m_args.DicomFolder;
      m_preProcessCheck.Checked = m_args.RunPreProcess;

      foreach (string appid in m_availableAppIds)
        m_appIds.Items.Add(appid);

      if (m_availableAppIds.Count > 0)
      {
        if (m_availableAppIds.Contains(m_args.AppId))
          m_appIds.SelectedItem = m_args.AppId;
        else
          m_appIds.SelectedItem = m_availableAppIds[0];
      }

      m_pulseGenerator = new Viatronix.v3D.UI.PulseGenerator(m_progressbar);
      m_pulseGenerator.Interval = 10;
      m_progressbar.Pulse = true;
    }//CADConsoleForm
       
    /// <summary>
    /// Start Processing method
    /// </summary>
    private void StartProcessing()
    {
      m_process.Args = m_args;
      m_process.Run();
    }//StartProcessing

    /// <summary>
    /// Validates the input variables
    /// </summary>
    /// <returns></returns>
    private bool ValidateInputs()
    {
      m_args.RunPreProcess = m_preProcessCheck.Checked;
      m_args.WorkingFolder = m_destinationFolder.Text;
      m_args.DicomFolder   = m_dicomFolder.Text;

      if (m_appIds.SelectedItem != null)
        m_args.AppId = m_appIds.SelectedItem.ToString();
      else 
        m_args.AppId = m_appIds.Text;

      #region Validate the DICOM Folder

      if (m_args.RunPreProcess)
      {
        if (m_args.DicomFolder == string.Empty)
        {
          Viatronix.v3D.UI.MessageBox.ShowWarning("Pick a valid DICOM folder to begin.");
          m_dicomFolder.Select();
          return false;
        }

        if (!System.IO.Directory.Exists(m_args.DicomFolder))
        {
          Viatronix.v3D.UI.MessageBox.ShowWarning("Invalid DICOM folder.");
          m_dicomFolder.Select();
          return false;
        }

        if (System.IO.Directory.GetFiles(m_args.DicomFolder).Length <= 0)
        {
          Viatronix.v3D.UI.MessageBox.ShowWarning("Selected DICOM folder is empty.");
          m_dicomFolder.Select();
          return false;
        }
      }
      #endregion

      #region Validate the Working Folder

      if ( m_args.WorkingFolder == string.Empty )
      {
        Viatronix.v3D.UI.MessageBox.ShowWarning("Pick a working folder to begin.");
        m_destinationFolder.Select();
        return false;
      }
      if ( m_args.RunPreProcess )
      {
        if ( System.IO.Directory.Exists(m_args.WorkingFolder) )
        {
          string warningMessage = "' already exists. Do you want to overwrite it?";
          if (!Viatronix.v3D.UI.MessageBox.ShowQuestion( "Folder '" + m_args.WorkingFolder + warningMessage))
            return false;
        }
        try
        {
          System.IO.Directory.CreateDirectory(m_args.WorkingFolder);
        }
        catch ( Exception ex )
        {
          Viatronix.v3D.UI.MessageBox.ShowError("Failed to create the working folder '" + m_args.WorkingFolder + "'. " + ex.Message);
          return false;
        }
      }
      else
      {
        if (!System.IO.Directory.Exists(m_args.WorkingFolder) )
        {
          Viatronix.v3D.UI.MessageBox.ShowWarning("Invalid Working folder.");
          m_dicomFolder.Select();
          return false;
        }
        //Check for Volume and PreProcessed folders.
        string volumeFolder = System.IO.Path.Combine( m_args.WorkingFolder, "Volume" );
        if (!System.IO.Directory.Exists(volumeFolder) || System.IO.Directory.GetFiles(volumeFolder, "*.vol").Length <= 0)
        {
          Viatronix.v3D.UI.MessageBox.ShowError(" The folder '" + volumeFolder + "' doesn't have any .vol files.Perform preprocessing if necessary");
          m_destinationFolder.Select();
          return false;
        }

        string preprocessedFolder = System.IO.Path.Combine(m_args.WorkingFolder, "PreProcessed");
        if (!System.IO.Directory.Exists(preprocessedFolder) || System.IO.Directory.GetFiles(preprocessedFolder).Length <= 0)
        {
          Viatronix.v3D.UI.MessageBox.ShowError(" The folder '" + preprocessedFolder + "' doesn't have any preprocessed results.Perform preprocessing if necessary");
          m_destinationFolder.Select();
          return false;
        }

      }

      #endregion

      #region Validate the AppID
      if ( m_args.AppId == string.Empty )
      {
        Viatronix.v3D.UI.MessageBox.ShowWarning( "Pick a plugin id to begin." );
        m_appIds.Select();
        return false;
      }
     
      //Log the selcted items 
      Logger.Dbg( "Selected Plugin ID is - " + m_args.AppId );
      Logger.Dbg( "Working folder is  - " + m_args.WorkingFolder );
      Logger.Dbg( "DICOM Folder is    - " + m_args.DicomFolder );
      Logger.Dbg( "Preprocess option  - " + m_args.RunPreProcess.ToString() );

      #endregion

      return true;
    }//ValidateInputs()

    #endregion

    #region eventHandlers
    /// <summary>
    /// Handles the run click event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnRunClickHandler(object sender, EventArgs e)
    {
      if (!ValidateInputs())
        return;

      Configuration.Save(m_args);

      m_progressbar.Visible = true;
      m_progressbar.BackColor = Color.Black;
      m_buttonRun.Enabled   = false;
      m_Close.Text = "Abort";

      StartProcessDelagate.BeginInvoke(null, null);

      //m_processThread = new System.Threading.Thread(new System.Threading.ThreadStart(StartProcessing));
      //m_processThread.Start();


    }//OnRunClickHandler(object sender, EventArgs e)

    private void OnProcessFinishedCallBack(IAsyncResult ar)
    {
      if (ar.IsCompleted)
      {
        //Ends the Asynchronous Call.
        StartProcessDelagate.EndInvoke(ar);
        // Invoke the delegate on the UI thread.
        //if (!this.IsDisposed )
        //  this.Invoke(m_logViewUpdateHandler);
      }
    }


    private void OnFormClosing(object sender, FormClosingEventArgs e)
    {
      if (m_process.IsRunning())
      {
        try
        {
          m_process.Abort();
        }
        catch (Exception ex)
        {
          Logger.Err(ex.Message);
        }
      }//if (m_process.IsRunning())
    }//OnProcessAbortedHandler(object sender, ProcessEventArg arg)

    /// <summary>
    /// Handles the close event args
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnCloseClickHandler(object sender, EventArgs e)
    {
      if ( m_process.IsRunning())
      {
        try
        {
          m_process.Abort();
        }
        catch (Exception ex)
        {
          Logger.Err(ex.Message);
        }
      }//if (m_process.IsRunning())
      if ( m_Close.Text == "Close" )
      {
        m_process.Started -= m_processStartEventHandler;
        m_process.Failed -= m_processFailedEventHandler;
        m_process.Progress -= m_processProgressEventHandler;
        m_process.Aborted -= m_processAbortedEventHandler;
        m_process.StageFinished -= m_stageFinishedEventHandler;
        m_process.AllProcessFinished -= m_enableUIEventHandler;

        this.Close();
      }
    }//OnCloseClickHandler

    /// <summary>
    /// Handles the DICOM browse button click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnDICOMBrowseClickHandler(object sender, EventArgs e)
    {
      FolderBrowserDialog browserDialog = new FolderBrowserDialog();
      browserDialog.ShowNewFolderButton = false;
      browserDialog.Description = "Select the DICOM folder";
      browserDialog.SelectedPath = m_dicomFolder.Text;

      if (browserDialog.ShowDialog() == DialogResult.OK)
      {
        m_dicomFolder.Text = browserDialog.SelectedPath;
        m_args.DicomFolder = browserDialog.SelectedPath;
      }
    }//OnDICOMBrowseClickHandler(object sender, EventArgs e)

    /// <summary>
    /// Handles the on leave event from the combo box
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnLeaveFocus(object sender, EventArgs e)
    {
      if (m_appIds.SelectedItem == null)
        return;

      string textSelected = m_appIds.SelectedItem.ToString();
      m_args.AppId = textSelected;
    }//OnLeaveFocus(object sender, EventArgs e)

    /// <summary>
    ///  handles the Destinatin Folder Browse ClickHandler(object sender, EventArgs e)
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDestFolderBrowseClickHandler(object sender, EventArgs e)
    {
      FolderBrowserDialog browserDialog = new FolderBrowserDialog();

      browserDialog.ShowNewFolderButton = true;
      browserDialog.Description  = "Select the working Folder ";
      browserDialog.SelectedPath = m_destinationFolder.Text;

      if (browserDialog.ShowDialog() == DialogResult.OK)
        m_destinationFolder.Text = browserDialog.SelectedPath;

    }//OnDestFolderBrowseClickHandler(object sender, EventArgs e)

    /// <summary>
    /// Handles the preprocess check handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnPreprocessCheckHandler(object sender, EventArgs e)
    {
      m_dicomFolder.Enabled = m_preProcessCheck.Checked;
      m_browseDICOM.Enabled = m_preProcessCheck.Checked;
    }//OnPreprocessCheckHandler(object sender, EventArgs e)

    #endregion

    #region process events handlers

    /// <summary>
    /// Enables the UI buttons
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnEnableUIButtons(object sender, EventArgs e)
    {
      if (this.InvokeRequired)
        this.Invoke( m_enableUIEventHandler, new object[] { sender, e });
      else
      {
        m_buttonRun.Enabled = true;
        m_Close.Enabled = true;
        m_Close.Text = "Close";
      }
    }//OnEnableUIButtons(object sender, EventArgs e)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnProcessStartHandler(object sender, ProcessEventArg arg)
    {
      // on a seperate thread invoke on UI thread
      if (this.InvokeRequired)
        this.Invoke(m_processStartEventHandler, new object[] { sender, arg });
      else
      {
        m_bStarted = false;
        m_progressbar.Text = arg.Message;
        if (!m_bStarted)
        {
          m_bStarted = true;
          m_progressbar.Pulse = true;
          m_progressbar.Value = 10;
          m_pulseGenerator.StartPulse();
        }//if (!m_bStarted)
      }
    }//OnProcessStartHandler(object sender, ProcessEventArg arg)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnStageFinshedHandler(object sender, ProcessEventArg arg)
    {
      // on a seperate thread invoke on UI thread
      if (this.InvokeRequired)
        this.Invoke(m_stageFinishedEventHandler, new object[] { sender, arg });

      else
      {
        if (!m_bStarted)
        {
          m_progressbar.Value = 10;
          m_bStarted = true;
          m_progressbar.Pulse = true;
          m_pulseGenerator.StartPulse();
        }//
        m_progressbar.Text = arg.Message;
        m_progressbar.Pulse = false;
        m_pulseGenerator.StopPulse();
        m_progressbar.Value = 0;
        m_bStarted = false;
      }
    }//OnStageFinshedHandler(object sender, ProcessEventArg arg)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnProcessProgressHandler(object sender, ProcessEventArg arg)
    {
      // on a seperate thread invoke on UI thread
      if (this.InvokeRequired)
        this.Invoke(m_processProgressEventHandler, new object[] { sender, arg });
      else
      {
        if ( arg.Message == string.Empty )
          arg.Message = "Stage progress not available";
        m_progressbar.Text = arg.Message;
      }
    }//OnProcessProgressHandler(object sender, ProcessEventArg arg)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnProcessFailedHandler(object sender, ProcessEventArg arg)
    {
      // on a seperate thread invoke on UI thread
      if (this.InvokeRequired)
        this.Invoke(m_processFailedEventHandler, new object[] { sender, arg });
      else
      {
        m_progressbar.Value = 10;
        m_progressbar.ForeColor = Color.Red;
        m_progressbar.BackColor = Color.Red;

        m_progressbar.Pulse = false;
        m_pulseGenerator.StopPulse();
        m_buttonRun.Enabled = true;
        m_progressbar.Text = arg.Message;
        m_Close.Enabled = true;
        m_Close.Text = "Close";
      }
    }//OnProcessFailedHandler(object sender, ProcessEventArg arg)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnProcessAbortedHandler(object sender, ProcessEventArg arg)
    {
      // on a seperate thread invoke on UI thread
      if (this.InvokeRequired)
        this.Invoke(m_processAbortedEventHandler, new object[] { sender, arg });
      else
      {
        m_progressbar.BackColor = Color.Red;
        m_progressbar.Text = arg.Message;

        m_pulseGenerator.StopPulse();
        m_progressbar.Pulse = false;
        m_progressbar.Value = 0;
        m_bStarted = false;
        m_buttonRun.Enabled = true;
        m_Close.Text = "Close";
      }
    }


    #endregion


    
  }//OnPreprocessCheckHandler(object sender, EventArgs e)
}//namespace Viatronix.v3D.Processor.Plugins.CAD.Test


#region
// $Log: MainForm.cs,v $
// Revision 1.9  2006/12/21 20:14:50  romy
// bug fix related to combo selection
//
// Revision 1.8  2006/12/21 19:35:43  romy
// More UI features added
//
// Revision 1.7  2006/12/21 15:41:04  romy
// More UI features added
//
// Revision 1.6  2006/12/21 15:33:18  romy
// added Validations
//
// Revision 1.4  2006/12/21 13:12:15  romy
// Simple abort added - only per stage
//
// Revision 1.2  2006/12/21 12:12:14  romy
// added more checks
//
// Revision 1.1  2006/12/21 11:27:23  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD.Test/MainForm.cs,v 1.9 2006/12/21 20:14:50 romy Exp $
// $Id: MainForm.cs,v 1.9 2006/12/21 20:14:50 romy Exp $
#endregion