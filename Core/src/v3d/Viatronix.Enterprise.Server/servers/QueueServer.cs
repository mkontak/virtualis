// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: QueueServer.cs
//
// Description: Implements a base Queue Server to be inherited by any server that needs to implement the standard queuing mechanism.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Threading;
using System.Data;
using System.Text;
using System.Xml;
using System.Collections;
using System.Data.SqlClient;
using System.Collections.Generic;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;
using System.Security.AccessControl;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Net;
using System.Net.Sockets;
using Viatronix.Messaging;
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Servers
{

  /// <summary>
  /// Abstract class for a queue server
  /// </summary>
  public class QueueServer : Server
  {


    #region  field


    /// <summary>
    /// Pending jobs event name
    /// </summary>
    private string m_pendingJobsEventName = string.Empty;
    
    /// <summary>
    /// Polling interval
    /// </summary>
    private Int32 m_pollingInterval = Timeout.Infinite;

    /// <summary>
    /// Pending Jobs Index
    /// </summary>
    private int m_pendingJobsEventIndex = 0;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    protected QueueServer(string name) : base(name) {}

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="name"></param>
    /// <param name="settings"></param>
    protected QueueServer(string name, string settings) : base(name, settings) { }

    #endregion

    #region properties

    /// <summary>
    /// Gets the pending jobs event index
    /// </summary>
    public int PendingJobsEvent
    { get { return m_pendingJobsEventIndex; } }


    #endregion

    #region methods

    /// <summary>
    /// Initialzies the QueuServer properties
    /// </summary>
    /// <param name="node"></param>
    protected override void Initialize(XmlNode node)
    {

      base.Initialize(node);

      node.TryGetAttributeValue<Int32>("pollingInterval", ref m_pollingInterval);

      // ============================================================
      // If the pending jobs event is defined then create the event
      // ============================================================
      if (node.TryGetAttributeValue<string>("pendingJobsEventName", ref m_pendingJobsEventName) && ! String.IsNullOrEmpty(m_pendingJobsEventName) )
      {
        // Event Security
        EventWaitHandleSecurity security = new EventWaitHandleSecurity();
        security.AddAccessRule(new EventWaitHandleAccessRule(new SecurityIdentifier(WellKnownSidType.WorldSid, null), EventWaitHandleRights.FullControl, AccessControlType.Allow));

        bool created;

        m_pendingJobsEventIndex = AddEvent(m_pendingJobsEventName, new EventWaitHandle(false, EventResetMode.ManualReset, "Global\\" + m_pendingJobsEventName, out created, security));

      } // END .. try to get the pendingJobsEventName

    } // Initialize(XmlNode node)

    /// <summary>
    /// Waits for pending jobs or any other events that may have been added to the list.
    /// Otherwise will just wait the milliseondsTimeout time before looking.
    /// </summary>
    protected virtual int WaitForPendingJobs(int milliseondsTimeout = -1)
    {
      int index = -1;

      try
      {


        // ==========================================================================================================
        // If there is a pending jobs event then wait for any event otherwise we are polling so we wait for
        // any event to be triggered within the polling interval.
        // =========================================================================================================
        if ( ! string.IsNullOrEmpty(m_pendingJobsEventName) )
        {
          LogInfo( string.Format("Waiting for a pending jobs, Timeout:  {0}ms", milliseondsTimeout), "QueueServer", "WaitForPendingJobs");

          index = EventWaitHandle.WaitAny(this.Events, milliseondsTimeout);
        }
        else
        {
          int timeOut = (milliseondsTimeout != -1 ? milliseondsTimeout : m_pollingInterval);

          
          if ( timeOut > 0 )
            LogDbg(string.Format("Waiting for the pending jobs, Timeout: {0}ms", timeOut), "QueueServer", "WaitForPendingJobs");
          else
            LogDbg("Waiting for the pending jobs", "QueueServer", "WaitForPendingJobs");

          index = EventWaitHandle.WaitAny(this.Events, ( milliseondsTimeout != -1 ? milliseondsTimeout : m_pollingInterval));
        }

        if (index == 0)
          throw new AbortException(Name + " is aborting");
        else 
          ResetEvent(index);
      }
      catch (AbortException)
      {
        // Ignore
      }
      catch (Exception ex)
      {
        LogErr("Waiting for pending jobs failed : " + ex.Message, "QueueServer", "WaitForPendingJobs");
      }

      return index;

    } // WaitForPendingJobs()

 
    #endregion

  } // class QueueServer 

} // namespace Viatronix.Enterprise.Servers
