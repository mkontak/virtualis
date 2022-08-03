using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Processing
{

  /// <summary>
  /// Interface used for pipeline procedures exectued within the workqueue server
  /// </summary>
  public interface IPipeline
  {

    /// <summary>
    ///  Start the pipeline
    /// </summary>
    /// <param name="args"></param>
    System.Threading.EventWaitHandle Start(Dictionary<string,string> args);

    /// <summary>
    /// Runs the pipeline in the current thread and returns a list of objects. Get the Return type
    /// from the ReturnType property.
    /// </summary>
    /// <param name="args"></param>
    void RunSynchronously(Dictionary<string, string> args);


    /// <summary>
    /// Abort
    /// </summary>
    void Abort();

    /// <summary>
    /// Gets the results of the processing
    /// </summary>
    /// <returns></returns>
    IPipelineResults GetResults();

    /// <summary>
    /// Event triggered when the pipeline started
    /// </summary>
    event PipelineEventHandler Started;

    /// <summary>
    /// Event triggered when the pipeline completed
    /// </summary>
    event PipelineEventHandler Completed;

    /// <summary>
    /// Event triggered when the pipeline failed
    /// </summary>
    event PipelineEventHandler Failed;

    /// <summary>
    /// Evenet triggered when the pipeline's states has been updated
    /// </summary>
    event PipelineEventHandler Updated;


    /// <summary>
    /// Job Id
    /// </summary>
    String JodId
    { get; }

    /// <summary>
    /// Exit code
    /// </summary>
    int ExitCode
    { get; }


  } // interface IPipeline

} // namespace Viatronix.Enterprise.Servers 
