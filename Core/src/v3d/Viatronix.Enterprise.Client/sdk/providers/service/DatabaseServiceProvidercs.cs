using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise.SDK.Providers
{

  /// <summary>
  /// Implements a service provider for the database
  /// </summary>
  class DatabaseServiceProvider : ProviderBase, IServiceProvider
  {

    #region fields

    /// <summary>
    /// Source used for connection strings
    /// </summary>
    private readonly string m_source = "service";


    /// <summary>
    /// Security provider
    /// </summary>
    private ISecurityProvider m_securityProvider;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="xml"></param>
    /// 
    public DatabaseServiceProvider(string xml, ISecurityProvider securityProvider) : base(xml)
    {
      if (securityProvider == null)
        throw new Exception("No security provider specified");

      m_securityProvider = securityProvider;
    } // DatabaseServiceProvider

    #endregion

    #region properties


    /// <summary>
    /// Security provider
    /// </summary>
    public ISecurityProvider SecurityProvider
    {
      get { return m_securityProvider; }
      set
      {
        if (value == null)
          throw new NotSupportedException("Null security provider is not supported");

        m_securityProvider = value;
      }
    }

    /// <summary>
    /// Returns the available flag
    /// </summary>
    public override bool IsAvailable
    { get { return SystemGateway.IsAvailable("service") && StorageGateway.IsAvailable("service") && SecurityGateway.IsAvailable("service") && AuditingGateway.IsAvailable("service"); } }

    /// <summary>
    /// Returns the authenticated flag
    /// </summary>
    public override bool IsAuthenticated
    { get { return (m_securityProvider != null && m_securityProvider.IsAuthenticated); } }

    #endregion


    /// <summary>
    /// Gets all Viatronix databases from the system
    /// </summary>
    /// <returns></returns>
    public List<Database> GetDatabases()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      List<Database> databases = new List<Database>();


      EntityCollection.Load<Database>(databases, Viatronix.Enterprise.Data.SystemGateway.Get(m_securityProvider.Context, "database", XmlBuilder.Create("database"), m_source));

      return databases;
    } // GetDatabases()


    /// <summary>
    /// Gets all the entities from the table specified
    /// </summary>
    /// <param name="table"></param>
    /// <returns></returns>
    public List<IEntity> GetTable(Table table)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<IEntity> entities = new List<IEntity>();


      if (table != null && ! string.IsNullOrEmpty(table.Name) )
      {
        XmlBuilder query = XmlBuilder.Create("service", new Dictionary<string, string>() { { "db", table.Database.Name }, { "tbl", table.Name } });

        if (string.Compare(table.Database.Name, "System", true) == 0)
          EntityCollection.Load(entities, Viatronix.Enterprise.Data.SystemGateway.Get(m_securityProvider.Context, "service", query, m_source));
        else if (string.Compare(table.Database.Name, "Auditing", true) == 0)
          EntityCollection.Load(entities, Viatronix.Enterprise.Data.AuditingGateway.Get(m_securityProvider.Context, "service", query, m_source));
        else if (string.Compare(table.Database.Name, "Storage", true) == 0)
          EntityCollection.Load(entities, Viatronix.Enterprise.Data.StorageGateway.Get(m_securityProvider.Context, "service", query, m_source));
        else if (string.Compare(table.Database.Name, "Security", true) == 0)
          EntityCollection.Load(entities, Viatronix.Enterprise.Data.SecurityGateway.Get(m_securityProvider.Context, "service", query, m_source));
        else if (string.Compare(table.Database.Name, "Licensing", true) == 0)
          EntityCollection.Load(entities, Viatronix.Enterprise.Data.LicensingGateway.Get(m_securityProvider.Context, "service", query, m_source));
        else
          throw new NotSupportedException(string.Format("Access to {0} is not supported", table.Database.Name));
      }

      return entities;

    } // GetTable()


    #region AgentJob

    /// <summary>
    /// Gets all agent jobs specified in the system
    /// </summary>
    /// <returns></returns>
    public List<AgentJob> GetAgentJobs()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<AgentJob> jobs = new List<AgentJob>();

      XmlBuilder query = XmlBuilder.Create("service", new Dictionary<string, string>() { { "db", "System" }, { "tbl", "AgentJobs" } });

      EntityCollection.Load(jobs, Viatronix.Enterprise.Data.SystemGateway.Get(m_securityProvider.Context, "service", query, m_source));

      return jobs;

    } // GetAgentJobs()

    /// <summary>
    /// Inserts the specified agent job into the system
    /// </summary>
    /// <returns></returns>
    public void Insert(AgentJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      Viatronix.Enterprise.Data.SystemGateway.Insert(m_securityProvider.Context, "agentjob", XmlBuilder.Create(job), m_source);

    } // Insert()

    /// <summary>
    /// Inserts the specified agent job into the system
    /// </summary>
    /// <returns></returns>
    public void Delete(AgentJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      Viatronix.Enterprise.Data.SystemGateway.Delete(m_securityProvider.Context, "agentjob", XmlBuilder.Create(job), m_source);

    } // Delete()

    #endregion

    #region Configuration File

    /// <summary>
    /// Gets the configuration file.
    /// </summary>
    /// <param name="filename">The filename.</param>
    /// <returns>The contents of the configuration file.</returns>
    public string GetConfigurationFile(string filename)
    {
      using (TextReader reader = new StreamReader(filename))
      {
        return reader.ReadToEnd();
      }
    } // GetGetConfigurationFile( filename )


    /// <summary>
    /// Updates the configuration file's contents.
    /// </summary>
    /// <param name="filename">The filename.</param>
    /// <param name="contents">The file contents.</param>
    public void UpdateConfigurationFile(string filename, string contents)
    {
      using(TextWriter writer = new StreamWriter(filename, false))
      {
        writer.Write(contents);
      }
    } // UpdateConfigurationFile( filename, contents )
    
    #endregion

  } 
}
