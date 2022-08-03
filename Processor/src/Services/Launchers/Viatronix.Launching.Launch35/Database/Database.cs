using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading.Tasks;
using System.Configuration;

namespace Viatronix.Launching.Database
{

  /// <summary>
  /// Static class for access and reading the connection string config file
  /// </summary>
  public static class Database
  {

    private static Dictionary<string, string> m_connectionStrings = null;


    /// <summary>
    /// Connection strings
    /// </summary>
    public static Dictionary<string, string> ConnectionStrings
    {

      get 
      {

        if (m_connectionStrings == null)
        {

          m_connectionStrings = new Dictionary<string, string>();

          string configDirectory = System.IO.Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Config");
          string connectionsConfigFile = (ConfigurationManager.AppSettings["connectionStrings"] != null ?
                                           ConfigurationManager.AppSettings["connectionStrings"] :
                                           System.IO.Path.Combine(configDirectory, "connections.config"));


          XmlDocument doc = new XmlDocument();

          doc.Load(connectionsConfigFile);

          string server = (doc.DocumentElement.Attributes["server"] == null ? string.Empty : doc.DocumentElement.Attributes["server"].Value);


          foreach (XmlNode node in doc.DocumentElement.SelectNodes("add"))
          {
            string name = node.Attributes["name"].Value;

            string connectionString = Crypto.Decrypt(node.Attributes["connectionString"].Value);


            if (!string.IsNullOrEmpty(server))
              connectionString = (server.Contains("\\") ? connectionString.Replace("(local)\\v3d", server) : connectionString.Replace("(local)", server));

            m_connectionStrings.Add(name, connectionString);
          }

 


        }

        return m_connectionStrings;
      }

    }

  }
}
