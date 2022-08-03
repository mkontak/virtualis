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
using System.Data;
using System.Drawing;
using System.Xml;
using Viatronix.v3D.Core;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Console;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Providers
{

  /// <summary>
  /// Implements the missing functionality that allows access of Study information from a PACS server.
  /// </summary>
  public class PacsProvider : ProviderBase
  {

    #region fields

    /// <summary>
    /// The Pacs Server.
    /// </summary>
    private Host m_host = new Host();

    /// <summary>
    /// Find
    /// </summary>
    private CFind m_find = null;

    /// <summary>
    /// 
    /// </summary>
    List<Study> m_collection = null;


    /// <summary>
    /// Stores the AETitle
    /// </summary>
    private string m_aeTitle = string.Empty;

    /// <summary>
    /// Disposed flag
    /// </summary>
    private bool m_disposed = false;


    #endregion

    #region constructor and finalizers


 
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="xml"></param>
    public PacsProvider(string xml) : base(xml)
    {
      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      doc.DocumentElement.TryGetAttributeValue<string>("aetitle", ref m_aeTitle);

    }


    /// <summary>
    /// Cleans up any resources
    /// </summary>
    /// <param name="disposing">true if managed resources can be freed; otherwise false</param>
    protected override void Dispose(bool disposing)
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
      }
    } // Dispose( disposing )

    #endregion

    #region override methods

    /// <summary>
    /// Aborts the search
    /// </summary>
    public override void Abort()
    {
      base.Abort();

      // =============================
      // Abort the find if possible
      // =============================
      if (m_find != null)
      {
        m_find.Abort();
        m_find = null;
      } // 

    } // Abort();


    /// <summary>
    /// Resets the data provider
    /// </summary>
    public override void Reset()
    {
      base.Reset();
    } // Reset()


    /// <summary>
    /// Contains the search functionality for the DataProvider.
    /// </summary>
    /// <param name="builder">Contains the where clause for the search.</param>
    /// <returns>A StudyCollection containing the contents of the search.</returns>
    public override List<Study> GetStudies( XmlBuilder query)
    {
      return new List<Study>();
      //QueryItem [] items = builder.Expressions;

      //foreach( QueryItem item in items )
      //{
      //  if ( item.Key == "SYSTEM" )
      //  {
      //    string pacsServer = (string)item.Value;
      //    if ( string.Compare(m_host.Name, pacsServer, true) != 0 )
      //    {
      //      if (m_find != null)
      //      {
      //        m_find.Dispose();
      //        m_find = null;
      //      }

      //      foreach(Host host in Viatronix.Dicom.HostTable.CurrentHostTable.GetHosts(ServiceTypes.QUERY))
      //      {
      //        if (host.Name == (string)item.Value)
      //        {
      //          m_host = host;
      //          break;
      //        }
      //      }

      //      if ( m_host == null )
      //        throw new System.Exception("Host not found in list");


      //    } // END ... If there is a new host
      //    builder.Remove(item);
      //    break;
      //  }
      //}


      //m_collection = new StudyCollection();
      //if ( ! string.IsNullOrEmpty(m_host.Name) )
      //{
      //  try
      //  {
      //    CFindParameters parameters = CFindParameters.Create();

      //    // Set the Log File if found in the app config
      //    parameters.LogFile = (System.Configuration.ConfigurationManager.AppSettings["DicomLog"] != null ? System.Configuration.ConfigurationManager.AppSettings["DicomLog"].ToString() : string.Empty);

      //    QueryItem lastName = builder["PATIENTS_LAST_NAME"];
      //    if(lastName != null)
      //    {
      //      parameters.AddPatientNames(lastName.Value.ToString());
      //    }

      //    QueryItem medicalid = builder["PATIENTS_MEDICAL_ID"];
      //    if (medicalid != null)
      //    {
      //      parameters.MedicalId = medicalid.Value.ToString();
      //    }

      //    QueryItem studyuid = builder["DICOM_STUDY_ID"];
      //    if (studyuid != null)
      //    {
      //      parameters.StudyId = studyuid.Value.ToString();
      //    }

      //    QueryItem modality = builder["MODALITY"];
      //    if (modality != null)
      //    {
      //      parameters.AddModalities(modality.Value.ToString());
      //    }

      //    // Can have two date expressions in the case of a date range.
      //    int index  = -1;
      //    int index2 = -1;
      //    for (int i = 0; i < builder.Expressions.Length; ++i)
      //    {
      //      if (builder.Expressions[i].Key == "date")
      //      {
      //        if (index < 0)
      //        {
      //          index = i;
      //        }
      //        else
      //        {
      //          index2 = i;
      //          break;
      //        }
      //      }
      //    }

      //    if (index > -1)
      //    {
      //      QueryItem date = builder.Expressions[index];

      //      if (index2 < 0)
      //      {
      //        DateTime value = Convert.ToDateTime(date.Value);
      //        if (date.Operation == RelationalOp.Equals)
      //        {
      //          parameters.StudyDate = XmlConvert.ToString(value, "yyyyMMdd");
      //        }
      //        else if (date.Operation == RelationalOp.LessThan || date.Operation == RelationalOp.LessThanOrEqual)
      //        {
      //          parameters.SetStudyDate(null, value);
      //        }
      //        else if (date.Operation == RelationalOp.GreaterThan || date.Operation == RelationalOp.GreaterThanOrEqual)
      //        {
      //          parameters.SetStudyDate(value, null);
      //        }
      //      }
      //      else
      //      {
      //        QueryItem dateExp  = builder.Expressions[index];
      //        QueryItem dateExp2 = builder.Expressions[index2];

      //        if (dateExp.Operation == RelationalOp.GreaterThan || dateExp.Operation == RelationalOp.GreaterThanOrEqual)
      //        {
      //          parameters.SetStudyDate(Convert.ToDateTime(dateExp.Value), Convert.ToDateTime(dateExp2.Value));
      //        }
      //        else
      //        {
      //          parameters.SetStudyDate(Convert.ToDateTime(dateExp2.Value), Convert.ToDateTime(dateExp.Value));
      //        }
      //      }
      //    }


      //    // If the host does not support a series level query we need to perform the query in 2 steps (Study then Series)
      //    parameters.Level = (m_host.SupportsSeriesLevelQuery ? "SERIES" : "STUDY");

      //    m_find = CFind.Create(m_aeTitle, m_host);
      //    m_find.AddParameters(parameters);

      //    m_find.Started   += new ClientEventHandler(OnQueryStarted);
      //    m_find.Progress  += new ClientEventHandler(OnQueryProgress);
      //    m_find.Completed += new ClientEventHandler(OnQueryCompleted);
      //    m_find.Failed    += new ClientEventHandler(OnQueryFailed);


      //    m_find.Start();

      //    m_find.Wait();


      //    Log.Debug( parameters.Level + " level query returned from " + m_host.Name, "PacsProvider", "Search");

      //    if (!m_host.SupportsSeriesLevelQuery)
      //    {
      //      foreach (Study study in m_collection)
      //      {
      //        m_find.ClearParameters();
      //        parameters.Level = "SERIES";
      //        parameters.StudyUids = study.Uid.ToString();
      //        m_find.AddParameters(parameters);

      //        m_find.Start();

      //        m_find.Wait();

      //        Log.Debug(parameters.Level + " level query for study " + study.Uid.ToString() + " returned from  " + m_host.Name, "PacsProvider", "Search");

      //      }
      //    }

      //    //OnSearchCompleted( studies );
      //    //return studies;

      //  }
      //  catch ( Exception ex )
      //  {
      //    Viatronix.Logging.Log.Error("Search failed for PACS : " + ex.Message, "PacsProvider", "Search");
      //    OnSearchFailed("Search failed on PACS " + m_host.Name + " : " + ex.Message);
      //    m_collection = new StudyCollection();
      //  }
      //}
      //else
      //  m_collection = new StudyCollection();

      //if( m_abortSearch )
      //  return null;
      //else
      //{
      //  OnSearchCompleted(m_collection);
      //  return m_collection;
      //}
    } // Search( builder, state )


     #endregion

    #region event handlers

    /// <summary>
    /// Handles the query started event.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnQueryStarted(object sender, ClientEventArgs args)
    {
    } // OnQueryStarted( sender, args )


    /// <summary>
    /// Handles the query's coming back with progress.  We initialize study and series and add them to the list.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryProgress(object sender, ClientEventArgs args)
    {


      Viatronix.Dicom.Image dataset = args.Image;

      Study study = null;
      
      if ( ! m_collection.TryFind(dataset.StudyInstanceUid, ref study) )
      {
        study = new Study();

        study.Name = new PersonName(dataset.PatientName);
        study.Dob = dataset.PatientBirthDateTime;
        //study.Patient.Gender = (Gender)Enum.Parse(typeof(Gender), dataset.PatientSex);
        study.MedicalId = dataset.MedicalId;
        study.Weight = (int)dataset.PatientWeight;

        study.Uid = dataset.StudyInstanceUid;
        study.AccessionNumber = dataset.AccessionNumber;
        study.Date = Viatronix.Dicom.Utility.GetScanDate(dataset);
        study.Description = dataset.StudyDescription;
        study.Id = dataset.StudyId;
        study.Physician = dataset.ReferringPhysician; // new PersonName(dataset.ReferringPhysician);

        m_collection.Add(study);
      }



      if (string.Compare(args.QueryLevel, "SERIES", true) == 0)
      {

        Series series = new Series();
        series.Uid = dataset.SeriesInstanceUid;
        series.Date = dataset.SeriesDateTime;
        //series.DateReceived = dataset.SeriesDateTime;
        series.Description = dataset.SeriesDescription;
        series.Modality = dataset.Modality;
        //series.PatientOrientation = Viatronix.Dicom.Utility.GetImageOrientation(dataset.ImageOrientation).ToString();
        series.Source = m_host.AETitle;

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

    } // OnQueryProgress( sender, args )


    /// <summary>
    /// Handles the search completed event.  
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryCompleted(object sender, ClientEventArgs args)
    {
    } // OnQueryCompleted( sender, args )


    /// <summary>
    /// Handles the search failing.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryFailed(object sender, ClientEventArgs args)
    {
      Viatronix.UI.MessageBox.ShowError("Failed to query the selected PACS, please see log for details.");
      Log.Error(string.Format("Query of PACS failed.  Server: {0}  [ERR={1}]", m_host.Server, args.Message), "PacsProvider", "OnQueryFailed");
    } // OnQueryFailed( sender, args )


    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the Dicom.Host
    /// </summary>
    public Host PacsServer
    {
      set { m_host = value; }
      get { return m_host; }
    } // PacsServer

    #endregion

  } // class PacsProvider

} // namespace Viatronix.Console


#region revision history

// $Log: PacsProvider.cs,v $
// Revision 1.3.2.2  2007/04/24 19:15:03  mkontak
// Fix issue 5597
//
// Revision 1.3.2.1  2007/03/26 18:21:28  gdavidson
// Issue #5516: When aborting a search block the main thread until the executing search thread has comlpeted
//
// Revision 1.3  2007/03/06 19:50:25  mkontak
// Coding standards
//
// Revision 1.2  2007/02/21 21:04:48  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 15:34:08  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.14  2006/09/06 13:16:31  mkontak
// Issue 4969
//
// Revision 1.13  2006/08/22 17:43:38  mkontak
// Fix abort when doing PACS search. Added new flag to disable the
// imbedded image query within the series query to save time.
//
// Revision 1.12  2006/07/10 18:12:59  gdavidson
// Added SearchFailed event
//
// Revision 1.11  2006/04/19 14:37:29  mkontak
// Minor changes
//
// Revision 1.10  2006/02/16 15:28:51  mkontak
// Added new interfaces to data providers
//
// Revision 1.9  2006/02/10 21:27:17  gdavidson
// Return the SearchComlpeted event even if no items are found.
//
// Revision 1.8  2005/12/06 20:04:21  romy
// removed non referenced local variables
//
// Revision 1.7  2005/11/23 11:52:40  mkontak
// Using new MessageBox implementation
//
// Revision 1.6  2005/11/02 14:22:24  gdavidson
// DataProvider implements IDisposable.
//
// Revision 1.5  2005/10/31 17:17:53  mkontak
// Minor fixes
//
// Revision 1.4  2005/10/21 17:45:34  mkontak
// Minor changes
//
// Revision 1.3  2005/10/12 17:00:49  gdavidson
// Added ability to cancel search
//
// Revision 1.2  2005/10/12 13:06:35  mkontak
// Changes to DataProvider abstract class
//
// Revision 1.1  2005/10/05 12:34:59  mkontak
// Stub to be used for PAC intergation
//
// Revision 1.19  2005/10/04 11:56:05  mkontak
// Changed the CreateCacheClient()
//
// Revision 1.18  2005/09/12 12:49:24  gdavidson
// Changed RetrieveData method so that it removes previous references before accessing the cache client.
//
// Revision 1.17  2005/09/02 13:13:30  gdavidson
// Added exception hadling in the RetrieveData method.
//
// Revision 1.16  2005/08/31 16:14:25  gdavidson
// Set the series's full path in the RetrieveData method.
//
// Revision 1.15  2005/08/31 13:48:50  gdavidson
// Added RetrieveData methods.
//
// Revision 1.14  2005/06/24 14:51:46  gdavidson
// Added exception handling in the GetPreviewImage method.
//
// Revision 1.13  2005/06/22 15:15:15  gdavidson
// Added Delete and GetPreviewImage methods.
//
// Revision 1.12  2005/06/16 13:02:27  gdavidson
// Added a Retrieve method.
//
// Revision 1.11  2005/06/02 13:35:55  gdavidson
// Commented code.
//
// Revision 1.10  2005/05/26 17:43:28  gdavidson
// Changed DataProvider to return StudyCollections.
//
// Revision 1.7  2005/05/16 20:22:01  gdavidson
// Retrieve db connection string from appSettings.
//
// Revision 1.6  2005/05/16 14:24:46  gdavidson
// Modified the record retrieval from the database.
//
// Revision 1.5  2005/05/05 17:41:34  gdavidson
// Removed the Retrieve method.
//
// Revision 1.4  2005/05/03 18:17:56  frank
// Compiles now, but needs work for the cache client
//
// Revision 1.3  2005/04/20 15:33:27  gdavidson
// Added retrieve method.
//
// Revision 1.2  2005/04/19 19:50:34  gdavidson
// Moved raising of the SearchCompleted event to the DataProvider class.
//
// Revision 1.1  2005/03/29 01:08:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/dataProviders/PacsProvider.cs,v 1.3.2.2 2007/04/24 19:15:03 mkontak Exp $
// $Id: PacsProvider.cs,v 1.3.2.2 2007/04/24 19:15:03 mkontak Exp $

#endregion
