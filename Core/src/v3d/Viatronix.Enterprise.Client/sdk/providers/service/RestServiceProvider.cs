using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Threading.Tasks;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise.SDK.Providers
{

  /// <summary>
  /// Implements a service provider for the database
  /// </summary>
  class RestServiceProvider : ProviderBase, IServiceProvider
  {

    #region fields

    /// <summary>
    /// Rest client
    /// </summary>
    private HttpClient m_client = null;

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
    public RestServiceProvider(string xml, ISecurityProvider securityProvider)
      : base(xml)
    {

      if (securityProvider == null)
        throw new Exception("No security provider specified");

      m_securityProvider = securityProvider;

      string url = string.Empty;

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      // Try to get the url necessary to connect
      if (!doc.DocumentElement.TryGetAttributeValue<string>("url", ref url))
        throw new MissingAttributeException("Missing url in the initialization xml");

      // Create a new rest client using the url
      m_client = new HttpClient(url);

      //====================================================================
      // Read in the client certificate which is embedded in the program
      //====================================================================
      using (System.IO.Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.Enterprise.res.client.pfx"))
      {
        try
        {
          // Allocate a buffer to read in the certificate
          byte[] certificateBuffer = new byte[stream.Length];

          // Read the entire certificate to the bufer
          stream.Read(certificateBuffer, 0, (int)stream.Length);

          // Set the private client certificate
          m_client.Certificate = new System.Security.Cryptography.X509Certificates.X509Certificate2(certificateBuffer);
        }
        catch (Exception ex)
        {
          throw new Exception("Failed to create enterprise client : " + ex.Message);
        }
      } // using

    } // RestServiceProvider

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
    { get { return m_client.IsAvailable; } }

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

      Messaging.RequestMessage request = Messaging.RequestMessage.Create(m_securityProvider.Context, Messaging.OperationTypes.Get, XmlBuilder.Create("database"));
      request.Source = "service";

      Messaging.ResponseMessage response = m_client.Get(request);

      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode );

      EntityCollection.Load(databases, response.Message);

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

      XmlBuilder query = XmlBuilder.Create("service", new Dictionary<string, string>() { { "db", table.Database.Name }, { "tbl", table.Name } });

      Messaging.RequestMessage request = Messaging.RequestMessage.Create(m_securityProvider.Context, Messaging.OperationTypes.Get, query);
      request.Source = "service";

      Messaging.ResponseMessage response = m_client.Get(request);

      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

 
      EntityCollection.Load(entities, response.Message);


      return entities;

    } // GetTable()


    #region AgentJob

    /// <summary>
    /// Gets all the entities from the table specified
    /// </summary>
    /// <returns></returns>
    public List<AgentJob> GetAgentJobs()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<AgentJob> jobs = new List<AgentJob>();

      XmlBuilder query = XmlBuilder.Create("service", new Dictionary<string, string>() { { "db", "System" }, { "tbl", "AgentJobs" } });

      Messaging.RequestMessage request = Messaging.RequestMessage.Create(m_securityProvider.Context, Messaging.OperationTypes.Get, query);
      request.Source = "service";

      Messaging.ResponseMessage response = m_client.Get(request);

      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);


      EntityCollection.Load(jobs, response.Message);


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

      Messaging.RequestMessage request = Messaging.RequestMessage.Create(m_securityProvider.Context, Messaging.OperationTypes.Insert, XmlBuilder.Create(job));
      request.Source = "service";

      Messaging.ResponseMessage response = m_client.Get(request);

      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

    } // Insert()

    /// <summary>
    /// Deletes the specified agent job from the system
    /// </summary>
    /// <returns></returns>
    public void Delete(AgentJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      Messaging.RequestMessage request = Messaging.RequestMessage.Create(m_securityProvider.Context, Messaging.OperationTypes.Delete, XmlBuilder.Create(job));
      request.Source = "service";

      Messaging.ResponseMessage response = m_client.Get(request);

      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

    } // Delete()

    #endregion


  } // class RestServiceProvider

} // namespace  Viatronix.Enterprise.SDK.Providers 
