using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Xml;
using System.Configuration;
using System.Runtime.Remoting.Messaging;
using System.Threading.Tasks;
using System.Windows.Forms;
using Viatronix.UI;
using Viatronix.v3D.Core;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
  public partial class DatabaseSearchPane : UserControl, ISearchPane, ISearchable
  {
    #region fields

    /// <summary>
    /// timer for key presses
    /// </summary>
    private System.Windows.Forms.Timer m_timer = null;

    /// <summary>
    /// group box
    /// </summary>
    private Viatronix.UI.GroupBox m_searchGroupBox;

    /// <summary>
    /// divider
    /// </summary>
    private System.Windows.Forms.GroupBox m_studyGroupBox;

    /// <summary>
    /// divider
    /// </summary>
    private System.Windows.Forms.GroupBox m_patientGroupBox;


    /// <summary>
    /// label
    /// </summary>
    private System.Windows.Forms.Label label1;

    /// <summary>
    /// textbox for the medical id
    /// </summary>
    private System.Windows.Forms.TextBox m_medicalIdTextBox;

    /// <summary>
    /// text box for the last name
    /// </summary>
    private System.Windows.Forms.TextBox m_patientNameTextBox;

    /// <summary>
    /// last name label
    /// </summary>
    private System.Windows.Forms.Label m_patientNameLabel;

    /// <summary>
    /// physician label
    /// </summary>
    private System.Windows.Forms.Label m_physicianLabel;

    /// <summary>
    /// textbox for the physician
    /// </summary>
    private System.Windows.Forms.TextBox m_physicianTextBox;

    /// <summary>
    /// scan date
    /// </summary>
    private Viatronix.Console.ScanDateControl m_scanDate;

    /// <summary>
    /// group box
    /// </summary>
    private System.Windows.Forms.GroupBox m_seriesGroupBox;

    /// <summary>
    /// subtype combobox
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxProduct;

    /// <summary>
    /// label for the modality
    /// </summary>
    private System.Windows.Forms.Label m_labelModality;

    /// <summary>
    /// combobox for the modality
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxModality;

    /// <summary>
    /// label for the subtype
    /// </summary>
    private System.Windows.Forms.Label m_labelSubtype;

    /// <summary>
    /// search control
    /// </summary>
    private Viatronix.Console.SearchControl m_searchControl;

    /// <summary>
    /// handler for the selected index changed
    /// </summary>
    private EventHandler m_selectedIndexChangedHandler = null;
    private Label label2;
    private System.Windows.Forms.ComboBox m_comboBoxStudyGroups; 

    // disables the search operation
    private bool m_disableSearch = false;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a DatabaseSearchPane control.
    /// </summary>
		public DatabaseSearchPane()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
      m_searchControl.SearchPane = this;
      m_searchControl.Clear += new EventHandler( this.OnSearchClearHandler );

      m_timer = new System.Windows.Forms.Timer();
      m_timer.Interval = 500;
      m_timer.Tick += new EventHandler( this.OnTimerTickHandler );

      //m_timerHandler = new System.Threading.TimerCallback( this.OnTimerTickHandler );
      m_selectedIndexChangedHandler = new System.EventHandler( this.OnSelectedIndexChangedHandler );

      XmlNode dicomInfo = (XmlNode)ConfigurationManager.GetSection( "searchInfo" );
 
      if (Global.StudyGroups.Count > 0)
      {
        foreach (StudyGroup studyGroup in Global.StudyGroups)
        {
          m_comboBoxStudyGroups.Items.Add(studyGroup);
        }
        m_comboBoxStudyGroups.SelectedIndex = 0;
      }
      else
      {
        StudyGroup sg = new StudyGroup();
        m_comboBoxStudyGroups.Items.Add(sg);
       
      }
      m_comboBoxStudyGroups.SelectedIndex = 0;
    

      int maxLength = 0;
      m_comboBoxModality.DropDownWidth = m_comboBoxModality.Width;
      m_comboBoxModality.Items.Add( "All" );
      m_comboBoxModality.SelectedIndex = 0;

      List<Modality> modalities = Global.DataProvider.GetModalities();

      foreach (Modality modality in modalities)
      {
        m_comboBoxModality.Items.Add(modality);
        maxLength = Math.Max(maxLength, modality.Code.Length + 3 + modality.Description.Length);
      }

      m_comboBoxModality.DropDownWidth = Math.Max((maxLength * 6 + 10), m_comboBoxModality.Width);



      maxLength = 0;
      m_comboBoxProduct.DropDownWidth = m_comboBoxProduct.Width;
      m_comboBoxProduct.Items.Add( "All" );
      m_comboBoxProduct.SelectedIndex = 0;

      List<Viatronix.Enterprise.Entities.Application> applications = Global.DataProvider.GetApplications();

      foreach (Viatronix.Enterprise.Entities.Application application in applications)
      {
        m_comboBoxProduct.Items.Add(application.Id + " - " + application.Name);
        maxLength = Math.Max(maxLength, application.Id.Length + 3 + application.Name.Length);

      }

      m_comboBoxProduct.DropDownWidth = Math.Max(( maxLength * 6 + 10), m_comboBoxModality.Width);


      m_scanDate.Style = ScanDateStyle.None;

		} // DatabaseSearchPane()

    #endregion

    #region event handlers

   
    /// <summary>
    /// Handles the key up event for the textboxes.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An KeyEventArgs that contains the event data.</param>
    private void OnKeyUpHandler( object sender, KeyEventArgs args )
    {
      if ( !ConsoleUtils.IsKeyValid( args ) )
        return;

      m_timer.Stop();
      m_timer.Start();
    } // OnKeyUpHandler( sender, args )


    /// <summary>
    /// Executes the search after a specified period of time.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnTimerTickHandler( object sender, EventArgs args )
    {
      m_timer.Stop();
      m_searchControl.Search( CreateQuery() );
    } // OnTimerTickHandler( sender, args )


    /// <summary>
    /// Performs a search when the combobox selection changes.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSelectedIndexChangedHandler( object sender, EventArgs args )
    {
      if( m_disableSearch )
        return;

      m_timer.Stop();
      m_timer.Start();
    } // OnSelectedIndexChangedHandler( object sender, EventArgs args )


    /// <summary>
    /// Handles the search clear event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnSearchClearHandler( object sender, EventArgs args )
    {
      ClearControls();
    } // OnSearchClearHandler( sender, args )

    #endregion

    #region ISearchPane Members

    /// <summary>
    /// Creates aXmlBuilderfor the provided search criteria.
    /// </summary>
    /// <returns>A QueryBuilder</returns>
    public XmlBuilder CreateQuery()
    {
      XmlBuilder query = new XmlBuilder("study");

      if ( ! string.IsNullOrEmpty(m_patientNameTextBox.Text) )
        query.AddAttribute("name", m_patientNameTextBox.Text.Trim() + "%");

      query.AddAttribute("mid", m_medicalIdTextBox.Text.Trim());
      query.AddAttribute("phys", m_physicianTextBox.Text.Trim());

      m_scanDate.AddDate(query);
 

      // append scan date information
      m_scanDate.AddDate(query);
       

      StudyGroup sg = ( StudyGroup )m_comboBoxStudyGroups.SelectedItem;
      if( sg.Name != "" )
        query.AddAttribute("grp", sg.Name);

      Modality modality = m_comboBoxModality.Items[m_comboBoxModality.SelectedIndex] as Modality;

      if (modality != null)
        query.AddAttribute("mods", "%" + modality.Code + "%");
      else
        query.AddAttribute("mods", m_comboBoxModality.Text.Trim());

      if( m_comboBoxProduct.SelectedIndex != 0 )
         query.AddAttribute("apps", "%" + m_comboBoxProduct.Text.Trim() + "%");

      return query;
    } // CreateQueryBuilder()


    /// <summary>
    /// Clears the child controls.
    /// </summary>
    public void ClearControls()
    {
      m_disableSearch = true;

      m_patientNameTextBox.Text = string.Empty;
      m_medicalIdTextBox.Text = string.Empty;

      m_scanDate.Style = ScanDateStyle.None;

      m_comboBoxStudyGroups.SelectedIndex = 0;
      m_comboBoxModality.SelectedIndex = 0;
      m_comboBoxProduct.SelectedIndex = 0;
      m_physicianTextBox.Text = string.Empty;

      m_disableSearch = false;
    } // ClearControls()

    #endregion

    #region ISearchable Members

    /// <summary>
    /// Gets or set the search manager objcet.
    /// </summary>
    [Browsable( false )]
    public SearchManager Searcher
    {
      get { return m_searchControl.Searcher; }
      set 
      { 
        if( m_searchControl.Searcher == null )
        {
          m_comboBoxModality.SelectedIndexChanged += m_selectedIndexChangedHandler;//new System.EventHandler( this.OnSelectedIndexChangedHandler );
          m_comboBoxProduct.SelectedIndexChanged  += m_selectedIndexChangedHandler;//new System.EventHandler( this.OnSelectedIndexChangedHandler );
        }

        m_searchControl.Searcher = value; 
      }
    } // Searcher

    #endregion
  }
}
