using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Processing
{

  /// <summary>
  /// Pipeline Evenet Delegate
  /// </summary>
  /// <param name="args"></param>
  /// <returns></returns>
  public delegate void PipelineEventHandler(PipelineEventArgs args);

  /// <summary>
  /// Pipeline Event Arguements
  /// </summary>
  public class PipelineEventArgs
  {

#region fields

    /// <summary>
    /// Abort flag
    /// </summary>
    private bool m_abort = false;

    /// <summary>
    /// Overall progress
    /// </summary>
    protected float m_progress = 0.0F;

    /// <summary>
    /// Remaining items
    /// </summary>
    protected int m_remaining = 0;

    /// <summary>
    /// Completed items
    /// </summary>
    protected int m_completed = 0;

    /// <summary>
    /// Total items
    /// </summary>
    protected int m_total = 0;
  
    /// <summary>
    /// Message
    /// </summary>
    protected string m_message = string.Empty;


    /// <summary>
    /// Stage in process
    /// </summary>
    protected string m_stage = string.Empty;

    /// <summary>
    /// Series Uid
    /// </summary>
    protected string m_seriesUid;

    /// <summary>
    /// JOb Id
    /// </summary>
    protected string m_jobId;

    /// <summary>
    /// Module/Task name
    /// </summary>
    protected string m_moduleName;

#endregion

#region methods

    /// <summary>
    /// Creates the pipeline event args
    /// </summary>
    /// <param name="stage"></param>
    /// <param name="message"></param>
    /// <param name="overallProgress"></param>
    /// <returns></returns>
    public static PipelineEventArgs Create(string moduleName, string stage, string message, float progress)
    {
      PipelineEventArgs args = new PipelineEventArgs();

      if (args == null)
        throw new OutOfMemoryException("Failed to allocate PipelineEventArgs");

      args.ModuleName   = moduleName;
      args.m_stage      = stage;
      args.m_message    = message;
      args.m_progress   = progress;

      return args;
    }

    /// <summary>
    /// Creates the pipeline event args
    /// </summary>
    /// <param name="stage"></param>
    /// <param name="message"></param>
    /// <returns></returns>
    public static PipelineEventArgs Create(string moduleName, string stage, string message )
    { return Create(moduleName, stage, message, 0.0F); }

#endregion

#region properties

  
    /// <summary>
    /// Sets/Gets the remaining
    /// </summary>
    public int Remaining
    { 
      get { return m_remaining; }
      set { m_remaining = value; }
    } // Remaining

    /// <summary>
    /// Sets/Gets completed
    /// </summary>
    public int Completed
    { 
      get { return m_completed; }
      set { m_completed = value; }
    } // Completed

    /// <summary>
    /// Sets/Gets the total count
    /// </summary>
    public int Total
    { 
      get { return m_total; }
      set { m_total = value; }
    } // Total

    /// <summary>
    /// Sets/Gets the  progress ( Range is 0.0 - 100.0 
    /// </summary>
    public float Progress
    { 
      get { return m_progress; }
      set { m_progress = ( value > 100.0F ? 100.0F : ( value < 0.0F ? 0.0F : value ) ); }
    } // Progress

    /// <summary>
    /// Sets/Gets the series uid
    /// </summary>
    public String SeriesUid
    {  
      get { return m_seriesUid; }
      set { m_seriesUid = value; }
    } // SeriesUid

    /// <summary>
    /// Sets/Gets the message
    /// </summary>
    public string Message
    { 
      get { return m_message; }
      set { m_message = value; }
    } // Message


    /// <summary>
    /// Sets/Gets the stage
    /// </summary>
    public string Stage
    { 
      get { return m_stage; }
      set { m_stage = value; }
    }  // Stage

    /// <summary>
    /// Sets the abort
    /// </summary>
    public bool Abort
    {
      set { m_abort = value; }
    } // Abort

    /// <summary>
    /// Gets/Stes the job id
    /// </summary>
    public string JodId
    {
      get { return m_jobId; }
      set { m_jobId = value; }
    } // JobId

    /// <summary>
    /// Gets/Stes the module name
    /// </summary>
    public string ModuleName
    {
      get { return m_moduleName; }
      set { m_moduleName = value; }
    } // ModuleName

#endregion



  } // clss PipelineEvenetArgs

} // namespace Viatronix.Enterprise.Pipeline
