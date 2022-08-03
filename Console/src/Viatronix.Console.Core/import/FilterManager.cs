// $Id: FilterManager.cs,v 1.1.2.1 2007/03/19 23:45:28 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Threading;
using Viatronix.v3D.Core;
using Viatronix.v3D.UI;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Responsible for all filter functionality.
	/// </summary>
	public sealed class FilterManager
	{

    #region fields

    /// <summary>
    /// ProgressDialog object
    /// </summary>
    private ProgressDialog m_dialog = null;

    /// <summary>
    /// implements the fitler operation
    /// </summary>
    private FilterThread m_filterThread = null;

    /// <summary>
    /// thread which executes the filter operation
    /// </summary>
    private Thread m_thread = null;

    /// <summary>
    /// the parent form
    /// </summary>
    private System.Windows.Forms.Control m_parent = null;
    
    /// <summary>
    /// raised when a filter has finished
    /// </summary>
    public FilterEventHandler FilterCompleted = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a FilterManager object.
    /// </summary>
    /// <param name="parent">Parent form</param>
		public FilterManager( System.Windows.Forms.Control parent )
		{
      m_parent = parent;

			m_filterThread = new FilterThread();
      m_filterThread.FilterCompleted += new FilterEventHandler( this.OnFilterCompleted );
      m_filterThread.ThreadEnded += new EventHandler( this.OnThreadEndedHandler );
		} // FilterManager()

    #endregion

    #region methods

    /// <summary>
    /// Filters the specified directory.
    /// </summary>
    /// <param name="directory">directory containing the data</param>
    /// <returns>Patient object if successful; otherwise null</returns>
    public Patient Filter( string directory )
    {
      PatientCollection patients = Filter( new string[] { directory });
      if( patients.Count > 0 )
        return patients[0];

      return null;
    } // Filter( directory )


    /// <summary>
    /// Filters the specified directories.
    /// </summary>
    /// <param name="directories">collection of directories containing data</param>
    /// <returns>A collection of patient objects</returns>
    public PatientCollection Filter( string[] directories )
    {
      m_dialog = new ProgressDialog( 0, 100 );
      m_filterThread.Dialog = m_dialog;
      m_filterThread.Directories = directories;

      m_thread = new Thread( new ThreadStart( m_filterThread.Filter ));
      m_thread.IsBackground = true;
      m_thread.Start();

      m_dialog.ShowDialog( m_parent );

      m_thread = null;
      m_dialog = null;

      return m_filterThread.Patients;
    } // Filter( directories )


    /// <summary>
    /// Raises the filter completed event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    private void OnFilterCompleted( object sender, FilterEventArgs args )
    {
      if( this.FilterCompleted != null )
        this.FilterCompleted( this, args );
    } // OnFilterCompleted( sender, args )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the ThreadEnded event of the FilterThread object.
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
    /// Handles the FilterCompleted event of the FilterThread object.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An FilterEventArgs that contains the event data.</param>
    private void OnFilterCompletedHandler( object sender, FilterEventArgs args )
    {
      m_parent.Invoke( new FilterEventHandler( OnFilterCompleted ), new object[] { sender, args });
    } // OnFilterCompletedHandler( sender, args )

    #endregion

	} // class FilterManager

  #region delegates

  /// <summary>
  /// Represents the method that will handle the event that has FilterEventArgs as data.
  /// </summary>
  public delegate void FilterEventHandler( object sender, FilterEventArgs args );

  #endregion

  #region class FilterThread

  /// <summary>
  /// Implments the filter operations.
  /// </summary>
  internal class FilterThread
  {

    #region fields

    /// <summary>
    /// Represents the method that will handle the event that has prgress information.
    /// </summary>
    private delegate void ProgressDelegate( int progress, int max );

    /// <summary>
    /// ProgressDialog object
    /// </summary>
    private ProgressDialog m_dialog = null;

    /// <summary>
    /// collection of directories being filtered.
    /// </summary>
    private string[] m_directories = null;

    /// <summary>
    /// collection of patients that have been created by the filters
    /// </summary>
    private PatientCollection m_patients = new PatientCollection();

    /// <summary>
    /// cancel event args used to abort the filter operation
    /// </summary>
    private System.ComponentModel.CancelEventArgs m_cancelArgs = new System.ComponentModel.CancelEventArgs( false );

    /// <summary>
    /// event handler assigned to the cancel event
    /// </summary>
    private EventHandler m_cancelHandler = null;

    /// <summary>
    /// raises the FilterCompleted event
    /// </summary>
    public FilterEventHandler FilterCompleted = null;

    /// <summary>
    /// raises he ThreadEnded event
    /// </summary>
    public EventHandler ThreadEnded = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a FilterThread
    /// </summary>
    public FilterThread()
    {
      m_cancelHandler = new EventHandler( this.OnCancelHandler );
    } // FilterThread( dialog )

    #endregion

    #region properties

    /// <summary>
    /// Returns a collection of filtered patients.
    /// </summary>
    public PatientCollection Patients
    {
      get { return m_patients; }
    } // Patients


    /// <summary>
    /// Gets or sets a collection of directories being filtered.
    /// </summary>
    public string[] Directories
    {
      get { return m_directories; }
      set { m_directories = value; }
    } // Directories


    /// <summary>
    /// Gets or sets the progress dialog.
    /// </summary>
    public ProgressDialog Dialog
    {
      get { return m_dialog; }
      set 
      {
        m_dialog = value; 
        m_dialog.SetMessage( "Importing..." );
      }
    } // Dialog

    #endregion

    #region methods

    /// <summary>
    /// Starts the filter operation.
    /// </summary>
    public void Filter()
    {
      m_dialog.Cancel += m_cancelHandler;
      m_cancelArgs.Cancel = false;

      m_patients.Clear();
      try
      {
        foreach( string directory in m_directories )
        { 
          Patient patient = Filter( directory );
          if( patient != null )
            m_patients.Add( patient );
        }
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( "Filter failed [ERROR=" + e.Message + "]" );
      }
      finally
      {
        m_dialog.Cancel -= m_cancelHandler;
      }

      /// stall to give time for the completed message to take
      System.Threading.Thread.Sleep(250);

      if( this.ThreadEnded != null )
        this.ThreadEnded( this, EventArgs.Empty );

    } // Filter()


    /// <summary>
    /// Filters the specified directory.
    /// </summary>
    /// <param name="directory">directory of data</param>
    /// <returns>Patient object or null</returns>
    private Patient Filter( string directory )
    {
      // retrieve a filter
      IFileStructureFilter filter = FileStructureFilterFactory.CreateFilter( directory );
      if( filter == null )
      {
        Viatronix.v3D.Logging.Logger.Wrn( "Unable to filter path. [PATH=" + directory + "]" );

        // raise event for failed filter
        if( this.FilterCompleted != null )
        {
          FilterEventArgs args = new FilterEventArgs();
          args.Patient = null;
          args.Filter = null;
          args.State = FilterState.Failed;
          args.Directory = directory;

          this.FilterCompleted( this, args );
        }

        return null;
      }

      /// Yield to the create
      System.Threading.Thread.Sleep(100);
        
      m_dialog.SetProgress( 0, 100, "Filtering..." );
      FilterProgressEventHandler handler = new FilterProgressEventHandler( this.OnFilterProgressHandler );
      try
      {
        filter.Progress += handler;
        Patient patient = filter.Process( directory, m_cancelArgs ); 

        // raise event for successful filter
        if( this.FilterCompleted != null )
        {
          FilterEventArgs args = new FilterEventArgs();
          args.Patient = patient;
          args.Filter = filter;
          args.State = (( patient != null ) ? FilterState.Filtered : FilterState.Failed );
          args.Directory = directory;

          this.FilterCompleted( this, args );
        }

        return patient;
      }
      catch( FileStructureFilterException filterEx )
      {
        Viatronix.v3D.Logging.Logger.Fyi( "Filter operation cancelled by user. [MESSAGE=" + filterEx.Message + "]" );
        throw filterEx;
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( "Filter was unable to process path [PATH=" + directory + "] [ERROR=" + e.Message + "]" );
        return null;
      }
      finally
      {
        filter.Progress -= handler;
      }
    } // Filter( directory )


    #endregion

    #region event handlers

    /// <summary>
    /// Handles the cancel event from the progress dialog.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCancelHandler( object sender, EventArgs args )
    {
      m_cancelArgs.Cancel = true;
    } // OnCancelHandler( sender, args )


    /// <summary>
    /// Handles the filter progress event.
    /// </summary>
    /// <param name="progress">Filter progress</param>
    /// <param name="max">Max progress</param>
    private void OnFilterProgressHandler( int progress, int max )
    {
      m_dialog.Invoke( new ProgressDelegate( m_dialog.SetProgress ), new object[] { progress, max });
    } // OnFilterProgressHandler( progress, max )

    #endregion

  } // class FilterThread

  #endregion

  #region enum FilterState

  /// <summary>
  /// enumeration of possible filter states
  /// </summary>
  public enum FilterState
  {
    Error,
    Failed,
    Filtered,
    Imported,
  } // FilterState

  #endregion

  #region class FilterEventArgs

  /// <summary>
  /// Contains data relevent to a filter operation.
  /// </summary>
  public class FilterEventArgs : EventArgs
  {

    #region fields

    /// <summary>
    /// patient object
    /// </summary>
    private Patient m_patient = null;

    /// <summary>
    /// state of the filter
    /// </summary>
    private FilterState m_state = FilterState.Failed;

    /// <summary>
    /// IFileStructureFilter object
    /// </summary>
    private IFileStructureFilter m_filter = null;

    /// <summary>
    /// Directory being filtered.
    /// </summary>
    private string m_directory = string.Empty;

    /// <summary>
    /// The index of the filter.
    /// </summary>
    private int m_index = 0;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public FilterEventArgs()
    {
    } // FilterEventArgs()

    #endregion

    #region properties

    /// <summary>
    /// Gets or set the patient
    /// </summary>
    public Patient Patient
    {
      get { return m_patient; }
      set { m_patient = value; }
    } // Patient


    /// <summary>
    /// Gets or sets the filter state
    /// </summary>
    public FilterState State
    {
      get { return m_state; }
      set { m_state = value; }
    } // State


    /// <summary>
    /// Gets or set the IFileStructureFilter object
    /// </summary>
    public IFileStructureFilter Filter
    {
      get { return m_filter; }
      set { m_filter = value; }
    } //Filter


    /// <summary>
    /// Gets or sets the directory being filtered.
    /// </summary>
    public string Directory
    {
      get { return m_directory; }
      set { m_directory = value; }
    } // Directory


    /// <summary>
    /// Gets or sets the index.
    /// </summary>
    public int Index
    {
      get { return m_index; }
      set { m_index = value; }
    } // Index

    #endregion

  } // class FilterEventArgs

  #endregion

} // namespace Viatronix.v3D.Console

#region revision history

// $Log: FilterManager.cs,v $
// Revision 1.1.2.1  2007/03/19 23:45:28  mkontak
// Fix crash wjen cancelling filter and import
//
// Revision 1.1  2006/10/30 15:34:46  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.5  2006/09/19 13:16:31  mkontak
// Fix for importing
//
// Revision 1.4  2006/08/18 16:29:00  gdavidson
// Set the threads to execute in the background
//
// Revision 1.3  2006/02/13 16:33:19  gdavidson
// Create new ProgressDialog in the Filter method.
//
// Revision 1.2  2006/02/10 21:29:23  gdavidson
// Added Error state to the FilterStates
//
// Revision 1.1  2006/02/07 18:07:55  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/FilterManager.cs,v 1.1.2.1 2007/03/19 23:45:28 mkontak Exp $
// $Id: FilterManager.cs,v 1.1.2.1 2007/03/19 23:45:28 mkontak Exp $

#endregion