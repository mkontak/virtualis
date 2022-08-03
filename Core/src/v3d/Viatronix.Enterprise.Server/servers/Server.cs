using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Servers
{
  /// <summary>
  /// Base server class
  /// </summary>
  public abstract class Server
  {

 

    #region fields


    /// <summary>
    /// Version of server (default for all servers is 3.6)
    /// </summary>
    protected string m_version = "3.6";

    /// <summary>
    /// Database connection strings
    /// </summary>
    protected Dictionary<string, string> m_connectionStrings = new Dictionary<string, string>();

    /// <summary>
    /// Logger
    /// </summary>
    protected string m_logger = "Default";

    /// <summary>
    /// Context ID
    /// </summary>
    protected readonly string m_context = Viatronix.Encryption.Crypto.Decrypt("cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ");

    /// <summary>
    /// Cancelation token
    /// </summary>
    private System.Threading.CancellationToken m_cancelToken = new System.Threading.CancellationToken();

    /// <summary>
    /// Name of the server
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Task to be started
    /// </summary>
    private Task m_task = null;

    /// <summary>
    /// State
    /// </summary>
    private ServerStates m_state = ServerStates.Idle;

    /// <summary>
    /// Date the server was started
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Events to wait for
    /// </summary>
    private List<EventWaitHandle> m_events = new List<EventWaitHandle>();

    /// <summary>
    /// Maps a name to an index
    /// </summary>
    private Dictionary<string, int> m_eventMap = new Dictionary<string, int>();

    #endregion


    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name">Name of the server</param>
    protected Server(string name)
    {

      // Add the abort event as the first event in the list
      m_events.Add(new ManualResetEvent(false));
      m_eventMap.Add("abort", 0);

      m_name = name;  
    } // Server


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name">Name of the server</param>
    protected Server(string name, string settings) : this(name)
    {
      XmlDocument doc = new XmlDocument();

      doc.LoadXml(settings);

      Initialize(doc.DocumentElement);
    } // Server


    #endregion

    #region properties

    /// <summary>
    /// Gets the abort event
    /// </summary>
    protected EventWaitHandle AbortEvent
    {
      get { return m_events[m_eventMap["abort"]]; }
    }

    /// <summary>
    /// Returns the Event List
    /// </summary>
    protected EventWaitHandle[] Events
    {
      get { return m_events.ToArray(); }
    } // Events

    /// <summary>
    /// Gets the version of the server
    /// </summary>
    public string Version
    {
      get { return m_version; }
    } // Version

    /// <summary>
    /// Gets the name of the server
    /// </summary>
    public string Name
    {
      get { return m_name;  }
    } // Name

    /// <summary>
    /// Gets the servers state
    /// </summary>
    public ServerStates State
    { get { return m_state;  } }

    #endregion

    #region events

    /// <summary>
    /// Aborted event
    /// </summary>
    public event EventHandler Aborted;

    /// <summary>
    /// Started event
    /// </summary>
    public event EventHandler Started;

    /// <summary>
    /// Failed event
    /// </summary>
    public event EventHandler<EventArgs<Exception>> Failed;



    #endregion

    #region event handlers

    /// <summary>
    /// Abort event handler
    /// </summary>
    protected virtual void OnAborted()
    {
      m_state = ServerStates.Aborted;
      if (Aborted != null)
        Aborted(this, EventArgs.Empty);
     } // OnAborted()


    /// <summary>
    /// Start event handler
    /// </summary>
    protected virtual void OnStarted()
    {
      m_state = ServerStates.Running;
      if (Started != null)
        Started(this, EventArgs.Empty);
    } // OnStarted()
    


    /// <summary>
    /// Failed event handler
    /// </summary>
    /// <param name="ex"></param>
    protected virtual void OnFailed(Exception ex)
    {
      m_state = ServerStates.Failed;
      if (Failed != null)
        Failed(this, new EventArgs<Exception>(ex));
    } // OnFailed(Exception ex)

    #endregion

    #region methods

    /// <summary>
    /// Gets the event
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    protected EventWaitHandle GetEvent(string name)
    {
      if (!m_eventMap.ContainsKey(name))
        throw new ViatronixException(name + " event does not exist", ErrorCodes.None);

      return m_events[m_eventMap[name]];

    } // GetEvent()

    /// <summary>
    /// Resets the named event
    /// </summary>
    /// <param name="name">Event name</param>
    protected void ResetEvent(string name)
    {
      if (m_eventMap.ContainsKey(name))
        m_events[m_eventMap[name]].Reset();
    } // ResetEvent(string name)

    /// <summary>
    /// Resets the named event
    /// </summary>
    /// <param name="index">Event index</param>
    protected void ResetEvent(int index)
    {
      // ===================================================
      // Only reset the event if it is within the range
      // ==================================================
      if ( index >= 0 && index < m_events.Count  )
        m_events[index].Reset(); 
    } // ResetEvent(int index)

    /// <summary>
    /// Sets the named event
    /// </summary>
    /// <param name="name">Event name</param>
    protected void SetEvent(string name)
    {
      if (m_eventMap.ContainsKey(name))
        m_events[m_eventMap[name]].Set();
    } // SetEvent(string name)

    /// <summary>
    /// Events
    /// </summary>
    /// <param name="handle"></param>
    protected int AddEvent(string name, EventWaitHandle handle)
    {
      if (m_eventMap.ContainsKey(name))
        throw new ViatronixException(name + " event already exists", ErrorCodes.None);
      else
      {
        int index = m_events.Count;
        m_events.Add(handle);
        m_eventMap.Add(name, index);
        return index;
      }
    }

    /// <summary>
    /// Initilaizes the server
    /// </summary>
    /// <param name="node"></param>
    protected virtual void Initialize(XmlNode node)
    {
      node.TryGetAttributeValue<string>("logger", ref m_logger);
    } // Initialize(XmlNode node)

    /// <summary>
    /// Generates the XML for this object
    /// </summary>
    /// <returns></returns>
    public virtual string ToXml()
    {
      string xml  = ( m_date.HasValue  ? string.Format("<server name=\"{0}\" state=\"{1}\" date=\"{2}\" />", m_name, m_state.ToString(), m_date )  :
                                         string.Format("<server name=\"{0}\" state=\"{1}\" date=\"\" />", m_name, m_state.ToString() ) ) ;

      return xml;
    } // ToXml()

    /// <summary>
    /// Starts the server
    /// </summary>
    /// <param name="parameters"></param>
    public virtual void Start(Dictionary<string, string> parameters = null)
    {

      // ===================================================================================================
      // If the task has not already been started or the task is started but aborted then star a new task
      // ===================================================================================================
      if (m_task == null || ( m_task != null && m_task.Exception.GetBaseException() is AbortException ) )
      {

        // Creates the task
        m_task = new Task(new Action(RunSynchronously), m_cancelToken);

        // Start the task
        m_task.Start();

        // Set the date started
        m_date = DateTimeOffset.Now;

      }
      else
        throw new ServerAlreadyStartedException(string.Format("{0} server is already started", this.Name));
      
    }


    /// <summary>
    /// Waits for server to complete. Blocks until the server completes.
    /// </summary>
    /// <param name="timeOut">Time in milliseconds for the server to wait</param>
    public virtual void Wait(int timeOut = 0)
    {
      // Wait for task
      m_task.Wait(timeOut);

      // Check the tasks state and if failted then throw the exception
      if (m_task.Status == TaskStatus.Faulted)
        throw m_task.Exception;
    }

    /// <summary>
    /// Run syncronously
    /// </summary>
    public virtual void RunSynchronously()
    {
      throw new NotImplementedException("RunSynchronously has not been implemented");
    } // RunSynchronously()

    /// <summary>
    /// Abort servers
    /// </summary>
    /// <param name="parameters"></param>
    public virtual void Abort(Dictionary<string, string> parameters)
    {
      Abort();
    } // Abort(Dictionary<string, string> parameters)


    /// <summary>
    /// Aborts the server
    /// </summary>
    public virtual void Abort()
    {

      // Set the abort event
      m_events[m_eventMap["abort"]].Set();


      // Log a server is abort message
      LogInfo("Aborting the server", "Server", "Abort");


      // Sets the date
      m_date = DateTimeOffset.Now;
    } // Abort()


    #region Logging interface

    /// <summary>
    /// Log information
    /// </summary>
    /// <param name="message"></param>
    /// <param name="className"></param>
    /// <param name="methodName"></param>
    protected void LogInfo(string message, string className, string methodName)
    {
      if (string.IsNullOrEmpty(m_logger))
        Logging.Log.Information(message, className, methodName);
      else
        Logging.Log.Information(m_logger, message, className, methodName);
    } // LogInfo()  

    /// <summary>
    /// Log debug
    /// </summary>
    /// <param name="message"></param>
    /// <param name="className"></param>
    /// <param name="methodName"></param>
    protected void LogDbg(string message, string className, string methodName)
    {
      if (string.IsNullOrEmpty(m_logger))
        Logging.Log.Debug(message, className, methodName);
      else
        Logging.Log.Debug(m_logger, message, className, methodName);
    } // LogDbg()
  
    /// <summary>
    /// Log error
    /// </summary>
    /// <param name="message"></param>
    /// <param name="className"></param>
    /// <param name="methodName"></param>
    protected void LogErr(string message, string className, string methodName)
    {
      if (string.IsNullOrEmpty(m_logger))
        Logging.Log.Error(message, className, methodName);
      else
        Logging.Log.Error(m_logger, message, className, methodName);
    } // LogDbg()  

    /// <summary>
    /// Log warning
    /// </summary>
    /// <param name="message"></param>
    /// <param name="className"></param>
    /// <param name="methodName"></param>
    protected void LogWrn(string message, string className, string methodName)
    {
      if (string.IsNullOrEmpty(m_logger))
        Logging.Log.Warning(message, className, methodName);
      else
        Logging.Log.Warning(m_logger, message, className, methodName);
    } // LogWrn()  

    #endregion

    #endregion


  }
}
