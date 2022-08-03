using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Dicom;
using Viatronix.Logging;
using System.Threading;
using System.Threading.Tasks;


namespace Viatronix.Enterprise.SDK.Providers
{

  #region delegates

  /// <summary>
  /// Get Method Delegate used to perform an aynchrnous GET
  /// </summary>
  /// <param name="query"></param>
  public delegate string GetMethodDelegate(XmlBuilder query);

  /// <summary>
  /// Update Method Delegate used to perform an aynchrnous update.
  /// 
  /// Updates the supplied record.
  /// </summary>
  /// <param name="record"></param>
  public delegate string UpdateMethodDelegate(XmlBuilder record);


  /// <summary>
  /// Insert Method Delegate used to perform an aynchrnous insert.
  /// 
  /// Inserts the supplied record(s). This is in xml format
  /// </summary>
  /// <param name="record">A single record or a collection</param>
  public delegate string InsertMethodDelegate(XmlBuilder record);

  /// <summary>
  /// Insert Method Delegate used to perform an aynchrnous insert.
  /// 
  /// Inserts the supplied record(s). This is in xml format
  /// </summary>
  /// <param name="record">A single record or a collection</param>
  public delegate string DeleteMethodDelegate(XmlBuilder record);

  /// <summary>
  /// Signature for a provider delegate
  /// </summary>
  /// <param name="resource">Resource requested</param>
  /// <param name="operation">operation</param>
  /// <param name="message">Error message if failed/xml if get/empty otherwise</param>
  public delegate void ProviderEventHandler(ProviderEventArgs args);

  #endregion


  /// <summary>
  /// Base implementation of an asynchronous provider
  /// </summary>
  public class AsynchronousProviderBase : ProviderBase, IAsynchronousProvider
  {

    #region fields

    /// <summary>
    /// Maximum concurrent tasks allowed
    /// </summary>
    private int m_maxConcurrentTasks = 2;

    /// <summary>
    /// List of tasks allowing for multiple running tasks
    /// </summary>
    private Dictionary<string, KeyValuePair<System.Threading.Tasks.Task, CancellationTokenSource>> m_tasks = new Dictionary<string, KeyValuePair<System.Threading.Tasks.Task, CancellationTokenSource>>();

    /// <summary>
    /// Abort flag
    /// </summary>
    private bool m_aborting = false;

    #endregion

    #region constructors

    /// <summary>
    /// Initializes the provider using only xml configutation. This is used for the 
    /// SecurityProvders
    /// </summary>
    /// <param name="xml"></param>
    protected AsynchronousProviderBase(string xml) : base(xml)
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      doc.DocumentElement.TryGetAttributeValue<int>("maxtasks", ref m_maxConcurrentTasks);

    } // AsynchronousProviderBase(string xml)


    #endregion

    #region properties

    /// <summary>
    /// Returns true if we are aborting, false if not
    /// </summary>
    public virtual bool IsAborting
    { get { return m_aborting;  } }

    /// <summary>
    /// Returns true since asynchronous support should be implemented if this interface/base abstract class is used
    /// </summary>
    public override bool SupportsAsynchronousOperations
    { get { return true; } }

 
    
    #endregion

    #region methods

    /// <summary>
    /// Cleanup tasks that are done running
    /// </summary>
    private void CleanupTasks()
    {

      Mutex mutex = new Mutex(false, "AsynchronousProviderBase_Tasks");

      try
      {

        mutex.WaitOne();

        List<string> removalItems = new List<string>();

        foreach (KeyValuePair<string, KeyValuePair<Task, CancellationTokenSource>> item in m_tasks)
        {
          if (item.Value.Key.IsCanceled || item.Value.Key.IsCompleted || item.Value.Key.IsFaulted)
            removalItems.Add(item.Key);
        }


        foreach (string id in removalItems)
        {
          m_tasks.Remove(id);
        }
      }
      finally
      {
        mutex.ReleaseMutex();
      }

    } // CleanupTasks()


    /// <summary>
    /// Add the task to the lists
    /// </summary>
    /// <param name="task">Task</param>
    /// <param name="token">Cancellation token</param>
    private void  AddTask(string id, Task task, CancellationTokenSource token)
    {
      CleanupTasks();

      Mutex mutex = new Mutex(false, "AsynchronousProviderBase_Tasks");

      try
      {

        mutex.WaitOne();


        m_tasks.Add(id, new KeyValuePair<Task, CancellationTokenSource>(task, token));


      }
      finally
      {
        mutex.ReleaseMutex();
      }

    } // AddTask(System.Threading.Tasks.Task task, CancellationTokenSource token)


    /// <summary>
    /// Removes the task by id
    /// </summary>
    /// <param name="id">Task id</param>
    private void RemoveTask(string id)
    {
      Mutex mutex = new Mutex(false, "AsynchronousProviderBase_Tasks");

      try
      {

        mutex.WaitOne();

        m_tasks.Remove(id);

      }
      finally
      {
        mutex.ReleaseMutex();
      }

    } // RemoveTask(int id)


    /// <summary>
    /// Determines if the task exists in the list
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    private bool ContainsTask(string id)
    {
      return m_tasks.ContainsKey(id);
    } // Contains(id)


    /// <summary>
    /// Gets the task if it exists, or return null if not
    /// </summary>
    /// <param name="id">Task id to get</param>
    /// <returns></returns>
    private Task GetTask(string id)
    {
      Task task = null;

      if (ContainsTask(id))
        task = m_tasks[id].Key;

      return task;
    }

 

    /// <summary>
    /// Abort operation
    /// </summary>
    public virtual void Abort(string taskId)
    {


      if (ContainsTask(taskId))
      {

        KeyValuePair<Task, CancellationTokenSource> taskItem =  m_tasks[taskId];


        /// Cancel task
        taskItem.Value.Cancel();

        RemoveTask(taskId);

      }
    }

    /// <summary>
    /// Aborts all operations
    /// </summary>
    public virtual void Abort()
    {

      Mutex mutex = new Mutex(false, "AsynchronousProviderBase_Tasks");

      try
      {

        mutex.WaitOne();

        List<string> removalItems = new List<string>();

        foreach (KeyValuePair<string, KeyValuePair<Task, CancellationTokenSource>> item in m_tasks)
        {
          item.Value.Value.Cancel();

          removalItems.Add(item.Key);
        }


        // ================================
        // Remove the task from the list
        // ================================
        foreach (string id in removalItems)
        {
          m_tasks.Remove(id);
        }

      }
      finally
      {
        mutex.ReleaseMutex();
      }



    }

    /// <summary>
    /// Resets the abort flag 
    /// </summary>
    protected void Reset()
    { }

    /// <summary>
    /// Perfoms the GET method asynchronously
    /// </summary>
    /// <param name="getMethod">Get Method as a delegate</param>
    /// <param name="query">Query parameters</param>
    /// <return>Returns the id associated with the task</return>
    protected string GetAsynchronously(GetMethodDelegate getMethod, XmlBuilder query)
    {

      var source = new CancellationTokenSource();


      string id = Guid.NewGuid().ToString();

      Action get = () => 
      {

        try
        {
          FireStartedEvent(query.Resource, "get", id);

          source.Token.ThrowIfCancellationRequested();

          string xml = getMethod(query);

          FireCompletedEvent(query.Resource, "get", xml, id);


        }
        catch (Exception ex)
        {
          FireFailedEvent(query.Resource, "get", ex.Message, id);
          throw;
        }

      };

      Task task = Task.Factory.StartNew(get, source.Token);


      AddTask(id, task, source);

      return id;


    } // GetAsynchronously(GetMethodDelegate getMethod, XmlBuilder query)


    /// <summary>
    /// Perfoms the GET method asynchronously
    /// </summary>
    /// <param name="getMethod">Get Method as a delegate</param>
    /// <param name="query">Query parameters</param>
    /// <return>Returns the id associated with the task</return>
    protected string UpdateAsynchronously(UpdateMethodDelegate updateMethod, XmlBuilder query)
    {
      var source = new CancellationTokenSource();

      string id = Guid.NewGuid().ToString();

      Action update = () =>
      {

        try
        {

          FireStartedEvent(query.Resource, "update", id);

          source.Token.ThrowIfCancellationRequested();

          string xml = updateMethod(query);

          FireCompletedEvent(query.Resource, "update", xml, id);


        }
        catch (Exception ex)
        {
          FireFailedEvent(query.Resource, "update", ex.Message, id);
          throw;
        }

      };


      Task task = Task.Factory.StartNew(update, source.Token);

      AddTask(id, task, source);

      return id;


    } // UpdateAsynchronously(UpdateMethodDelegate updateMethod, XmlBuilder query)

    #endregion

    #region events

    /// <summary>
    /// Occurs when a operation has started.
    /// </summary>
    public event ProviderEventHandler Started = null;


    /// <summary>
    /// Occurs when a operation has completed.
    /// </summary>
    public event ProviderEventHandler Completed = null;

    /// <summary>
    /// Occurs when a operation has failed.
    /// </summary>
    public event ProviderEventHandler Failed = null;

    /// <summary>
    /// Occurs when an operation has been aborted
    /// </summary>
    public event ProviderEventHandler Aborted = null;

    #region event methods

    /// <summary>
    /// Fires the started event
    /// </summary>
    /// <param name="taskId">Task identifier</param>
    /// <param name="resource">Resource name</param>
    /// <param name="operation">Operation performed</param>
    protected virtual void FireStartedEvent(string resource, string operation, string taskId = "")
    {

      string[] sections = this.GetType().ToString().Split(new char[] { '.' });

      Log.Information(string.Format("{0}{1} started", operation.ToUpper(), resource.ToUpper()), "AsynchronousProviderBase", sections[sections.Length - 1]);

      if (this.Started != null)
        this.Started(new ProviderEventArgs(resource, operation, taskId));

    } // FireStartedEvent(List<Study> collection)

    /// <summary>
    /// Fires the completed event
    /// </summary>
    /// <param name="taskId">Task identifier</param>
    /// <param name="resource">Resource name</param>
    /// <param name="operation">Operation performed</param>
    /// <param name="message">Messgae</param>
    protected virtual void FireCompletedEvent(string resource, string operation, string message, string taskId = "")
    {

      string [] sections = this.GetType().ToString().Split(new char[] { '.' });

      Log.Information(string.Format("{0}{1} completed", operation.ToUpper(), resource.ToUpper()), "AsynchronousProviderBase", sections[sections.Length - 1]);

      if (this.Completed != null)
        this.Completed(new ProviderEventArgs(resource, operation, message, taskId));

    } // FireCompletedEvent(List<Study> collection)



    /// <summary>
    /// Fires the aborted event
    /// </summary>
    /// <param name="taskId">Task identifier</param>
    /// <param name="resource">Resource name</param>
    /// <param name="operation">Operation performed</param>
    protected virtual void FireAbortedEvent(string resource, string operation, string taskId = "")
    {
      string[] sections = this.GetType().ToString().Split(new char[] { '.' });

      Log.Debug(string.Format("{0}{1} cancelled", operation.ToUpper(), resource.ToUpper()), "AsynchronousProviderBase", sections[sections.Length - 1]);

      if (this.Aborted != null)
        this.Aborted(new ProviderEventArgs(resource, operation, taskId));

    } // FireAbortedEvent()


    
    /// <summary>
    /// Fires the failed event
    /// </summary>
    /// <param name="taskId">Task identifier</param>
    /// <param name="resource">Resource name</param>
    /// <param name="operation">Operation performed</param>
    /// <param name="message">Messgae</param>
    protected virtual void FireFailedEvent(string resource, string operation, string message, string taskId = "")
    {
      string[] sections = this.GetType().ToString().Split(new char[] { '.' });

      Log.Error(string.Format("{0}{1} failed : {2}", operation.ToUpper(), resource.ToUpper(), message), "AsynchronousProviderBase", sections[sections.Length - 1]);

      if (this.Failed != null)
        this.Failed(new ProviderEventArgs(resource, operation, message, taskId));
    } // FireFailedEvent( message )


    #endregion

    #endregion



  } // class AsynchronousProviderBase

} // namespace Viatronix.Enterprise.SDK.Providers

