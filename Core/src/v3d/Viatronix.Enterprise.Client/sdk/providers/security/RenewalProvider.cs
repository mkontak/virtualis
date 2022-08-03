using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Messaging;
using System.Globalization;

namespace Viatronix.Enterprise.SDK.Providers
{

  public sealed class RenewalProvider : ProviderBase, IRenewalProvider, IDisposable
  {

    #region fields

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Time the renewal started
    /// </summary>
    private DateTime? m_started = null;

    /// <summary>
    /// Context TTL in MS
    /// </summary>
    private Int64 m_contextTtl = 0;

    /// <summary>
    /// Renewal time (ms)
    /// </summary>
    private Int64 m_renewalTime = 0;

    /// <summary>
    /// Renewal timer
    /// </summary>
    private System.Threading.Timer m_timer = null;


    /// <summary>
    /// Last error message from a failed renewal
    /// </summary>
    private string m_errorMessage = string.Empty;

    /// <summary>
    /// Renewal failed flag
    /// </summary>
    private bool m_failed = false;


    /// <summary>
    /// Security provider
    /// </summary>
    private ISecurityProvider m_securityProvider;

    #endregion


    #region contructors

    /// <summary>
    /// Constructs a renewal timer
    /// </summary>
    /// <param name="securityProvider"></param>
    public RenewalProvider(string xml, ISecurityProvider securityProvider) :
      base(xml)
    {

      if (securityProvider == null)
        throw new ApplicationException("No security peovider specified");

      m_securityProvider = securityProvider;


      if (xml.Length > 0)
      {

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);

        if ( doc.DocumentElement.TryGetAttributeValue<Int64>("renewal", ref m_renewalTime) )
           m_renewalTime *= 60000;

      }
 
    }

    #endregion


    #region properties

    /// <summary>
    /// Returns true of availabl
    /// </summary>
    public override bool IsAvailable
    { get { return m_securityProvider.IsAvailable; }  }


    /// <summary>
    /// Sets/Gets tghe renewal time
    /// </summary>
    public Int64 RenewalTime
    {
      get { return m_renewalTime; }
      set { m_renewalTime = value; }
    } // RenewlTime

    /// <summary>
    /// Gets the renewal error message 
    /// </summary>
    public string LastRenewalErrorMessage
    {
      get { return m_errorMessage; }
    }

    /// <summary>
    /// Renewal failed
    /// </summary>
    public bool IsRenewalFailed
    {
      get { return m_failed; }
    } // IsRenewalFailed

    /// <summary>
    /// Returns flag indicating if the renewl timer is running or not
    /// </summary>
    public bool IsRenewalTimerRunning
    {
      get { return m_timer != null; }
    }   // IsRenewalProviderRunning

    #endregion

    #region events


    /// <summary>
    /// Renewal failed event
    /// </summary>
    public event RenewalEventHandler RenewalFailed;

 
    /// <summary>
    /// Raises the renewal failed event
    /// </summary>
    /// <param name="message"></param>
    private void OnRenewalFailed(string message)
    {
      if (RenewalFailed != null)
        RenewalFailed(new ProviderEventArgs("context", "renew", message));
    } // OnRenewalFailed()

    #endregion


    #region methods


    /// <summary>
    /// Timer callback for the renwal of the context
    /// </summary>
    /// <param name="status"></param>
    private void Renewal(object status)
    {
      try
      {
        Log.Information("Renewing context", "RestSecurityProvider", "Renewal");
        m_securityProvider.Renew();
        m_started = DateTime.Now;
      }
      catch (Exception ex)
      {

        TimeSpan span = DateTime.Now.Subtract(m_started.Value);

        if ((Int64)span.TotalMilliseconds > m_contextTtl)
        {
          Log.Error(string.Format("Context renewal failed : {0}", ex.Message), "RestSecurityProvider", "Renewal");
          m_failed = true;
          m_errorMessage = ex.Message;

          try
          {
            m_securityProvider.Logout();
          }
          catch (Exception)
          {
            // Ignore the Logout exception
          }

          OnRenewalFailed(ex.Message);

        }
        else
        {
          Log.Error(string.Format("Context renewal failed : {0}, adjusting time and continuing", ex.Message), "RestSecurityProvider", "Renewal");

          Int64 renewalTime = (m_renewalTime > 1 ? m_renewalTime / 2 : m_renewalTime);
          m_timer.Change(renewalTime, renewalTime);
        }

      }
    }


    #endregion

    #region IRenewalProvider

    /// <summary>
    /// Starts the renewal timer, using the specified time, if it is not already running and return true if started,
    /// false if not started.  
    /// </summary>
    /// <param name="renewlTime">New renewal time</param>
    /// <returns></returns>
    public bool StartRenewalTimer(Int64 renewlTime)
    {
 
      if (!m_securityProvider.IsAuthenticated)
        throw new InvalidOperationException("This operation requires a login");

      // started flag
      bool started = false;


      try
      {
        // ==================================================================================
        // Make sure the renewal time is greater then 0 and the timer is not already running
        // =================================================================================
        if (!IsRenewalTimerRunning && renewlTime > 0)
        {


          /// Gets the context TTL
          GetContextTtl();

          // ===========================================================================
          // If the renewal time specified is greater then the context time to live 
          // adjust it to half the context ttl
          // =========================================================================
          if (renewlTime > m_contextTtl)
          {

            m_renewalTime = m_contextTtl / 2;

            Log.Warning(string.Format("Renewal time of {0}ms is greater then the contexts ttl of {1}ms, adjusting the renewal time down to {2}ms",
              renewlTime, m_contextTtl, m_renewalTime), "RestSecurityProvider", "RestSecurityProvider");
          }
          else
            m_renewalTime = renewlTime;

          m_timer = new System.Threading.Timer(Renewal, null, m_renewalTime, m_renewalTime);

          m_started = DateTime.Now;

          started = true;

        }

        return started;
      }
      catch (Exception ex)
      {
        Log.Error("Failed to start the renewal timer " + ex.Message, "RenewalProvider", "Start");
        throw;
      }

    }

    /// <summary>
    /// Starts the renewal timer, using the renewal time already set in the object, if it is not already running and return true if started,
    /// false if not started.  
    /// </summary>
    /// <returns>True is started, flase if not</returns>
    public bool StartRenewalTimer()
    {

      return StartRenewalTimer(m_renewalTime);
    } // StartRenewalProvider()


    /// <summary>
    /// Stops the renewal time if it is running
    /// </summary>
    public void StopRenewalTimer()
    {
      if (IsRenewalTimerRunning)
      {
        m_timer.Dispose();
        m_timer = null;
        m_started = null;
      }
    } // Stop()


    /// <summary>
    /// Gets the context Ttl in MS
    /// </summary>
    /// <returns></returns>
    public void GetContextTtl()
    {

      if ( ! m_securityProvider.IsAuthenticated )
        throw new InvalidOperationException("This operation requires a login");

      if (m_contextTtl > 0)
        return; 

      m_contextTtl = 0;

      try
      {
        // Get the Contextttl and its units from the settings
        Setting contextTtl = m_securityProvider.GetSetting("ContextTtl");
        Setting contextTtlUnits = m_securityProvider.GetSetting("ContextTtlUnits");

        // Assume MS
        m_contextTtl = Int64.Parse(contextTtl.Value);

        // Convert from minutes
        if (string.Compare(contextTtlUnits.Value, "minute", true) == 0)
          m_contextTtl *= 60000;

        // Convert from seconds
        else if (string.Compare(contextTtlUnits.Value, "second", true) == 0)
          m_contextTtl *= 1000;

        // Convert from hours
        else if (string.Compare(contextTtlUnits.Value, "hour", true) == 0)
          m_contextTtl *= 3600000;

      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed to get the context ttl : {0}", ex.Message), "RestScurityProvider", "GetContextttl");
        throw;
      }

    } // GetContextTtl() 


    #endregion



    #region IDisposable Members


    /// <summary>
    /// Disposes of object
    /// </summary>
    public void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    } // Dispose()

    /// <summary>
    /// Disposes the objects resources
    /// </summary>
    /// <param name="disposing">true if managed object should be cleaned up; otherwise only cleanup unmanaged</param>
    private void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {
          StopRenewalTimer();
          System.Threading.Thread.Sleep(200);
        }

        m_disposed = true;
      }
    } // Dispose( disposing )

    #endregion

  }
}
