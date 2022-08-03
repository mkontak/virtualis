// $Id: ImportManager.cs,v 1.2.2.4 2007/03/26 20:40:35 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Threading;
using Viatronix.v3D.Core;
using Viatronix.v3D.UI;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Responsible for all import functionality.
	/// </summary>
	public class ImportManager
	{

    #region fields

    /// <summary>
    /// Aborted flag
    /// </summary>
    private bool m_aborted = false;

    /// <summary>
    /// progress dialog
    /// </summary>
    private ProgressDialog m_dialog = null;

    /// <summary>
    /// implements the import operation
    /// </summary>
    private ImportThread m_importThread = null;

    /// <summary>
    /// thread which executes the import operation
    /// </summary>
    private Thread m_thread = null;

    /// <summary>
    /// the parent form
    /// </summary>
    private System.Windows.Forms.Control m_parent = null;

    /// <summary>
    /// IDataProvider object
    /// </summary>
    private IDataProvider m_dataProvider = null;

    /// <summary>
    /// User object.
    /// </summary>
    private User m_user = null;

    /// <summary>
    /// raised when a patient has been imported
    /// </summary>
    public FilterEventHandler PatientImported = null;

    #endregion


    #region construction

    /// <summary>
    /// Creates a new instance of a ImportManager.
    /// </summary>
    /// <param name="parent">Parent control.</param>
    /// <param name="dataProvider">IDataProvider</param>
		public ImportManager( System.Windows.Forms.Control parent, IDataProvider dataProvider, User user )
		{
      m_parent = parent;
      m_dataProvider = dataProvider;
      m_user = user;

      m_importThread = new ImportThread( dataProvider, user );
      m_importThread.ThreadEnded += new EventHandler( this.OnThreadEndedHandler );
      m_importThread.PatientImported += new FilterEventHandler( this.OnPatientImportedHandler );

		} // ImportManager( parent, dataProvider )

    #endregion

    #region methods

    /// <summary>
    /// Imports the collection of filtered patients into the system.
    /// </summary>
    /// <param name="filterArgs">ArrayList of FilterEventArgs</param>
    /// <returns>A collection of FilterEventArgs that failed to import</returns>
    public ArrayList Import( ArrayList filterArgs )
    {
      m_dialog = new ProgressDialog( 0, 100 );
      m_dialog.EnableCloseBox = true;
      m_importThread.Dialog = m_dialog;
      m_importThread.Failures.Clear();
      m_importThread.FilterArgs = filterArgs;

      m_thread = new Thread( new ThreadStart( m_importThread.Import ));
      m_thread.IsBackground = true;
      m_thread.Start();

      m_dialog.ShowDialog( m_parent );

      m_thread = null;
      m_dialog = null;

      return m_importThread.Failures;
    } // Import( filterArgs )


    /// <summary>
    /// Raises the PatientImport event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    protected void OnPatientImported(object sender, FilterEventArgs args)
    {
      if (!m_aborted)
      {
        if (this.PatientImported != null)
          this.PatientImported(sender, args);
      }
    } // nPatientImported( sender, args )


    #endregion
    
    #region event handlers

    /// <summary>
    /// Handles the ThreadEnded event of the ImportThread object.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnThreadEndedHandler( object sender, EventArgs args )
    {
      if (m_dialog != null)
      {
        m_parent.Invoke(new System.Windows.Forms.MethodInvoker(m_dialog.Close));
      }
    } // OnThreadEndedHandler( sender, args )


    /// <summary>
    /// Handles the PatientImported event of the ImportThread object.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    private void OnPatientImportedHandler( object sender, FilterEventArgs args )
    {
      m_parent.Invoke( new FilterEventHandler( OnPatientImported ), new object[] { this, args });
    } // OnPatientImportedHandler( sender, args )


    /// <summary>
    /// Handles the PatientImported event of the ImportThread object.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">Evenet args</param>
    private void OnImportAbortedHandler(object sender, EventArgs args)
    {
    } // OnImportAbortedHandler( sender, args )


    #endregion

	} // class ImportManager

  #region class ImportThread

  /// <summary>
  /// Implements the import threading functionality.
  /// </summary>
  internal class ImportThread
  {
    
    #region fields

    /// <summary>
    /// IDataProvider object.
    /// </summary>
    private IDataProvider m_dataProvider = null;

    /// <summary>
    /// User object.
    /// </summary>
    private User m_user = null;

    /// <summary>
    /// Completed flag
    /// </summary>
    private bool m_completed = false;

    /// <summary>
    /// ProgressDialog object.
    /// </summary>
    private ProgressDialog m_dialog = null;

    /// <summary>
    /// collection of FilterEventArgs
    /// </summary>
    private ArrayList m_filtersArgs = new ArrayList();

    /// <summary>
    /// collection of FilterEventArgs that didnot import properly
    /// </summary>
    private ArrayList m_failures = new ArrayList();

    /// <summary>
    /// reference to the current FilterEventArg
    /// </summary>
    private FilterEventArgs m_currentArgs = null;

    /// <summary>
    /// event handler for the import operation
    /// </summary>
    private ImportingEventHandler m_importHandler = null;

    /// <summary>
    /// raised when a patient is imported into the system
    /// </summary>
    public FilterEventHandler PatientImported = null;

    /// <summary>
    /// raised when the thread has completed
    /// </summary>
    public EventHandler ThreadEnded = null;



    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of an ImportThread
    /// </summary>
    /// <param name="dataProvider">DataProvider</param>
    public ImportThread( IDataProvider dataProvider, User user )
    {
      m_dataProvider = dataProvider;     
      m_user = user;
      m_importHandler = new ImportingEventHandler( this.OnImportingHandler );
    } // ImportThread( dataProvider, dialog )

    #endregion

    #region properties


    /// <summary>
    /// Gets or sets a collection of FilterArgs
    /// </summary>
    public ArrayList FilterArgs
    {
      get { return m_filtersArgs; }
      set { m_filtersArgs = value; }
    } // FilterArgs


    /// <summary>
    /// Returns a collection of failed imports.
    /// </summary>
    public ArrayList Failures
    {
      get { return m_failures; }
    } // Failures


    /// <summary>
    /// Gets or sets the progress dialog.
    /// </summary>
    public ProgressDialog Dialog
    {
      get { return m_dialog; }
      set 
      {
        m_dialog = value;
        m_dialog.ShowCancel = false;
        m_dialog.ControlBox = false;              // Removes the close [X}] in the top left hand corner
        m_dialog.SetMessage( "Importing..." );
      }
    } // Dialog

    #endregion

    #region methods

    /// <summary>
    /// Import implementation.
    /// </summary>
    public void Import()
    {


      m_failures.Clear();
      m_dataProvider.Importing += m_importHandler;
      try
      {
        // iterate over the patients and retrieve the assocaited filter
        foreach( FilterEventArgs args in this.FilterArgs )
        {

          m_currentArgs = args;
          m_dialog.SetProgress( 0, 100, "Importing " + m_currentArgs.Patient.Name.ToString() );
          try
          {
            // import the patient (relative paths are determined by the filter
            m_dataProvider.Add( args.Patient, args.Filter.IsRelative );
            
            args.State = FilterState.Imported;
            if( this.PatientImported != null )
              this.PatientImported( this, args );
          }
          catch( Exception e )
          {
            Viatronix.v3D.Logging.Logger.Err( "Filter import failed. [PATIENT=" +  args.Patient.Name.FullName + "] [PATH=" +  args.Patient.Location + "]" );
            Viatronix.v3D.Logging.Logger.Err( "Exception occured while importing [ERROR=" + e.Message + "]" );

            args.State = FilterState.Failed;
            m_failures.Add( args );
          }
        }

        // iterate over the patients and delete any temporary files
        foreach( FilterEventArgs args in this.FilterArgs )
        {
          try
          {
            // the filter determines if temporary fiels exist
            if( args.Filter.RequiresDeletion )
              Directory.Delete( Path.Combine( Utilities.Registry.SystemSettings.TempDirectory, args.Patient.Location ), true );
          }
          catch( Exception e )
          {
            Viatronix.v3D.Logging.Logger.Err( "Unable to delete files from temp directory [PATH=" +  args.Patient.Location + "] [ERROR=" + e.Message + "]");
            
            args.State = FilterState.Error;
            m_failures.Add( args );
          }
        }

      }
      finally
      {
        m_completed = true;

        m_dialog.SetComplete("Importing Complete");

        m_dataProvider.Importing -= m_importHandler;
        m_currentArgs = null;

        if( this.ThreadEnded != null )
          this.ThreadEnded( m_dialog, EventArgs.Empty );
      }
    } // Import()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the progress notification.
    /// </summary>
    /// <param name="total">Total items being imported.</param>
    /// <param name="imported">Items imported.</param>
    /// <param name="file">The file being imported.</param>
    private void OnImportingHandler( int total, int imported, string file )
    {
      if (!m_completed)
      {
        if (imported == 0)
        {
          m_dialog.Maximum = total;
          m_dialog.SetMessage("Importing " + ((m_currentArgs != null) ? m_currentArgs.Patient.Name.ToString() : string.Empty));
        }
        else
        {
          m_dialog.Increment();
        }
      }
      else
      {
        m_dialog.SetComplete("Importing Complete");
      }
    } // OnImporting( total, imported, file)

    #endregion

  } // class ImportThread

  #endregion

} // namespace Viatronix.v3D.Console


#region revision history

// $Log: ImportManager.cs,v $
// Revision 1.2.2.4  2007/03/26 20:40:35  mkontak
// Fix the completion detection
//
// Revision 1.2.2.3  2007/03/23 15:02:33  mkontak
// Make sure th progess reaches 100 %
//
// Revision 1.2.2.2  2007/03/23 14:53:29  mkontak
// Disale the progess dialogs close box
//
// Revision 1.2.2.1  2007/03/19 23:45:28  mkontak
// Fix crash wjen cancelling filter and import
//
// Revision 1.2  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.4  2006/08/18 16:29:00  gdavidson
// Set the threads to execute in the background
//
// Revision 1.3  2006/02/13 16:33:34  gdavidson
// Create new ProgressDialog for the import method.
//
// Revision 1.2  2006/02/10 21:32:37  gdavidson
// Set the filter satte to failed or error on exceptional cases.
//
// Revision 1.1  2006/02/07 18:07:55  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/ImportManager.cs,v 1.2.2.4 2007/03/26 20:40:35 mkontak Exp $
// $Id: ImportManager.cs,v 1.2.2.4 2007/03/26 20:40:35 mkontak Exp $

#endregion