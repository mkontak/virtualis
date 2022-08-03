// $Id: Wizard.cs,v 1.8 2007/03/01 15:11:59 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (gdavidson@viatronix.com )


using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
	/// <summary>
	/// Summary description for Wizard.
	/// </summary>
	public class Wizard : Viatronix.UI.Panel
	{

    #region fields

    /// <summary>
    /// version
    /// </summary>
    private const string VERSION = "1.0";

    /// <summary>
    /// button panel
    /// </summary>
    private System.Windows.Forms.Panel m_buttonPanel;

    /// <summary>
    /// next button
    /// </summary>
    private Viatronix.UI.Button m_next;

    /// <summary>
    /// back button
    /// </summary>
    private Viatronix.UI.Button m_back;

    /// <summary>
    /// cancel button
    /// </summary>
    private Viatronix.UI.Button m_cancel;

    /// <summary>
    /// finish button
    /// </summary>
    private Viatronix.UI.Button m_finish;

    /// <summary>
    /// collection of pages
    /// </summary>
    private WizardPageCollection m_pages = null;

    /// <summary>
    /// selected page index
    /// </summary>
    private int m_selectedIndex = 0;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// next panel
    /// </summary>
    private System.Windows.Forms.Panel m_nextPanel;

    /// <summary>
    /// back panel
    /// </summary>
    private System.Windows.Forms.Panel m_backPanel;

    /// <summary>
    /// cancel panel
    /// </summary>
    private System.Windows.Forms.Panel m_cancelPanel;

    /// <summary>
    /// button display style
    /// </summary>
    private WizardDisplayButtons m_displayButtons = WizardDisplayButtons.All;

    /// <summary>
    /// Cancelled event
    /// </summary>
    public event EventHandler Cancelled = null;

    /// <summary>
    /// Finished event
    /// </summary>
    public event EventHandler Finished = null;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
		public Wizard()
		{
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      SetStyle( ControlStyles.OptimizedDoubleBuffer, true ); 

      m_pages = new WizardPageCollection( this );

      this.BottomPadding = 0;
      this.TopPadding = 0;
      this.LeftPadding = 0;
      this.RightPadding = 0;

			InitializeComponent();
		} // Wizard()

    #endregion

    #region properties

    /// <summary>
    /// Returns a reference to a collection of WizardPages.
    /// </summary>
    public WizardPageCollection Pages
    {
      get { return m_pages; }
    } // Pages


    /// <summary>
    /// Gets or sets the selected WizardPage
    /// </summary>
    public WizardPage SelectedPage
    {
      get { return m_pages[ m_selectedIndex ]; }
      set { m_selectedIndex = m_pages.IndexOf( value ); }
    } // SelectedPage


    /// <summary>
    /// Gets or sets the selected index
    /// </summary>
    public int SelectedIndex
    {
      get { return m_selectedIndex; }
      set { m_selectedIndex = value; }
    } // SelectedIndex


    /// <summary>
    /// Gets or sets which buttons should be displayed
    /// </summary>
    public WizardDisplayButtons DisplayButtons
    {
      get { return m_displayButtons; }
      set
      {
        m_displayButtons = value;

        m_buttonPanel.Visible = this.IsButtonPanelVisible;

        m_next.Visible = this.IsNextVisible;
        m_back.Visible = this.IsBackVisible;
        m_finish.Visible = this.IsFinishVisible;
        m_cancel.Visible = this.IsCancelVisible;

        m_nextPanel.Visible = this.IsNextVisible || this.IsFinishVisible;
        m_cancelPanel.Visible = this.IsCancelVisible;
        m_backPanel.Visible = this.IsBackVisible;

        Invalidate();
      }
    } // DisplayButtons


    /// <summary>
    /// Gets or sets if the next should be enabled
    /// </summary>
    public bool EnableNext
    {
      get { return m_next.Enabled; }
      set 
      { 
        m_next.Enabled = value; 
        Invalidate();
      }
    } // EnableNext


    /// <summary>
    /// Gets or sets if the back should be enabled
    /// </summary>
    public bool EnableBack
    {
      get { return m_back.Enabled; }
      set 
      { 
        m_back.Enabled = value; 
        Invalidate();
      }
    } // EnableBack


    /// <summary>
    /// Gets or sets if the finish should be enabled
    /// </summary>
    public bool EnableFinish
    {
      get { return m_finish.Enabled; }
      set 
      { 
        m_finish.Enabled = value; 
        Invalidate();
      }
    } // EnableFinish


    /// <summary>
    /// Gets or sets if the cancel should be enabled
    /// </summary>
    public bool EnableCancel
    {
      get { return m_cancel.Enabled; }
      set 
      { 
        m_cancel.Enabled = value; 
        Invalidate();
      }
    } // EnableCancel


    /// <summary>
    /// Returns the visible state of the next button
    /// </summary>
    public bool IsNextVisible
    {
      get { return ( m_displayButtons & WizardDisplayButtons.Next ) == WizardDisplayButtons.Next; }
    } // IsNextVisible


    /// <summary>
    /// Returns the visible state of the back button
    /// </summary>
    public bool IsBackVisible
    {
      get { return ( m_displayButtons & WizardDisplayButtons.Back ) == WizardDisplayButtons.Back; }
    } // IsBackVisible


    /// <summary>
    /// Returns the visible state of the finish button
    /// </summary>
    public bool IsFinishVisible
    {
      get { return ( m_displayButtons & WizardDisplayButtons.Finish ) == WizardDisplayButtons.Finish; }
    } // IsFinishVisible


    /// <summary>
    /// Returns the visible state of the cancel button
    /// </summary>
    public bool IsCancelVisible
    {
      get { return ( m_displayButtons & WizardDisplayButtons.Cancel ) == WizardDisplayButtons.Cancel; }
    } // IsCancelVisible


    /// <summary>
    /// Returns the visible state of the button panel
    /// </summary>
    public bool IsButtonPanelVisible
    {
      get { return m_displayButtons != WizardDisplayButtons.None; }
    } // IsButtonPanelVisible

    #endregion

    #region override methods

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}


    /// <summary>
    /// Raises the Resize event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )

    #endregion

    #region methods

    /// <summary>
    /// Selects the next page
    /// </summary>
    public void Next()
    {
      if( m_selectedIndex >= ( m_pages.Count - 1 ))
        OnFinished();
      else
      {
        this.SelectedPage.OnWizardNext();
        this.SelectedPage.ClosePage();

        this.Controls.Remove( this.SelectedPage );
        m_selectedIndex++;
        this.Controls.Add( this.SelectedPage );

        m_next.Visible = this.IsNextVisible && ( m_selectedIndex != ( m_pages.Count - 1 ));
        m_finish.Visible = this.IsFinishVisible && ( m_selectedIndex == ( m_pages.Count - 1 ));
        m_back.Enabled = this.IsBackVisible && ( m_selectedIndex > 0 );

        this.SelectedPage.OnPageLoad();
      }
    } // Next()


    /// <summary>
    /// Selects the previous page
    /// </summary>
    public void Back()
    {
      this.SelectedPage.OnWizardBack();
      this.SelectedPage.ClosePage();

      if( m_selectedIndex >= 1 )
      {
        this.Controls.Remove( this.SelectedPage );
        m_selectedIndex--;
        this.Controls.Add( this.SelectedPage );

        m_next.Visible = this.IsNextVisible;
        m_finish.Visible = false;
        m_back.Enabled = this.IsBackVisible && ( m_selectedIndex > 0 );

        this.SelectedPage.OnPageLoad();
      }      
    } // Back()


    /// <summary>
    /// Cancels the wizard control
    /// </summary>
    public void Cancel()
    {
      OnCancelled();
    } // Cancel()


    /// <summary>
    /// Loads the control from a file
    /// </summary>
    /// <param name="file">path to a file</param>
    public void LoadXml( string file )
    {
      XmlDocument doc = new XmlDocument();
      doc.Load( file );

      LoadXml( doc.DocumentElement );
    } // LoadXml( file )


    /// <summary>
    /// Loads the control from xml
    /// </summary>
    /// <param name="node">an xml node</param>
    public void LoadXml( XmlNode node )
    {
      if( node.Attributes[ "version" ].Value != VERSION )
        throw new ArgumentException( "Incorrect version for wizard control." );

      if( node.Attributes[ "buttons" ] != null )
        this.DisplayButtons = (WizardDisplayButtons) Enum.Parse( typeof( WizardDisplayButtons ), node.Attributes[ "buttons" ].Value, true );

      foreach( XmlNode pageNode in node.SelectNodes( "//pages/page" ))
      {
        string typename = pageNode.Attributes[ "typename" ].Value;
        WizardPage page = (WizardPage) Activator.CreateInstance( Type.GetType( typename ));
        page.Initialize( pageNode.OuterXml );

        m_pages.Add( page );
      }
    } // LoadXml( node )


    /// <summary>
    /// Raises the cancelled event
    /// </summary>
    protected virtual void OnCancelled()
    {
      this.SelectedPage.OnCancel();

      if( this.Cancelled != null )
        this.Cancelled( this, EventArgs.Empty );
    } // OnCancelled()


    /// <summary>
    /// Raises the finished event
    /// </summary>
    protected virtual void OnFinished()
    {
      this.SelectedPage.OnWizardFinish();
      this.SelectedPage.ClosePage();
     
      if( this.Finished != null )
        this.Finished( this, EventArgs.Empty );
    } // OnFinished()


    /// <summary>
    /// Creates a Wizard control from the xml.
    /// </summary>
    /// <param name="xml">xlm defining the wizard control.</param>
    /// <returns>A Wizard control.</returns>
    public static Wizard FromXml( string xml )
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml( xml );

      Wizard wizard = new Wizard();
      wizard.LoadXml( xml );

      return wizard;
    } // FromXml( xml )

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the next button's click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnNextClickHandler( object sender, EventArgs args )
    {
      Next();
    } // OnNextClickHandler( sender, args )


    /// <summary>
    /// Handles the back button's click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnBackClickHandler( object sender, EventArgs args )
    {
      Back();
    } // OnBackClickHandler( sender, args )


    /// <summary>
    /// Handles the cancel button's click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnCancelClickHandler( object sender, EventArgs args )
    {
      OnCancelled();
    } // OnCancelClickHandler( sender, args )


    /// <summary>
    /// Handles the finish button's click event
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnFinishClickHandler( object sender, EventArgs args )
    {
      OnFinished();
    } // OnFinishClickHandler( sender, args )

    #endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.m_buttonPanel = new System.Windows.Forms.Panel();
      this.m_cancelPanel = new System.Windows.Forms.Panel();
      this.m_cancel = new Viatronix.UI.Button();
      this.m_backPanel = new System.Windows.Forms.Panel();
      this.m_back = new Viatronix.UI.Button();
      this.m_nextPanel = new System.Windows.Forms.Panel();
      this.m_next = new Viatronix.UI.Button();
      this.m_finish = new Viatronix.UI.Button();
      this.m_buttonPanel.SuspendLayout();
      this.m_cancelPanel.SuspendLayout();
      this.m_backPanel.SuspendLayout();
      this.m_nextPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_buttonPanel
      // 
      this.m_buttonPanel.BackColor = System.Drawing.Color.Transparent;
      this.m_buttonPanel.Controls.Add(this.m_cancelPanel);
      this.m_buttonPanel.Controls.Add(this.m_backPanel);
      this.m_buttonPanel.Controls.Add(this.m_nextPanel);
      this.m_buttonPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_buttonPanel.Location = new System.Drawing.Point(0, 336);
      this.m_buttonPanel.Name = "m_buttonPanel";
      this.m_buttonPanel.Size = new System.Drawing.Size(472, 40);
      this.m_buttonPanel.TabIndex = 0;
      // 
      // m_cancelPanel
      // 
      this.m_cancelPanel.Controls.Add(this.m_cancel);
      this.m_cancelPanel.Dock = System.Windows.Forms.DockStyle.Right;
      this.m_cancelPanel.Location = new System.Drawing.Point(208, 0);
      this.m_cancelPanel.Name = "m_cancelPanel";
      this.m_cancelPanel.Size = new System.Drawing.Size(88, 40);
      this.m_cancelPanel.TabIndex = 4;
      // 
      // m_cancel
      // 
      this.m_cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_cancel.CustomImages = new System.Drawing.Bitmap[] {
                                                                 null,
                                                                 null,
                                                                 null};
      this.m_cancel.ForeColor = System.Drawing.Color.Black;
      this.m_cancel.Location = new System.Drawing.Point(0, 8);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.TabIndex = 2;
      this.m_cancel.Text = "Cancel";
      this.m_cancel.Click += new System.EventHandler(this.OnCancelClickHandler);
      // 
      // m_backPanel
      // 
      this.m_backPanel.Controls.Add(this.m_back);
      this.m_backPanel.Dock = System.Windows.Forms.DockStyle.Right;
      this.m_backPanel.Location = new System.Drawing.Point(296, 0);
      this.m_backPanel.Name = "m_backPanel";
      this.m_backPanel.Size = new System.Drawing.Size(88, 40);
      this.m_backPanel.TabIndex = 3;
      // 
      // m_back
      // 
      this.m_back.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_back.CustomImages = new System.Drawing.Bitmap[] {
                                                               null,
                                                               null,
                                                               null};
      this.m_back.Enabled = false;
      this.m_back.ForeColor = System.Drawing.Color.Black;
      this.m_back.Location = new System.Drawing.Point(0, 8);
      this.m_back.Name = "m_back";
      this.m_back.OldStockButton = false;
      this.m_back.TabIndex = 1;
      this.m_back.Text = "Back";
      this.m_back.Click += new System.EventHandler(this.OnBackClickHandler);
      // 
      // m_nextPanel
      // 
      this.m_nextPanel.Controls.Add(this.m_next);
      this.m_nextPanel.Controls.Add(this.m_finish);
      this.m_nextPanel.Dock = System.Windows.Forms.DockStyle.Right;
      this.m_nextPanel.Location = new System.Drawing.Point(384, 0);
      this.m_nextPanel.Name = "m_nextPanel";
      this.m_nextPanel.Size = new System.Drawing.Size(88, 40);
      this.m_nextPanel.TabIndex = 1;
      // 
      // m_next
      // 
      this.m_next.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_next.CustomImages = new System.Drawing.Bitmap[] {
                                                               null,
                                                               null,
                                                               null};
      this.m_next.Enabled = false;
      this.m_next.ForeColor = System.Drawing.Color.Black;
      this.m_next.Location = new System.Drawing.Point(0, 8);
      this.m_next.Name = "m_next";
      this.m_next.OldStockButton = false;
      this.m_next.TabIndex = 0;
      this.m_next.Text = "Next";
      this.m_next.Click += new System.EventHandler(this.OnNextClickHandler);
      // 
      // m_finish
      // 
      this.m_finish.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_finish.CustomImages = new System.Drawing.Bitmap[] {
                                                                 null,
                                                                 null,
                                                                 null};
      this.m_finish.ForeColor = System.Drawing.Color.Black;
      this.m_finish.Location = new System.Drawing.Point(0, 8);
      this.m_finish.Name = "m_finish";
      this.m_finish.OldStockButton = false;
      this.m_finish.TabIndex = 3;
      this.m_finish.Text = "Finish";
      this.m_finish.Visible = false;
      this.m_finish.Click += new System.EventHandler(this.OnFinishClickHandler);
      // 
      // Wizard
      // 
      this.Controls.Add(this.m_buttonPanel);
      this.Name = "Wizard";
      this.Size = new System.Drawing.Size(472, 376);
      this.m_buttonPanel.ResumeLayout(false);
      this.m_cancelPanel.ResumeLayout(false);
      this.m_backPanel.ResumeLayout(false);
      this.m_nextPanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
		#endregion

    #region WizardPageCollection class

    public class WizardPageCollection : CollectionBase
    {

      #region fields

      /// <summary>
      /// parent wizard control.
      /// </summary>
      private Wizard m_parent = null;

      #endregion

      #region constrction

      /// <summary>
      /// Creates a new instance of a WizardCollection
      /// </summary>
      /// <param name="wizard">Parent wizard collection.</param>
      internal WizardPageCollection( Wizard wizard )
      {
        m_parent = wizard;
      } // WizardPageCollection( wizard )

      #endregion

      #region properties

      /// <summary>
      /// Indexer
      /// </summary>
      /// <param name="index">index of a WizardPage</param>
      /// <returns>a WizardPage</returns>
      public WizardPage this[ int index ]
      {
        get { return (WizardPage) this.List[ index ]; }
        set { this.List[ index ] = value; }
      } // Item

      #endregion

      #region methods

      /// <summary>
      /// Adds a WizardPage to the collection.
      /// </summary>
      /// <param name="page">WizardPage being added</param>
      public void Add( WizardPage page )
      {
        this.List.Add( page );
      } // Add( page )


      /// <summary>
      /// Removes a WizardPage from the collection.
      /// </summary>
      /// <param name="page">WizardPage being removed</param>
      public void Remove( WizardPage page )
      {
        this.List.Remove( page );
      } // Removed( page )


      /// <summary>
      /// Returns the index of the provided WizardPage
      /// </summary>
      /// <param name="page">a WizardPage</param>
      /// <returns>index of the page</returns>
      public int IndexOf( WizardPage page )
      {
        return this.List.IndexOf( page );
      } // IndexOf( page )


      /// <summary>
      /// Determines if the WizardPage is contained in the collection
      /// </summary>
      /// <param name="page">a WizardPage</param>
      /// <returns>true if the page is in the collection</returns>
      public bool Contains( WizardPage page )
      {
        return this.List.Contains( page );
      } // Contains( page )

      #endregion

      #region override methods


      /// <summary>
      /// Inserts the value into the collection.
      /// </summary>
      /// <param name="index">Index in the collection.</param>
      /// <param name="value">Objec tbeing inserted.</param>
      protected override void OnInsert( int index, object value )
      {
        base.OnInsert( index, value );

        WizardPage page = (WizardPage) value;
        page.Dock = DockStyle.Fill;
        page.Wizard = m_parent;

        if( this.List.Count == 0 )
        {
          m_parent.Controls.Add( page );
          page.OnPageLoad();
        }
      } // OnInsert( index, value )


      /// <summary>
      /// Removes the value from the collection.
      /// </summary>
      /// <param name="index">Index of the object.</param>
      /// <param name="value">Object being removed.</param>
      protected override void OnRemoveComplete( int index, object value )
      {
        base.OnRemoveComplete( index, value );

        WizardPage page = (WizardPage) value;
        page.Wizard = null;
        m_parent.Controls.Remove( page );

        m_parent.m_next.Enabled = ( this.List.Count > 1 );
      } // OnRemove( index, value )

      #endregion

    } // class WizardPageCollection

    #endregion

	} // class Wizard

  #region WizardDisplayButtons enum

  [Flags]
  public enum WizardDisplayButtons : uint
  {
    None    = 0x00000000,
    Back    = 0x00000001,
    Next    = 0x00000002,
    Finish  = 0x00000004,
    Cancel  = 0x00000008,
    All     = 0xffffffff,
  } // WizardDisplayButtons

  #endregion

  #region WizardPage class

  public class WizardPage : System.Windows.Forms.ContainerControl
  {

    #region fields

    /// <summary>
    /// version
    /// </summary>
    private const string VERSION = "1.0";

    /// <summary>
    /// parent wizard control
    /// </summary>
    private Wizard m_wizard = null;

    /// <summary>
    /// identifier
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// tracks if the page was unloaded
    /// </summary>
    private bool m_pageUnloaded = false;

    #endregion

    #region construction

    /// <summary>
    /// constructor
    /// </summary>
    public WizardPage()
    {
      // enable double buffering
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
      SetStyle( ControlStyles.OptimizedDoubleBuffer, true ); 
    } // WizardPage()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the id for the page
    /// </summary>
    public string ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID


    /// <summary>
    /// Gets or sets the parent wizard control
    /// </summary>
    public Wizard Wizard
    {
      get { return m_wizard; }
      set { m_wizard = value; }
    } // Wizard

    #endregion

    #region methods

    /// <summary>
    /// Initializes the control from xml
    /// </summary>
    /// <param name="xml">xml</param>
    public void Initialize( string xml ) 
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml( xml );

      if( doc.DocumentElement.Attributes[ "version" ].Value != VERSION )
        throw new ArgumentException( "Incorrect version for wizard page." );

      m_id = doc.DocumentElement.Attributes[ "id" ].Value;
 
      OnInitialize( doc.DocumentElement );
    } // Initialize( xml ) 


    /// <summary>
    /// Raises the Resize event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    } // OnResize( args )


    /// <summary>
    /// Cleans up any used resources
    /// </summary>
    /// <param name="disposing">true if managed resources can be consumed</param>
    protected override void Dispose( bool disposing )
    {
      if( disposing && m_pageUnloaded == false )
        OnPageUnload();

      base.Dispose( disposing );
    } // Dispose( disposing )


    /// <summary>
    /// Closes the page
    /// </summary>
    internal void ClosePage()
    {
      OnPageUnload();
      m_pageUnloaded = true; 
    } // ClosePage()

    #endregion

    #region virtual methods

    /// <summary>
    /// Allows cutom initialization of the control
    /// </summary>
    /// <param name="node">xml</param>
    protected virtual void OnInitialize( XmlNode node ) {}


    /// <summary>
    /// Called when the wizard is displaying the next page
    /// </summary>
    protected internal virtual void OnWizardNext() {}


    /// <summary>
    /// Called when the wizard is displaying the previous page
    /// </summary>
    protected internal virtual void OnWizardBack() {}


    /// <summary>
    /// Called when the wizard is finished
    /// </summary>
    protected internal virtual void OnWizardFinish() {}


    /// <summary>
    /// Called when the wizard is being loaded
    /// </summary>
    protected internal virtual void OnPageLoad() {}


    /// <summary>
    /// Called when the wizard is being unloaded
    /// </summary>
    protected virtual void OnPageUnload() {}


    /// <summary>
    /// Called when the wizard is cancelled
    /// </summary>
    protected internal virtual void OnCancel() {}

    #endregion

  } // class WizardPage

  #endregion

} // namespace Viatronix.UI


#region revision history

// $Log: Wizard.cs,v $
// Revision 1.8  2007/03/01 15:11:59  gdavidson
// Commented code
//
// Revision 1.7  2006/08/28 21:47:34  gdavidson
// Modified changing of wizard pages
//
// Revision 1.6  2006/08/23 15:55:24  gdavidson
// WizardPage calls UnloadPage method during object disposal
//
// Revision 1.5  2006/08/05 15:12:49  gdavidson
// Added double buffering to the control
//
// Revision 1.4  2006/08/01 15:13:59  gdavidson
// Added code to initialize the wizard pages
//
// Revision 1.3  2006/05/25 13:37:38  gdavidson
// Modified the OnFinished method
//
// Revision 1.2  2006/05/18 17:48:06  gdavidson
// Added OnPageLoad and OnPageUnload methods
//
// Revision 1.1  2006/05/16 17:07:33  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Wizard.cs,v 1.8 2007/03/01 15:11:59 gdavidson Exp $
// $Id: Wizard.cs,v 1.8 2007/03/01 15:11:59 gdavidson Exp $

#endregion
