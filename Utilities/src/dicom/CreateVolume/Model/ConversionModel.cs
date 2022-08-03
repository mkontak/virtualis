
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using Viatronix.UI;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using Viatronix.Enterprise.Dicom.Conversion;

namespace Viatronix.Utilities.Dicom.CreateVolume.Model
{

  /// <summary>
  /// Conversion Processes
  /// </summary>
  enum ConversionProcess
  {
    None,
    Analysis,
    Create
  }

  /// <summary>
  /// View Model for the creation of a volume
  /// </summary>
  class ConversionModel 
  {


    #region fields

 
    /// <summary>
    /// Processing thread
    /// </summary>
    private Thread m_thread;

   /// <summary>
    /// Path of dicom files
    /// </summary>
    private string m_path = string.Empty;

    /// <summary>
    /// Patterm to look for
    /// </summary>
    private string m_pattern = "*.dcm";

    /// <summary>
    /// Volume file to create
    /// </summary>
    private string m_volumeFile = string.Empty;

    /// <summary>
    /// Current process
    /// </summary>
    private ConversionProcess m_process = ConversionProcess.None;

     #endregion


    #region constructors/destructors

    /// <summary>
    /// Constructor
    /// </summary>
    public ConversionModel()
    {
    } // ConversionModel()


    #endregion

    #region events

    /// <summary>
    /// Analysis started event
    /// </summary>
    public event ConversionDelegate AnalysisStarted;

    /// <summary>
    /// Analysis updated event
    /// </summary>
    public event ConversionDelegate AnalysisUpdated;

    /// <summary>
    /// Analysis completed
    /// </summary>
    public event ConversionDelegate AnalysisCompleted;

    /// <summary>
    /// Create started event
    /// </summary>
    public event ConversionDelegate CreateStarted;

    /// <summary>
    /// Create updated event
    /// </summary>
    public event ConversionDelegate CreateUpdated;

    /// <summary>
    /// Create completed
    /// </summary>
    public event ConversionDelegate CreateCompleted;


    /// <summary>
    /// Conversion started
    /// </summary>
    public event ConversionDelegate ConversionStarted;

    /// <summary>
    /// Conversion fialed
    /// </summary>
    public event ConversionDelegate ConversionFailed;

    /// <summary>
    /// Conversion completed
    /// </summary>
    public event ConversionDelegate ConversionCompleted;


    /// <summary>
    /// Analysis started handler
    /// </summary>
    public void OnAnalysisStarted(int remaining, int completed, string message)
    {
      if (AnalysisStarted != null)
        AnalysisStarted(new ConversionEventArgs(remaining, completed, message));
    } // OnAnalysisStarted()


    /// <summary>
    /// Analysis completed handler
    /// </summary>
    public void OnAnalysisCompleted(int remaining, int completed, string message)
    {
      if (AnalysisCompleted != null)
        AnalysisCompleted(new ConversionEventArgs(remaining, completed, message));
    } // OnAnalysisCompleted()


    /// <summary>
    /// Create updated handler
    /// </summary>
    public void OnCreateUpdated(int remaining, int completed, string message)
    {
      if (CreateUpdated != null)
        CreateUpdated(new ConversionEventArgs(remaining, completed, message));
    } // OnCreateUpdated(int remaining, int completed, string message)

    /// <summary>
    /// Create started handler
    /// </summary>
    public void OnCreateStarted(int remaining, int completed, string message)
    {
      if (CreateStarted != null)
        CreateStarted(new ConversionEventArgs(remaining, completed, message));
    } // OnCreateStarted()


    /// <summary>
    /// Create completed handler
    /// </summary>
    public void OnCreateCompleted(int remaining, int completed, string message)
    {
      if (CreateCompleted != null)
        CreateCompleted(new ConversionEventArgs(remaining, completed, message));
    } // OnCreateCompleted()


    /// <summary>
    /// Analysis updated handler
    /// </summary>
    public void OnAnalysisUpdated(int remaining, int completed, string message)
    {
      if (AnalysisUpdated != null)
        AnalysisUpdated(new ConversionEventArgs(remaining, completed, message));
    } // OnAnalysisUpdated(int remaining, int completed, string message)


    /// <summary>
    /// Conversion completed handler
    /// </summary>
    public void OnConversionStarted()
    {
      if ( ConversionStarted != null )
        ConversionStarted(new ConversionEventArgs(0, 0, "Conversion Started"));
    } // OnConversionCompleted()


    /// <summary>
    /// Conversion completed handler
    /// </summary>
    public void OnConversionCompleted()
    {
      if ( ConversionCompleted != null )
        ConversionCompleted(new ConversionEventArgs(0, 0, "Conversion Completed"));
      m_thread = null;
    } // OnConversionCompleted()


    /// <summary>
    /// Conversion failed handler
    /// </summary>
    public void OnConversionFailed(int remaining, int completed, string message)
    {
      if ( ConversionFailed != null )
        ConversionFailed(new ConversionEventArgs(remaining, completed, message));
      m_thread = null;
    } // OnConversionCompleted()

    #endregion

    #region methods


    /// <summary>
    /// Handles the create volume event
    /// </summary>
    public void Create()
    {

      if (m_path.Length == 0 || !System.IO.Directory.Exists(m_path))
        throw new System.IO.InvalidDataException("Path missing or does not exists");

      if (m_volumeFile.Length == 0)
        throw new System.IO.InvalidDataException("Volume file is not specified");

      m_thread = new Thread(new ParameterizedThreadStart(this.RunCreateVolume));

      m_thread.Start();

    } // Create()



    /// <summary>
    /// Handles the cancel event
    /// </summary>
    public void Abort()
    {

      if (m_thread != null)
        m_thread.Abort();

    } // Abort()


    /// <summary>
    /// 
    /// </summary>
    /// <param name="obj"></param>
    private void RunCreateVolume(object obj)
    {

      try
      {

        OnConversionStarted();

        string dicomInfoFile = System.IO.Path.Combine(m_path, "dicominfo.xml");


        CallbackDelegate callback = new CallbackDelegate(this.CallbackHandler);

        m_process = ConversionProcess.Analysis;

        DicomInfo dicomInfo = DicomInfo.Create(m_path, m_pattern, callback);

        ImageCollection imageCollection = dicomInfo.GetImages();

        m_process = ConversionProcess.Create;

        Volume.Create(0, imageCollection, m_volumeFile, callback);

        imageCollection.Serialize(System.IO.Path.Combine(System.IO.Path.GetDirectoryName(m_volumeFile), System.IO.Path.GetFileNameWithoutExtension(m_volumeFile) + ".xml"));

        OnConversionCompleted();
 
      }
      catch (Exception ex)
      {
        OnConversionFailed(0, 0, ex.Message);
      }
    } // RunCreateVolume(object obj)

   #endregion



    #region callback handlers

    /// <summary>
    /// Handles the callback from the analysis and creation
    /// </summary>
    /// <param name="type"></param>
    /// <param name="remaining"></param>
    /// <param name="completed"></param>
    /// <param name="message"></param>
    private void CallbackHandler(EventTypes type, int remaining, int completed, string message)
    {


      switch ( type )
      {
        case EventTypes.Started:
          {
           if (  m_process == ConversionProcess.Analysis )
             OnAnalysisStarted(remaining, completed, message);
           else
             OnCreateStarted(remaining, completed, message);
          }
          break;

        case EventTypes.Updated:
          {
           if (  m_process == ConversionProcess.Analysis )
             OnAnalysisUpdated(remaining, completed, message);
           else
             OnCreateUpdated(remaining, completed, message);
          }
          break;

        case EventTypes.Completed:
         {
           if (  m_process == ConversionProcess.Analysis )
             OnAnalysisCompleted(remaining, completed, message);
           else
             OnCreateCompleted(remaining, completed, message);
          }
          break;

        case EventTypes.Failed:
          {
            OnConversionFailed(remaining, completed, message);
          }
          break;

      }
    } // CallbackHandler(EventTypes type, int remaining, int completed, string message)




    #endregion


    #region properties

    /// <summary>
    /// Flag indicating the model is processing
    /// </summary>
    public bool Processing
    {
      get { return (m_thread == null ? false : true); }
    } // Processing

    /// <summary>
    /// Gets/Sets the pattern
    /// </summary>
    public string Pattern
    {
      get { return m_pattern; }
      set { m_pattern = value; }
    } // Pattern


    /// <summary>
    /// Gets/Sets the Path
    /// </summary>
    public string Path
    {
      get { return m_path; }
      set 
      { 
        m_path = value;


        if (string.IsNullOrEmpty(m_path))
          m_volumeFile = string.Empty;
        else
          m_volumeFile = m_path + "\\" + System.IO.Path.GetFileName(m_path) + ".vol";
        
      }
    } //Path


    /// <summary>
    /// Gets/Sets the volume file
    /// </summary>
    public string VolumeFile
    {
      get { return m_volumeFile; }
      set { m_volumeFile = value; }
    } // VolumeFile


    #endregion



  }
}
