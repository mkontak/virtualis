using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{


  #region class ProviderEventArgs

  /// <summary>
  /// Provider event arguements
  /// </summary>
  public class ProviderEventArgs : EventArgs
  {
    #region fields

    /// <summary>
    /// Unique task id
    /// </summary>
    private string m_taskId = string.Empty;

    /// <summary>
    /// Resource
    /// </summary>
    private string m_resource = string.Empty;

    /// <summary>
    /// Operation
    /// </summary>
    private string m_operation = string.Empty;

    /// <summary>
    /// Message
    /// </summary>
    private string m_message = string.Empty;

    #endregion

    #region constructors


    /// <summary>
    /// Constructor
    /// </summary>
    public ProviderEventArgs()
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="taskId">task identifier</param>
    /// <param name="resource"></param>
    /// <param name="operation"></param>
    /// <param name="message"></param>
    public ProviderEventArgs(string resource, string operation, string message, string taskId = "")
    {
      m_taskId = taskId;
      m_resource = resource;
      m_operation = operation;
      m_message = message;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="taskId">task identifier</param>
    /// <param name="resource"></param>
    /// <param name="operation"></param>
    public ProviderEventArgs(string resource, string operation, string taskId = "")
    {
      m_resource = resource;
      m_operation = operation;
    }

    #endregion

    #region properties

    /// <summary>
    /// Task Id
    /// </summary>
    public string TaskId
    { get { return m_taskId; } }

    /// <summary>
    /// Gets the resource
    /// </summary>
    public string Resource
    { get { return m_resource; } }

    /// <summary>
    /// Gets the operation
    /// </summary>
    public string Operation
    { get { return m_operation; } }

    /// <summary>
    /// Gets the message
    /// </summary>
    public string Message
    { get { return m_message; } }

    #endregion

  }


  #endregion

 
  /// <summary>
  /// Asynchronous Provider (events)
  /// </summary>
  public interface IAsynchronousProvider : IProvider
  {

    #region properties

    /// <summary>
    /// Returns true if the operation is aborting, false otherwise
    /// </summary>
    bool IsAborting
    { get; }


    #endregion

    #region methods

    /// <summary>
    /// Aborts all tasks
    /// </summary>
    void Abort();

    /// <summary>
    /// Aborts the current task
    /// <param name="taskId">Task id returned from Asyn methods</param>
    /// </summary>
    void Abort(string taskId);

    #endregion

    #region events

    /// <summary>
    /// Occurs when an operation has completed.
    /// </summary>
    event ProviderEventHandler Completed;

    /// <summary>
    /// Occurs when an operation has failed.
    /// </summary>
    event ProviderEventHandler Failed;

    /// <summary>
    /// Occurs when an operation has been aborted.
    /// </summary>
    event ProviderEventHandler Aborted;


    #endregion


  }

}
