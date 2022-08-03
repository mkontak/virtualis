using System;
using System.Collections.Generic;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Text;
using System.Xml;
using System.IO;
using Viatronix.v3D.Core;
using System.ComponentModel;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;
using System.Xml.Linq;

namespace Viatronix.Providers
{
  /// <summary>
  /// Implements the missing functionality that allows access of Study information from a database.
  /// </summary>
  public class DatabaseProvider : ProviderBase
  {



    #region fields


    private string m_contextId = string.Empty;


    private User m_user = null;
  
    #endregion

    #region constructors


 
    /// <summary>
    /// Constructor
    /// </summary>
    public DatabaseProvider(string xml) : base(xml)
    {
      
    }

    #endregion

    #region properties

    /// <summary>
    /// Return true if the provider is available
    /// </summary>
    public bool IsAvailabe
    { get { return  (StorageGateway.IsAvailable && SecurityGateway.IsAvailable && SystemGateway.IsAvailable); } }


    /// <summary>
    /// User 
    /// </summary>
    public override User User
    { get { return m_user; } }

    /// <summary>
    /// Returns true if we are authenticated
    /// </summary>
    public override bool IsAuthenticated
    { get { return (!String.IsNullOrEmpty( m_contextId)); } }

    #endregion

 
    #region ISecurityProvider Methods


    #region Login/Logout methods

    /// <summary>
    /// Log into system
    /// </summary>
    /// <param name="user">User name</param>
    /// <param name="password">User password</param>
    /// <param name="context">Context</param>
    /// <returns> m_contextId/Authentication token</returns>
    public override User Login(string login, string password, string context = "")
    {

      m_contextId = SecurityGateway.AcquireContext(login, password, context);


      m_user = User.Create(SecurityGateway.GetHandler( m_contextId, "user", XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", login } }).ToXml()));

      return m_user;

    } // Login(string user, string password)


    /// <summary>
    /// Log out of system
    /// </summary>
    /// <param name=" m_contextId"> m_contextId/Authentication token</param>
    public override void Logout(string login = "")
    {

      
      SecurityGateway.ReleaseContext( m_contextId, login);

       m_contextId = string.Empty;

       m_user = null;

    } // Logout()

    /// <summary>
    /// Log out of system
    /// </summary>
    /// <param name=" m_contextId"> m_contextId/Authentication token</param>
    public override void Renew()
    {
      try
      {
        SecurityGateway.RenewContext( m_contextId);
      }
      catch (Exception e)
      {
         m_contextId = string.Empty;
        throw e;
      }
    } // Renew()


    #endregion

    #region User methods

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="login"></param>
    /// <returns>Xml of the users</returns>
    public override User GetUser(string login)
    {
      List<User> users = GetUsers(XmlBuilder.Create("user", new Dictionary<string, string>() { { "login", login } }));

      if (users.Count == 0)
        throw new NotFoundException("User [" + login + "] was not found");

      return users[0];


    } // GetUser( XmlBuilder builder)

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override List<User> GetUsers()
    {
      return GetUsers(XmlBuilder.Create("user"));

    } // GetUsers( XmlBuilder builder)

    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override List<User> GetUsers(Role role)
    {
      return GetUsers( XmlBuilder.Create("user", new Dictionary<string, string>() { { "role", role.Name } }) );
 
   } // GetUsers( XmlBuilder builder)


    /// <summary>
    /// Gets the users 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override List<User> GetUsers(XmlBuilder query)
    {

      List<User> users = new List<User>();

      EntityCollection.Load(users, SecurityGateway.GetHandler( m_contextId, "user", query.ToXml()));

      return users;

    } // GetUsers( XmlBuilder builder)


    /// <summary>
    /// Add the user
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public override void AddUser(User user, string password)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(user.ToXml());

      doc.DocumentElement.SetAttribute("pwd", password);


      SecurityGateway.InsertHandler( m_contextId, "user", doc.InnerText);


    } // AddUser( User user)

    /// <summary>
    /// Add the user
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public override void ChangePassword(User user, string password)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(user.ToXml());

      doc.DocumentElement.SetAttribute("pwd", password);


      SecurityGateway.UpdateHandler( m_contextId, "user", doc.InnerText);


    } // ChangePassword( User user)


    /// <summary>
    /// Delete the user
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public override void Delete(User user)
    {

      SecurityGateway.DeleteUser( m_contextId, user.Login);

    } // DeleteUser( User user)

    /// <summary>
    /// Update the user
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="user">User to add</param>
    /// <returns>Xml of the users</returns>
    public override void Update(User user)
    {
      SecurityGateway.UpdateUser( m_contextId, user.ToXml());

    } // UpdateUser( User user)

    #endregion

    #region Roles methods

    /// <summary>
    /// Gets the roles 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override Role GetRole(string name)
    {
      List<Role> roles = new List<Role>();

      EntityCollection.Load(roles, SecurityGateway.GetHandler( m_contextId, "role", XmlBuilder.Create("role", (new Dictionary<string, string>() { { "name", name } })).ToXml()));

      if (roles.Count == 0)
        throw new NotFoundException("Role " + name + " not found");

      return roles[0];

    } // GetRole( XmlBuilder builder)

    /// <summary>
    /// Gets the roles 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override List<Role> GetRoles(XmlBuilder query = null)
    {

      List<Role> roles = new List<Role>();

      XmlBuilder qry = (query == null ? XmlBuilder.Create("role") : query);

      EntityCollection.Load(roles, SecurityGateway.GetHandler(m_contextId, "role", qry.ToXml()));

      return roles;

    } // GetRole( XmlBuilder builder)



    #endregion

    #region Rights methods

    /// <summary>
    /// Gets the rights 
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="builder"></param>
    /// <returns>Xml of the users</returns>
    public override List<Right> GetRights()
    {
      List<Right> rights = new List<Right>();

      EntityCollection.Load(rights, SecurityGateway.GetHandler( m_contextId, "right", XmlBuilder.Create("right").ToXml()));

      return rights;

    } // GetRight( XmlBuilder builder)



    #endregion

    #region Authentication Type Methods

    /// <summary>
    /// Gets the authentication types
    /// </summary>
    /// <returns></returns>
    public override  List<AuthenticationType> GetAuthenticationTypes(XmlBuilder query = null)
    {

      List<AuthenticationType> types = new List<AuthenticationType>();

      XmlBuilder qry = ( query == null ? XmlBuilder.Create("authenticationtype")  : query );

      EntityCollection.Load(types, SecurityGateway.GetHandler(m_contextId, "authenticationtype", qry.ToXml()));

      return types;

    } 

    #endregion

    #region Settings


    /// <summary>
    /// Gets the security settings
    /// </summary>
    /// <returns></returns>
    public override Setting GetSetting(string name)
    {
      List<Setting> settings = GetSettings(XmlBuilder.Create("setting", new Dictionary<string, string>() { { "subsystem", "Security" }, { "name", name } }));

      if (settings.Count == 0)
        throw new NotFoundException("Setting " + name + " was not found");

      return settings[0];

 
    } // GetSetting(string name)

    /// <summary>
    /// Gets the security settings
    /// </summary>
    /// <returns></returns>
    public override List<Setting> GetSettings(XmlBuilder query = null)
    {
      List<Setting> settings = new List<Setting>();

      XmlBuilder qry = ( query == null ? XmlBuilder.Create("setting", (new Dictionary<string, string>() { { "subsystem", "Security" } })) : query );

      EntityCollection.Load(settings, SecurityGateway.GetHandler( m_contextId, "setting", qry.ToXml()));

      return settings;

    } // GetSettings()

    #endregion

    #region Domain

    /// <summary>
    /// Gets the joined domain of the server
    /// </summary>
    /// <param name="request"></param>
    /// <returns></returns>
    public override string GetDomain()
    {

      string domainName = string.Empty;

      try
      {

        System.DirectoryServices.ActiveDirectory.Domain domain = System.DirectoryServices.ActiveDirectory.Domain.GetComputerDomain();

        if (domain != null)
          domainName = domain.Name;
      }
      catch (System.DirectoryServices.ActiveDirectory.ActiveDirectoryObjectNotFoundException)
      {
        // Not joined
      }

      return domainName;

    }

    #endregion


    #endregion


    #region IDataProvider Methods


 
    #region Get methods

    /// <summary>
    ///  Get the site
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <returns></returns>
    public override Site GetSite()
    {
      return Site.Create(SystemGateway.GetHandler( m_contextId, "site", XmlBuilder.Create("site").ToXml()));
    } //

    /// <summary>
    /// Gets the preview image for the series
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    public override Image GetPreviewImage(Series series)
    {
      // Get from the file system
      return Bitmap.FromFile("test");
    }


    /// <summary>
    /// Gets the dicom header for the series udi specified
    /// </summary>
    /// <param name="uid">Series uid</param>
    /// <returns></returns>
    public override string GetDicomHeader(string uid)
    {
      return StorageGateway.GetHandler(m_contextId, "dicomheader", XmlBuilder.Create("dicomheader", new Dictionary<string, string>() { { "uid", uid  } }).ToXml());
    } // GetDicomHeader(Uid uid)



    /// <summary>
    /// Gets the list of installed products
    /// </summary>
    /// <param name=" m_contextId"> m_contextId</param>
    /// <returns></returns>
    public override List<Product> GetProducts()
    {

      List<Product> products = new List<Product>();


      string xml = StorageGateway.GetHandler( m_contextId, "application", XmlBuilder.Create("application").ToXml());

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("app"))
      {
        products.Add(Product.Create(node));
      }

      return products;
    } // 

    #region Study methods

    /// <summary>
    /// Gets the studies from the provider
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="builder">Get</param>
    /// <returns>StudyCollection</returns>
    public override List<Study> GetStudies(XmlBuilder query)
    {

      List<Study> studies = new List<Study>();


      try
      {

        string xml = StorageGateway.GetHandler( m_contextId, "study", query.ToXml());


        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);


        foreach (XmlNode node in doc.DocumentElement.SelectNodes("study"))
        {
          studies.Add(Study.Create(node));

        }

        OnCompleted("study", "get", xml);

        return studies;
      }
      catch (Exception ex)
      {
        OnFailed("study", "get", ex.Message);
        throw;
      }

    } // GetStudies(XmlBuilder builder)

    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public override Study GetStudy(string uid)
    {
      try
      {

        XmlBuilder query = XmlBuilder.Create("study");
        query.AddAttribute("uid", uid);

        string xml = StorageGateway.GetHandler( m_contextId, "study", query.ToXml());

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);

        XmlNode studyNode = doc.DocumentElement.SelectSingleNode("study");


        if (studyNode == null)
          throw new ApplicationException("Study not found " + uid);

        Study study = Study.Create(studyNode);

        return study;
      }
      catch (Exception)
      {
        throw;
      }
    } //  GetStudy(string uid)

    /// <summary>
    /// Gets a single study specified by the uid
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="uid"></param>
    /// <returns></returns>
    public override Study GetStudy(Series series)
    {

      return GetStudy( series.StudyUid );

    } // GetStudy(Series series)

    #endregion


    #region Series methods

    /// <summary>
    /// Gets file list for the series
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="series"></param>
    /// <returns></returns>
    public override List<string> GetFiles(Series series, string location = "Remote")
    {

      if (series.Path.Length == 0)
      {
        XmlBuilder query = XmlBuilder.Create("seriespath");
        query.AddAttribute("uid", series.Uid);
        query.AddAttribute("loc", location);

        string xml = StorageGateway.GetHandler( m_contextId, "seriespath", query.ToXml());

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);

        series.Path = doc.DocumentElement.Attributes["path"].Value;
      }

      series.Files.Clear();
      Viatronix.Enterprise.Helper.GetFiles(series.Path, series.Path, series.Files);

      return series.Files;


    } // GetFiles(Series series)



    /// <summary>
    /// Gets the reference series associated with the study specified
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="study">Study</param>
    /// <returns>List<Series></returns>
    public override List<Series> GetReferences(Series series)
    {

      try
      {

        XmlBuilder query = XmlBuilder.Create("reference");
        query.AddAttribute("uid", series.Uid);

        string xml = StorageGateway.GetHandler( m_contextId, "reference", query.ToXml());


        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);


        series.References.Clear();

        foreach (XmlNode node in doc.DocumentElement.SelectNodes("series"))
        {
          series.References.Add(Series.Create(node));
        }

        return series.References;
      }
      catch (Exception ex)
      {
        OnFailed("reference", "get", ex.Message);
        throw;
      }
    } // GetReferences(Series series)

    /// <summary>
    /// Gets the dependent series associated with the study specified
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="study">Study</param>
    /// <returns>List<Series></returns>
    public override List<Series> GetDependents(Series series)
    {

      try
      {

        XmlBuilder query = XmlBuilder.Create("dependent");
        query.AddAttribute("uid", series.Uid);

        string xml = StorageGateway.GetHandler( m_contextId, "dependent", query.ToXml());

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);


        series.Dependents.Clear();

        foreach (XmlNode node in doc.DocumentElement.SelectNodes("series"))
        {
          series.Dependents.Add(Series.Create(node));
        }


        return series.Dependents;
      }
      catch (Exception ex)
      {
        OnFailed("dependent", "get", ex.Message);
        throw;
      }
    } // GetDependents(Series series)

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="study">Study</param>
    /// <returns>List<Series></returns>
    public override List<Series> GetSeries(Study study)
    {

      try
      {

        XmlBuilder query = XmlBuilder.Create("series");
        query.AddAttribute("suid", study.Uid);

        string xml = StorageGateway.GetHandler( m_contextId, "series", query.ToXml());


        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);


        study.Series.Clear();

        foreach (XmlNode node in doc.DocumentElement.SelectNodes("series"))
        {
          study.Series.Add(Series.Create(node));
        }


        OnCompleted("series", "get", xml);

        return study.Series;
      }
      catch (Exception ex)
      {
        OnFailed("series", "get", ex.Message);
        throw;
      }
    } // GetSeries(Study study)

    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name=" m_contextId">Authentication  m_contextId/token</param>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    public override Series GetSeries(string uid)
    {
      try
      {

        XmlBuilder query = XmlBuilder.Create("series");
        query.AddAttribute("uid", uid);

        string xml = StorageGateway.GetHandler( m_contextId, "series", query.ToXml());

        XmlDocument doc = new XmlDocument();

        doc.LoadXml(xml);

        XmlNode seriesNode = doc.DocumentElement.SelectSingleNode("series");


        if (seriesNode == null)
          throw new ApplicationException("Series not found " + uid);

        Series series = Series.Create(seriesNode);

        return series;
      }
      catch (Exception ex)
      {
        OnFailed("series", "get", ex.Message);

        throw;
      }
    } // GetSeries(Uid uid)

    #endregion

    #endregion

    #region Delete methods

    /// <summary>
    /// Delete the specified series
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="series"></param>
    public override void Delete(Series series)
    {
      StorageGateway.DeleteHandler( m_contextId, "series", series.ToXml());
    } // Delete(Series series)

    /// <summary>
    /// Delete the specified study
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="study"></param>
    public override void Delete(Study study)
    {
      StorageGateway.DeleteHandler( m_contextId, "study", study.ToXml());
    } // Delete(Study study)


    #endregion

    #region Update methods

    /// <summary>
    /// Update the specified series
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="series"></param>
    public override void Update(Series series)
    {
      StorageGateway.UpdateHandler( m_contextId, "series", series.ToXml());
    } // Update(Series series)

    /// <summary>
    /// Delete the specified study
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="study"></param>
    public override void Update(Study study)
    {
      StorageGateway.UpdateHandler( m_contextId, "study", study.ToXml());
    } // Update(Study study)


    #endregion

    #region Add methods

    /// <summary>
    /// Add the specified series
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="series"></param>
    public override void Add(Series series)
    {
      StorageGateway.InsertHandler( m_contextId, "series", series.ToXml());
    }

    /// <summary>
    /// Add the specified study
    /// </summary>
    /// <param name=" m_contextId"></param>
    /// <param name="study"></param>
    public override void Add(Study study)
    {
      StorageGateway.InsertHandler( m_contextId, "study", study.ToXml());
    }


    #endregion

    #region Store methods

    /// <summary>
    /// Get specified store (Default is the default)
    /// </summary>
    /// <param name=" m_contextId">Auth token</param>
    /// <param name="name">STore name</param>
    /// <returns>Store</returns>
    public override Store GetStore(string name = "Default")
    {

      return Store.Create(SystemGateway.GetHandler( m_contextId, "store", XmlBuilder.Create("store", (new Dictionary<string, string>() { { "name", name } })).ToXml()));

    } //  GetStore(string name = "Default")

    /// <summary>
    /// Get all stores
    /// </summary>
    /// <param name=" m_contextId">Auth token</param>
    /// <returns>List or stores</returns>
    public override List<Store> GetStores()
    {

      List<Store> stores = new List<Store>();

      string xml = SystemGateway.GetHandler( m_contextId, "store", XmlBuilder.Create("store").ToXml());

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("store"))
      {
        stores.Add(Store.Create(node));
      }

      return stores;
    } //  GetStore(string name = "Default")


    #endregion

    /// <summary>
    ///  Aborts any operation
    /// </summary>
    public override void Abort()
    { base.Abort();  }


    /// <summary>
    /// Resets the provider
    /// </summary>
    public override void Reset()
    { base.Reset();  }



    #endregion



  } // class DatabaseProvider


} // namespace Viatronix.Console


