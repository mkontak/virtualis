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

  public sealed class RestDataProvider : AsynchronousProviderBase, IAsynchronousDataProvider, IDisposable
  {

    #region fields

 
    /// <summary>
    /// Rest client
    /// </summary>
    private HttpClient m_client = null;

    /// <summary>
    /// Security Provider
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    #endregion

    #region construction



    /// <summary>
    /// private constructor
    /// </summary>
    public RestDataProvider(string xml, ISecurityProvider securityProvider)
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
    }



    #endregion

    #region properties

    /// <summary>
    /// Gets the security provider
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
    /// Determines if we are authenticated
    /// </summary>
    public override bool IsAuthenticated
    {
      get { return (m_securityProvider != null && m_securityProvider.IsAuthenticated); }
    }

    #endregion

    #region create methods


 
    /// <summary>
    /// Creates a new instance of EnterpriseClient
    /// </summary>
    /// <returns>EnterpriseClient object</returns>
    public static RestDataProvider Create(string xml, ISecurityProvider securityProvider)
    {

      // new client
      RestDataProvider provider = new RestDataProvider(xml, securityProvider);

      if (provider == null)
        throw new OutOfMemoryException("Failed to allocate RestDataProvider");


      return provider;

    } // Create()

    #endregion

    #region IDisposable Members

    /// <summary>
    /// Dispose
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
          m_client = null;
        }

        m_disposed = true;
      }

    } // Dispose( disposing )

    #endregion


    #region IDataProvider Methods


    #region Domain

    /// <summary>
    /// Gets the domain we are on
    /// </summary>
    /// <returns></returns>
    public string GetDomain()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("domain"));

      string xml = response.Message;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return doc.DocumentElement.Attributes["name"].Value;

    } // GetDomain()

    #endregion

    #region Pacs Queue


    /// <summary>
    /// Gets the list of pacs jobs
    /// </summary>
    /// <param name="query"></param>
    /// <returns></returns>
    public  List<PacsJob> GetPacsJobs(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<PacsJob> jobs = new List<PacsJob>();

      XmlBuilder qry = (query == null ? XmlBuilder.Create("pacsjob") : query);

      ResponseMessage response = m_client.Get(m_securityProvider.Context, qry);

      EntityCollection.Load(jobs, response.Message);


      return jobs;

    } // GetPacsJobs( XmlBuilder query = null)

    /// <summary>
    /// Delete the pacs job specified
    /// </summary>
    /// <param name="job"></param>
    public  void Delete(PacsJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create(job));
    } // DeletePacsJob( PacsJob job)

    /// <summary>
    /// Creates the pacs job 
    /// </summary>
    /// <param name="job"></param>
    /// <returns></returns>
    public  void Create(PacsJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Insert(m_securityProvider.Context, XmlBuilder.Create(job));
    }


    #endregion

    #region Processing

    #region Template methods

    /// <summary>
    /// Gets the site
    /// </summary>
    /// <returns></returns>
    public  List<Template> GetTemplates(string application, string modality)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Template> templates = new List<Template>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("template", new Dictionary<string, string>() { { "app", application }, { "mod", modality } }));

      EntityCollection.Load(templates, response.Message);

      return templates;

    } // GetTemplates( string application, string modality)

    /// <summary>
    /// Gets default template
    /// </summary>
    /// <returns></returns>
    public  Template GetDefaultTemplate()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Template> templates = new List<Template>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("template", new Dictionary<string, string>() { { "default", "true" } }));

      EntityCollection.Load(templates, response.Message);


      if (templates.Count == 0)
        throw new NotFoundException("No default template found");

      return templates[0];

    } // GetTemplates( string application, string modality)

    #endregion


    #region Job methods

    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <returns></returns>
    public  List<Job> GetPendingJobs()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      XmlBuilder query = new XmlBuilder("job");
      query.AddAttribute("state", ((int)(States.Pending | States.Receiving)).ToString());

      return GetJobs(query);
    } // GetPendingJobs()


    /// <summary>
    /// Gets jobs
    /// </summary>
    /// <returns></returns>
    public  List<Job> GetJobs(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Job> jobs = new List<Job>();


      ResponseMessage response = m_client.Get(m_securityProvider.Context, (query == null ? XmlBuilder.Create("job") : query));

      EntityCollection.Load(jobs, response.Message);

      return jobs;

    } // GetJobs( XmlBuilder query)


    /// <summary>
    /// Deletes the specified job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to be deleted</param>
    public  void Delete(Job job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("job", (new Dictionary<string, string>() { { "id", job.Id } })));

    } // DeleteJob( Job job)

    /// <summary>
    /// Starts the specified job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to be deleted</param>
    public  void Start(Job job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Update(m_securityProvider.Context, XmlBuilder.Create(job));
    } // Start( Job job)

    /// <summary>
    /// Creates the job for the series using the template supplied
    /// </summary>
    /// <param name="series">Source series</param>
    /// <param name="template">processing template</param>
    /// <param name="state">Initial state</param>
    public  void Create(Series series, Template template, States state = States.Pending)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Insert(m_securityProvider.Context, XmlBuilder.Create("job", new Dictionary<string, string>() { { "uid", series.Uid }, { "tid", template.Id }, { "state", ((int)state).ToString() } } ));
    } // CreatJob();

    /// <summary>
    /// Cancels the job
    /// </summary>
    /// <param name="job"></param>
    public  void Cancel(Job job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      XmlBuilder builder = new XmlBuilder("job");
      builder.AddAttribute("id", job.Id);
      builder.AddAttribute("state", ((int)States.Aborted).ToString());

      m_client.Update(m_securityProvider.Context, builder);

    }


    #endregion

    #endregion

    #region DicomHost methods

    /// <summary>
    /// Gets the dicom hosts.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The list of dicom hosts.</returns>
    public List<DicomHost> GetDicomHosts(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<DicomHost> hosts = new List<DicomHost>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, (query == null ? XmlBuilder.Create("DicomHost") : query));

      EntityCollection.Load(hosts, response.Message);

      return hosts;
    } // GetDicomHosts( query )


    /// <summary>
    /// Inserts a new dicom host.
    /// </summary>
    /// <param name="dicomHost">The query.</param>
    public void InsertDicomHost(DicomHost dicomHost)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Insert(m_securityProvider.Context, XmlBuilder.Create(dicomHost));
    } // InsertDicomHost( dicomHost )


    /// <summary>
    /// Updates an existing dicom host.
    /// </summary>
    /// <param name="dicomHost">The query.</param>
    public void UpdateDicomHost(DicomHost dicomHost)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Update(m_securityProvider.Context, XmlBuilder.Create(dicomHost));
    } // UpdateDicomHost( dicomHost )


    /// <summary>
    /// Deletes the selected dicom host.
    /// </summary>
    /// <param name="dicomHost"></param>
    public void DeleteDicomHost(DicomHost dicomHost)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create(dicomHost));
    } // DeleteDicomHost( dicomHost )

    #endregion

    #region SystemSetting methods

    /// <summary>
    /// Gets the system settings.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>Gets the system settings.</returns>
    public List<SystemSetting> GetSystemSettings(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<SystemSetting> settings = new List<SystemSetting>();
      ResponseMessage response = m_client.Get(m_securityProvider.Context, (query == null ? XmlBuilder.Create("SystemSetting") : query));
      EntityCollection.Load(settings, response.Message);
      return settings;
    } // GetSystemSettings( query )

    #endregion

    #region Message methods

    /// <summary>
    /// Gets all messages
    /// </summary>
    /// <returns></returns>
    public  List<Message> GetMessages(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Message> messages = new List<Message>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, (query == null ? XmlBuilder.Create("message") : query));

      EntityCollection.Load(messages, response.Message);

      return messages;
    }

    /// <summary>
    /// Gets all error messages
    /// </summary>
    /// <returns></returns>
    public  List<Message> GetErrorMessages()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      return GetMessages(XmlBuilder.Create("message", (new Dictionary<string, string>() { { "type", "Error" } })));
    }

    /// <summary>
    /// Deletes the message matching the constraints that are in the xml builder
    /// </summary>
    /// <param name="builder"></param>
    public  void DeleteMessages(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, query);
    }

    #endregion

    #region Get methods

    /// <summary>
    /// Returns a list of dicom hosts defined in the rest service. Just the code and the description
    /// </summary>
    /// <returns>List of modalities</returns>
    public List<DicomHost> GetDicomHosts(DicomServiceTypes type = DicomServiceTypes.All)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<DicomHost> hosts = new List<DicomHost>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("dicomhost", new Dictionary<string, string> { { "services", ((int)type).ToString() } }));

      // Check response for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      EntityCollection.Load(hosts, response.Message);

      XmlDocument doc = new XmlDocument();
 
      return hosts;

    } // GetDicomHosts()

    /// <summary>
    /// Returns a list of modalities defined in the rest service. Just the code and the description
    /// </summary>
    /// <returns>List of modalities</returns>
    public List<Modality> GetModalities()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Modality> modalities = new List<Modality>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("modality"));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);


      EntityCollection.Load(modalities, response.Message);

      return modalities;

    }

    /// <summary>
    /// Gets the receiving entries
    /// </summary>
    /// <param name="query"></param>
    /// <returns></returns>
    public  List<Receiver> GetReceivers(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Receiver> receivers = new List<Receiver>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, (query == null ? XmlBuilder.Create("receiver") : query));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);


      EntityCollection.Load(receivers, response.Message);

      return receivers;
    } // 


    /// <summary>
    /// Gets the current date and time from the server
    /// </summary>
    /// <returns></returns>
    public  DateTimeOffset GetDate()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("date"));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(response.Message);

      return DateTimeOffset.Parse(doc.DocumentElement.Attributes["current"].Value, CultureInfo.InvariantCulture);
    } // GetDate()

    /// <summary>
    /// Gets the study groups 
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <returns>List of study groups</returns>
    public  List<StudyGroup> GetStudyGroups()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<StudyGroup> groups = new List<StudyGroup>();


      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("studygroup"));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      EntityCollection.Load(groups, response.Message);

      return groups;
    }

    /// <summary>
    /// Gets the list of dicom servers
    /// </summary>
    /// <returns></returns>
    public  List<DicomServer> GetDicomServers()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<DicomServer> servers = new List<DicomServer>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("dicomserver"));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      EntityCollection.Load(servers, response.Message);

      return servers;


    } // GetDicomServers()


    /// <summary>
    /// Gets the site
    /// </summary>
    /// <returns></returns>
    public  Site GetSite()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      List<Site> sites = new List<Site>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("site"));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      EntityCollection.Load(sites, response.Message);

      return ( sites.Count == 0 ? null : sites[0] );

    }

    /// <summary>
    /// Gets the preview image for the series
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    public  Image GetPreviewImage(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      XmlBuilder query = new XmlBuilder("preview");
      query.AddAttribute("uid", series.Uid);


      ResponseMessage response = m_client.Get(m_securityProvider.Context, query);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      byte[] buffer = Convert.FromBase64String(response.Message);

      using (System.IO.MemoryStream stream = new System.IO.MemoryStream(buffer))
      {
        return Bitmap.FromStream(stream);
      }


    } // GetPreviewImage( Series series)


    /// <summary>
    /// Gets the dicom header for the series udi specified
    /// </summary>
    /// <param name="context">Authentication token</param>
    /// <param name="uid">Series uid</param>
    /// <returns></returns>
    public  string GetDicomHeader(string uid)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("dicomheader", new Dictionary<string, string>() { { "uid", uid } }));

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      return response.Message;
    } // GetDicomHeader( string uid)


    /// <summary>
    /// Gets the list of installed allplications
    /// </summary>
    /// <param name="context">Context</param>
    /// <returns></returns>
    public List<Application> GetApplications(bool installedOnly = true)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Application> applications = new List<Application>();

      XmlBuilder builder = (installedOnly ? XmlBuilder.Create("application", "<application inst=\"true\" />") : XmlBuilder.Create("application"));

      ResponseMessage response = m_client.Get(m_securityProvider.Context, builder);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      EntityCollection.Load(applications, response.Message);

      return applications;

    } // 

    /// <summary>
    /// Gets the list of installed applications associated with the series list
    /// </summary>
    /// <param name="series">Series list</param>
    /// <returns></returns>
    public List<Application> GetApplications(List<Series> series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Application> applications = new List<Application>();

      if ( series.Count > 0 )
      { 
        XmlBuilder builder = XmlBuilder.Create("application", new Dictionary<string, string>() { { "uids", series.GetUids() } });

        ResponseMessage response = m_client.Get(m_securityProvider.Context, builder);

        // Check response for error
        if (response.Failed)
          throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

        EntityCollection.Load(applications, response.Message);
      }

      return applications;

    } // 



    /// <summary>
    /// Gets the launch list
    /// </summary>
    /// <param name="aplication">Application</param>
    /// <param name="uids">Uid list</param>
    /// <returns></returns>
    public string GetLaunchList(Applications application, string uids)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      XmlBuilder builder = XmlBuilder.Create("launchlist");
      builder.AddAttribute<string>("uids", uids);
      builder.AddAttribute<string>("app", application.ToString());
      builder.AddAttribute<string>("sys", System.Environment.MachineName);
      builder.AddAttribute<string>("login", m_securityProvider.User.Login);


      ResponseMessage response = m_client.Get(m_securityProvider.Context, builder);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      return response.Message;

    } //  GetLaunchList(Applications application, string uids)

    /// <summary>
    /// Gets the transfer list
    /// </summary>
    /// <param name="uids">Uid list</param>
    /// <returns></returns>
    public TransferList GetTransferList(string uids)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      XmlBuilder builder = XmlBuilder.Create("transferlist");
      builder.AddAttribute<string>("uids", uids);
      builder.AddAttribute<string>("sys", System.Environment.MachineName);
      builder.AddAttribute<string>("login", m_securityProvider.User.Login);


      ResponseMessage response = m_client.Get(m_securityProvider.Context, builder);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      return TransferList.Create( response.Message.ToXmlNode() );

    } // GetTransferList(string uids)


    #region Study methods

    /// <summary>
    /// Gets the studies using the query supplied. Returns the xml results
    /// </summary>
    /// <param name="query">Query</param>
    /// <returns>Xml</returns>
    private string GetStudiesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      ResponseMessage response = m_client.Get(m_securityProvider.Context, query);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      return response.Message;
    } // GetStudiesInternal(XmlBuilder query)

    /// <summary>
    /// Performs an asynchronous get for the studies
    /// </summary>
    /// <param name="query"></param>
    public void GetStudiesAsynchronously(XmlBuilder query)
    {
      GetAsynchronously(this.GetStudiesInternal, query);
    } // GetStudiesAsynchronously(XmlBuilder query)

    /// <summary>
    /// Gets the studies from the provider
    /// </summary>
    /// <param name="builder">Get</param>
    /// <returns>List<Study></returns>
    public  List<Study> GetStudies(XmlBuilder query)
    {

      List<Study> studies = new List<Study>();

      try
      {

        string xml = GetStudiesInternal(query);

        EntityCollection.Load(studies, xml);

        FireCompletedEvent("study", "get", xml);



        return studies;
      }
      catch (Exception ex)
      {
        FireFailedEvent(query.Resource, "get", ex.Message);
        throw;
      }

    } // GetStudies( XmlBuilder builder)

    /// <summary>
    /// Gets the studies from the provider
    /// </summary>
    /// <param name="builder">Get</param>
    /// <returns>List<Study></returns>
    public List<Study> GetStudiesAsync(XmlBuilder query)
    {

      List<Study> studies = new List<Study>();

      try
      {

        string xml = GetStudiesInternal(query);

        EntityCollection.Load(studies, xml);

        FireCompletedEvent("study", "get", xml);



        return studies;
      }
      catch (Exception ex)
      {
        FireFailedEvent(query.Resource, "get", ex.Message);
        throw;
      }

    } // GetStudies( XmlBuilder builder)


    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name="uid"></param>
    /// <returns></returns>
    public  Study GetStudy(string uid)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      string xml = string.Empty;

      try
      {
        List<Study> studies = new List<Study>();


        ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("study", new Dictionary<string, string>() { { "uid", uid } }));

        EntityCollection.Load(studies, response.Message);

        if (studies.Count == 0)
          throw new NotFoundException("Study " + uid + " was not found");

        return studies[0];

      }
      catch (Exception)
      {
        FireFailedEvent("study", "get", xml);
        throw;
      }
    } //  GetStudy( Uid uid)


    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    public void GetStudy(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {



        ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("study", new Dictionary<string, string>() { { "uid", study.Uid } }));

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(response.Message);

        XmlNodeList list = doc.DocumentElement.SelectNodes("study");

        if (list.Count == 0)
          throw new NotFoundException("Study " + study.Uid + " was not found");


        study.Load(list[0]);

      }
      catch (Exception ex)
      {
        FireFailedEvent("study", "get", ex.Message);

        throw;
      }
    } // GetSeries( Uid uid)

    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public  Study GetStudy(Series series)
    {
      return GetStudy(series.StudyUid);
    } // GetStudy( Series series)

    #endregion


    #region Series methods

    /// <summary>
    /// Gets the series path
    /// </summary>
    /// <param name="series"></param>
    /// <param name="location"></param>
    /// <returns></returns>
    public  string GetSeriesPath(Series series, Location location = Location.Remote)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      XmlBuilder parameters = XmlBuilder.Create("seriespath");
      parameters.AddAttribute("uid", series.Uid);
      parameters.AddAttribute("loc", location.ToString().ToLower());

      ResponseMessage response = m_client.Get(m_securityProvider.Context, parameters);

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(response.Message);

      series.Path = doc.DocumentElement.Attributes["path"].Value;

      return series.Path;

    } // GetSeriesPath( Series series, string location)


    /// <summary>
    /// Gets the files for the series specified
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="series">Series to get file list for</param>
    /// <returns>File list</returns>
    public List<string> GetFiles(Series series, Location location = Location.Remote)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      ResponseMessage response = null;

      if (series.Path.Length == 0)
      {
        XmlBuilder parameters = XmlBuilder.Create("seriespath");
        parameters.AddAttribute("uid", series.Uid);
        parameters.AddAttribute("loc", location.ToString().ToLower());

        response = m_client.Get(m_securityProvider.Context, parameters);

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(response.Message);

        series.Path = doc.DocumentElement.Attributes["path"].Value;
      }

      series.Files.Clear();


      response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("filelist", (new Dictionary<string, string>() { { "path", series.Path } })));


      XmlDocument filesDoc = new XmlDocument();
      filesDoc.LoadXml(response.Message);

      // Get the root path, which is the series path
      string path = filesDoc.DocumentElement.Attributes["path"].Value;

      // =========================================================
      // Loop through all the files and add the file to the list
      // =========================================================
      foreach (XmlNode node in filesDoc.DocumentElement.SelectNodes("file"))
      {
        series.Files.Add(node.Attributes["name"].Value);
      }

      return series.Files;

    } // GetFiles( Series series)

    /// <summary>
    /// Gets the reference series associated with the study specified
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public  List<Series> GetReferences(Series series)
    {

      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");
      try
      {

        XmlBuilder parameters = XmlBuilder.Create("reference");
        parameters.AddAttribute("uid", series.Uid);


        series.References.Clear();

        ResponseMessage response = m_client.Get(m_securityProvider.Context, parameters);

        EntityCollection.Load(series.References, response.Message);


        return series.References;

      }
      catch (Exception ex)
      {
        FireFailedEvent("reference", "get", ex.Message);
        throw;
      }
    } // GetReferences( Series series)

    /// <summary>
    /// Gets the dependent series associated with the study specified
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public  List<Series> GetDependents(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {

        XmlBuilder parameters = XmlBuilder.Create("dependent");
        parameters.AddAttribute("uid", series.Uid);


        series.Dependents.Clear();

        ResponseMessage response = m_client.Get(m_securityProvider.Context, parameters);

        EntityCollection.Load(series.Dependents, response.Message);


        return series.Dependents;

      }
      catch (Exception ex)
      {
        FireFailedEvent("dependent", "get", ex.Message);
        throw;
      }
    } // GetDependents( Series series)


    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    private string GetSeriesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      ResponseMessage response = m_client.Get(m_securityProvider.Context, query);

      // Check reponse for error
      if (response.Failed)
        throw new ViatronixException(response.Message, (ErrorCodes)response.ErrorCode);

      return response.Message;

    } // GetSeriesInternal(XmlBuilder query)


    /// <summary>
    /// Gets the series for the specified study asynchronously
    /// </summary>
    /// <param name="study">Study to get series for</param>
    public void GetSeriesAsynchronously(Study study)
    {
      GetAsynchronously(this.GetSeriesInternal, XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", study.Uid } }));
    } // GetSeriesAsynchrnously(Study study)

    /// <summary>
    /// Gets the series for the specified studies asynchronously
    /// </summary>
    /// <param name="studies">Studies to get series for</param>
    public void GetSeriesAsynchronously(List<Study> studies)
    {
      GetAsynchronously(this.GetSeriesInternal, XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", studies.GetUids() } }));
    } // GetSeriesAsynchrnously(List<Study> studies)

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public  List<Series> GetSeries(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {

        string xml = GetSeriesInternal( XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", study.Uid } }));

        // Clear the original list
        study.Series.Clear();

        EntityCollection.Load(study.Series, xml);

        FireCompletedEvent("series", "get", xml);

        return study.Series;
      }
      catch (Exception ex)
      {
        FireFailedEvent("series", "get", ex.Message);
        throw;
      }
    } // GetSeries( Study study)

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public List<Series> GetSeries(List<Study> studies)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      List<Series> series = new List<Series>();

      try
      {

        ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", studies.GetUids()} }));

        EntityCollection.Load(series, response.Message);

        FireCompletedEvent("series", "get", response.Message);

        return series;
      }
      catch (Exception ex)
      {
        FireFailedEvent("series", "get", ex.Message);
        throw;
      }
    } // GetSeries( Study study)

    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    public  Series GetSeries(string uid)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {

        List<Series> series = new List<Series>();


        ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("series", new Dictionary<string, string>() { { "uid", uid } }));

        EntityCollection.Load(series, response.Message);


        if (series.Count == 0)
          throw new NotFoundException("Series " + uid + " was not found");


        return series[0];

      }
      catch (Exception ex)
      {
        FireFailedEvent("series", "get", ex.Message);

        throw;
      }
    } // GetSeries( Uid uid)


    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    public void GetSeries(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {



        ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("series", new Dictionary<string, string>() { { "uid", series.Uid } }));

        XmlDocument doc = new XmlDocument();
        
        doc.LoadXml(response.Message);

        XmlNodeList list = doc.DocumentElement.SelectNodes("series");

        if ( list.Count == 0)
          throw new NotFoundException("Series " + series.Uid + " was not found");


        series.Load(list[0]);

      }
      catch (Exception ex)
      {
        FireFailedEvent("series", "get", ex.Message);

        throw;
      }
    } // GetSeries( Uid uid)

    #endregion

    #endregion

    #region Delete methods

    /// <summary>
    /// Delete the specified series
    /// </summary>
    /// <param name="series"></param>
    public  void Delete(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("series", (new Dictionary<string, string>() { { "uid", series.Uid } })));
    } // Delete( Series series)

    /// <summary>
    /// Delete all series in thes list
    /// </summary>
    /// <param name="series">Series list</param>
    public void Delete(List<Series> series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("series", (new Dictionary<string, string>() { { "uid", series.GetUids() } })));
    } // Delete( Series series)


    /// <summary>
    /// Delete the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Delete(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("study", (new Dictionary<string, string>() { { "uid", study.Uid } })));
    } // Delete( Study study)

    /// <summary>
    /// Delete the list of studies
    /// </summary>
    /// <param name="studies">Studies list</param>
    public void Delete(List<Study> studies)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("study", (new Dictionary<string, string>() { { "uid", studies.GetUids() } })));
    } // Delete( Study study)

    #endregion

    #region Update methods

    /// <summary>
    /// Updates the Template
    /// </summary>
    /// <param name="template"></param>
    public  void SetAsDefault(Template template)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      template.Default = true;
      m_client.Update(m_securityProvider.Context, XmlBuilder.Create(template));
    }

    /// <summary>
    /// Update the specified series
    /// </summary>
    /// <param name="series"></param>
    public  void Update(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Update(m_securityProvider.Context, XmlBuilder.Create(series));
    } // Update( Series series)

    /// <summary>
    /// Update the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Update(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Update(m_securityProvider.Context, XmlBuilder.Create(study));
    } // Update( Study study)


    #endregion


    #region Insert methods

    /// <summary>
    /// Add the specified series
    /// </summary>
    /// <param name="series"></param>
    public  void Insert(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Insert(m_securityProvider.Context, XmlBuilder.Create(series));
    }

    /// <summary>
    /// Insert the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Insert(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Insert(m_securityProvider.Context, XmlBuilder.Create(study));
    }


    #endregion

    #region Store methods

    /// <summary>
    /// Get specified store (Default is the default)
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="name">STore name</param>
    /// <returns>Store</returns>
    public  Store GetStore(string name = "Default")
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Store> stores = new List<Store>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("store", new Dictionary<string, string>() { { "name", name } }));

      EntityCollection.Load(stores, response.Message);

      if (stores.Count == 0)
        throw new NotFoundException("Store " + name + " not found");

      return stores[0];

    } //  GetStore( string name = "Default")

    /// <summary>
    /// Get all stores
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <returns>List or stores</returns>
    public  List<Store> GetStores()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Store> stores = new List<Store>();

      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("store"));

      EntityCollection.Load(stores, response.Message);

      return stores;

    } //  GetStore( string name = "Default")


    #endregion


    #region Lock methods



    /// <summary>
    /// Delets all the locks associated iwth the specified transaction
    /// </summary>
    /// <param name="transactionId">transaction id</param>
    public  void DeleteLocks(string transactionId)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Delete(m_securityProvider.Context, XmlBuilder.Create("lock", new Dictionary<string, string>() { { "tid", transactionId } }));

    } // DeleteLock( Series series)
    #endregion



    #region misc

    /// <summary>
    /// Gets auto processing
    /// </summary>
    /// <returns></returns>
    public  bool GetAutoProcessing()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      bool auto = false;


      XmlDocument doc = new XmlDocument();


      ResponseMessage response = m_client.Get(m_securityProvider.Context, XmlBuilder.Create("systemsetting", new Dictionary<string, string>() { { Setting.Attributes.Subsystem, "Processing" }, { Setting.Attributes.Name, "Auto" } }));

      doc.LoadXml(response.Message);

      doc.DocumentElement.TryGetAttributeValue<bool>(Setting.Attributes.Value, ref auto);


      return auto;

    }

    /// <summary>
    /// Sets auto processing
    /// </summary>
    /// <param name="auto"></param>
    public  void SetAutoProcessing(bool auto)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      m_client.Update(m_securityProvider.Context, XmlBuilder.Create("systemsetting",
                                         new Dictionary<string, string>() { { Setting.Attributes.Subsystem, "Processing" }, 
                                                                            { Setting.Attributes.Name, "Auto" }, 
                                                                            { Setting.Attributes.Value, auto.ToString() } }));
    } // SetAutoProcessing()

    #endregion



    #endregion



 

  } // class RestDataProvider

} // namespace Viatronix
