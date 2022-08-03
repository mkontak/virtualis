using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.CompilerServices;
using Viatronix.UI;
using Viatronix.v3D.Core;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
  public partial class SearchControl : UserControl
  {
   #region fields


    /// <summary>
    /// Search completed event handler.
    /// </summary>
    private StudyCollectionDelegate m_searchCompletedHandler = null;

    /// <summary>
    /// Search failed event handler.
    /// </summary>
    private MessageDelegate m_searchFailedHandler = null;

    /// <summary>
    /// Search started event handler.
    /// </summary>
    private EventHandler m_searchStartedHandler = null;

    /// <summary>
    /// determines how log the results lable is visible
    /// </summary>
    private Timer m_resultsTimer = null;

    /// <summary>
    /// clear button
    /// </summary>
    private Viatronix.UI.Button m_clearButton;

    /// <summary>
    /// label for results
    /// </summary>
    private Label m_resultsLabel;

    /// <summary>
    /// search button
    /// </summary>
    private Viatronix.UI.Button m_searchButton;

    /// <summary>
    /// cancel button
    /// </summary>
    private Viatronix.UI.Button m_cancelButton;

    /// <summary>
    /// reference to a search manager
    /// </summary>
    private SearchManager m_searchManager = null;

    /// <summary>
    /// reference to the ISearchPane
    /// </summary>
    private ISearchPane m_searchPane = null;

    /// <summary>
    /// search progressbar
    /// </summary>
    private Viatronix.UI.ProgressBar2 m_progressbar;

    /// <summary>
    /// pulse generator
    /// </summary>
    private PulseGenerator m_pulseGenerator = null;

    ///// <summary>
    ///// Clear event
    ///// </summary>
    public event EventHandler Clear = null;

    ///// <summary>
    ///// BeginSearch event
    ///// </summary>
    public event EventHandler BeginSearch = null;


    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a SearchPane control.
    /// </summary>
		public SearchControl()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      InitializeComponent();

      m_resultsTimer = new System.Windows.Forms.Timer();
      m_resultsTimer.Interval = 5000;
      m_resultsTimer.Tick += new EventHandler(this.OnResultTimerTickHandler);

      m_searchCompletedHandler = new StudyCollectionDelegate(this.OnSearchCompletedHandler);
      //m_searchFailedHandler = new EventHandler( this.OnSearchFailedHandler );
      m_searchStartedHandler = new EventHandler(this.OnSearchStartedHandler);

 
		} // SearchControl()

    #endregion
    
    #region properties

    /// <summary>
    /// Gets or sets the search pane
    /// </summary>
    [Browsable( false )]
    public ISearchPane SearchPane
    {
      get { return m_searchPane; }
      set { m_searchPane = value; }
    } // SearchPane

    #endregion

    #region methods

    /// <summary>
    /// Resets the results label.
    /// </summary>
    public void Reset()
    {
      m_resultsLabel.Text = string.Empty;
    } // Reset()


    /// <summary>
    /// Performs the data search.
    /// </summary>
    public void Search()
    {
    //  Search( new XmlBuilder("study") );
    } // Search()


    /// <summary>
    /// Performs the data search.
    /// </summary>
    /// <param name="builder">The where clause for the search.</param>
    [MethodImpl(MethodImplOptions.Synchronized)]
    public void Search(XmlBuilder query)
    {
      this.Searcher.Reset();
      this.Searcher.Search(query);
    } // Search( builder )



    #endregion

    #region event handlers

    /// <summary>
    /// Handles the search click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSearchClickHandler(object sender, EventArgs args)
    {

      if (this.BeginSearch != null)
        this.BeginSearch(sender, args);

      Search(m_searchPane.CreateQuery());
    } // OnSearchClickHandler( sender, args )


    /// <summary>
    /// Handles the cancel click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCancelClickHandler( object sender, EventArgs args )
    {
      this.Searcher.Cancel();

      m_searchButton.Visible = true;
      m_cancelButton.Visible = false;

      if (m_pulseGenerator != null)
        m_pulseGenerator.StopPulse();

      m_pulseGenerator = null;

      m_progressbar.Visible = false;
      m_resultsLabel.Visible = true;
    } // OnSearchClickHandler( sender, args )


    /// <summary>
    /// Handles the clear click event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnClearClickhandler( object sender, EventArgs args )
    {
      if (this.Clear != null)
        this.Clear(sender, args);
    } // OnClearClickHandler( sender, args )


    /// <summary>
    /// Handles the search complete event.
    /// </summary>
    /// <param name="studies">A collection of studies.</param>
    private void OnSearchCompletedHandler( List<Study> studies )
    {
      m_searchButton.Visible = true;
      m_cancelButton.Visible = false;

      if (m_pulseGenerator != null)
        m_pulseGenerator.StopPulse();

      Global.Frame.ProgressBar.Visible = false;
      m_pulseGenerator = null;
      m_progressbar.Visible = false;

      m_resultsLabel.Visible = true;
      m_resultsLabel.Text = studies.Count + " Stud" + ((studies.Count == 1) ? "y" : "ies") + " Found";

      m_resultsTimer.Start();
    } // OnSearchCompletedHandler( studies )

 
    /// <summary>
    /// Handles the search failed event.
    /// </summary>
    /// <param name="message">The failed search message</param>
    private void OnSearchFailedHandler( string message )
    {
      m_searchButton.Visible = true;
      m_cancelButton.Visible = false;

      if (m_pulseGenerator != null)
        m_pulseGenerator.StopPulse();

      Global.Frame.ProgressBar.Visible = false;
      m_pulseGenerator = null;
      m_progressbar.Visible = false;

      m_resultsLabel.Visible = true;
    } // OnSearchCompletedHandler( studies )


    /// <summary>
    /// Handles the search started event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSearchStartedHandler( object sender, EventArgs args )
    {
      m_resultsLabel.Visible = false;
      m_searchButton.Visible = false;
      m_cancelButton.Visible = true;

      m_pulseGenerator = new PulseGenerator(m_progressbar);
      m_progressbar.Value = 0;
      m_pulseGenerator.Interval = 10;

      m_progressbar.Pulse = true;
      m_progressbar.Text = "Searching...";
      m_progressbar.Visible = true;
      m_pulseGenerator.StartPulse();

      m_resultsTimer.Stop();
    } // OnSearchStartedHandler( sender, args )


    /// <summary>
    /// Removes the search completed event handler.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangingHandler( object sender, EventArgs args )
    {
      // ignore the abort SearchCompleted event by removing its handler 
      this.Searcher.SearchCompleted -= m_searchCompletedHandler;
      this.Searcher.SearchFailed -= m_searchFailedHandler;
      this.Searcher.SearchStarted -= m_searchStartedHandler;
    } // OnViewChangingHandler( sender, args )


    /// <summary>
    /// Adds the search completed event handler.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnViewChangedHandler( object source, EventArgs args )
    {
      m_resultsLabel.Text = string.Empty;
    } // OnViewChangedHandler( source, args )


    /// <summary>
    /// Hides the results label after 10 seconds
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnResultTimerTickHandler( object sender, EventArgs args )
    {
      m_resultsTimer.Stop();

      m_resultsLabel.Text = string.Empty;
      m_resultsLabel.Invalidate();     
    } // OnResultTimerTickHandler( sender, args )

    #endregion

    #region ISearchable Members

    ///// <summary>
    ///// Gets or set the search manager objcet.
    ///// </summary>
    //[Browsable( false )]
    public SearchManager Searcher
    {
      get { return m_searchManager; }
      set
      {
        if (m_searchManager != null)
        {
          m_searchManager.SearchCompleted -= m_searchCompletedHandler;
          m_searchManager.SearchFailed -= m_searchFailedHandler;
          m_searchManager.SearchStarted -= m_searchStartedHandler;
        }

        if (value != null)
        {
          m_searchManager = value;
          m_searchManager.SearchCompleted += m_searchCompletedHandler;
          m_searchManager.SearchFailed += m_searchFailedHandler;
          m_searchManager.SearchStarted += m_searchStartedHandler;
        }
      }
    }


    #endregion

  }
}
