// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: LoggingServer.cs
//
// Description: 
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
  public class LoggingServer : Server
  {


    #region fields

    private Viatronix.Logging.Server m_server = null;

  
    #endregion

    #region constructors and destructors

    /// <summary>
    /// Constructor
    /// </summary>
    private LoggingServer() : base("Logging") { }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="settings"></param>
    public LoggingServer(string settings) : base("Logging", settings) { }
    

    #endregion


    #region methods

    /// <summary>
    /// Object initializer override
    /// </summary>
    /// <param name="node"></param>
    protected override void Initialize(XmlNode node)
    {

      base.Initialize(node);

       UInt32 port = 0;

      if (node.TryGetAttributeValue<UInt32>("port", ref port))
        throw new MissingAttributeException("No port specified for logging server");

      m_server = Viatronix.Logging.Server.Create(port);


    } // Initialize(XmlNode node)



    /// <summary>
    /// Creates the server using the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>LoggingServer object</returns>
    public static LoggingServer Create(XmlNode node)
    {
      LoggingServer server = new LoggingServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate LoggingServer");


      UInt32 port = 0;

      if (node.TryGetAttributeValue< UInt32 >("port", ref port))
        throw new MissingAttributeException("No port specified for logging server");

      server.m_server = Viatronix.Logging.Server.Create(port);


      return server;




    } // Create(XmlNode node)



    /// <summary>
    /// Aborts the work queue server
    /// </summary>
    public override void Abort(Dictionary<string, string> parameters = null)
    {

      LogInfo("Aborting the logging server", "LoggingServer", "Abort");
      m_server.Stop();
  
    } // Abort()


    /// <summary>
    /// 
    /// </summary>
    /// <param name="parameters"></param>
    public override void Start(Dictionary<string, string> parameters = null)
    {
      m_server.Start();
    }


 
    /// <summary>
    /// Executes the Logging 
    /// </summary>
    public override void RunSynchronously()
    {

      // Do nothing the threading is implemented in the Logging.Server
  
    } // Run()



    #endregion

  }
}
