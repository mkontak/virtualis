using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Viatronix.Logging;

namespace Viatronix.Console
{



  /// <summary>
  /// Implements a generic refresh timer
  /// </summary>
  /// <typeparam name="T">Type being refreshed</typeparam>
  class RefreshTimer<T>
  {

    #region delegates

    /// <summary>
    /// Refresh deletage
    /// </summary>
    /// <param name="list">List of object for the specified type</param>
    public delegate void RefreshDelegate(ref List<T> list);

    /// <summary>
    /// Uppdate delgate
    /// </summary>
    /// <param name="sender">Object sending the event</param>
    /// <param name="args">Arguements</param>
    public delegate void UpdateDelegate(object sender, EventArgs args);

    #endregion


    #region fields

    /// <summary>
    /// Timer
    /// </summary>
    private System.Windows.Forms.Timer m_timer = new System.Windows.Forms.Timer();

    /// <summary>
    /// Parent control IVIEW
    /// </summary>
    private System.Windows.Forms.UserControl m_parent;

    /// <summary>
    /// refresh intervale
    /// </summary>
    private int m_interval = 1000;

    /// <summary>
    /// Name
    /// </summary>
    private string m_name;

    /// <summary>
    /// List 
    /// </summary>
    private List<T> m_list = new List<T>();

    #endregion

    #region properties

    /// <summary>
    /// Gets the internal list
    /// </summary>
    public List<T> List
    { get { return m_list;  } }

    /// <summary>
    /// Gets/Sets the enabled
    /// </summary>
    public bool Enabled
    { 
      get { return m_timer.Enabled; }
      set { m_timer.Enabled = value;  }
    } // Enabled

    /// <summary>
    /// Gets/Sets the interval
    /// </summary>
    public int Interval
    {
      get { return m_timer.Interval;  }
      set { m_timer.Interval = value;  }
    } // Interval

    #endregion


    #region constructors

  
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name">Name of the timer</param>
    /// <param name="node">Xml node to initialize</param>
    /// <param name="parent">Parent GUI</param>
    public RefreshTimer(string name, System.Windows.Forms.UserControl parent)
    {
      // Name of the timer
      m_name = name;

      // Parnet GUI
      m_parent = parent;

       /// Hook up the Tick event handler
      m_timer.Tick += new EventHandler(OnTickHandler);

     }  // RefreshTimer(string name, XmlNode node, System.Windows.Forms.UserControl parent)

    #endregion


    #region events

    /// <summary>
    /// Updates the Gui
    /// </summary>
    public UpdateDelegate Update;

    /// <summary>
    /// Refreshes the list
    /// </summary>
    public RefreshDelegate Refresh;

    #endregion


    #region methods

    /// <summary>
    /// Initializes the timer using the xml node
    /// </summary>
    /// <param name="node"></param>
    public void Initialize(XmlNode node)
    {
      /// Sets the refresh interval
      m_timer.Interval = m_interval = Convert.ToInt32(node.Attributes["RefreshRate"].Value) * 1000;

    } // Initialize()

    /// <summary>
    /// Asynchronous callback used during the Invoke of the Refresh delegate
    /// </summary>
    /// <param name="ar"></param>
    private void Callback(IAsyncResult ar)
    {

      if ( ar.IsCompleted )
      {

        // Ends the Asynchronous Call.
        Refresh.EndInvoke(ref m_list, ar);

        // Invoke the delegate on the UI thread.
        try
        {

          // ==================================================
          // We need to invoke the UPDATE on the GUID thread
          // ==================================================
          if ( ! m_parent.IsDisposed  && Update != null )
            m_parent.Invoke(Update, new object [2] { this, EventArgs.Empty } );

        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Warning(string.Format("Invoke of Update failed : {0} ", ex.Message), "RefreshTimer", "Callback");
        }

      }

    } // Callback( IAsyncResult ar )
    


    /// <summary>
    /// Handles the Timer's Tick event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnTickHandler(object sender, EventArgs args)
    {
      try
      {

        // Stop the timer
        Stop();

        /// ==========================================================================================
        /// We want to refresh the list and then callback to the Update so that the gui is updated
        /// ==========================================================================================
        if (Update != null && Refresh != null)
          Refresh.BeginInvoke(ref m_list, new AsyncCallback(Callback), null);
        else
          Log.Error("No refresh delegate setup", "RefreshTimer", "OnTickHandler");

      }
      catch (Exception e)
      {

        Log.Warning(string.Format("Failed to refresh the list {0} : ", e.Message), "RefreshTimer", "OnTickHandler");
      }
      finally
      {

        // Starts the timer
        Start();
      }

    } // OnTickHandler( object sender, EventArgs args )


    /// <summary>
    /// Resets the timer
    /// </summary>
    public void Reset()
    {
      if (m_timer.Enabled)
      {
        Stop();
        Start();
      }
    } // Reset()

    /// <summary>
    /// Stops the timer
    /// </summary>
    public void Stop()
    {
      m_timer.Stop(); 
    } // Stop()


    /// <summary>
    /// Starts the timer
    /// </summary>
    public void Start()
    {
      if ( !Enabled )
      {
        // Rests the timer back to the original interval
        m_timer.Interval = m_interval;

        // Starts the timer
        m_timer.Start();
      }
    
    } // Start()

    /// <summary>
    /// Runs the the timer Synchronously. Will stop and start it
    /// </summary>
    public void RunSynchronously()
    {
      OnTickHandler(this, EventArgs.Empty);
    }

    #endregion

  } // class RefreshTimer

} // namespace Viatronix.Console
