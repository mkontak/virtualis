using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading.Tasks;

namespace Viatronix.Enterprise.Data
{

  /// <summary>
  /// Implements the database connection strings object
  /// </summary>
  public class DatabaseConnectionStrings
  {
   
    
    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Server = "server";          // Server Instance
      public const string Name = "name";              // Name of connection strings (application or provider)
      public const string ConnectionString = "connectionString";
     };

    #endregion

    #region fields

    /// <summary>
    /// Name associated with these connection strings
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Database instance server
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// Connection strings
    /// </summary>
    private Dictionary<string, string> m_connectionStrings = new Dictionary<string, string>();

    #endregion


    #region properties

    /// <summary>
    /// Name associated with these connection strings
    /// </summary>
    public string Name
    { get { return m_name; } }

    /// <summary>
    /// Database instance
    /// </summary>
    public string Server
    { get { return m_server; } }

    /// <summary>
    /// Database Connection strings
    /// </summary>
    public Dictionary<string, string> ConnectionStrings
    { get { return m_connectionStrings; } }

    #endregion


    #region static methods


    /// <summary>
    /// Creates the connection strings object from the xml string
    /// </summary>
    /// <param name="xml">Xml string</param>
    /// <returns>CoinnectionStrings</returns>
    public static DatabaseConnectionStrings Create(string xml)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);

    } // ConnectionStrings Create(string xml)

    /// <summary>
    /// Creates the connection stringfs object from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>ConnectionStrings</returns>
    public static DatabaseConnectionStrings Create(XmlNode node)
    {

      DatabaseConnectionStrings connectionStrings = new DatabaseConnectionStrings();

      if (connectionStrings == null)
        throw new OutOfMemoryException("Failed to allocate DatabaseConnectionStrings object");


      node.TryGetAttribute<string>(Attributes.Server, ref connectionStrings.m_server);
      node.TryGetAttribute<string>(Attributes.Name, ref connectionStrings.m_name);

      
      foreach (XmlNode csnode in node.SelectNodes(Attributes.ConnectionString) )
      {
        string connectionString = Viatronix.Encryption.Crypto.Decrypt(csnode.Attributes["value"].Value) + connectionStrings.m_server;
        connectionStrings.m_connectionStrings.Add(csnode.Attributes["name"].Value, connectionString);
      } // END ... For each connection node


      return connectionStrings;
    }

    #endregion

  } // class DatabaseConnectionStrings

} // namespace Viatronix.Enterprise.Data
