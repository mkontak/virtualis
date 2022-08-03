// $Id: PacsProvider.cs,v 1.3.2.2 2007/04/24 19:15:03 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Dicom;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{
  /// <summary>
  /// Implements the missing functionality that allows access of Study information from a PACS server.
  /// </summary>
  public class DirectPacsProvider : AsynchronousProviderBase, IAsynchronousPacsProvider, IDisposable
  {
 
    #region fields
 
    /// <summary>
    /// The Pacs Server.
    /// </summary>
    private DicomHost m_server = null;

    /// <summary>
    /// The finder object.
    /// </summary>
    private CFind m_find = null;

    /// <summary>
    /// The list of studies that came back.
    /// </summary>
    private List<Study> m_studies = new List<Study>();
    
    /// <summary>
    /// Stores the AETitle
    /// </summary>
    private string m_aeTitle = string.Empty;
    
    /// <summary>
    /// Was this instance disposed?
    /// </summary>
    private bool m_disposed = false;
    
    /// <summary>
    /// The security provider.
    /// </summary>
    private ISecurityProvider m_securityProvider = null;

    #endregion

    #region constructor and finalizers

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="xml">The xml defining the provider information.</param>
    /// <param name="securityProvider">The Security Provider to authenticate against.</param>
    public DirectPacsProvider(string xml, ISecurityProvider securityProvider) : base(xml)
    {
      if (securityProvider == null)
        throw new InvalidDateException("No security provider supplied");

      m_securityProvider = securityProvider;

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      if (!doc.DocumentElement.TryGetAttributeValue<string>("aetitle", ref m_aeTitle))
      {
        throw new ConfigurationException("Invalid configuration missing aetitle");
      }
    } // DirectPacsProvider( xml, securityProvider )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the Dicom.Host
    /// </summary>
    public virtual DicomHost Server
    {
      set { m_server = value; }
      get { return m_server; }
    } // Server


    /// <summary>
    /// Gets the security provider associated with this provider
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
    } // SecurityProvider


    /// <summary>
    /// Is Authenticated
    /// </summary>
    public override bool IsAuthenticated
    { 
      get { return (m_securityProvider != null && m_securityProvider.IsAuthenticated); }
    } // IsAuthenticated
    
    #endregion

    #region IDisposable

    /// <summary>
    /// Dispose
    /// </summary>
    public virtual void Dispose()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    } // Dispose()

    /// <summary>
    /// Cleans up any resources
    /// </summary>
    /// <param name="disposing">true if managed resources can be freed; otherwise false</param>
    private void Dispose(bool disposing)
    {
      if (!m_disposed)
      {
        if (disposing)
        {

        }

        if (m_find != null)
        {
          m_find.Dispose();
          m_find = null;
        }

        m_disposed = true;
      }
    } // Dispose( disposing )

    #endregion

    #region methods

    /// <summary>
    /// Aborts the search
    /// </summary>
    public override void Abort()
    {
      
      // =============================
      // Abort the find if possible
      // =============================
      if (m_find != null)
      {
        m_find.Abort();
        m_find = null;
      } // 

      base.Abort();
    } // Abort();

    /// <summary>
    /// Aborts the search
    /// </summary>
    /// <param name="id">Task id to abort</param>
    public override void Abort(string id)
    {
      // =============================
      // Abort the find if possible
      // =============================
      if (m_find != null)
      {
        m_find.Abort();
        m_find = null;
      } // 

      base.Abort(id);
    } // Abort(id);


    #region Study Methods

    /// <summary>
    /// Performs the get studies for the data provider
    /// </summary>
    /// <param name="builder">Contains the query parameters</param>
    /// <returns>Xml results</returns>
    private string GetStudiesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        throw new NotAuthorizedException("This operation requires a login");

      string results = string.Empty;
      m_studies.Clear();

      if (m_server != null)
      {
        m_find = CFind.Create(m_aeTitle, m_server.ToXml());
        m_find.Progress += new ClientEventHandler(OnQueryProgressHandler);

        CFindParameters cFindParameters = CFindParameters.Create();

        string attribute = string.Empty;

        if (query.TryGetAttributeValue<string>("name", ref attribute))
          cFindParameters.AddPatientNames(attribute);

        if (query.TryGetAttributeValue<string>("mid", ref attribute))
          cFindParameters.MedicalId = attribute;

        if (query.TryGetAttributeValue<string>("studyUid", ref attribute))
          cFindParameters.AddStudyUids(attribute);

        if (query.TryGetAttributeValue<string>("seriesUid", ref attribute))
          cFindParameters.AddSeriesUids(attribute);

        if (query.TryGetAttributeValue<string>("id", ref attribute))
          cFindParameters.StudyId = attribute;
        
        if (query.TryGetAttributeValue<string>("mods", ref attribute) && attribute.ToLower() != "all")
          cFindParameters.AddModalities(attribute);

        if (query.TryGetAttributeValue<string>("date", ref attribute) && attribute != string.Empty)
        {
          string dateRange = query.Attributes["date"].Replace("RANGE:", "");
          string[] dates = dateRange.Split(',');

          DateTime lhs = dates[0] != string.Empty ? DateTime.Parse(dates[0]) : DateTime.MinValue;
          DateTime rhs = dates[1] != string.Empty ? DateTime.Parse(dates[1]) : DateTime.MaxValue;

          cFindParameters.SetStudyDate(lhs, rhs);
        }

        m_find.AddParameters(cFindParameters);

        m_find.Start();

        // Wait until find ends.
        m_find.Wait();

        results = m_studies.ToXml();

      }

      return results;

   } // GetStudiesInternal(XmlBuilder query)


    /// <summary>
    /// Gets the studies asynchronously
    /// </summary>
    /// <param name="query">Query paremeters</param>
    public void GetStudiesAsynchronously(XmlBuilder query)
    {
      GetAsynchronously(this.GetStudiesInternal, query);
    } // GetStudiesAsynchronously(XmlBuilder query)

    /// <summary>
    /// Contains the search functionality for the DataProvider.
    /// </summary>
    /// <param name="builder">Contains the where clause for the search.</param>
    /// <returns>A StudyCollection containing the contents of the search.</returns>
    public virtual List<Study> GetStudies(XmlBuilder query)
    {
      if (!IsAuthenticated)
        throw new NotAuthorizedException("This operation requires a login");

      string xml = string.Empty;

      if (m_server != null)
      {
        try
        {
          xml = GetStudiesInternal(query);

          EntityCollection.Load<Study>(m_studies, xml);

          // find is done, send back the results.
          FireCompletedEvent("study", "get", xml);

          return m_studies;
        }
        catch (Exception ex)
        {
          FireFailedEvent("study", "get", ex.Message);
          throw;
        }

      }


      FireCompletedEvent("study", "get", xml);

      return m_studies;
    } // Search( builder, state )


    public virtual Study GetStudy(string uid)
    { throw new NotSupportedException("GetStudy by uid is not supported by " + this.Name + " provider "); }


    /// <summary>
    /// Performs the refresh.
    /// </summary>
    /// <param name="study">The study to refresh.</param>
    public virtual void GetStudy(Study study)
    {
      // Do nothing...
    } // GetStudy( study )


    /// <summary>
    /// Gets the study by the series.
    /// </summary>
    /// <param name="series">The series.</param>
    /// <returns></returns>
    public virtual Study GetStudy(Series series)
    {
      if (!IsAuthenticated)
        throw new NotAuthorizedException("This operation requires a login");

      string xml = string.Empty;

      List<Study> studies = new List<Study>();

      if (m_server != null)
      {
        try
        {
          // Gets the series using the interal method
          xml = GetStudiesInternal(XmlBuilder.Create("study", new Dictionary<string, string> {{ "studyUid", series.StudyUid }} ));

          EntityCollection.Load<Study>(studies, xml);

          FireCompletedEvent("study", "get", xml);

          return studies.Count > 0 ? studies[0] : null;
        }
        catch (Exception ex)
        {
          FireFailedEvent("series", "get", ex.Message);
          throw;
        }
      }

      FireCompletedEvent("series", "get", xml);

      return null;
    } // GetStudy( series )


    #endregion


    #region Series Methods

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    private string GetSeriesInternal(XmlBuilder query)
    {
      if (!IsAuthenticated)
        throw new Viatronix.Enterprise.NotAuthorizedException("This operation requires a login");

      string results = string.Empty;

      if (m_server != null)
      {

        try
        {
          m_find = CFind.Create(m_aeTitle, m_server.ToXml());
          m_find.Progress += new ClientEventHandler(OnQueryProgressHandler);

          CFindParameters cFindParameters = CFindParameters.Create();


          string uids = string.Empty;
          if (query.TryGetAttributeValue<string>("suid", ref uids))
            cFindParameters.AddStudyUids(uids);

          Study study = null;
          if (m_studies.TryFind(uids, ref study))
            study.Series.Clear();

          m_find.AddParameters(cFindParameters);

          m_find.Start();

          // Wait until find ends.
          m_find.Wait();

          // find is done, send back the results.
          if (m_studies.TryFind(uids, ref study))
            results = study.Series.ToXml();  

        }
        catch (Exception )
        {
          throw;
        }
      }

      return results;
    } // GetSeriesInternal( query )

    /// <summary>
    /// Gets the series for the specified study asynchronously
    /// </summary>
    /// <param name="study">Study tp get series for</param>
    public virtual void GetSeriesAsynchronously(Study study)
    {
      GetAsynchronously(this.GetSeriesInternal, XmlBuilder.Create("series", new Dictionary<string, string> { { "suid", study.Uid } } ) ) ;
    }   // GetSeriesAsynchronously(Study study)

    /// <summary>
    /// Gets the series for the specified study asynchronously
    /// </summary>
    /// <param name="study">Study tp get series for</param>
    public virtual void GetSeriesAsynchronously(List<Study> studies)
    { throw new NotSupportedException("GetSeriesAsynchronously for study list is not supported by " + this.Name + " provider "); }

    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="study">Study</param>
    /// <returns>SeriesCollection</returns>
    public virtual List<Series> GetSeries(Study study)
    {
      if (!IsAuthenticated)
        throw new NotAuthorizedException("This operation requires a login");


      string xml= string.Empty;

      List<Series> series = new List<Series>();

      if (m_server != null)
      {
        try
        {
          // Gets the series using the interal method
          xml = GetSeriesInternal(XmlBuilder.Create("series", new Dictionary<string,string> { { "suid", study.Uid} } ) );

          EntityCollection.Load<Series>(series, xml);

          FireCompletedEvent("series", "get", xml);

          return series;
          
        }
        catch (Exception ex)
        {
          FireFailedEvent("series", "get", ex.Message);
          throw;
        }
      }

      FireCompletedEvent("series", "get", xml);


      return series;
    } // GetSeries( study )


    /// <summary>
    /// Gets the series associated with the study specified
    /// </summary>
    /// <param name="studies">Study</param>
    /// <returns>SeriesCollection</returns>
    public virtual List<Series> GetSeries(List<Study> studies)
    { throw new NotSupportedException("GetSeries for study list is not supported by " + this.Name + " provider "); }

    /// <summary>
    /// Gets a single series specified by the uid
    /// </summary>
    /// <param name="uid">Series uid</param>
    /// <returns>Series</returns>
    public virtual Series GetSeries(string uid)
    { throw new NotSupportedException("GetSeries by uid is not supported by " + this.Name + " provider "); }


    /// <summary>
    /// Refreshes the series state.
    /// </summary>
    /// <param name="series">The series to update.</param>
    public virtual void GetSeries(Series series)
    {
      // Do nothing...
    } // GetSeries( series )

    #endregion

    #endregion

    #region event handlers

    /// <summary>
     /// Handles the query's coming back with progress.  We initialize study and series and add them to the list.
     /// </summary>
     /// <param name="sender">The origin of the event.</param>
     /// <param name="args">Arguments to take into account.</param>
    private void OnQueryProgressHandler(object sender, ClientEventArgs args)
    {
      Viatronix.Dicom.Image dataset = args.Image;

      Study study = null;

      if (!m_studies.TryFind(dataset.StudyInstanceUid, ref study))
      {
        study = new Study();

        study.Name = new PersonName(dataset.PatientName);
        study.Dob = dataset.PatientBirthDateTime;

        try
        {
          study.Gender = (Genders)Enum.Parse(typeof(Genders), dataset.PatientSex);
        }
        catch (ArgumentException )
        {
          study.Gender = Genders.Unknown;
        }

        study.MedicalId = dataset.MedicalId;
        study.Weight = (int)dataset.PatientWeight;

        study.Uid = dataset.StudyInstanceUid;
        study.AccessionNumber = dataset.AccessionNumber;
        study.Date = Viatronix.Dicom.Utility.GetScanDate(dataset);
        study.Description = dataset.StudyDescription;
        study.Id = dataset.StudyId;
        study.Physician = dataset.ReferringPhysician; // new PersonName(dataset.ReferringPhysician);

        m_studies.Add(study);
      }

      if (string.Compare(args.QueryLevel, "SERIES", true) == 0)
      {

        Series series = new Series();
        series.Uid = dataset.SeriesInstanceUid;
        series.Date = dataset.SeriesDateTime;
        //series.DateReceived = dataset.SeriesDateTime;
        series.Description = dataset.SeriesDescription;
        series.Modality = dataset.Modality;
        series.PatientOrientation = Viatronix.Dicom.Utility.GetImageOrientation(dataset.ImageOrientation).ToString();
        series.Source = m_server.AETitle;

         // ----------------------------------------------------------------
         // Need to get the image count out of the returned dataset.
         // ----------------------------------------------------------------
        int imageCount = 0;
        if (dataset.TryGetValue(0x0020, 0x1209, ref imageCount))
        {
          series.ImageCount = imageCount;
        }

        series.StudyUid = study.Uid;
        study.Series.Add(series);
      }
    } // OnQueryProgressHandler( sender, args )

    #endregion

  } // class DirectPacsProvider
} // namespace Viatronix.Console

