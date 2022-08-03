// $Id: VCLauncher.cs,v 1.12.2.14 2008/10/03 19:33:30 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using Viatronix.Logging;
using Viatronix.UI;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
	/// <summary>
	/// Implements all functionality necessary to launch a VC viewer.
	/// </summary>
	public class VCLauncher : Launcher
  {

    #region exceptions

    /// <summary>
    /// An exception for the VC Viewer being locked.
    /// </summary>
    public class VCViewerLockedException : Exception
    {

      #region construction

      /// <summary>
      /// Constructor.
      /// </summary>
      /// <param name="message">The message to relay.</param>
      public VCViewerLockedException(string message) : base(message)
      {
      } // VCViewerLockedException( message )

      #endregion

    } // class VCViewerLockedException

    #endregion

    #region interop

    [DllImport("oleaut32.Dll")]
    private static extern uint GetActiveObject( ref Guid clsid, uint reserved, [MarshalAs(UnmanagedType.IUnknown)] out object ppunk);

    #endregion

    #region fields

    /// <summary>
    /// identifier for the colon viewer
    /// </summary>
    private static Guid CLSID_ColonViewer = new Guid( "{CE999526-8935-40d1-ABCE-3B5DA8B75427}" );

     /// <summary>
    /// primary dataset
    /// </summary>
    private List<Series> m_primary = null;

    /// <summary>
    /// secondary dataset
    /// </summary>
    private List<Series> m_secondary = null;

    /// <summary>
    /// Reference to the session.
    /// </summary>
    private Series m_session = null;

    #endregion

    #region properties

    /// <summary>
    /// Returns a reference to the session series.
    /// </summary>
    protected Series Session
    {
      get { return m_session; }
      set { m_session = value; }
    } // Session


    /// <summary>
    /// Returns a reference to the Primary List<Series>
    /// </summary>
    protected List<Series> PrimaryCollection
    {
      get { return m_primary; }
      set { m_primary = value; }
    } // PrimaryCollection


    /// <summary>
    /// Returns a reference to the Secondary List<Series>
    /// </summary>
    protected List<Series> SecondaryCollection
    {
      get { return m_secondary; }
      set { m_secondary = value; }
    } // SecondaryCollection

    #endregion

    #region override methods

    /// <summary>
    /// Groups the series into series collections based on datasets.
    /// </summary>
    protected override void OnInitializeLaunch()
    {
      foreach (List<Series> collection in this.Datasets)
      {
        // TODO: New ( Retrieves the file list for all series in the collection and all their reefrences
        //this.DataProvider.RetrieveData(collection, true);
      }

      if( this.IsSession )
      {
        m_session = this.Datasets[0][0];
        m_primary = this.Datasets[1];
        m_secondary = (this.Datasets.Count > 2) ? this.Datasets[2] : new List<Series>();

        CommandLine = string.Format(" -uid {0}", m_session.Uid); 
      }
      else
      {
        m_primary = this.Datasets[0];
        m_secondary = (this.Datasets.Count > 1) ? this.Datasets[1] : new List<Series>();

        Series primary = m_primary[0];
        Series secondary = m_secondary.Count > 0 ? m_secondary[0] : null;

        CommandLine = string.Format(" -uid {0} {1}", primary.Uid + (secondary != null ? "|" + secondary.Uid : string.Empty), NewSession ? "-newsession" : string.Empty); 
      }
    } // SetupLaunch()



    /// <summary>
    /// Displays the form
    /// </summary>
    protected override void OnShowForm( Rectangle bounds, System.Windows.Forms.FormWindowState state )
    {
      // application is locked
      //Frame frame = this.Form as Frame;
      //if( frame != null )
      //{
      //  if( frame.IsApplicationClosed )
      //  {
      //    frame.Close();
      //    return;
      //  }

      //  if( frame.IsApplicationLocked )
      //    return;

      //  // lockout application
      //  if (m_viewerLocked)
      //  {
      //    throw new VCViewerLockedException("Showing console frame when viewer is locked is invalid.");
      //  }
      //}

      WindowUtilities.ShowWindow( this.Form, bounds, state );
    } // OnShowForm()


    /// <summary>
    /// Clean's up any work performed by the launcher
    /// </summary>
    protected override void OnCleanupLaunch()
    {
 	    base.OnCleanupLaunch();

      // TODO: New
      //if( this.IsSession == false && m_session != null )
      //  this.DataProvider.Delete( m_session );
    } // OnCleanupLaunch()

    #endregion

    #region methods

 
    /// <summary>
    /// Writes user xml to a XmlTextWriter
    /// </summary>
    /// <param name="writer">XmlTextWriter</param>
    protected void WriteUserXml( XmlTextWriter writer )
    {
      writer.WriteStartElement( "user" );
      writer.WriteAttributeString( "role", this.LauncherArgs.User.Role );
      writer.WriteAttributeString( "name", this.LauncherArgs.User.Name );
      writer.WriteAttributeString( "login", this.LauncherArgs.User.Login );
      writer.WriteAttributeString( "ctx", this.LauncherArgs.User.ContextId );
      writer.WriteAttributeString( "prm", this.LauncherArgs.User.Permissions.ToString());
    
 
      writer.WriteStartElement("rights");
      // right elements
      foreach (Right right in this.LauncherArgs.User.AllRights)
      {
        writer.WriteRaw(right.ToXml());
      }

      writer.WriteEndElement();

 
      writer.WriteEndElement();
    } // WriterUserXml( writer )


    /// <summary>
    /// Creates a default session series.
    /// </summary>
    /// <param name="primary">The primary dataset.</param>
    /// <param name="secondary">The secondary dataset.</param>
    /// <returns>A session series.</returns>
    //private Series CreateVCSession( List<Series> primary, List<Series> secondary )
    //{

    //  // Check for the training  mode flag
    //  object trainingModeObject = System.Configuration.ConfigurationManager.AppSettings["TrainingMode"];
    //  bool trainingMode = (trainingModeObject != null ? System.Convert.ToBoolean(trainingModeObject) : false);

    //  Series series = new Series();

    //  DateTime today = System.DateTime.Now;

    //  series.Type           = "Session";
    //  series.Application        = "VC";
    //  series.Uid            = Uid.GenerateUid();

    //  // Add training to the description so that it can be easily parsed from the database
    //  series.Description    = (trainingMode ? "VC Training Session" : "VC Session - ") + today.ToString();
    //  series.Date           = today;
      

    //  bool found = false;

    //  Log.Warning("Start Primary [" + DateTime.Now.ToString() + "]", "VCLauncher", "CreateVCSssion");

    //  // primary
    //  if( primary != null )
    //  {


    //    foreach( Series refSeries in primary )
    //    {
    //      if( refSeries.IsType( VcRule.Session ))
    //        continue;

    //      refSeries.Directory = Database.GetSeriesDirectory(refSeries.Uid);
    //      refSeries.Path = Path.Combine(Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.CacheBasePath, refSeries.Location);

    //      refSeries.Study.Directory = Path.GetDirectoryName(refSeries.Directory);
    //      refSeries.Study.Patient.Directory = Path.GetDirectoryName(refSeries.Study.Directory);

    //      ////////////////////////////////////////////////////////////////////////////////////////
    //      // Make sure there is a patient location before setting the study in the new series
    //      // we need all the parent links to be valid.
    //      ///////////////////////////////////////////////////////////////////////////////////////
    //      if ( refSeries.Study.Patient.Directory.Length > 0 && refSeries.Study.Uid.ToString() != "" && refSeries.Study.Patient.MedicalId != "" ) 
    //      {
    //        Logging.Log.Debug("Found core series in collection [SERIES_Uid=" + refSeries.Uid.ToString() + "] [STUDY_Uid=" + refSeries.Study.Uid.ToString() + "] [PATIENT_MID=" + refSeries.Study.Patient.MedicalId + "]", "VCLuancher", "CreateVCSession");

    //        found = true;
    //        //series.PatientOrientation   = refSeries.PatientOrientation;
    //        series.Study      = refSeries.Study;
    //        series.Directory = Viatronix.v3D.Core.Series.GenerateDirecrtoryName(series);
    //        series.Location = Viatronix.Utilities.IO.PathUtilities.CombinePaths(series.Study.Directory, series.Directory);
    //        series.Path   = Viatronix.Utilities.IO.PathUtilities.CombinePaths(Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.CacheBasePath, series.Location);
    //      } // END ... If we have a fully linked series

    //      series.References.Add( refSeries );
    //    }
    //  }

    //  Log.Warning("Start Secondary [" + DateTime.Now.ToString() + "]", "VCLuancher", "CreateVCSession");

    //  // secondary
    //  if( secondary != null )
    //  {
    //    foreach( Series refSeries in secondary )
    //    {
    //      if( refSeries.IsType( VcRule.Session ))
    //        continue;

    //      refSeries.Directory = Database.GetSeriesDirectory(refSeries.Uid);
    //      refSeries.Path = Path.Combine(Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.CacheBasePath, refSeries.Location);

    //      refSeries.Study.Directory = Path.GetDirectoryName(refSeries.Directory);
    //      refSeries.Study.Patient.Directory = Path.GetDirectoryName(refSeries.Study.Directory);

    //      ////////////////////////////////////////////////////////////////////////////////////////
    //      // Make sure there is a patient location before setting the study in the new series
    //      // we need all the parent links to be valid.
    //      ///////////////////////////////////////////////////////////////////////////////////////
    //      if ( refSeries.Study.Patient.Directory.Length > 0 && refSeries.Study.Uid.ToString() != "" && refSeries.Study.Patient.MedicalId != "" ) 
    //      {
    //        Logging.Log.Debug("Found core series in collection [SERIES_Uid=" + refSeries.Uid.ToString() + "] [STUDY_Uid=" + refSeries.Study.Uid.ToString() + "] [PATIENT_MID=" + refSeries.Study.Patient.MedicalId + "]", "VCLuancher", "CreateVCSession");

    //        found = true;
    //        //series.PatientOrientation   = refSeries.PatientOrientation;
    //        series.Study                = refSeries.Study;
    //        series.Directory = Viatronix.v3D.Core.Series.GenerateDirecrtoryName(series);
    //        series.Location = Viatronix.Utilities.IO.PathUtilities.CombinePaths(series.Study.Directory, series.Directory);
    //        series.Path = Viatronix.Utilities.IO.PathUtilities.CombinePaths(Viatronix.Utilities.Registry.SystemSettings.CurrentSystemSettings.CacheBasePath, series.Location);
    //      } // END ... If we have a fully linked series

    //      series.References.Add( refSeries );

    //    }
    //  }

    //  if ( ! found )
    //  {
    //    Logging.Log.Error("Failed to find core series in collection", "VCLuancher", "CreateVCSession");
    //    throw new Exception("Could not find the core series with a valid Study and Patient in the collection");
    //  }


    //  /////////////////////////////////////////////////////////////////////////////////////
    //  // Dont allow the useion if we are in training moder to change the session descript
    //  //////////////////////////////////////////////////////////////////////////////////////
    //  if (!trainingMode)
    //  {

    //    Log.Warning("Start Description override [" + DateTime.Now.ToString() + "]", "VCLuancher", "CreateVCSession");

    //    SessionDescriptionDialog dialog = new SessionDescriptionDialog();
    //    dialog.Description = series.Description;
    //    dialog.AllowCancel = true;
    //    if ( dialog.ShowDialog(Global.Frame) == DialogResult.Cancel )
    //    {
    //      throw new LaunchException(LaunchStage.Cancelled, "Launch of VC viewer was cancelled");
    //    }
    //    else
    //    {
    //      series.Description = dialog.Description;
    //    }
    //  } // END ... If we are not in training mode



    //  this.DataProvider.Create( ref series );
    //  this.DataProvider.Update( series );

    //  return series;
    //} // CreateSession( primary, secondary )


 

    #endregion

 
 
  } // VCLauncher
} // namespace Viatronix.Console

#region revision history

// $Log: VCLauncher.cs,v $
// Revision 1.12.2.14  2008/10/03 19:33:30  kchalupa
// Pay Per Procedure Functionality
//
// Revision 1.12.2.13  2008/09/16 17:13:09  kchalupa
// Issue
// 6150  No Movie Preview Console Crashes
// VCSessionViewer
// 6122 VC Sessions Not Showing Completed
// VCLauncher
// 6073 Each Movie (VC) Is Listed Twice On Patient CD
// VCSessionProvider
// 6153 Movies that have no preview do not show up in Patient CD HTML Report
// RemovableMediaForPatient
// ReportTags
//
// Revision 1.12.2.13  2008/09/10 11:59:32  kchalupa
// Issue #6122
// Session when "marked" as incomplete was not being
// set to reflect the change properly.
//
// Revision 1.12.2.12  2008/04/10 20:20:12  mkontak
// Added the ability to cancel the laucnh for a VC
//
// Revision 1.12.2.11  2008/01/25 20:08:56  mkontak
// Training mode changes
//
// Revision 1.12.2.10  2008/01/04 19:33:53  mkontak
// VC completed session for reports marked completed
//
// Revision 1.12.2.9  2008/01/03 21:01:49  mkontak
// Check the MNarkedCOmpleted flag of the VRX file and mark the session completed
// if the report is marked completed
//
// Revision 1.12.2.8  2007/09/10 19:33:32  mkontak
// Added the time into the default VC session description
//
// Revision 1.12.2.7  2007/09/10 17:17:10  mkontak
// Issue 5774
//
// Revision 1.12.2.6  2007/06/18 13:51:29  gdavidson
// Issue 5686: Decrypt password on login
//
// Revision 1.12.2.5  2007/06/15 21:51:25  gdavidson
// Issue 5686: Check the login for a valid user admin or service user
//
// Revision 1.12.2.4  2007/05/25 17:57:41  mkontak
// Kodak laucher
//
// Revision 1.12.2.3  2007/05/04 20:12:13  gdavidson
// Issue #5550: Modfied closing operation
//
// Revision 1.12.2.2  2007/03/28 17:50:12  gdavidson
// Issue #5420: Made viewer load call asynchronous
//
// Revision 1.12.2.1  2007/03/20 20:10:22  mkontak
// Sets the VC sessions series date to today
//
// Revision 1.12  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.11  2007/03/05 16:17:30  gdavidson
// Issue #5338: Perform all launch operations on the main thread
//
// Revision 1.10  2007/03/02 22:09:09  gdavidson
// Modified log messages
//
// Revision 1.9  2007/02/21 15:41:42  gdavidson
// Added additional logging for tracking down crashes
//
// Revision 1.8  2007/02/13 18:48:52  gdavidson
// Commented methods
//
// Revision 1.7  2007/01/23 15:19:36  gdavidson
// Fixed a problem with maintaing window bounds and state when switching applications
//
// Revision 1.6  2007/01/03 22:25:09  gdavidson
// Issue #5261: Added a flag to track when license is not available
//
// Revision 1.5  2006/12/21 16:45:30  gdavidson
// Implemented LockApplication and modified the login
//
// Revision 1.4  2006/12/19 15:33:26  gdavidson
// Issue #5019: Added Login method
//
// Revision 1.3  2006/11/11 01:34:35  gdavidson
// Modified the OnCreateViewerProcess method
//
// Revision 1.2  2006/11/06 17:34:11  gdavidson
// Modified how we access the running object table
//
// Revision 1.1  2006/10/30 15:41:21  gdavidson
// Moved from Viatronix.Console (IVewerController renamed ILauncher)
//
// Revision 1.13  2006/08/10 19:04:55  gdavidson
// Issue #4922: Allow launching of alternate patient orientations.
//
// Revision 1.12  2006/07/25 17:15:56  gdavidson
// Added user name and login to the xml
//
// Revision 1.11  2006/07/18 14:52:27  mkontak
// Fix rights and transferring into the system
//
// Revision 1.10  2006/07/07 13:24:28  gdavidson
// Implements the vc interface for communication with console
//
// Revision 1.9  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.8  2006/04/27 19:45:21  mkontak
// Make field non case sensitive
//
// Revision 1.7  2006/04/07 20:17:06  gdavidson
// Issue #4558: Changed the in IViewer interface
//
// Revision 1.6  2006/04/06 16:12:19  gdavidson
// Issue #4684: Chnaged the session description to match the V3D viewers
//
// Revision 1.5  2006/03/09 21:08:57  gdavidson
// Moved ShowWindow method from Viatroinx.v3D.UI.Utilities to Viatroinx.v3D.Core.WindowUtilities
//
// Revision 1.4  2006/02/06 20:09:35  gdavidson
// Added exception handling to the GetViewer method
//
// Revision 1.3  2006/01/16 14:28:57  gdavidson
// Fixed how the interface is released in the ViewerClosing method.
//
// Revision 1.2  2006/01/03 16:08:21  gdavidson
// Added a release calls for the managed wrapper when closing the viewer.
//
// Revision 1.1  2005/12/14 20:03:01  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/VCLauncher.cs,v 1.12.2.14 2008/10/03 19:33:30 kchalupa Exp $
// $Id: VCLauncher.cs,v 1.12.2.14 2008/10/03 19:33:30 kchalupa Exp $

#endregion
