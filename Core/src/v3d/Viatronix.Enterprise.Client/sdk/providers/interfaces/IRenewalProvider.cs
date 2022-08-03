using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{

 
  #region delegates


  /// <summary>
  /// Signature for a provider delegate
  /// </summary>
  /// <param name="resource">Resource requested</param>
  /// <param name="operation">operation</param>
  /// <param name="message">Error message if failed/xml if get/empty otherwise</param>
  public delegate void RenewalEventHandler(ProviderEventArgs args);

  #endregion



  public interface IRenewalProvider : IProvider
  {

    /// <summary>
    /// Renewal Time in MS
    /// </summary>
    Int64 RenewalTime
    { get; set; }

    /// <summary>
    /// True is the timer is running
    /// </summary>
    bool IsRenewalTimerRunning
    { get; }

    /// <summary>
    /// Flag as true if the renewal failed
    /// </summary>
    bool IsRenewalFailed
    { get; }

    /// <summary>
    /// Returns the renewal failed message
    /// </summary>
    string LastRenewalErrorMessage
    { get; }

    /// <summary>
    /// Starts the timer using the renewl time already set
    /// </summary>
    bool StartRenewalTimer();

    /// <summary>
    /// Starts the timer using a new renewal time
    /// </summary>
    /// <param name="renewalTime">Renewal time</param>
    bool StartRenewalTimer(Int64 renewalTime);


    /// <summary>
    /// Stops the timer, requires a manual Start() to be called
    /// </summary>
    void StopRenewalTimer();
   
    /// <summary>
    /// Renewal failed event
    /// </summary>
    event RenewalEventHandler RenewalFailed;

  } // interface IRenewal
}
