// $Id: WizardControl.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Data;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.ComponentModel;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;
using Viatronix.v3D.Dicom.UI.Docking.Collections;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxBitmap(typeof(WizardControl))]
  [DefaultProperty("Profile")]
  [Designer(typeof(WizardControlDesigner))]
  public class WizardControl : UserControl
  {
    // Define enumerations and structures
    public enum Status
    {
      Default,
      Yes,
      No
    }
	    
    public enum Profiles
    {
      Install,
      Configure,
      Controller
    }
	    
    // Class wide constants
    protected const int m_panelGap = 10;
    protected const int m_buttonGap = 10;
    protected static Image m_standardPicture;
	
    // Instance fields
    protected Image m_picture;
    protected string m_title;
    protected Font m_fontTitle;
    protected Font m_fontSubTitle;
    protected Color m_colorTitle;
    protected Color m_colorSubTitle;
    protected Profiles m_profile;
    protected bool m_assignDefault;
    protected WizardPage m_selectedPage;
    protected Status m_showUpdate, m_enableUpdate;
    protected Status m_showCancel, m_enableCancel;
    protected Status m_showBack, m_enableBack;
    protected Status m_showNext, m_enableNext;
    protected Status m_showFinish, m_enableFinish;
    protected Status m_showClose, m_enableClose;
    protected Status m_showHelp, m_enableHelp;
    protected WizardPageCollection m_wizardPages;
	    
    // Instance designer fields
    protected System.Windows.Forms.Panel m_panelTop;
    protected System.Windows.Forms.Panel m_panelBottom;
    protected System.Windows.Forms.Button m_buttonUpdate;
    protected System.Windows.Forms.Button m_buttonCancel;
    protected System.Windows.Forms.Button m_buttonBack;
    protected System.Windows.Forms.Button m_buttonNext;
    protected System.Windows.Forms.Button m_buttonFinish;
    protected System.Windows.Forms.Button m_buttonClose;
    protected System.Windows.Forms.Button m_buttonHelp;
    protected Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl m_tabControl;
    protected System.ComponentModel.Container components = null;

    // Delegate definitions
    public delegate void WizardPageHandler(WizardPage wp, WizardControl wc);

    // Instance events
    public event WizardPageHandler WizardPageEnter;
    public event WizardPageHandler WizardPageLeave;
    public event EventHandler WizardCaptionTitleChanged;
    public event EventHandler SelectionChanged;
    public event EventHandler UpdateClick;
    public event EventHandler CancelClick;
    public event EventHandler FinishClick;
    public event EventHandler CloseClick;
    public event EventHandler HelpClick;
    public event CancelEventHandler NextClick;
    public event CancelEventHandler BackClick;

    static WizardControl()
    {
      // Create a strip of images by loading an embedded bitmap resource
      m_standardPicture = ResourceHelper.LoadBitmap(Type.GetType("Viatronix.v3D.Dicom.UI.Docking.Controls.WizardControl"),
        "Viatronix.v3D.Dicom.UI.Docking.Resources.WizardPicture.bmp");
    }

    public WizardControl()
    {
      InitializeComponent();
			
      // No page currently selected
      m_selectedPage = null;
			
      // Hook into tab control events
      m_tabControl.SelectionChanged += new EventHandler(OnTabSelectionChanged);

      // Create our collection of wizard pages
      m_wizardPages = new WizardPageCollection();
            
      // Hook into collection events
      m_wizardPages.Cleared += new Collections.CollectionClear(OnWizardCleared);
      m_wizardPages.Inserted += new Collections.CollectionChange(OnWizardInserted);
      m_wizardPages.Removed += new Collections.CollectionChange(OnWizardRemoved);

      // Hook into drawing events
      m_panelTop.Resize += new EventHandler(OnRepaintPanels);
      m_panelTop.Paint += new PaintEventHandler(OnPaintTopPanel);
      m_panelBottom.Resize += new EventHandler(OnRepaintPanels);
      m_panelBottom.Paint += new PaintEventHandler(OnPaintBottomPanel);

      // Initialize state
      m_showUpdate = m_enableUpdate = Status.Default;
      m_showCancel = m_enableUpdate = Status.Default;
      m_showBack = m_enableBack = Status.Default;
      m_showNext = m_enableNext = Status.Default;
      m_showFinish = m_enableFinish = Status.Default;
      m_showClose = m_enableClose = Status.Default;
      m_showHelp = m_enableHelp = Status.Default;
            
      // Default properties
      ResetProfile();
      ResetTitle();
      ResetTitleFont();
      ResetTitleColor();
      ResetSubTitleFont();
      ResetSubTitleColor();
      ResetPicture();
      ResetAssignDefaultButton();
            
      // Position and enable/disable control button state
      UpdateControlButtons();
    }

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

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_tabControl = new Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl();
      this.m_panelTop = new System.Windows.Forms.Panel();
      this.m_panelBottom = new System.Windows.Forms.Panel();
      this.m_buttonUpdate = new System.Windows.Forms.Button();
      this.m_buttonBack = new System.Windows.Forms.Button();
      this.m_buttonNext = new System.Windows.Forms.Button();
      this.m_buttonCancel = new System.Windows.Forms.Button();
      this.m_buttonFinish = new System.Windows.Forms.Button();
      this.m_buttonClose = new System.Windows.Forms.Button();
      this.m_buttonHelp = new System.Windows.Forms.Button();
      this.m_panelBottom.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_tabControl
      // 
      this.m_tabControl.Anchor = (((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right);
      this.m_tabControl.Appearance = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.VisualAppearance.MultiDocument;
      this.m_tabControl.IDEPixelBorder = false;
      this.m_tabControl.Location = new System.Drawing.Point(0, 80);
      this.m_tabControl.Multiline = true;
      this.m_tabControl.MultilineFullWidth = true;
      this.m_tabControl.Name = "_tabControl";
      this.m_tabControl.ShowArrows = false;
      this.m_tabControl.ShowClose = false;
      this.m_tabControl.Size = new System.Drawing.Size(424, 264);
      this.m_tabControl.TabIndex = 0;
      // 
      // m_panelTop
      // 
      this.m_panelTop.BackColor = System.Drawing.SystemColors.Window;
      this.m_panelTop.Dock = System.Windows.Forms.DockStyle.Top;
      this.m_panelTop.Name = "_panelTop";
      this.m_panelTop.Size = new System.Drawing.Size(424, 80);
      this.m_panelTop.TabIndex = 1;
      // 
      // m_panelBottom
      // 
      this.m_panelBottom.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                                this.m_buttonUpdate,
                                                                                this.m_buttonBack,
                                                                                this.m_buttonNext,
                                                                                this.m_buttonCancel,
                                                                                this.m_buttonFinish,
                                                                                this.m_buttonClose,
                                                                                this.m_buttonHelp});
      this.m_panelBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_panelBottom.Location = new System.Drawing.Point(0, 344);
      this.m_panelBottom.Name = "_panelBottom";
      this.m_panelBottom.Size = new System.Drawing.Size(424, 48);
      this.m_panelBottom.TabIndex = 2;
      // 
      // m_buttonUpdate
      // 
      this.m_buttonUpdate.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonUpdate.Location = new System.Drawing.Point(8, 14);
      this.m_buttonUpdate.Name = "_buttonUpdate";
      this.m_buttonUpdate.TabIndex = 4;
      this.m_buttonUpdate.Text = "Update";
      this.m_buttonUpdate.Click += new System.EventHandler(this.OnButtonUpdate);
      // 
      // m_buttonBack
      // 
      this.m_buttonBack.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonBack.Location = new System.Drawing.Point(56, 14);
      this.m_buttonBack.Name = "_buttonBack";
      this.m_buttonBack.TabIndex = 3;
      this.m_buttonBack.Text = "< Back";
      this.m_buttonBack.Click += new System.EventHandler(this.OnButtonBack);
      // 
      // m_buttonNext
      // 
      this.m_buttonNext.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonNext.Location = new System.Drawing.Point(120, 14);
      this.m_buttonNext.Name = "_buttonNext";
      this.m_buttonNext.TabIndex = 2;
      this.m_buttonNext.Text = "Next >";
      this.m_buttonNext.Click += new System.EventHandler(this.OnButtonNext);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonCancel.Location = new System.Drawing.Point(184, 14);
      this.m_buttonCancel.Name = "_buttonCancel";
      this.m_buttonCancel.TabIndex = 1;
      this.m_buttonCancel.Text = "Cancel";
      this.m_buttonCancel.Click += new System.EventHandler(this.OnButtonCancel);
      // 
      // m_buttonFinish
      // 
      this.m_buttonFinish.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonFinish.Location = new System.Drawing.Point(248, 14);
      this.m_buttonFinish.Name = "_buttonFinish";
      this.m_buttonFinish.TabIndex = 0;
      this.m_buttonFinish.Text = "Finish";
      this.m_buttonFinish.Click += new System.EventHandler(this.OnButtonFinish);
      // 
      // m_buttonClose
      // 
      this.m_buttonClose.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonClose.Location = new System.Drawing.Point(304, 14);
      this.m_buttonClose.Name = "_buttonClose";
      this.m_buttonClose.TabIndex = 0;
      this.m_buttonClose.Text = "Close";
      this.m_buttonClose.Click += new System.EventHandler(this.OnButtonClose);
      // 
      // m_buttonHelp
      // 
      this.m_buttonHelp.Anchor = (System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right);
      this.m_buttonHelp.Location = new System.Drawing.Point(360, 14);
      this.m_buttonHelp.Name = "_buttonHelp";
      this.m_buttonHelp.TabIndex = 0;
      this.m_buttonHelp.Text = "Help";
      this.m_buttonHelp.Click += new System.EventHandler(this.OnButtonHelp);
      // 
      // WizardControl
      // 
      this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                  this.m_tabControl,
                                                                  this.m_panelTop,
                                                                  this.m_panelBottom});
      this.Name = "WizardControl";
      this.Size = new System.Drawing.Size(424, 392);
      this.m_panelBottom.ResumeLayout(false);
      this.ResumeLayout(false);

    }
    #endregion
		
    [Category("Wizard")]
    [Description("Access to underlying TabControl instance")]
    public Controls.TabControl TabControl
    {
      get { return m_tabControl; }
    }

    [Category("Wizard")]
    [Description("Access to underlying header panel")]
    public Panel HeaderPanel
    {
      get { return m_panelTop; }
    }

    [Category("Wizard")]
    [Description("Access to underlying trailer panel")]
    public Panel TrailerPanel
    {
      get { return m_panelBottom; }
    }

    [Category("Wizard")]
    [Description("Collection of wizard pages")]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
    public WizardPageCollection WizardPages
    {
      get { return m_wizardPages; }
    }
		
    [Category("Wizard")]
    [Description("Determine default operation of buttons")]
    [DefaultValue(typeof(Profiles), "Configure")]
    public Profiles Profile
    {
      get { return m_profile; }
		    
      set 
      {
        if (m_profile != value)
        {
          m_profile = value;
		            
          switch(m_profile)
          {
            case Profiles.Install:
            case Profiles.Configure:
              // Current page selection determines if full page is needed
              if (m_tabControl.SelectedIndex != -1)
              {
                // Get the selected wizard page
                WizardPage wp = m_wizardPages[m_tabControl.SelectedIndex];
                
                // Should we be presented in full page?
                if (wp.FullPage)
                  EnterFullPage();
                else
                {
                  // Controller profile is not allowed to be outside of FullMode
                  if (m_profile != Profiles.Controller)
                    LeaveFullPage();
                }
              }
              else
                LeaveFullPage();
                            
              m_tabControl.HideTabsMode = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.HideTabsModes.HideAlways; 
              break;
            case Profiles.Controller:
              // Controller is always full page
              EnterFullPage();
                                
              m_tabControl.HideTabsMode = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.HideTabsModes.ShowAlways;
              break;
          }
		            
          // Position and enable/disable control button state
          UpdateControlButtons();
        }
      }
    }

    public void ResetProfile()
    {
      Profile = Profiles.Configure;
    }
        
    [Category("Wizard")]
    [Description("Main title text")]
    public Image Picture
    {
      get { return m_picture; }
            
      set
      {
        m_picture = value;
        m_panelTop.Invalidate();
      }
    }

    protected bool ShouldSerializePicture()
    {
      return m_picture.Equals(m_standardPicture);
    }
        
    public void ResetPicture()
    {
      Picture = m_standardPicture;
    }
        
    [Category("Wizard")]
    [Description("Main title text")]
    [Localizable(true)]
    public string Title
    {
      get { return m_title; }
		    
      set
      {
        m_title = value;
        m_panelTop.Invalidate();
      }
    }
		
    public void ResetTitle()
    {
      Title = "Welcome to the Wizard Control";
    }

    protected bool ShouldSerializeTitle()
    {
      return !m_title.Equals("Welcome to the Wizard Control");
    }
    
    [Category("Wizard")]
    [Description("Font for drawing main title text")]
    public Font TitleFont
    {
      get { return m_fontTitle; }
		    
      set
      {
        m_fontTitle = value;
        m_panelTop.Invalidate();
      }
    }
		
    public void ResetTitleFont()
    {
      TitleFont = new Font("Tahoma", 10, FontStyle.Bold);
    }

    protected bool ShouldSerializeTitleFont()
    {
      return !m_fontTitle.Equals(new Font("Tahoma", 10, FontStyle.Bold));
    }
    
    [Category("Wizard")]
    [Description("Font for drawing main sub-title text")]
    public Font SubTitleFont
    {
      get { return m_fontSubTitle; }
		    
      set
      {
        m_fontSubTitle = value;
        m_panelTop.Invalidate();
      }
    }
		
    public void ResetSubTitleFont()
    {
      m_fontSubTitle = new Font("Tahoma", 8, FontStyle.Regular);
    }

    protected bool ShouldSerializeSubTitleFont()
    {
      return !m_fontSubTitle.Equals(new Font("Tahoma", 8, FontStyle.Regular));
    }

    [Category("Wizard")]
    [Description("Color for drawing main title text")]
    public Color TitleColor
    {
      get { return m_colorTitle; }
		    
      set
      {
        m_colorTitle = value;
        m_panelTop.Invalidate();
      }
    }

    public void ResetTitleColor()
    {
      TitleColor = base.ForeColor;
    }

    protected bool ShouldSerializeTitleColor()
    {
      return !m_colorTitle.Equals(base.ForeColor);
    }
		
    [Category("Wizard")]
    [Description("Determine is a default button should be auto-assigned")]
    [DefaultValue(false)]
    public bool AssignDefaultButton
    {
      get { return m_assignDefault; }
            
      set
      {
        if (m_assignDefault != value)
        {
          m_assignDefault = value;
          AutoAssignDefaultButton();
        }
      }
    }

    public void ResetAssignDefaultButton()
    {
      AssignDefaultButton = false;
    }

    [Category("Wizard")]
    [Description("Color for drawing main sub-title text")]
    public Color SubTitleColor
    {
      get { return m_colorSubTitle; }
		    
      set
      {
        m_colorSubTitle = value;
        m_panelTop.Invalidate();
      }
    }

    public void ResetSubTitleColor()
    {
      SubTitleColor = base.ForeColor;
    }

    protected bool ShouldSerializeSubTitleColor()
    {
      return !m_colorSubTitle.Equals(base.ForeColor);
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Update button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowUpdateButton
    {
      get { return m_showUpdate; }
            
      set 
      { 
        if (m_showUpdate != value)
        {
          m_showUpdate = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Update button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableUpdateButton
    {
      get { return m_enableUpdate; }
            
      set 
      { 
        if (m_enableUpdate != value)
        {
          m_enableUpdate = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Update control button")]
    [DefaultValue("Update")]
    [Localizable(true)]
    public string ButtonUpdateText
    {
      get { return m_buttonUpdate.Text; }
      set { m_buttonUpdate.Text = value; }
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Cancel button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowCancelButton
    {
      get { return m_showCancel; }
            
      set 
      { 
        if (m_showCancel != value)
        {
          m_showCancel = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Cancel button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableCancelButton
    {
      get { return m_enableCancel; }
            
      set 
      { 
        if (m_enableCancel != value)
        {
          m_enableCancel = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Cancel control button")]
    [DefaultValue("Cancel")]
    [Localizable(true)]
    public string ButtonCancelText
    {
      get { return m_buttonCancel.Text; }
      set { m_buttonCancel.Text = value; }
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Back button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowBackButton
    {
      get { return m_showBack; }
            
      set 
      { 
        if (m_showBack != value)
        {
          m_showBack = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Back button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableBackButton
    {
      get { return m_enableBack; }
            
      set 
      { 
        if (m_enableBack != value)
        {
          m_enableBack = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Back control button")]
    [DefaultValue("< Back")]
    [Localizable(true)]
    public string ButtonBackText
    {
      get { return m_buttonBack.Text; }
      set { m_buttonBack.Text = value; }
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Next button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowNextButton
    {
      get { return m_showNext; }
            
      set 
      { 
        if (m_showNext != value)
        {
          m_showNext = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Next button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableNextButton
    {
      get { return m_enableBack; }
            
      set 
      { 
        if (m_enableNext != value)
        {
          m_enableNext = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Next control button")]
    [DefaultValue("Next >")]
    [Localizable(true)]
    public string ButtonNextText
    {
      get { return m_buttonNext.Text; }
      set { m_buttonNext.Text = value; }
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Finish button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowFinishButton
    {
      get { return m_showFinish; }
            
      set 
      { 
        if (m_showFinish != value)
        {
          m_showFinish = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Finish button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableFinishButton
    {
      get { return m_enableFinish; }
            
      set 
      { 
        if (m_enableFinish != value)
        {
          m_enableFinish = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Finish control button")]
    [DefaultValue("Finish")]
    [Localizable(true)]
    public string ButtonFinishText
    {
      get { return m_buttonFinish.Text; }
      set { m_buttonFinish.Text = value; }
    }
        
    [Category("Control Buttons")]
    [Description("Define visibility of Close button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowCloseButton
    {
      get { return m_showClose; }
            
      set 
      { 
        if (m_showClose != value)
        {
          m_showClose = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Close button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableCloseButton
    {
      get { return m_enableClose; }
            
      set 
      { 
        if (m_enableClose != value)
        {
          m_enableClose = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Close control button")]
    [DefaultValue("Close")]
    [Localizable(true)]
    public string ButtonCloseText
    {
      get { return m_buttonClose.Text; }
      set { m_buttonClose.Text = value; }
    }

    [Category("Control Buttons")]
    [Description("Define visibility of Help button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status ShowHelpButton
    {
      get { return m_showHelp; }
            
      set 
      { 
        if (m_showHelp != value)
        {
          m_showHelp = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Define selectability of Help button")]
    [DefaultValue(typeof(Status), "Default")]
    public Status EnableHelpButton
    {
      get { return m_enableHelp; }
            
      set 
      { 
        if (m_enableHelp != value)
        {
          m_enableHelp = value;
          UpdateControlButtons();
        }
      }
    }

    [Category("Control Buttons")]
    [Description("Modify the text for the Help control button")]
    [DefaultValue("Help")]
    [Localizable(true)]
    public string ButtonHelpText
    {
      get { return m_buttonHelp.Text; }
      set { m_buttonHelp.Text = value; }
    }

    [Category("Wizard")]
    [Description("Index of currently selected WizardPage")]
    public int SelectedIndex
    {
      get { return m_tabControl.SelectedIndex; }
      set { m_tabControl.SelectedIndex = value; }
    }
        
    public virtual void OnWizardPageEnter(WizardPage wp)
    {
      if (WizardPageEnter != null)
        WizardPageEnter(wp, this);
    }

    public virtual void OnWizardPageLeave(WizardPage wp)
    {
      if (WizardPageLeave != null)
        WizardPageLeave(wp, this);
    }

    public virtual void OnSelectionChanged(EventArgs e)
    {
      if (SelectionChanged != null)
        SelectionChanged(this, e);
    }
                
    public virtual void OnCloseClick(EventArgs e)
    {
      if (CloseClick != null)
        CloseClick(this, e);
    }

    public virtual void OnFinishClick(EventArgs e)
    {
      if (FinishClick != null)
        FinishClick(this, e);
    }
    
    public virtual void OnNextClick(CancelEventArgs e)
    {
      if (NextClick != null)
        NextClick(this, e);
    }
    
    public virtual void OnBackClick(CancelEventArgs e)
    {
      if (BackClick != null)
        BackClick(this, e);
    }

    public virtual void OnCancelClick(EventArgs e)
    {
      if (CancelClick != null)
        CancelClick(this, e);
    }
        
    public virtual void OnUpdateClick(EventArgs e)
    {
      if (UpdateClick != null)
        UpdateClick(this, e);
    }
        
    public virtual void OnHelpClick(EventArgs e)
    {
      if (HelpClick != null)
        HelpClick(this, e);
    }

    protected void UpdateControlButtons()
    {
      // Track next button inserted position
      int x = this.Width - m_buttonGap - m_buttonFinish.Width;
            
      bool showHelp = ShouldShowHelp();
      bool showClose = ShouldShowClose();
      bool showFinish = ShouldShowFinish();
      bool showNext = ShouldShowNext();
      bool showBack = ShouldShowBack();
      bool showCancel = ShouldShowCancel();
      bool showUpdate = ShouldShowUpdate();
            
      if (showHelp) 
      {
        m_buttonHelp.Left = x;
        x -= m_buttonHelp.Width + m_buttonGap;
        m_buttonHelp.Enabled = ShouldEnableHelp();
        m_buttonHelp.Show();
      }
      else
        m_buttonHelp.Hide();

      if (showClose) 
      {
        m_buttonClose.Left = x;
        x -= m_buttonClose.Width + m_buttonGap;
        m_buttonClose.Enabled = ShouldEnableClose();
        m_buttonClose.Show();
      }
      else
        m_buttonClose.Hide();

      if (showFinish) 
      {
        m_buttonFinish.Left = x;
        x -= m_buttonFinish.Width + m_buttonGap;
        m_buttonFinish.Enabled = ShouldEnableFinish();
        m_buttonFinish.Show();
      }
      else
        m_buttonFinish.Hide();

      if (showNext) 
      {
        m_buttonNext.Left = x;
        x -= m_buttonNext.Width + m_buttonGap;
        m_buttonNext.Enabled = ShouldEnableNext();
        m_buttonNext.Show();
      }
      else
        m_buttonNext.Hide();

      if (showBack) 
      {
        m_buttonBack.Left = x;
        x -= m_buttonBack.Width + m_buttonGap;
        m_buttonBack.Enabled = ShouldEnableBack();
        m_buttonBack.Show();
      }
      else
        m_buttonBack.Hide();

      if (showCancel) 
      {
        m_buttonCancel.Left = x;
        x -= m_buttonCancel.Width + m_buttonGap;
        m_buttonCancel.Enabled = ShouldEnableCancel();
        m_buttonCancel.Show();
      }
      else
        m_buttonCancel.Hide();

      if (showUpdate) 
      {
        m_buttonUpdate.Left = x;
        x -= m_buttonUpdate.Width + m_buttonGap;
        m_buttonUpdate.Enabled = ShouldEnableUpdate();
        m_buttonUpdate.Show();
      }
      else
        m_buttonUpdate.Hide();
                
      AutoAssignDefaultButton();
    }
        
    protected void AutoAssignDefaultButton()
    {
      // Get our parent Form instance
      Form parentForm = this.FindForm();
            
      // Cannot assign a default button if we are not on a Form
      if (parentForm != null)
      {
        // Can only assign a particular button if we have been requested 
        // to auto- assign and we are on a selected page
        if (m_assignDefault && (m_tabControl.SelectedIndex >= 0))
        {
          // Button default depends on the profile mode
          switch(m_profile)
          {
            case Profiles.Install:
              // Is this the last page?
              if (m_tabControl.SelectedIndex == (m_tabControl.TabPages.Count - 1))
              {
                // Then use the Close button
                parentForm.AcceptButton = m_buttonClose;
              }
              else
              {
                // Is this the second from last page?
                if (m_tabControl.SelectedIndex == (m_tabControl.TabPages.Count - 2))
                {
                  // Then use the Cancel button
                  parentForm.AcceptButton = m_buttonCancel;
                }
                else
                {
                  // Then use the Next button
                  parentForm.AcceptButton = m_buttonNext;
                }
              }
              break;
            case Profiles.Configure:
              // Is this the last page?
              if (m_tabControl.SelectedIndex == (m_tabControl.TabPages.Count - 1))
              {
                // Then always use the Finish button
                parentForm.AcceptButton = m_buttonFinish;
              }
              else
              {
                // Else we are not on last page, use the Next button
                parentForm.AcceptButton = m_buttonNext;
              }
              break;
            case Profiles.Controller:
              // Always use the Update button
              parentForm.AcceptButton = m_buttonUpdate;
              break;
          }
        }
        else
        {
          // Remove any assigned default button
          parentForm.AcceptButton = null;
        }
      }
    }
        
    protected bool ShouldShowClose()
    {
      bool ret = false;
        
      switch(m_showClose)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            // Must have at least one page
            if (m_tabControl.SelectedIndex != -1)
            {
              // Cannot 'Close' unless on the last page
              if (m_tabControl.SelectedIndex == (m_tabControl.TabPages.Count - 1))
                ret = true;
            }
            break;
          case Profiles.Configure:
            break;
          case Profiles.Controller:
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableClose()
    {
      bool ret = false;
        
      switch(m_enableClose)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldShowFinish()
    {
      bool ret = false;
        
      switch(m_showFinish)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            break;
          case Profiles.Configure:
            ret = true;
            break;
          case Profiles.Controller:
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableFinish()
    {
      bool ret = false;
        
      switch(m_enableFinish)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldShowNext()
    {
      bool ret = false;

      switch(m_showNext)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            // Must have at least one page
            if (m_tabControl.SelectedIndex != -1)
            {
              // Cannot 'Next' when at the last or second to last pages
              if (m_tabControl.SelectedIndex < (m_tabControl.TabPages.Count - 2))
                ret = true;
            }
            break;
          case Profiles.Configure:
            ret = true;
            break;
          case Profiles.Controller:
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableNext()
    {
      bool ret = false;

      switch(m_enableNext)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            // Must have at least one page
            if (m_tabControl.SelectedIndex != -1)
            {
              // Cannot 'Next' when at the last or second to last pages
              if (m_tabControl.SelectedIndex < (m_tabControl.TabPages.Count - 2))
                ret = true;
            }
            break;
          case Profiles.Configure:
          case Profiles.Controller:
            // Must have at least one page
            if (m_tabControl.SelectedIndex != -1)
            {
              // Cannot 'Next' when at the last or second to last pages
              if (m_tabControl.SelectedIndex < (m_tabControl.TabPages.Count - 1))
                ret = true;
            }
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldShowBack()
    {
      bool ret = false;

      switch(m_showBack)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            // Cannot 'Back' when one the first page or on the last two special pages
            if ((m_tabControl.SelectedIndex > 0) && (m_tabControl.SelectedIndex < (m_tabControl.TabPages.Count - 2)))
              ret = true;
            break;
          case Profiles.Configure:
            ret = true;
            break;
          case Profiles.Controller:
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableBack()
    {
      bool ret = false;

      switch(m_enableBack)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          // Cannot 'Back' when one the first page
          if (m_tabControl.SelectedIndex > 0)
            ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldShowCancel()
    {
      bool ret = false;

      switch(m_showCancel)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            // Must have at least one page
            if (m_tabControl.SelectedIndex != -1)
            {
              // Cannot 'Cancel' on the last page of an Install
              if (m_tabControl.SelectedIndex < (m_tabControl.TabPages.Count - 1))
                ret = true;
            }
            break;
          case Profiles.Configure:
            ret = true;
            break;
          case Profiles.Controller:
            ret = true;
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableCancel()
    {
      bool ret = false;

      switch(m_enableCancel)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldShowUpdate()
    {
      bool ret = false;

      switch(m_showUpdate)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
        switch(m_profile)
        {
          case Profiles.Install:
            break;
          case Profiles.Configure:
            break;
          case Profiles.Controller:
            ret = true;
            break;
        }
          break;
      }

      return ret;
    }

    protected bool ShouldEnableUpdate()
    {
      bool ret = false;

      switch(m_enableUpdate)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldEnableHelp()
    {
      bool ret = false;

      switch(m_enableCancel)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          ret = true;
          break;
      }

      return ret;
    }

    protected bool ShouldShowHelp()
    {
      bool ret = false;

      switch(m_showUpdate)
      {
        case Status.No:
          break;
        case Status.Yes:
          ret = true;
          break;
        case Status.Default:
          break;
      }

      return ret;
    }

    protected void LeaveFullPage()
    {
      m_panelTop.Show();
      m_tabControl.Top = m_panelTop.Height;
      m_tabControl.Height = m_panelBottom.Top - m_panelTop.Height - 1;
    }
        
    protected void EnterFullPage()
    {
      m_panelTop.Hide();
      m_tabControl.Top = 0;
      m_tabControl.Height = m_panelBottom.Top - 1;
    }

    protected void OnTabSelectionChanged(object sender, EventArgs e)
    {
      // Update buttons to reflect change
      UpdateControlButtons();
            
      if (m_tabControl.SelectedIndex != -1)
      {
        // Get the selected wizard page
        WizardPage wp = m_wizardPages[m_tabControl.SelectedIndex];
                
        // Should we be presented in full page?
        if (wp.FullPage)
          EnterFullPage();
        else
        {
          // Controller profile is not allowed to be outside of FullMode
          if (m_profile != Profiles.Controller)
            LeaveFullPage();
        }
      }
      else
      {
        // Controller profile is not allowed to be outside of FullMode
        if (m_profile != Profiles.Controller)
          LeaveFullPage();
      }
            
      // Update manual drawn text
      m_panelTop.Invalidate();
            
      // Generate raw selection changed event
      OnSelectionChanged(EventArgs.Empty);
            
      // Generate page leave event if currently on a valid page
      if (m_selectedPage != null)
      {
        OnWizardPageLeave(m_selectedPage);
        m_selectedPage = null;
      }
            
      // Remember which is the newly seleced page
      if (m_tabControl.SelectedIndex != -1)
        m_selectedPage = m_wizardPages[m_tabControl.SelectedIndex] as WizardPage;
            
      // Generate page enter event is now on a valid page
      if (m_selectedPage != null)
        OnWizardPageEnter(m_selectedPage);
    }

    protected void OnButtonHelp(object sender, EventArgs e)
    {
      // Fire event for interested handlers
      OnHelpClick(EventArgs.Empty);
    }

    protected void OnButtonClose(object sender, EventArgs e)
    {
      // Fire event for interested handlers
      OnCloseClick(EventArgs.Empty);
    }

    protected void OnButtonFinish(object sender, EventArgs e)
    {
      // Fire event for interested handlers
      OnFinishClick(EventArgs.Empty);
    }

    protected void OnButtonNext(object sender, EventArgs e)
    {
      CancelEventArgs ce = new CancelEventArgs(false);
            
      // Give handlers chance to cancel this action
      OnNextClick(ce);
            
      if (!ce.Cancel)
      {
        // Move to the next page if there is one
        if (m_tabControl.SelectedIndex < m_tabControl.TabPages.Count - 1)
          m_tabControl.SelectedIndex++;
      }
    }

    protected void OnButtonBack(object sender, EventArgs e)
    {
      CancelEventArgs ce = new CancelEventArgs(false);
            
      // Give handlers chance to cancel this action
      OnBackClick(ce);
            
      if (!ce.Cancel)
      {
        // Move to the next page if there is one
        if (m_tabControl.SelectedIndex > 0)
          m_tabControl.SelectedIndex--;
      }
    }

    protected void OnButtonCancel(object sender, EventArgs e)
    {
      // Fire event for interested handlers
      OnCancelClick(EventArgs.Empty);
    }

    protected void OnButtonUpdate(object sender, EventArgs e)
    {
      // Fire event for interested handlers
      OnUpdateClick(EventArgs.Empty);
    }

    protected void OnWizardCleared()
    {
      // Unhook from event handlers for each page
      foreach(WizardPage wp in m_tabControl.TabPages)
      {
        wp.FullPageChanged -= new EventHandler(OnWizardFullPageChanged);
        wp.SubTitleChanged -= new EventHandler(OnWizardSubTitleChanged);
        wp.CaptionTitleChanged -= new EventHandler(OnWizardCaptionTitleChanged);
      }
        
      // Reflect change on underlying tab control
      m_tabControl.TabPages.Clear();

      // Update buttons to reflect status
      UpdateControlButtons();
    }
        
    protected void OnWizardInserted(int index, object value)
    {
      WizardPage wp = value as WizardPage;
           
      // Monitor property changes
      wp.FullPageChanged += new EventHandler(OnWizardFullPageChanged);
      wp.SubTitleChanged += new EventHandler(OnWizardSubTitleChanged);
      wp.CaptionTitleChanged += new EventHandler(OnWizardCaptionTitleChanged);
        
      // Reflect change on underlying tab control
      m_tabControl.TabPages.Insert(index, wp);

      // Update buttons to reflect status
      UpdateControlButtons();
    }
        
    protected void OnWizardRemoved(int index, object value)
    {
      WizardPage wp = m_tabControl.TabPages[index] as WizardPage;
        
      // Unhook from event handlers
      wp.FullPageChanged -= new EventHandler(OnWizardFullPageChanged);
      wp.SubTitleChanged -= new EventHandler(OnWizardSubTitleChanged);
      wp.CaptionTitleChanged -= new EventHandler(OnWizardCaptionTitleChanged);

      // Reflect change on underlying tab control
      m_tabControl.TabPages.RemoveAt(index);

      // Update buttons to reflect status
      UpdateControlButtons();
    }
        
    protected void OnWizardFullPageChanged(object sender, EventArgs e)
    {
      WizardPage wp = sender as WizardPage;
            
      // Is it the current page that has changed FullPage?
      if (m_tabControl.SelectedIndex == m_wizardPages.IndexOf(wp))
      {
        // Should we be presented in full page?
        if (wp.FullPage)
          EnterFullPage();
        else
        {
          // Controller profile is not allowed to be outside of FullMode
          if (m_profile != Profiles.Controller)
            LeaveFullPage();
        }
      }
    }

    protected void OnWizardSubTitleChanged(object sender, EventArgs e)
    {
      WizardPage wp = sender as WizardPage;
           
      // Is it the current page that has changed sub title?
      if (m_tabControl.SelectedIndex == m_wizardPages.IndexOf(wp))
      {
        // Force the sub title to be updated now
        m_panelTop.Invalidate();
      }
    }        
        
    protected void OnWizardCaptionTitleChanged(object sender, EventArgs e)
    {
      // Generate event so any dialog containing use can be notify
      if (WizardCaptionTitleChanged != null)
        WizardCaptionTitleChanged(this, e);
    }
    
    protected override void OnResize(EventArgs e)
    {
      this.PerformLayout();
    }

    protected void OnRepaintPanels(object sender, EventArgs e)
    {
      m_panelTop.Invalidate();
      m_panelBottom.Invalidate();
    }

    protected void OnPaintTopPanel(object sender, PaintEventArgs pe)
    {
      int right = m_panelTop.Width;
        
      // Any picture to draw?
      if (m_picture != null)
      {
        // Calculate starting Y position to give equal space above and below image
        int Y = (int)((m_panelTop.Height - m_picture.Height) / 2);
                
        pe.Graphics.DrawImage(m_picture, m_panelTop.Width - m_picture.Width - Y, Y, m_picture.Width, m_picture.Height);
                
        // Adjust right side by width of width and gaps around it
        right -= m_picture.Width + Y + m_panelGap;
      }
        
      // Create main title drawing rectangle
      RectangleF drawRectF = new Rectangle(m_panelGap, m_panelGap, right - m_panelGap, m_fontTitle.Height);
                                                
      StringFormat drawFormat = new StringFormat();
      drawFormat.Alignment = StringAlignment.Near;
      drawFormat.LineAlignment = StringAlignment.Center;
      drawFormat.Trimming = StringTrimming.EllipsisCharacter;
      drawFormat.FormatFlags = StringFormatFlags.NoClip |
        StringFormatFlags.NoWrap;
            
      using(SolidBrush mainTitleBrush = new SolidBrush(m_colorTitle))
        pe.Graphics.DrawString(m_title, m_fontTitle, mainTitleBrush, drawRectF, drawFormat);            
             
      // Is there a selected tab for display?   
      if (m_tabControl.SelectedIndex != -1)
      {                
        // Adjust rectangle for rest of the drawing text space
        drawRectF.Y = drawRectF.Bottom + (m_panelGap / 2);
        drawRectF.X += m_panelGap;
        drawRectF.Width -= m_panelGap;
        drawRectF.Height = m_panelTop.Height - drawRectF.Y - (m_panelGap / 2);

        // No longer want to prevent word wrap to extra lines
        drawFormat.LineAlignment = StringAlignment.Near;
        drawFormat.FormatFlags = StringFormatFlags.NoClip;

        WizardPage wp = m_tabControl.TabPages[m_tabControl.SelectedIndex] as WizardPage;

        using(SolidBrush subTitleBrush = new SolidBrush(m_colorSubTitle))
          pe.Graphics.DrawString(wp.SubTitle, m_fontSubTitle, subTitleBrush, drawRectF, drawFormat);
      }                          
        
      using(Pen lightPen = new Pen(m_panelTop.BackColor),
              darkPen = new Pen(ControlPaint.Light(ControlPaint.Dark(this.BackColor))))
      {
        pe.Graphics.DrawLine(darkPen, 0, m_panelTop.Height - 2, m_panelTop.Width, m_panelTop.Height - 2);
        pe.Graphics.DrawLine(lightPen, 0, m_panelTop.Height - 1, m_panelTop.Width, m_panelTop.Height - 1);
      }            
    }
        
    protected void OnPaintBottomPanel(object sender, PaintEventArgs pe)
    {
      using(Pen lightPen = new Pen(ControlPaint.Light(this.BackColor)),
              darkPen = new Pen(ControlPaint.Light(ControlPaint.Dark(this.BackColor))))
      {
        pe.Graphics.DrawLine(darkPen, 0, 0, m_panelBottom.Width, 0);
        pe.Graphics.DrawLine(lightPen, 0, 1, m_panelBottom.Width, 1);
      }            
    }
  }
}

#region revision history

// $Log: WizardControl.cs,v $
// Revision 1.1  2005/10/11 14:42:12  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/WizardControl/WizardControl.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $
// $Id: WizardControl.cs,v 1.1 2005/10/11 14:42:12 mkontak Exp $

#endregion


