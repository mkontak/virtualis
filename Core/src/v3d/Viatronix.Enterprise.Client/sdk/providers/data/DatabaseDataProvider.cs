using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.Security.Principal;
using System.Runtime.InteropServices;
using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;
using System.Globalization;
using System.IO;

namespace Viatronix.Enterprise.SDK.Providers
{
  /// <summary>
  /// Database Provider 
  /// </summary>
  public class DatabaseDataProvider : AsynchronousProviderBase, IAsynchronousDataProvider, IDisposable
  {
    
    #region fields


    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Security provider
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    #endregion

    #region constructors


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="xml"></param>
    public DatabaseDataProvider(string xml, ISecurityProvider securityProvider)
      : base(xml)
    {

      if (securityProvider == null)
        throw new Exception("No security provider specified");

      m_securityProvider = securityProvider;
    }

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
    { get { return SystemGateway.IsAvailable() && StorageGateway.IsAvailable() && SecurityGateway.IsAvailable(); }  }

    /// <summary>
    /// Returns the authenticated flag
    /// </summary>
    public override bool IsAuthenticated
    { get { return (m_securityProvider != null && m_securityProvider.IsAuthenticated); } }

    #endregion
      
    #region IDataProvider Methods

    #region Domain

    /// <summary>
    /// Gets the domain we are on
    /// </summary>
    /// <returns></returns>
    public string GetDomain()
    {
      return string.Empty;
      // TODO
      //return Viatronix.Enterprise.Providers.SystemResourceProvider.GetDomain();
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
        new NotAuthorizedException("The get pacs jobs operation requires a login");

      List<PacsJob> jobs = new List<PacsJob>();

      EntityCollection.Load(jobs, SystemGateway.Get(m_securityProvider.Context, "pacsjob", (query == null ? XmlBuilder.Create("pacsjob") : query)));


      return jobs;

    } // GetPacsJobs( XmlBuilder query = null)

    /// <summary>
    /// Delete the pacs job specified
    /// </summary>
    /// <param name="job"></param>
    public  void Delete(PacsJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The delete pacs job operation requires a login");

      StorageGateway.Delete(m_securityProvider.Context, "pacsjob", job);
    } // DeletePacsJob( PacsJob job)

    /// <summary>
    /// Creates the pacs job 
    /// </summary>
    /// <param name="job"></param>
    /// <returns></returns>
    public  void Create(PacsJob job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The create pacs job operation requires a login");

      StorageGateway.Insert(m_securityProvider.Context, "pascjob", job);
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
        new NotAuthorizedException("The get templates operation requires a login");


      List<Template> templates = new List<Template>();

      EntityCollection.Load(
        templates,
        SystemGateway.Get(m_securityProvider.Context, "template", XmlBuilder.Create("template", new Dictionary<string, string>() { { "app", application }, { "mod", modality } })));

      return templates;

    } // GetTemplates( string application, string modality)

    /// <summary>
    /// Gets default template
    /// </summary>
    /// <returns></returns>
    public  Template GetDefaultTemplate()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The get default template operation requires a login");


      List<Template> templates = new List<Template>();

      EntityCollection.Load(templates, SystemGateway.Get(m_securityProvider.Context, "template", XmlBuilder.Create("template", new Dictionary<string, string>() { { "default", "true" } })));


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
        new NotAuthorizedException("The get pending jobs operation requires a login");


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
        new NotAuthorizedException("The get jobs operation requires a login");


      List<Job> jobs = new List<Job>();

      EntityCollection.Load(jobs, StorageGateway.Get(m_securityProvider.Context, "job", (query == null ? XmlBuilder.Create("job") : query)));

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
        new NotAuthorizedException("The delete job operation requires a login");


      StorageGateway.Delete(m_securityProvider.Context, "job", XmlBuilder.Create("job", (new Dictionary<string, string>() { { "id", job.Id } })));

    } // DeleteJob( Job job)

    /// <summary>
    /// Starts the specified job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to be deleted</param>
    public  void Start(Job job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The start job operation requires a login");

      /// Sets the state to pending
      job.State = States.Pending;

      StorageGateway.Update(m_securityProvider.Context, "job", job);
    } // Start( Job job)

    /// <summary>
    /// Creates the job for the series using the template supplied
    /// </summary>
    /// <param name="series">Source series</param>
    /// <param name="template">Processing template</param>
    /// <param name="state">Initial state of the job (Only Pending or Paused are valid all others are ignored)</param>
    public  void Create(Series series, Template template, States state = States.Pending)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The create job operation requires a login");

      StorageGateway.Insert(m_securityProvider.Context, "job", XmlBuilder.Create("job", new Dictionary<string, string>() { { "uid", series.Uid }, { "tid", template.Id }, { "state", ((int)state).ToString() } }));
    } // CreatJob();

    /// <summary>
    /// Cancels the job
    /// </summary>
    /// <param name="job"></param>
    public  void Cancel(Job job)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The cancel job operation requires a login");

      XmlBuilder builder = new XmlBuilder("job");
      builder.AddAttribute("id", job.Id);
      builder.AddAttribute("state", ((int)States.Aborted).ToString());

      StorageGateway.Update(m_securityProvider.Context, "job", builder);

    }


    #endregion

    #endregion

    #region DicomHost methods

    /// <summary>
    /// Gets the dicom hosts.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The dicom hosts.</returns>
    public List<DicomHost> GetDicomHosts(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The get pacs jobs operation requires a login");

      List<DicomHost> dicomhosts = new List<DicomHost>();
      EntityCollection.Load(dicomhosts, SystemGateway.Get(m_securityProvider.Context, "DicomHost", query != null ? query : XmlBuilder.Create("dicomhost")));

      return dicomhosts;
    } // GetDicomHosts( query )


    /// <summary>
    /// Inserts the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    public void InsertDicomHost(DicomHost host)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The get pacs jobs operation requires a login");

      SystemGateway.Insert(m_securityProvider.Context, "DicomHost", host);
    } // InsertDicomHost( host )


    /// <summary>
    /// Updates the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    public void UpdateDicomHost(DicomHost host)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The get pacs jobs operation requires a login");

      SystemGateway.Update(m_securityProvider.Context, "DicomHost", host);
    } // UpdateDicomHost( host )


    /// <summary>
    /// Deletes the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    public void DeleteDicomHost(DicomHost host)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("The get pacs jobs operation requires a login");

      SystemGateway.Delete(m_securityProvider.Context, "DicomHost", host);
    } // DeleteDicomHost( host )

    #endregion

    #region SystemSettings methods

    /// <summary>
    /// Gets the system settings.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The system settings.</returns>
    public List<SystemSetting> GetSystemSettings(XmlBuilder query = null)
    {
      if (!IsAuthenticated)
        throw new NotAuthorizedException("This operation requires a login");

      string results = SystemGateway.Get(m_securityProvider.Context, "SystemSetting", query != null ? query : XmlBuilder.Create("systemsetting"));
        
      List<SystemSetting> settings = new List<SystemSetting>();
      EntityCollection.Load(settings, results);

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

      EntityCollection.Load(messages, StorageGateway.Get(m_securityProvider.Context, "message", (query == null ? XmlBuilder.Create("message") : query)));

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

      StorageGateway.Delete(m_securityProvider.Context, "message", query);
    }

    #endregion
    
    #region Get methods

    /// <summary>
    /// Returns a list of modalities defined in the database. Just the code and the description
    /// </summary>
    /// <returns>List of modalities</returns>
    public List<Modality> GetModalities()
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Modality> modalities = new List<Modality>();

      EntityCollection.Load(modalities, SystemGateway.Get(m_securityProvider.Context, "modality", XmlBuilder.Create("modality")));

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

      EntityCollection.Load(receivers, StorageGateway.Get(m_securityProvider.Context, "receiver", (query == null ? XmlBuilder.Create("receiver") : query)));

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

      return DateTimeOffset.Now;
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

      EntityCollection.Load(groups, SystemGateway.Get(m_securityProvider.Context, "studygroup", XmlBuilder.Create("studygroup")));

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

      EntityCollection.Load(servers, StorageGateway.Get(m_securityProvider.Context, "dicomserver", XmlBuilder.Create("dicomserver")));

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

      EntityCollection.Load(sites, SystemGateway.Get(m_securityProvider.Context, "site", XmlBuilder.Create("site")));


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

      string uid = series.Uid;

      string filename = (string.IsNullOrEmpty(uid) ? "preview.png" : uid.Replace(".", "_") + ".png");
      string path = StorageGateway.GetSeriesPath(SecurityProvider.Context, uid, "local");

      // If the is no path we are done
      if (path.Length == 0)
        throw new InvalidDataException("No path specified, cannot get the preview");


      string imageFile = System.IO.Path.Combine(path, filename);

      // ========================================================================
      // If the file does not exist then check a file with the name preview.png
      // ========================================================================
      if (!System.IO.File.Exists(imageFile))
        throw new FileNotFoundException("No preview file was found");


      return Image.FromFile(imageFile);

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

      return StorageGateway.Get(m_securityProvider.Context, "dicomheader", XmlBuilder.Create("dicomheader", new Dictionary<string, string>() { { "uid", uid } }));
    } // GetDicomHeader( string uid)


    /// <summary>
    /// Gets the list of installed applications
    /// </summary>
    /// <param name="context">Context</param>
    /// <returns></returns>
    public List<Application> GetApplications(bool installedOnly = true)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Application> applications = new List<Application>();

      XmlBuilder builder = ( installedOnly ? XmlBuilder.Create("application", "<application inst=\"true\" />") : XmlBuilder.Create("application") );

      EntityCollection.Load(applications, SystemGateway.Get(m_securityProvider.Context, "application", builder ));

      return applications;

    } // 

    /// <summary>
    /// Gets the list of installed applications associated with the series specified
    /// </summary>
    /// <param name="context">Context</param>
    /// <returns></returns>
    public List<Application> GetApplications(List<Series> series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      List<Application> applications = new List<Application>();

      if ( series.Count > 0 )
      {
        XmlBuilder builder = XmlBuilder.Create("application", new Dictionary<string, string>() { { "uids", series.GetUids() } });

        EntityCollection.Load(applications, SystemGateway.Get(m_securityProvider.Context, "application", builder));



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
      builder.AddAttribute<string>("uids",uids);
      builder.AddAttribute<string>("app",application.ToString());
      builder.AddAttribute<string>("sys",System.Environment.MachineName);
      builder.AddAttribute<string>("login", m_securityProvider.User.Login);


      return StorageGateway.Get(m_securityProvider.Context, "launchlist", builder);


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


      return TransferList.Create(StorageGateway.Get(m_securityProvider.Context, "transferlist", builder).ToXmlNode());


    } // GetTransferList(string uids)


    #region Study methods

    /// <summary>
    /// Gets the studies using the query supplied. This is the call through for all GetStudies() methods.
    /// </summary>
    /// <param name="query">Query parameters</param>
    /// <returns>Xml results</returns>
    private string GetStudiesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      try
      {

        return StorageGateway.Get(m_securityProvider.Context, "study", query);

      }
      catch (Exception)
      {
        throw;
      }

    }

    /// <summary>
    /// Gets the studies from the provider ayncrounously
    /// </summary>
    /// <param name="builder">Get</param>
    public void GetStudiesAsynchronously(XmlBuilder query)
    {

      GetAsynchronously(this.GetStudiesInternal, query);

    } // GetStudies( XmlBuilder builder)


    /// <summary>
    /// Gets the studies from the provider
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="builder">Get</param>
    /// <returns>List<Study></returns>
    public  List<Study> GetStudies(XmlBuilder query)
    {
      List<Study> studies = new List<Study>();

      try
      {

        string xml = GetStudiesInternal(query);

        FireCompletedEvent("study", "get", xml);


        EntityCollection.Load(studies, xml);

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
    /// <param name="context">Authentication context/token</param>
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


        EntityCollection.Load(studies, StorageGateway.Get(m_securityProvider.Context, "study", XmlBuilder.Create("study", new Dictionary<string, string>() { { "uid", uid } })));

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
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public void GetStudy(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      string xml = string.Empty;

      try
      {

        string response = StorageGateway.Get(m_securityProvider.Context, "study", XmlBuilder.Create("study", new Dictionary<string, string>() { { "uid", study.Uid } }));


        XmlDocument doc = new XmlDocument();

        doc.LoadXml(response);

        XmlNodeList list = doc.DocumentElement.SelectNodes("study");

        if (list.Count == 0)
          throw new NotFoundException("Study " + study.Uid + " was not found");


        study.Load(list[0]);


      }
      catch (Exception)
      {
        FireFailedEvent("study", "get", xml);
        throw;
      }
    } //  GetStudy( Uid uid)


    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public  Study GetStudy(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

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
    public  string GetSeriesPath(Series series, Location location)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      XmlBuilder parameters = XmlBuilder.Create("seriespath");
      parameters.AddAttribute("uid", series.Uid);
      parameters.AddAttribute("loc", location.ToString().ToLower());

      string xml = StorageGateway.Get(m_securityProvider.Context, "seriespath", parameters);

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      series.Path = doc.DocumentElement.Attributes["path"].Value;

      return series.Path;

    } // GetSeriesPath( Series series, string location)


    /// <summary>
    /// Gets the files for the series specified
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="series">Series to get file list for</param>
    /// <returns>File list</returns>
    public  List<string> GetFiles(Series series, Location location = Location.Remote)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      string xml = string.Empty;

      if (series.Path.Length == 0)
        series.Path = GetSeriesPath(series, location);


      series.Files.Clear();


      foreach (string file in System.IO.Directory.GetFiles(series.Path))
      {
        series.Files.Add(System.IO.Path.Combine(series.Path, file));
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

        EntityCollection.Load(series.References, StorageGateway.Get(m_securityProvider.Context, "reference", parameters));


        return series.References;

      }
      catch (Exception )
      {
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

        EntityCollection.Load(series.Dependents, StorageGateway.Get(m_securityProvider.Context, "dependent", parameters));


        return series.Dependents;

      }
      catch (Exception )
      {
        throw;
      }
    } // GetDependents( Series series)

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public string GetSeriesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      try
      {

        return StorageGateway.Get(m_securityProvider.Context, query.Resource,  query);
      }
      catch (Exception )
      {
        throw;
      }
    } // GetSeriesInternal( Study study)


    /// <summary>
    /// Gets the series from the provider ayncrounously
    /// </summary>
    /// <param name="builder">Get</param>
    public void GetSeriesAsynchronously(Study study)
    {

      GetAsynchronously(this.GetSeriesInternal, XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", study.Uid } }) );

    } // GetSeriesAsynchronously(Study study)

    /// <summary>
    /// Gets the series from the provider ayncrounously
    /// </summary>
    /// <param name="builder">Get</param>
    public void GetSeriesAsynchronously(List<Study> studies)
    {

      GetAsynchronously(this.GetSeriesInternal, XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", studies.GetUids() } }) );

    } // GetSeriesAsynchronously(List<Study> studies)

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="context">Authentication context/token</param>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public  List<Series> GetSeries(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      try
      {


        string xml = GetSeriesInternal( XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", study.Uid } }) );

        // Clears the list first
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
    /// <param name="studies">Study</param>
    /// <returns>SeriesCollection</returns>
    public  List<Series> GetSeries(List<Study> studies)
    {

      List<Series> series = new List<Series>();

      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      try
      {

        string xml = StorageGateway.Get(m_securityProvider.Context, "series", XmlBuilder.Create("series", new Dictionary<string, string>() { { "suid", studies.GetUids() } }));

        EntityCollection.Load(series, xml);

        FireCompletedEvent("series", "get", xml);

        return series;
      }
      catch (Exception ex)
      {
        FireFailedEvent("series", "get", ex.Message);
        throw;
      }
    } // GetSeries( List<Study> study)

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

        EntityCollection.Load(series, StorageGateway.Get(m_securityProvider.Context, "series", XmlBuilder.Create("series", new Dictionary<string, string>() { { "uid", uid } })));


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


        string response = StorageGateway.Get(m_securityProvider.Context, "series", XmlBuilder.Create("series", new Dictionary<string, string>() { { "uid", series.Uid } }));


        XmlDocument doc = new XmlDocument();

        doc.LoadXml(response);

        XmlNodeList list = doc.DocumentElement.SelectNodes("series");

        if (list.Count == 0)
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

      StorageGateway.Delete(m_securityProvider.Context, "series", XmlBuilder.Create("series", (new Dictionary<string, string>() { { "uid", series.Uid } })));
    } // Delete( Series series)

    /// <summary>
    /// Delete all the series in the list
    /// </summary>
    /// <param name="series"></param>
    public void Delete(List<Series> series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Delete(m_securityProvider.Context, "series", XmlBuilder.Create("series", (new Dictionary<string, string>() { { "uid", series.GetUids() } })));
    } // Delete( Series series)

    /// <summary>
    /// Delete the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Delete(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Delete(m_securityProvider.Context, "study", XmlBuilder.Create("study", (new Dictionary<string, string>() { { "uid", study.Uid } })));
    } // Delete( Study study)

    /// <summary>
    /// Delete the studies in the list
    /// </summary>
    /// <param name="study"></param>
    public void Delete(List<Study> studies)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");


      StorageGateway.Delete(m_securityProvider.Context, "study", XmlBuilder.Create("study", (new Dictionary<string, string>() { { "uid", studies.GetUids() } })));
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
      StorageGateway.Update(m_securityProvider.Context, "template", template);
    }

    /// <summary>
    /// Update the specified series
    /// </summary>
    /// <param name="series"></param>
    public  void Update(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Update(m_securityProvider.Context, "series", series);
    } // Update( Series series)

    /// <summary>
    /// Update the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Update(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Update(m_securityProvider.Context, "study", study);
    } // Update( Study study)


    #endregion
    
    #region Add methods

    /// <summary>
    /// Insert the specified series
    /// </summary>
    /// <param name="series"></param>
    public  void Insert(Series series)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Insert(m_securityProvider.Context, "series", series);
    }

    /// <summary>
    /// Insert the specified study
    /// </summary>
    /// <param name="study"></param>
    public  void Insert(Study study)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Insert(m_securityProvider.Context, "study", study);
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

      EntityCollection.Load(stores, SystemGateway.Get(m_securityProvider.Context, "store", XmlBuilder.Create("store", new Dictionary<string, string>() { { "name", name } })));

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

      EntityCollection.Load(stores, SystemGateway.Get(m_securityProvider.Context, "store", XmlBuilder.Create("store")));

      return stores;

    } //  GetStore( string name = "Default")


    #endregion
    
    #region Lock methods


    /// <summary>
    /// Delets all the locks associated with the transaction
    /// </summary>
    /// <param name="series"></param>
    public  void DeleteLocks(string transactionId)
    {
      if (!IsAuthenticated)
        new NotAuthorizedException("This operation requires a login");

      StorageGateway.Delete(m_securityProvider.Context, "lock", XmlBuilder.Create("lock", new Dictionary<string, string>() { { "tid", transactionId } }));

    } // DeleteLocks( string transactionId)

 

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

      doc.LoadXml(SystemGateway.Get(m_securityProvider.Context, "systemsetting",
                  XmlBuilder.Create("systemsetting", new Dictionary<string, string>() { { Setting.Attributes.Subsystem, "Processing" }, { Setting.Attributes.Name, "Auto" } })));

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

      SystemGateway.Update(m_securityProvider.Context, "systemsetting",
                                  XmlBuilder.Create("systemsetting", new Dictionary<string, string>() { { Setting.Attributes.Subsystem, "Processing" }, 
                                                                                                  { Setting.Attributes.Name, "Auto" }, 
                                                                                                  { Setting.Attributes.Value, auto.ToString() } }));
    } // SetAutoProcessing()

    #endregion
    
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
  
        m_disposed = true;
      }


    } // Dispose( disposing )

    #endregion

  } // class DatabaseDataProvider
} // namespace Viatronix.Enterprise.SDK.Providers
