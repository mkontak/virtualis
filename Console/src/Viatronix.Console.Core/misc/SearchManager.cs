// $Id: SearchManager.cs,v 1.5.2.1 2007/03/26 18:20:45 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Threading; 
using System.Xml;
using Viatronix.UI;
using Viatronix.Encryption;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix.Console
{
	/// <summary>
  /// Implements functionality for all search operations.
  /// </summary>
	public sealed class SearchManager : IDisposable
	{

    #region fieldss

     /// <summary>
    /// reference to the currently loaded IBrowserProvider
    /// </summary>
    private IAsynchronousBrowserProvider m_browserProvider = null;

    /// <summary>
    /// reference to a XmlBuilder
    /// </summary>
    private XmlBuilder m_query = new XmlBuilder("study");

    /// <summary>
    /// reference to the currently executing search thread
    /// </summary>
    private SearchThread m_searchThread = null;

    /// <summary>
    /// search completed event handler for the search thread
    /// </summary>
    private ProviderEventHandler m_completedHandler = null;

 
    /// <summary>
    /// search failed event handler for the search thread
    /// </summary>
    private ProviderEventHandler m_failedHandler = null;


    /// <summary>
    /// search completed event handler for the search thread
    /// </summary>
    private ProviderEventHandler m_updateCompleted = null;


    /// <summary>
    /// search failed event handler for the search thread
    /// </summary>
    private ProviderEventHandler m_updateFailed = null;


    /// <summary>
    /// tracks the disposal for the object
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// Reference to the frame.
    /// </summary>
    private Frame m_frame = null;

    /// <summary>
    /// reset event event
    /// </summary>
    public event EventHandler ResetSearch = null;

    /// <summary>
    /// search completed event
    /// </summary>
    public event StudyCollectionDelegate SearchCompleted = null;

    /// <summary>
    /// search failed event
    /// </summary>
    public event MessageDelegate SearchFailed = null;

    /// <summary>
    /// search started event
    /// </summary>
    public event EventHandler SearchStarted = null;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SearchManager object.
    /// </summary>
    /// <param name="frame">reference to a parent frame</param>
    /// <param name="provider">Asynchronous Browser Provider</param>
    public SearchManager(Frame frame, IAsynchronousBrowserProvider provider)
    {
      m_frame           = frame;
      m_browserProvider = provider;

      //m_dataProvider = CreateDataProvider();
      m_searchThread = new SearchThread(m_browserProvider, m_query);

      m_completedHandler = new ProviderEventHandler(this.OnGetConpleted);
      m_failedHandler = new ProviderEventHandler(this.OnFailed);

      // create a new data provider and hook up its events
      //m_dataProvider = CreateDataProvider();
      m_browserProvider.Completed += m_completedHandler;
      m_browserProvider.Failed += m_failedHandler;

      m_updateCompleted = new ProviderEventHandler(this.UpdateSearchCompleted);
      m_updateFailed = new ProviderEventHandler(this.OnSearchFailed);
    } // SearchManager( browser, initializeDataProvider )


    /// <summary>
    /// Destructor
    /// </summary>
    ~SearchManager()
    {
      Dispose( false );
    } // ~SearchManager()

    #endregion

    #region properties

    /// <summary>
    /// Returns a reference toa DataProvider.
    /// </summary>
    public IAsynchronousBrowserProvider BrowserProvider
    {
      get { return m_browserProvider; }
    } // DataProvider


    /// <summary>
    /// Gets or sets the XmlBuilder
    /// </summary>
    public XmlBuilder XmlBuilder
    {
      get { return m_query; }
      set { m_query = value; }
    } // Query


    #endregion

    #region methods

    /// <summary>
    /// Performs a search.
    /// </summary>
    public void Search()
    {
      Search( m_query );
    } // Search()

    
    /// <summary>
    /// Performs a search with the provided XmlBuilder
    /// </summary>
    /// <param name="builder">search criteria</param>
    public void Search(XmlBuilder query)
    {
      m_query = query;
      Search( m_query, true );
    } // Search( builder )


    /// <summary>
    /// Performs a search with the provided XmlBuilder
    /// </summary>
    /// <param name="builder">search criteria</param>
    /// <param name="isAsyncSearch">true for asynchronous search; otherwise false</param>
    public void Search(XmlBuilder query, bool isAsyncSearch)
    {

      // raise a search started event
      OnSearchStarted();

      if( isAsyncSearch )
        m_browserProvider.GetStudiesAsynchronously(query);
      else
        m_browserProvider.GetStudies(query);
    } // Search( builder, isAsyncSearch )
     
        
    /// <summary>
    /// Aborts the current thread.
    /// </summary>
    public void Cancel()
    {

      m_searchThread.AbortAndWait();

      m_browserProvider.Abort();

    } // Cancel()


    /// <summary>
    /// Aborts and disconnects the current search thread.
    /// </summary>
    public void Reset()
    {
      m_searchThread.AbortAndWait();

      if( this.ResetSearch != null )
        this.ResetSearch( this, EventArgs.Empty );
    } // Reset()




    /// <summary>
    /// Updates the view browser and raises a SearchCompleted event.
    /// </summary>
    /// <param name="collection">Collection of studies</param>
    private void UpdateSearchCompleted( ProviderEventArgs args )
    {

      List<Study> studies = new List<Study>();

      EntityCollection.Load(studies, args.Message);

      ISearchView searchView = Frame.ActiveFrame.ActiveView as ISearchView;
      if (searchView != null)
        searchView.AddAllSearchResults(studies);

      if (this.SearchCompleted != null)
        this.SearchCompleted(studies);
    } // UpdateSearchCompleted( collection )



    /// <summary>
    /// Raises the search failed event
    /// </summary>
    /// <param name="message">Error message</param>
    private void OnSearchFailed( ProviderEventArgs args )
    {
      if( this.SearchFailed != null )
        this.SearchFailed(args.Message);
    } // OnSearchFailed( message )


    /// <summary>
    /// Raises the search started event.
    /// </summary>
    private void OnSearchStarted()
    {
      if( m_frame.InvokeRequired )
        Frame.ActiveFrame.Invoke( new System.Windows.Forms.MethodInvoker( OnSearchStarted ) );
      else
      {
        if( this.SearchStarted != null )
          this.SearchStarted( m_frame, EventArgs.Empty );
      }
    } // OnSearchStarted()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the IDataProvider's search completed handler.
    /// </summary>
    /// <param name="collection">Collection of studies</param>
    private void OnGetConpleted( ProviderEventArgs args )
    {

      if (args.Resource != "study" || string.IsNullOrEmpty(args.Message))
        return;


     if (Global.Frame.InvokeRequired)
        Global.Frame.Invoke(m_updateCompleted, (new object [] { args} ));
      else
        m_updateCompleted(args);

    } // OnSearchCompletedHandler( collection )



    /// <summary>
    /// Handles the IDataProvider's study found handler.
    /// </summary>
    /// <param name="message">Error message.</param>
    private void OnFailed( ProviderEventArgs args )
    {
      Viatronix.Logging.Log.Error(string.Format("{0}-{1} failed. {2}", args.Operation, args.Resource, args.Message), "SearchManager", "OnFailed");
      m_frame.Invoke( m_updateFailed, ( new object [] { args } ) );
    } // OnFailed( message )

    #endregion

    #region IDisposable Members

    /// <summary>
    /// Releases any resources
    /// </summary>
    public void Dispose()
    {
      Dispose( true );
      GC.SuppressFinalize( this );
    } // Dispose()


    /// <summary>
    /// Releases any resourses
    /// </summary>
    /// <param name="disposing">true frees any managed resourses</param>
    private void Dispose( bool disposing )
    {
      if( !m_disposed )
      {
        if( disposing )
        {
          m_browserProvider.Completed -= m_completedHandler;
          m_browserProvider.Failed    -= m_failedHandler;
        }
      }

      m_disposed = true;
    } // Dispose( disposing )

    #endregion

  } // class SearchManager

  #region SearchThread class

  /// <summary>
  /// Implements the search thread
  /// </summary>
  public class SearchThread
  {

    #region fields

    /// <summary>
    /// Reference to a XmlBuilder
    /// </summary>
    private XmlBuilder m_query = null;

    /// <summary>
    /// Reference to a IDataProvider
    /// </summary>
    private IAsynchronousBrowserProvider m_browserProvider = null;

    /// <summary>
    /// determines if the thread has been aborted
    /// </summary>
    private bool m_abort = false;

    /// <summary>
    /// collection of studies
    /// </summary>
    private List<Study> m_studies = new List<Study>();

    /// <summary>
    /// Reference to a thread
    /// </summary>
    private Thread m_thread = null;

 
    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SearchThread.
    /// </summary>
    /// <param name="provider">IDataProvider</param>
    /// <param name="builder">XmlBuilder</param>
    public SearchThread(IAsynchronousBrowserProvider provider, XmlBuilder query)
    {
      m_browserProvider = provider;
      m_query = query;
      m_thread = new Thread( new ThreadStart( this.Search ));
      m_thread.Name = Guid.NewGuid().ToString();
      m_thread.IsBackground = true;

      m_browserProvider.Aborted += new ProviderEventHandler(this.OnCanceledSearchHandler);
    } // SearchThread( provider, builder )

    #endregion

    #region properties

    /// <summary>
    /// Returns an IDataProvider.
    /// </summary>
    public IBrowserProvider Provider
    {
      get { return m_browserProvider; }
    } // Provider


    /// <summary>
    /// Returns a collection of studies.
    /// </summary>
    public List<Study> Studies
    {
      get { return m_studies; }
    } // Studies

    #endregion

    #region methods

    /// <summary>
    /// Thread's run method.
    /// </summary>
    public void Run()
    {
      m_thread.Start();
    } // Run()


    /// <summary>
    /// Aborts the thread's execution.
    /// </summary>
    public void AbortAndWait()
    {
      m_browserProvider.Abort();
      m_abort = true;

      // wait for the thread to complete
      if( m_thread.IsAlive )
        m_thread.Join();
    } // Abort()


    /// <summary>
    /// Performs the search operation.
    /// </summary>
    private void Search()
    {
      try
      {
        m_studies = m_browserProvider.GetStudies(m_query);
      }
      catch (Exception ex)
      {
        Logging.Log.Error("Search failed : " + ex.Message, "SearchManager", "Search");
        throw;
      }
    } // Search()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the cancel search event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An CancelEventArgs that contains the event data.</param>
    private void OnCanceledSearchHandler( ProviderEventArgs args )
    {
      //args.Cancel = m_abort;
    } // OnCanceledSearchHandler( sender, args )

    #endregion

  } // class SearchThread

  #endregion

} // namespace Viatronix.Console


#region revision history

// $Log: SearchManager.cs,v $
// Revision 1.5.2.1  2007/03/26 18:20:45  gdavidson
// Issue #5516: When aborting a search block the main thread until the executing search thread has comlpeted
//
// Revision 1.5  2007/03/13 12:18:09  mkontak
// coding standards
//
// Revision 1.4  2007/03/08 18:08:36  mkontak
// Added exception handling to try and find the arithmetic overflow
//
// Revision 1.3  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.2  2007/03/05 22:03:53  romy
// added log for tracking the arithmetic overflow error
//
// Revision 1.1  2006/10/30 15:42:55  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.6  2006/09/06 13:16:31  mkontak
// Issue 4969
//
// Revision 1.5  2006/08/22 17:43:38  mkontak
// Fix abort when doing PACS search. Added new flag to disable the
// imbedded image query within the series query to save time.
//
// Revision 1.4  2006/07/10 18:35:11  gdavidson
// Issue #4836: Modified SearchManager
//
// Revision 1.3  2006/02/03 13:47:30  mkontak
// Added try/catch aroune search start
//
// Revision 1.2  2005/11/02 14:19:10  gdavidson
// Removed the PaneExpanded event handlers
//
// Revision 1.1  2005/10/18 17:47:47  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/misc/SearchManager.cs,v 1.5.2.1 2007/03/26 18:20:45 gdavidson Exp $
// $Id: SearchManager.cs,v 1.5.2.1 2007/03/26 18:20:45 gdavidson Exp $

#endregion
