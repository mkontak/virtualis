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
  /// <param name="message">Error message if failed/xml if get/empty otherwise</param>
  public delegate void MessageDelegate(string message);


  #endregion

  public interface IDataProvider : IProvider, IBrowserProvider, IDisposable
  {

    #region Message methods

    /// <summary>
    /// Gets all messages
    /// </summary>
    /// <returns></returns>
    List<Message> GetMessages(XmlBuilder query);

    /// <summary>
    /// Gets all error messages
    /// </summary>
    /// <returns></returns>
    List<Message> GetErrorMessages();

    /// <summary>
    /// Deletes the message matching the constraints that are in the xml builder
    /// </summary>
    /// <param name="builder"></param>
    void DeleteMessages(XmlBuilder builder);

    #endregion

    #region Get methods
    
    /// <summary>
    /// Get all modalities from the system
    /// </summary>
    /// <returns>List of modalities</returns>
    List<Modality> GetModalities();

    /// <summary>
    /// Get all stores
    /// </summary>
    /// <returns></returns>
    List<Store> GetStores();

    /// <summary>
    /// Gets a named stored
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    Store GetStore(string name = "Default");



    /// <summary>
    /// Gets the domain we are joined to
    /// </summary>
    /// <returns></returns>
    string GetDomain();


    /// <summary>
    /// Gets the current date and time from the server
    /// </summary>
    /// <returns></returns>
    DateTimeOffset GetDate();

    /// <summary>
    /// Gets the list of dicom server
    /// </summary>
    /// <returns></returns>
    List<DicomServer> GetDicomServers();


    /// <summary>
    /// Gets the list of series begin received
    /// </summary>
    /// <returns></returns>
    List<Receiver> GetReceivers(XmlBuilder query = null);

 
    /// <summary>
    /// Gets the site
    /// </summary>
    /// <returns></returns>
    Site GetSite();

    /// <summary>
    /// Gets the list of installed applications
    /// </summary>
    /// <returns></returns>
    List<Application> GetApplications(bool installedOnly = true);

    /// <summary>
    /// Gets the list of installed applications that are associated with the series specified
    /// </summary>
    /// <returns></returns>
    List<Application> GetApplications(List<Series> series);

    /// <summary>
    /// Gets the preview image for the series
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    Image GetPreviewImage(Series series);

    /// <summary>
    /// Gets the dicom header for the series uid specified
    /// </summary>
    /// <param name="context">Authentication token</param>
    /// <param name="uid">Series uid</param>
    /// <returns>Xml string of empty</returns>
    string GetDicomHeader(string uid);

    ///// <summary>
    ///// Gets the specified setting
    ///// </summary>
    /////// <param name="name"></param>
    ///// <returns></returns>
    //Setting GetSetting(string name);

    ///// <summary>
    ///// Gets the specified settings
    ///// </summary>
    /////// <param name="name"></param>
    ///// <returns></returns>
    //List<Setting> GetSettings(XmlBuilder query);

    /// <summary>
    /// Gets trhe series references
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    List<Series> GetReferences(Series series);


    /// <summary>
    /// Gets the series dependents
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    List<Series> GetDependents(Series series);


    /// <summary>
    /// Gets the series file list
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    List<string> GetFiles(Series series, Location location = Location.Remote );

    /// <summary>
    /// Gets the series path
    /// </summary>
    /// <param name="series"></param>
    /// <param name="location"></param>
    /// <returns></returns>
    string GetSeriesPath(Series series, Location location = Location.Remote);

    /// <summary>
    /// Gets the launcg list for the application and uid list specified
    /// </summary>
    /// <param name="application">Application</param>
    /// <param name="uids">Uid list (delimited by a | )</param>
    /// <returns></returns>
    string GetLaunchList(Applications application, string uids);

    /// <summary>
    /// Gets the transfer list from the udis specified
    /// </summary>
    /// <param name="uids">Uid list (delimited by a | )</param>
    /// <returns></returns>
    TransferList GetTransferList(string uids);

    #endregion

    #region Delete methods

    /// <summary>
    /// Deletes the locks associated with the transaction id
    /// </summary>
    /// <param name="transactionId"></param>
    void DeleteLocks(string transactionId);


    /// <summary>
    /// Delete the specified series
    /// </summary>
    /// <param name="series"></param>
    void Delete(Series series);

    /// <summary>
    /// Deletes all series in the list
    /// </summary>
    /// <param name="series">List of series to be deleted</param>
    void Delete(List<Series> series);

    /// <summary>
    /// Delete the specified study
    /// </summary>
    /// <param name="study"></param>
    void Delete(Study study);

    /// <summary>
    /// Delete all studies in the list
    /// </summary>
    /// <param name="study">Studies to be deleted</param>
    void Delete(List<Study> studies);


    #endregion

    #region Update methods


    /// <summary>
    /// Update the specified series
    /// </summary>
    /// <param name="series"></param>
    void Update(Series series);

    /// <summary>
    /// Update the specified study
    /// </summary>
    /// <param name="study"></param>
    void Update(Study study);

    #endregion

    #region Insert methods


    /// <summary>
    /// Insert the specified series
    /// </summary>
    /// <param name="series"></param>
    void Insert(Series series);

    /// <summary>
    /// Insert the specified study
    /// </summary>
    /// <param name="study"></param>
    void Insert(Study study);

    #endregion

    #region Pacs Queue


    /// <summary>
    /// Gets the list of pacs jobs
    /// </summary>
    /// <param name="query"></param>
    /// <returns></returns>
    List<PacsJob> GetPacsJobs(XmlBuilder query = null);

    /// <summary>
    /// Delete the pacs job specified
    /// </summary>
    /// <param name="job"></param>
    void Delete(PacsJob job);

    /// <summary>
    /// Creates the pacs job 
    /// </summary>
    /// <param name="job"></param>
    /// <returns></returns>
    void Create(PacsJob job);

    #endregion

    #region DicomHosts methods

    /// <summary>
    /// Gets the dicom hosts.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The list of hosts matching the query.</returns>
    List<DicomHost> GetDicomHosts(XmlBuilder query = null);

    /// <summary>
    /// Inserts the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    void InsertDicomHost(DicomHost host);

    /// <summary>
    /// Updates the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    void UpdateDicomHost(DicomHost host);

    /// <summary>
    /// Deletes the dicom host.
    /// </summary>
    /// <param name="host">The host.</param>
    void DeleteDicomHost(DicomHost host);

    #endregion

    #region SystemSettings methods

    /// <summary>
    /// Gets the system settings.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns></returns>
    List<SystemSetting> GetSystemSettings(XmlBuilder query = null);

    #endregion

    #region Processing


    #region Job methods

    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <returns></returns>
    List<Job> GetPendingJobs();

    /// <summary>
    /// Gets jobs
    /// </summary>
    /// <returns></returns>
    List<Job> GetJobs(XmlBuilder query = null);


    /// <summary>
    /// Creates the job for the series using the template 
    /// </summary>
    /// <param name="series">Source series</param>
    /// <param name="template">Processing template</param>
    /// <param name="state">Initial state</param>
    void Create(Series series, Template template, States state = States.Pending);

    /// <summary>
    /// Deletes the specified job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to be deleted</param>
    void Delete(Job job);

    /// <summary>
    /// Starts the specified job
    /// </summary>
    /// <param name="job">Job to be deleted</param>
    void Start(Job job);

    /// <summary>
    /// Cancel the specified job
    /// </summary>
    /// <param name="job"></param>
    void Cancel(Job job);

    #endregion

    #region Template methods

    /// <summary>
    /// Gets the list of templates for the application/modality combo
    /// </summary>
    /// <param name="application"></param>
    /// <param name="modality"></param>
    /// <returns></returns>
    List<Template> GetTemplates(string application, string modality);

    /// <summary>
    /// Gets the list of templates for the application/modality combo
    /// </summary>
    /// <param name="application"></param>
    /// <param name="modality"></param>
    /// <returns></returns>
    Template GetDefaultTemplate();

    /// <summary>
    /// Sets the template specified as the default
    /// </summary>
    /// <param name="template"></param>
    void SetAsDefault(Template template);

    #endregion



    #endregion

    #region misc

    /// <summary>
    /// Gets auto processing
    /// </summary>
    /// <returns></returns>
    bool GetAutoProcessing();

    /// <summary>
    /// Sets auto processing
    /// </summary>
    /// <param name="auto"></param>
    void SetAutoProcessing(bool auto);


    /// <summary>
    /// Gets the list of study groups
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <returns></returns>
    List<StudyGroup> GetStudyGroups();


    #endregion

  } // interface IDataProvider
} // namespace
