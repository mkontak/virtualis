using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.Providers
{

  public class ServerResourceProvider : ResourceProviderBase
  {

    #region fields

    /// <summary>
    /// Servers
    /// </summary>
    private static Dictionary<string, Viatronix.Enterprise.Servers.Server> m_servers = null;

    #endregion

    /// <summary>
    /// Gets the servers
    /// </summary>
    public static Dictionary<string, Viatronix.Enterprise.Servers.Server> Servers
    {
      get
      {
        if (m_servers == null)
        {

          m_servers = new Dictionary<string, Viatronix.Enterprise.Servers.Server>();

          try
          {
            // ==============================================
            // Add each server defined in the config file
            // ==============================================
            foreach (Viatronix.Enterprise.Configuration.ServerElement element in Viatronix.Enterprise.Configuration.ServersSection.GetSection().Servers)
            {
              Viatronix.Enterprise.Servers.Server server = (Viatronix.Enterprise.Servers.Server)Activator.CreateInstance(element.Type, element.Settings.Settings);

              m_servers.Add(server.Name, server);
            } // END .. For each server in the config file

          }
          catch (NotFoundException)
          {
            // ==========================================================================================================
            // If the Servers section is not found then ignore the exception it just means no servers are to be started
            // ==========================================================================================================
          }

        }

        return m_servers;
      }
    } // Servers

    /// <summary>
    /// Starts any associated internal server defined from the config file
    /// </summary>
    public static void StartServers()
    {

      foreach ( KeyValuePair<string, Viatronix.Enterprise.Servers.Server> pair in ServerResourceProvider.Servers )
      {
        if (pair.Value.State != ServerStates.Running)
          pair.Value.Start();
        
      }


    } //  StartServers()


    /// <summary>
    /// Stop any running internal servers
    /// </summary>
    public static void StopServers()
    {
      foreach (KeyValuePair<string, Viatronix.Enterprise.Servers.Server> pair in ServerResourceProvider.Servers)
      {
        Log.Information(string.Format("Stopping server {0}", pair.Key), "ServerResourceProvider", "StopServers");
        pair.Value.Abort();
      }

    } // StopServers


    /// <summary>
    /// Gets the server(s) specified
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    private string GetServer(RequestMessage request)
    {

      StringBuilder xml = new StringBuilder();

      xml.Append("<servers>");

      string name = "";
     
      // Gets the state
      string state = "";
      request.TryGetValue("state", out state);

      // ==========================================
      // Check if we are looking for only 1 server
      // ==========================================
      if ( request.TryGetValue("name", out name) )
      {
        if (ServerResourceProvider.Servers.ContainsKey(name) )
          if (state == null || ( state != null &&  ServerResourceProvider.Servers[name].State.ToString() == state ) )
            xml.Append(ServerResourceProvider.Servers[name].ToXml());
         
      }
      else
      {
        foreach (KeyValuePair<string, Viatronix.Enterprise.Servers.Server> pair in ServerResourceProvider.Servers)
        {
          if ( state.Length > 0 && pair.Value.State.ToString() == state ) 
            xml.Append(pair.Value.ToXml() );
        }

      }

      xml.Append("</servers>");


      return xml.ToString();
    } // GetServer()

    /// Stops a server
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml with new id</returns>
    private void StopServer(RequestMessage request)
    {

      string name = "";
      if (! request.TryGetValue("name", out name) )
        throw new MissingAttributeException("No server name specified");
      else
      {
        if (!ServerResourceProvider.Servers.ContainsKey(name))
          throw new NotFoundException(string.Format("Server {0} specified is not found in list", name));

        if (ServerResourceProvider.Servers[name].State == ServerStates.Running)
        {
          Log.Information(string.Format("Stopping {0}", name), "ServerResourceProvider", "StopServer");
          ServerResourceProvider.Servers[name].Abort(request.Parameters);
        }

      }
    }

    /// Stops a server
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns>Context xml with new id</returns>
    private void StartServer(RequestMessage request)
    {

      string name = "";
      if ( ! request.TryGetValue("name", out name))
        throw new MissingAttributeException("No server name specified");
      else
      {
        if (!ServerResourceProvider.Servers.ContainsKey(name))
          throw new NotFoundException(string.Format("Server {0} specified is not found in list", name));

        if (ServerResourceProvider.Servers[name].State != ServerStates.Running)
        {
          Log.Information(string.Format("Starting {0} server", name), "ServerResourceProvider", "StopServer");

          ServerResourceProvider.Servers[name].Start(request.Parameters);
        }

      }
    }


  }
}
