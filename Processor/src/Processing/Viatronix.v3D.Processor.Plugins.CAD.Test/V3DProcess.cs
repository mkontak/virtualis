// $Id: V3DProcess.cs,v 1.4 2006/12/21 14:31:53 romy Exp $ 
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
using System.Text;
using Viatronix.v3D.Utilities;

#endregion


namespace Viatronix.v3D.Processor.Plugins.CAD.Test
{
  /// <summary>
  /// Delegate object
  /// </summary>
  /// <param name="sender">object</param>
  /// <param name="arg">ProcessEventArg</param>
  public delegate void ProcessEventHandler(object sender, ProcessEventArg arg);

  /// <summary>
  /// Class which performs the Test processing
  /// </summary>
  class V3DProcess
  {
    #region events
    /// <summary>
    /// Started
    /// </summary>
    public event ProcessEventHandler Started = null;

    /// <summary>
    /// Aborted
    /// </summary>
    public event ProcessEventHandler Aborted = null;

    /// <summary>
    /// Stage Finished
    /// </summary>
    public event ProcessEventHandler StageFinished = null;

    /// <summary>
    /// Failed
    /// </summary>
    public event ProcessEventHandler Failed = null;

    /// <summary>
    /// Progress
    /// </summary>
    public event ProcessEventHandler Progress = null;

    /// <summary>
    /// AllProcess Finished
    /// </summary>
    public event EventHandler AllProcessFinished = null;

    #endregion


    #region feilds

    /// <summary>
    /// Process Event Arg object
    /// </summary>
    private ProcessEventArg m_processEventArgs = new ProcessEventArg();

    /// <summary>
    /// current Stage text
    /// </summary>
    private string m_currentStage = string.Empty;

    /// <summary>
    /// Pipeline object
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.Pipeline m_pipeline = new Viatronix.v3D.Utilities.Pipelining.Pipeline();

    /// <summary>
    /// Pipeline started event handler
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler m_pipelineStartedEventHandler = null;

    /// <summary>
    /// Pipeline completed event handler
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler m_pipelineCompletedEventHandler = null;

    /// <summary>
    /// Pipeline failed event handler
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler m_pipelineFailedEventHandler = null;

    /// <summary>
    /// Pipeline aborted event handler
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler m_pipelineAbortedEventHandler = null;

    /// <summary>
    /// Pipeline updated event handler
    /// </summary>
    Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler m_pipelineUpdatedEventHandler = null;


    /// <summary>
    /// Flag indicates the running status
    /// </summary>
    bool m_bIsRunning = false;

    /// <summary>
    /// cancel flag
    /// </summary>
    bool m_bCancel = false;

    /// <summary>
    /// Environment args object
    /// </summary>
    private EnvrionmentArgs m_args;

    #endregion

    #region property

    /// <summary>
    /// Environment Args
    /// </summary>
    public EnvrionmentArgs Args
    {
      get { return m_args; }
      set { m_args = value; }
    }

    #endregion

    #region methods

    /// <summary>
    /// Constructor method
    /// </summary>
    public V3DProcess()
    {
      m_pipelineStartedEventHandler = new Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler(OnPipelineStartedHandler);
      m_pipelineCompletedEventHandler = new Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler(OnStageCompletedHandler);
      m_pipelineFailedEventHandler = new Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler(OnPipelineFailedHandler);
      m_pipelineAbortedEventHandler = new Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler(OnPipelineAbortedHandler);
      m_pipelineUpdatedEventHandler = new Viatronix.v3D.Utilities.Pipelining.PipelineEventHandler(OnPipelineUpdatedHandler);
    }//V3DProcess()

    /// <summary>
    /// Run method
    /// </summary>
    public void Run()
    {
      m_bCancel = false;
      OrganizeDestinationFolders();
      if (m_args.RunPreProcess)
      {
        RunConversion();
        RunPipeline();
      }
      RunCAD();

      //cleanup the pipeline object
      m_pipeline.Dispose();
      m_pipeline = null;

      //Send the all finished message box
      AllProcessFinishedEventHandler(this, null);
    }//Run

    /// <summary>
    /// Is running
    /// </summary>
    /// <returns>running state</returns>
    public bool IsRunning()
    {
      if (m_pipeline == null)
        return false;

      return m_bIsRunning;
    }// IsRunning()

    /// <summary>
    /// Abort method
    /// </summary>
    public void Abort()
    {
      if (m_pipeline != null)
      {
        try
        {
          m_bCancel = true;
          m_pipeline.Abort();
        }
        catch (Exception ex)
        {
          Logging.Logger.Err(ex.Message);
        }
      }
    }//Abort()

    /// <summary>
    /// Organize the Destination Folders
    /// </summary>
    private void OrganizeDestinationFolders()
    {
      // string datePatt = @"_yyyydM_hhmmss";
      //string appendingString = System.DateTime.Now.ToString(datePatt);

      string volumeFolder = "Volume"; //+ appendingString;
      volumeFolder = System.IO.Path.Combine(m_args.WorkingFolder, volumeFolder);

      if (m_args.RunPreProcess)
        System.IO.Directory.CreateDirectory(volumeFolder);

      m_args.VolumeFolder = volumeFolder;

      string PreProcessedFolder = "PreProcessed"; // + appendingString;
      PreProcessedFolder = System.IO.Path.Combine(m_args.WorkingFolder, PreProcessedFolder);

      if (m_args.RunPreProcess)
        System.IO.Directory.CreateDirectory(PreProcessedFolder);

      m_args.PreProcessedFolder = PreProcessedFolder;

      string cadFolder = "CAD"; //+ appendingString;
      cadFolder = System.IO.Path.Combine(m_args.WorkingFolder, cadFolder);
      System.IO.Directory.CreateDirectory(cadFolder);
      m_args.CadFolder = System.IO.Path.Combine(m_args.WorkingFolder, cadFolder);
    }//OrganizeDestinationFolders

    /// <summary>
    /// Run the conversion
    /// </summary>
    private void RunConversion()
    {
      m_currentStage = "Conversion";

      if (m_pipeline != null)
      {
        m_pipeline.Dispose();
        m_pipeline = null;
      }
      if (m_bCancel)
      {
        SendUserAbortMessage();
        return;
      }

      m_pipeline = new Viatronix.v3D.Utilities.Pipelining.Pipeline();
      AttachEventHandlers();

      m_processEventArgs.Message = "Launching conversion...";
      m_processEventArgs.Progress = 0;
      m_processEventArgs.Stage = "Converting";

      //raise the start event
      this.Started(this, m_processEventArgs);

      m_pipeline.DicomFilePath = m_args.DicomFolder;
      m_pipeline.SourceFilePath = m_args.DicomFolder;
      m_pipeline.DestinationFilePath = System.IO.Path.Combine(m_args.VolumeFolder, "Volume");

      m_pipeline.ExecutablePath = m_args.ConversionApp;
      m_pipeline.RunPipeline(true);
    }//RunConversion


    /// <summary>
    /// Run Pipeline
    /// </summary>
    private void RunPipeline()
    {
      m_currentStage = "Pipeline";

      if (m_pipeline != null)
      {
        m_pipeline.Dispose();
        m_pipeline = null;
      }
      if (m_bCancel)
      {
        SendUserAbortMessage();
        return;
      }

      m_pipeline = new Viatronix.v3D.Utilities.Pipelining.Pipeline();
      AttachEventHandlers();

      m_processEventArgs.Message = "Launching pipeline...";
      m_processEventArgs.Progress = 0;
      m_processEventArgs.Stage = "pipeline";

      //raise the start event
      this.Started(this, m_processEventArgs);

      m_pipeline.DicomFilePath = m_args.DicomFolder;
      m_pipeline.VolumeFilePath = System.IO.Path.Combine(m_args.VolumeFolder, "Volume");
      m_pipeline.SourceFilePath = System.IO.Path.Combine(m_args.VolumeFolder, "Volume");
      m_pipeline.DestinationFilePath = System.IO.Path.Combine(m_args.PreProcessedFolder, "PreProcessed");

      m_pipeline.ExecutablePath = m_args.PipelineApp;
      m_pipeline.RunPipeline(true);
    }//RunPipeline


    /// <summary>
    /// Runs CAD processing
    /// </summary>
    private void RunCAD()
    {
      m_currentStage = "CAD";

      if (m_pipeline != null)
      {
        m_pipeline.Dispose();
        m_pipeline = null;
      }

      if (m_bCancel)
      {
        SendUserAbortMessage();
        return;
      }

      m_pipeline = new Viatronix.v3D.Utilities.Pipelining.Pipeline();
      AttachEventHandlers();

      m_processEventArgs.Message = "Launching CAD processing...";
      m_processEventArgs.Progress = 0;
      m_processEventArgs.Stage = "CAD";

      //raise the start event
      this.Started(this, m_processEventArgs);

      m_pipeline.DicomFilePath = m_args.DicomFolder;

      m_pipeline.SourceFilePath = m_args.PreProcessedFolder;
      m_pipeline.VolumeFilePath = m_args.VolumeFolder;
      m_pipeline.DestinationFilePath = System.IO.Path.Combine(m_args.CadFolder, "CAD");

      //string dicomOnly = "-dicomonly ";
      string parameters = " -appid " + m_args.AppId;

      m_pipeline.ExecutablePath = m_args.CADPlugin;
      m_pipeline.RunPipeline(m_pipeline.ExecutablePath, parameters, true);
    }//RunCAD()

    /// <summary>
    /// Sends an user abort message
    /// </summary>
    private void SendUserAbortMessage()
    {
      m_processEventArgs.Message = "Aborting " + m_currentStage;
      m_processEventArgs.Progress = 0;
      m_processEventArgs.Stage = "";

      //raise the start event..ok just for this 
      this.Started(this, m_processEventArgs);

      OnPipelineAbortedHandler(null, null );

    }//SendUserAbortMessage()


    /// <summary>
    /// AttachEventHandlers
    /// </summary>
    private void AttachEventHandlers()
    {
      m_pipeline.Started += m_pipelineStartedEventHandler;
      m_pipeline.Completed += m_pipelineCompletedEventHandler;
      m_pipeline.Failed += m_pipelineFailedEventHandler;
      m_pipeline.Aborted += m_pipelineAbortedEventHandler;
      m_pipeline.Updated += m_pipelineUpdatedEventHandler;

      m_pipeline.EnableMessageHandler = true;
    }//AttachEventHandlers()

    #endregion

    #region eventHandlers

    /// <summary>
    /// Handles the OnFinished event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnStageCompletedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)
    {
      m_processEventArgs.Message = "Finished " + e.Message;
      if (e.Message == string.Empty)
        m_processEventArgs.Message += m_currentStage;

      if (this.StageFinished != null)
        this.StageFinished(sender, m_processEventArgs);

      m_bIsRunning = false;

    }//OnStageCompletedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)

    /// <summary>
    /// Handles the OnStarted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnPipelineStartedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)
    {
      m_bIsRunning = true;

      m_processEventArgs.Message = e.Message;
      if (this.Started != null)
        this.Started(sender, m_processEventArgs);

    }//OnPipelineStartedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)

    /// <summary>
    /// Handles the OnFailed event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnPipelineFailedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)
    {
      m_bIsRunning = false;

      m_processEventArgs.Message = e.Message;
      if (this.Failed != null)
        this.Failed(sender, m_processEventArgs);
    }//OnPipelineFailedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)

    /// <summary>
    /// handles the OnAborted event from the pipeline
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguments</param>
    private void OnPipelineAbortedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)
    {
      m_bIsRunning = false;

      if (e != null)
        m_processEventArgs.Message = "User aborted processing. " + e.Message;
      else
        m_processEventArgs.Message = "User aborted processing. ";

      if (this.Aborted != null)
        this.Aborted(sender, m_processEventArgs);

    } // OnPipelineAborted(object sender, vxUtils.Pipelining.PipelineEventArgs e)


    private void OnPipelineUpdatedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)
    {
      m_processEventArgs.Message = m_currentStage + ": Stage " + e.StatusMessage.Stage.ToLower() + " (" + e.StatusMessage.OverallProgress + "%)";

      if (this.Progress != null)
        this.Progress(sender, m_processEventArgs);

    } //OnPipelineUpdatedHandler(object sender, Viatronix.v3D.Utilities.Pipelining.PipelineEventArgs e)

    /// <summary>
    /// Raises this event when everything finishes
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void AllProcessFinishedEventHandler(object sender, EventArgs e)
    {
      m_bIsRunning = false;

      if (this.AllProcessFinished != null)
        this.AllProcessFinished(sender, e);
    }//AllProcessFinishedEventHandler(object sender, EventArgs e)

    #endregion

  }//V3DProcess

  /// <summary>
  /// Class represents Event Args
  /// </summary>
  class ProcessEventArg
  {
    #region fields

    /// <summary>
    /// Message string
    /// </summary>
    private string m_message;

    /// <summary>
    /// Stage string
    /// </summary>
    private string m_stage;

    /// <summary>
    /// Total Progress
    /// </summary>
    private int m_progress;

    #endregion

    #region Property
    /// <summary>
    /// Message
    /// </summary>
    public string Message
    {
      get { return m_message; }
      set { m_message = value; }
    }//Message

    /// <summary>
    /// Stage
    /// </summary>
    public string Stage
    {
      get { return m_stage; }
      set { m_stage = value; }
    }//Stage

    /// <summary>
    /// Progress
    /// </summary>
    public int Progress
    {
      get { return m_progress; }
      set { m_progress = value; }
    }//Progress

    #endregion

  }//ProcessEventArg

}//namespace Viatronix.v3D.Processor.Plugins.CAD.Test



#region
// $Log: V3DProcess.cs,v $
// Revision 1.4  2006/12/21 14:31:53  romy
// abort fully functional
//
// Revision 1.2  2006/12/21 12:12:14  romy
// added more checks
//
// Revision 1.1  2006/12/21 11:27:23  romy
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD.Test/V3DProcess.cs,v 1.4 2006/12/21 14:31:53 romy Exp $
// $Id: V3DProcess.cs,v 1.4 2006/12/21 14:31:53 romy Exp $
#endregion