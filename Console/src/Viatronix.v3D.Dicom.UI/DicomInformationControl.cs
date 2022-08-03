using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using Viatronix.v3D.Imaging;
using Viatronix.v3D.Visualization;
using vxDicom = Viatronix.v3D.Dicom;
using vxUI = Viatronix.v3D.UI;

namespace Viatronix.v3D.Dicom.UI
{
  /// <summary>
  /// Summary description for DicomInformationControl.
  /// </summary>
  public class DicomInformationControl : System.Windows.Forms.UserControl
  {

    #region variables 

    /// <summary>
    /// Flag to turn element saving on and off
    /// </summary>
    bool m_bAllowElementSaving = false;

    /// <summary>
    /// Flag indicating if the title is centered
    /// </summary>
    bool m_centered = true;

    /// <summary>
    /// Element list filter
    /// </summary>
    private string m_filter = String.Empty;

    /// <summary>
    /// Spaces
    /// </summary>
    private static string m_spaces = "                                                                                                                                                                                                                                                                                                                                                                                                   ";

    /// <summary>
    /// Data grids title
    /// </summary>
    private static string m_dataGridCaption = "Dicom Tags";
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.ContextMenu m_contextMenu;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAs;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAsTxt;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAsXml;
    private System.Windows.Forms.Label m_labelDicomGroup;
    private System.Windows.Forms.ComboBox m_comboBoxGroups;
    private System.Windows.Forms.GroupBox m_groupBoxDicomInformation;
    private System.Windows.Forms.Label m_labelSearch;
    private System.Windows.Forms.TextBox m_textBoxSearch;
    private System.Windows.Forms.Button m_buttonSearch;
    private System.Windows.Forms.DataGrid m_dataGridDicom;
    private System.ComponentModel.IContainer components;

    #endregion

    #region initialization

    public DicomInformationControl()
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      if ( m_bAllowElementSaving )
      {
        m_dataGridDicom.ContextMenu = m_contextMenu;
      }
      else
      {
        m_dataGridDicom.ContextMenu = null;
      }

    }

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

    #endregion

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(DicomInformationControl));
      this.m_contextMenu = new System.Windows.Forms.ContextMenu();
      this.m_menuItemSaveElementAs = new System.Windows.Forms.MenuItem();
      this.m_menuItemSaveElementAsTxt = new System.Windows.Forms.MenuItem();
      this.m_menuItemSaveElementAsXml = new System.Windows.Forms.MenuItem();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_groupBoxDicomInformation = new System.Windows.Forms.GroupBox();
      this.m_dataGridDicom = new System.Windows.Forms.DataGrid();
      this.m_buttonSearch = new System.Windows.Forms.Button();
      this.m_textBoxSearch = new System.Windows.Forms.TextBox();
      this.m_labelSearch = new System.Windows.Forms.Label();
      this.m_comboBoxGroups = new System.Windows.Forms.ComboBox();
      this.m_labelDicomGroup = new System.Windows.Forms.Label();
      this.m_groupBoxDicomInformation.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.m_dataGridDicom)).BeginInit();
      this.SuspendLayout();
      // 
      // m_contextMenu
      // 
      this.m_contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                  this.m_menuItemSaveElementAs});
      this.m_contextMenu.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_contextMenu.RightToLeft")));
      // 
      // m_menuItemSaveElementAs
      // 
      this.m_menuItemSaveElementAs.Enabled = ((bool)(resources.GetObject("m_menuItemSaveElementAs.Enabled")));
      this.m_menuItemSaveElementAs.Index = 0;
      this.m_menuItemSaveElementAs.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                            this.m_menuItemSaveElementAsTxt,
                                                                                            this.m_menuItemSaveElementAsXml});
      this.m_menuItemSaveElementAs.Shortcut = ((System.Windows.Forms.Shortcut)(resources.GetObject("m_menuItemSaveElementAs.Shortcut")));
      this.m_menuItemSaveElementAs.ShowShortcut = ((bool)(resources.GetObject("m_menuItemSaveElementAs.ShowShortcut")));
      this.m_menuItemSaveElementAs.Text = resources.GetString("m_menuItemSaveElementAs.Text");
      this.m_menuItemSaveElementAs.Visible = ((bool)(resources.GetObject("m_menuItemSaveElementAs.Visible")));
      // 
      // m_menuItemSaveElementAsTxt
      // 
      this.m_menuItemSaveElementAsTxt.Enabled = ((bool)(resources.GetObject("m_menuItemSaveElementAsTxt.Enabled")));
      this.m_menuItemSaveElementAsTxt.Index = 0;
      this.m_menuItemSaveElementAsTxt.Shortcut = ((System.Windows.Forms.Shortcut)(resources.GetObject("m_menuItemSaveElementAsTxt.Shortcut")));
      this.m_menuItemSaveElementAsTxt.ShowShortcut = ((bool)(resources.GetObject("m_menuItemSaveElementAsTxt.ShowShortcut")));
      this.m_menuItemSaveElementAsTxt.Text = resources.GetString("m_menuItemSaveElementAsTxt.Text");
      this.m_menuItemSaveElementAsTxt.Visible = ((bool)(resources.GetObject("m_menuItemSaveElementAsTxt.Visible")));
      // 
      // m_menuItemSaveElementAsXml
      // 
      this.m_menuItemSaveElementAsXml.Enabled = ((bool)(resources.GetObject("m_menuItemSaveElementAsXml.Enabled")));
      this.m_menuItemSaveElementAsXml.Index = 1;
      this.m_menuItemSaveElementAsXml.Shortcut = ((System.Windows.Forms.Shortcut)(resources.GetObject("m_menuItemSaveElementAsXml.Shortcut")));
      this.m_menuItemSaveElementAsXml.ShowShortcut = ((bool)(resources.GetObject("m_menuItemSaveElementAsXml.ShowShortcut")));
      this.m_menuItemSaveElementAsXml.Text = resources.GetString("m_menuItemSaveElementAsXml.Text");
      this.m_menuItemSaveElementAsXml.Visible = ((bool)(resources.GetObject("m_menuItemSaveElementAsXml.Visible")));
      // 
      // m_groupBoxDicomInformation
      // 
      this.m_groupBoxDicomInformation.AccessibleDescription = resources.GetString("m_groupBoxDicomInformation.AccessibleDescription");
      this.m_groupBoxDicomInformation.AccessibleName = resources.GetString("m_groupBoxDicomInformation.AccessibleName");
      this.m_groupBoxDicomInformation.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_groupBoxDicomInformation.Anchor")));
      this.m_groupBoxDicomInformation.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("m_groupBoxDicomInformation.BackgroundImage")));
      this.m_groupBoxDicomInformation.Controls.Add(this.m_dataGridDicom);
      this.m_groupBoxDicomInformation.Controls.Add(this.m_buttonSearch);
      this.m_groupBoxDicomInformation.Controls.Add(this.m_textBoxSearch);
      this.m_groupBoxDicomInformation.Controls.Add(this.m_labelSearch);
      this.m_groupBoxDicomInformation.Controls.Add(this.m_comboBoxGroups);
      this.m_groupBoxDicomInformation.Controls.Add(this.m_labelDicomGroup);
      this.m_groupBoxDicomInformation.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_groupBoxDicomInformation.Dock")));
      this.m_groupBoxDicomInformation.Enabled = ((bool)(resources.GetObject("m_groupBoxDicomInformation.Enabled")));
      this.m_groupBoxDicomInformation.Font = ((System.Drawing.Font)(resources.GetObject("m_groupBoxDicomInformation.Font")));
      this.m_groupBoxDicomInformation.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_groupBoxDicomInformation.ImeMode")));
      this.m_groupBoxDicomInformation.Location = ((System.Drawing.Point)(resources.GetObject("m_groupBoxDicomInformation.Location")));
      this.m_groupBoxDicomInformation.Name = "m_groupBoxDicomInformation";
      this.m_groupBoxDicomInformation.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_groupBoxDicomInformation.RightToLeft")));
      this.m_groupBoxDicomInformation.Size = ((System.Drawing.Size)(resources.GetObject("m_groupBoxDicomInformation.Size")));
      this.m_groupBoxDicomInformation.TabIndex = ((int)(resources.GetObject("m_groupBoxDicomInformation.TabIndex")));
      this.m_groupBoxDicomInformation.TabStop = false;
      this.m_groupBoxDicomInformation.Text = resources.GetString("m_groupBoxDicomInformation.Text");
      this.m_toolTip.SetToolTip(this.m_groupBoxDicomInformation, resources.GetString("m_groupBoxDicomInformation.ToolTip"));
      this.m_groupBoxDicomInformation.Visible = ((bool)(resources.GetObject("m_groupBoxDicomInformation.Visible")));
      this.m_groupBoxDicomInformation.Resize += new System.EventHandler(this.OnGroupBoxResize);
      // 
      // m_dataGridDicom
      // 
      this.m_dataGridDicom.AccessibleDescription = resources.GetString("m_dataGridDicom.AccessibleDescription");
      this.m_dataGridDicom.AccessibleName = resources.GetString("m_dataGridDicom.AccessibleName");
      this.m_dataGridDicom.AllowNavigation = false;
      this.m_dataGridDicom.AllowSorting = false;
      this.m_dataGridDicom.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_dataGridDicom.Anchor")));
      this.m_dataGridDicom.BackgroundColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.m_dataGridDicom.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("m_dataGridDicom.BackgroundImage")));
      this.m_dataGridDicom.CaptionBackColor = System.Drawing.Color.Silver;
      this.m_dataGridDicom.CaptionFont = ((System.Drawing.Font)(resources.GetObject("m_dataGridDicom.CaptionFont")));
      this.m_dataGridDicom.CaptionForeColor = System.Drawing.Color.Navy;
      this.m_dataGridDicom.CaptionText = resources.GetString("m_dataGridDicom.CaptionText");
      this.m_dataGridDicom.DataMember = "";
      this.m_dataGridDicom.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_dataGridDicom.Dock")));
      this.m_dataGridDicom.Enabled = ((bool)(resources.GetObject("m_dataGridDicom.Enabled")));
      this.m_dataGridDicom.Font = ((System.Drawing.Font)(resources.GetObject("m_dataGridDicom.Font")));
      this.m_dataGridDicom.ForeColor = System.Drawing.Color.Navy;
      this.m_dataGridDicom.GridLineColor = System.Drawing.Color.Navy;
      this.m_dataGridDicom.HeaderBackColor = System.Drawing.SystemColors.ActiveBorder;
      this.m_dataGridDicom.HeaderFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.m_dataGridDicom.HeaderForeColor = System.Drawing.Color.Navy;
      this.m_dataGridDicom.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_dataGridDicom.ImeMode")));
      this.m_dataGridDicom.Location = ((System.Drawing.Point)(resources.GetObject("m_dataGridDicom.Location")));
      this.m_dataGridDicom.Name = "m_dataGridDicom";
      this.m_dataGridDicom.ParentRowsForeColor = System.Drawing.Color.Navy;
      this.m_dataGridDicom.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_dataGridDicom.RightToLeft")));
      this.m_dataGridDicom.RowHeaderWidth = 10;
      this.m_dataGridDicom.SelectionBackColor = System.Drawing.Color.RoyalBlue;
      this.m_dataGridDicom.Size = ((System.Drawing.Size)(resources.GetObject("m_dataGridDicom.Size")));
      this.m_dataGridDicom.TabIndex = ((int)(resources.GetObject("m_dataGridDicom.TabIndex")));
      this.m_dataGridDicom.Tag = "Dicom information ";
      this.m_toolTip.SetToolTip(this.m_dataGridDicom, resources.GetString("m_dataGridDicom.ToolTip"));
      this.m_dataGridDicom.Visible = ((bool)(resources.GetObject("m_dataGridDicom.Visible")));
      // 
      // m_buttonSearch
      // 
      this.m_buttonSearch.AccessibleDescription = resources.GetString("m_buttonSearch.AccessibleDescription");
      this.m_buttonSearch.AccessibleName = resources.GetString("m_buttonSearch.AccessibleName");
      this.m_buttonSearch.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_buttonSearch.Anchor")));
      this.m_buttonSearch.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("m_buttonSearch.BackgroundImage")));
      this.m_buttonSearch.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_buttonSearch.Dock")));
      this.m_buttonSearch.Enabled = ((bool)(resources.GetObject("m_buttonSearch.Enabled")));
      this.m_buttonSearch.FlatStyle = ((System.Windows.Forms.FlatStyle)(resources.GetObject("m_buttonSearch.FlatStyle")));
      this.m_buttonSearch.Font = ((System.Drawing.Font)(resources.GetObject("m_buttonSearch.Font")));
      this.m_buttonSearch.Image = ((System.Drawing.Image)(resources.GetObject("m_buttonSearch.Image")));
      this.m_buttonSearch.ImageAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_buttonSearch.ImageAlign")));
      this.m_buttonSearch.ImageIndex = ((int)(resources.GetObject("m_buttonSearch.ImageIndex")));
      this.m_buttonSearch.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_buttonSearch.ImeMode")));
      this.m_buttonSearch.Location = ((System.Drawing.Point)(resources.GetObject("m_buttonSearch.Location")));
      this.m_buttonSearch.Name = "m_buttonSearch";
      this.m_buttonSearch.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_buttonSearch.RightToLeft")));
      this.m_buttonSearch.Size = ((System.Drawing.Size)(resources.GetObject("m_buttonSearch.Size")));
      this.m_buttonSearch.TabIndex = ((int)(resources.GetObject("m_buttonSearch.TabIndex")));
      this.m_buttonSearch.Text = resources.GetString("m_buttonSearch.Text");
      this.m_buttonSearch.TextAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_buttonSearch.TextAlign")));
      this.m_toolTip.SetToolTip(this.m_buttonSearch, resources.GetString("m_buttonSearch.ToolTip"));
      this.m_buttonSearch.Visible = ((bool)(resources.GetObject("m_buttonSearch.Visible")));
      this.m_buttonSearch.Click += new System.EventHandler(this.OnSearchButtonClick);
      // 
      // m_textBoxSearch
      // 
      this.m_textBoxSearch.AccessibleDescription = resources.GetString("m_textBoxSearch.AccessibleDescription");
      this.m_textBoxSearch.AccessibleName = resources.GetString("m_textBoxSearch.AccessibleName");
      this.m_textBoxSearch.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_textBoxSearch.Anchor")));
      this.m_textBoxSearch.AutoSize = ((bool)(resources.GetObject("m_textBoxSearch.AutoSize")));
      this.m_textBoxSearch.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("m_textBoxSearch.BackgroundImage")));
      this.m_textBoxSearch.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_textBoxSearch.Dock")));
      this.m_textBoxSearch.Enabled = ((bool)(resources.GetObject("m_textBoxSearch.Enabled")));
      this.m_textBoxSearch.Font = ((System.Drawing.Font)(resources.GetObject("m_textBoxSearch.Font")));
      this.m_textBoxSearch.ForeColor = System.Drawing.Color.Navy;
      this.m_textBoxSearch.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_textBoxSearch.ImeMode")));
      this.m_textBoxSearch.Location = ((System.Drawing.Point)(resources.GetObject("m_textBoxSearch.Location")));
      this.m_textBoxSearch.MaxLength = ((int)(resources.GetObject("m_textBoxSearch.MaxLength")));
      this.m_textBoxSearch.Multiline = ((bool)(resources.GetObject("m_textBoxSearch.Multiline")));
      this.m_textBoxSearch.Name = "m_textBoxSearch";
      this.m_textBoxSearch.PasswordChar = ((char)(resources.GetObject("m_textBoxSearch.PasswordChar")));
      this.m_textBoxSearch.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_textBoxSearch.RightToLeft")));
      this.m_textBoxSearch.ScrollBars = ((System.Windows.Forms.ScrollBars)(resources.GetObject("m_textBoxSearch.ScrollBars")));
      this.m_textBoxSearch.Size = ((System.Drawing.Size)(resources.GetObject("m_textBoxSearch.Size")));
      this.m_textBoxSearch.TabIndex = ((int)(resources.GetObject("m_textBoxSearch.TabIndex")));
      this.m_textBoxSearch.Text = resources.GetString("m_textBoxSearch.Text");
      this.m_textBoxSearch.TextAlign = ((System.Windows.Forms.HorizontalAlignment)(resources.GetObject("m_textBoxSearch.TextAlign")));
      this.m_toolTip.SetToolTip(this.m_textBoxSearch, resources.GetString("m_textBoxSearch.ToolTip"));
      this.m_textBoxSearch.Visible = ((bool)(resources.GetObject("m_textBoxSearch.Visible")));
      this.m_textBoxSearch.WordWrap = ((bool)(resources.GetObject("m_textBoxSearch.WordWrap")));
      // 
      // m_labelSearch
      // 
      this.m_labelSearch.AccessibleDescription = resources.GetString("m_labelSearch.AccessibleDescription");
      this.m_labelSearch.AccessibleName = resources.GetString("m_labelSearch.AccessibleName");
      this.m_labelSearch.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_labelSearch.Anchor")));
      this.m_labelSearch.AutoSize = ((bool)(resources.GetObject("m_labelSearch.AutoSize")));
      this.m_labelSearch.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_labelSearch.Dock")));
      this.m_labelSearch.Enabled = ((bool)(resources.GetObject("m_labelSearch.Enabled")));
      this.m_labelSearch.Font = ((System.Drawing.Font)(resources.GetObject("m_labelSearch.Font")));
      this.m_labelSearch.Image = ((System.Drawing.Image)(resources.GetObject("m_labelSearch.Image")));
      this.m_labelSearch.ImageAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_labelSearch.ImageAlign")));
      this.m_labelSearch.ImageIndex = ((int)(resources.GetObject("m_labelSearch.ImageIndex")));
      this.m_labelSearch.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_labelSearch.ImeMode")));
      this.m_labelSearch.Location = ((System.Drawing.Point)(resources.GetObject("m_labelSearch.Location")));
      this.m_labelSearch.Name = "m_labelSearch";
      this.m_labelSearch.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_labelSearch.RightToLeft")));
      this.m_labelSearch.Size = ((System.Drawing.Size)(resources.GetObject("m_labelSearch.Size")));
      this.m_labelSearch.TabIndex = ((int)(resources.GetObject("m_labelSearch.TabIndex")));
      this.m_labelSearch.Text = resources.GetString("m_labelSearch.Text");
      this.m_labelSearch.TextAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_labelSearch.TextAlign")));
      this.m_toolTip.SetToolTip(this.m_labelSearch, resources.GetString("m_labelSearch.ToolTip"));
      this.m_labelSearch.Visible = ((bool)(resources.GetObject("m_labelSearch.Visible")));
      // 
      // m_comboBoxGroups
      // 
      this.m_comboBoxGroups.AccessibleDescription = resources.GetString("m_comboBoxGroups.AccessibleDescription");
      this.m_comboBoxGroups.AccessibleName = resources.GetString("m_comboBoxGroups.AccessibleName");
      this.m_comboBoxGroups.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_comboBoxGroups.Anchor")));
      this.m_comboBoxGroups.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("m_comboBoxGroups.BackgroundImage")));
      this.m_comboBoxGroups.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_comboBoxGroups.Dock")));
      this.m_comboBoxGroups.Enabled = ((bool)(resources.GetObject("m_comboBoxGroups.Enabled")));
      this.m_comboBoxGroups.Font = ((System.Drawing.Font)(resources.GetObject("m_comboBoxGroups.Font")));
      this.m_comboBoxGroups.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxGroups.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_comboBoxGroups.ImeMode")));
      this.m_comboBoxGroups.IntegralHeight = ((bool)(resources.GetObject("m_comboBoxGroups.IntegralHeight")));
      this.m_comboBoxGroups.ItemHeight = ((int)(resources.GetObject("m_comboBoxGroups.ItemHeight")));
      this.m_comboBoxGroups.Location = ((System.Drawing.Point)(resources.GetObject("m_comboBoxGroups.Location")));
      this.m_comboBoxGroups.MaxDropDownItems = ((int)(resources.GetObject("m_comboBoxGroups.MaxDropDownItems")));
      this.m_comboBoxGroups.MaxLength = ((int)(resources.GetObject("m_comboBoxGroups.MaxLength")));
      this.m_comboBoxGroups.Name = "m_comboBoxGroups";
      this.m_comboBoxGroups.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_comboBoxGroups.RightToLeft")));
      this.m_comboBoxGroups.Size = ((System.Drawing.Size)(resources.GetObject("m_comboBoxGroups.Size")));
      this.m_comboBoxGroups.TabIndex = ((int)(resources.GetObject("m_comboBoxGroups.TabIndex")));
      this.m_comboBoxGroups.Text = resources.GetString("m_comboBoxGroups.Text");
      this.m_toolTip.SetToolTip(this.m_comboBoxGroups, resources.GetString("m_comboBoxGroups.ToolTip"));
      this.m_comboBoxGroups.Visible = ((bool)(resources.GetObject("m_comboBoxGroups.Visible")));
      this.m_comboBoxGroups.SelectedIndexChanged += new System.EventHandler(this.OnGroupSelectedIndexChanged);
      // 
      // m_labelDicomGroup
      // 
      this.m_labelDicomGroup.AccessibleDescription = resources.GetString("m_labelDicomGroup.AccessibleDescription");
      this.m_labelDicomGroup.AccessibleName = resources.GetString("m_labelDicomGroup.AccessibleName");
      this.m_labelDicomGroup.Anchor = ((System.Windows.Forms.AnchorStyles)(resources.GetObject("m_labelDicomGroup.Anchor")));
      this.m_labelDicomGroup.AutoSize = ((bool)(resources.GetObject("m_labelDicomGroup.AutoSize")));
      this.m_labelDicomGroup.Dock = ((System.Windows.Forms.DockStyle)(resources.GetObject("m_labelDicomGroup.Dock")));
      this.m_labelDicomGroup.Enabled = ((bool)(resources.GetObject("m_labelDicomGroup.Enabled")));
      this.m_labelDicomGroup.Font = ((System.Drawing.Font)(resources.GetObject("m_labelDicomGroup.Font")));
      this.m_labelDicomGroup.Image = ((System.Drawing.Image)(resources.GetObject("m_labelDicomGroup.Image")));
      this.m_labelDicomGroup.ImageAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_labelDicomGroup.ImageAlign")));
      this.m_labelDicomGroup.ImageIndex = ((int)(resources.GetObject("m_labelDicomGroup.ImageIndex")));
      this.m_labelDicomGroup.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("m_labelDicomGroup.ImeMode")));
      this.m_labelDicomGroup.Location = ((System.Drawing.Point)(resources.GetObject("m_labelDicomGroup.Location")));
      this.m_labelDicomGroup.Name = "m_labelDicomGroup";
      this.m_labelDicomGroup.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("m_labelDicomGroup.RightToLeft")));
      this.m_labelDicomGroup.Size = ((System.Drawing.Size)(resources.GetObject("m_labelDicomGroup.Size")));
      this.m_labelDicomGroup.TabIndex = ((int)(resources.GetObject("m_labelDicomGroup.TabIndex")));
      this.m_labelDicomGroup.Text = resources.GetString("m_labelDicomGroup.Text");
      this.m_labelDicomGroup.TextAlign = ((System.Drawing.ContentAlignment)(resources.GetObject("m_labelDicomGroup.TextAlign")));
      this.m_toolTip.SetToolTip(this.m_labelDicomGroup, resources.GetString("m_labelDicomGroup.ToolTip"));
      this.m_labelDicomGroup.Visible = ((bool)(resources.GetObject("m_labelDicomGroup.Visible")));
      // 
      // DicomInformationControl
      // 
      this.AccessibleDescription = resources.GetString("$this.AccessibleDescription");
      this.AccessibleName = resources.GetString("$this.AccessibleName");
      this.AllowDrop = true;
      this.AutoScroll = ((bool)(resources.GetObject("$this.AutoScroll")));
      this.AutoScrollMargin = ((System.Drawing.Size)(resources.GetObject("$this.AutoScrollMargin")));
      this.AutoScrollMinSize = ((System.Drawing.Size)(resources.GetObject("$this.AutoScrollMinSize")));
      this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(128)), ((System.Byte)(143)), ((System.Byte)(180)));
      this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
      this.Controls.Add(this.m_groupBoxDicomInformation);
      this.Enabled = ((bool)(resources.GetObject("$this.Enabled")));
      this.Font = ((System.Drawing.Font)(resources.GetObject("$this.Font")));
      this.ForeColor = System.Drawing.Color.Navy;
      this.ImeMode = ((System.Windows.Forms.ImeMode)(resources.GetObject("$this.ImeMode")));
      this.Location = ((System.Drawing.Point)(resources.GetObject("$this.Location")));
      this.Name = "DicomInformationControl";
      this.RightToLeft = ((System.Windows.Forms.RightToLeft)(resources.GetObject("$this.RightToLeft")));
      this.Size = ((System.Drawing.Size)(resources.GetObject("$this.Size")));
      this.m_toolTip.SetToolTip(this, resources.GetString("$this.ToolTip"));
      this.Resize += new System.EventHandler(this.OnResize);
      this.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnDragEnter);
      this.FontChanged += new System.EventHandler(this.OnFontChanged);
      this.DragLeave += new System.EventHandler(this.OnDragLeave);
      this.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnDragDrop);
      this.BackColorChanged += new System.EventHandler(this.OnBackColorChange);
      this.DragOver += new System.Windows.Forms.DragEventHandler(this.OnDragOver);
      this.m_groupBoxDicomInformation.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.m_dataGridDicom)).EndInit();
      this.ResumeLayout(false);

    }
    #endregion

    #region properties

    public string Title
    {
      set { m_groupBoxDicomInformation.Text = value; }
      get { return m_groupBoxDicomInformation.Text; }
    }

    public bool AllowElementSaving
    {
      set { m_bAllowElementSaving = value; }
      get { return m_bAllowElementSaving; }
    }

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the group boxes resize event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnGroupBoxResize(object sender, System.EventArgs e)
    {
      int width = ( Width - 8 ) - m_comboBoxGroups.Left;

      if ( width <= 0 )
      {
        m_comboBoxGroups.Visible = false;
        m_buttonSearch.Visible = false;
        m_textBoxSearch.Visible = false;

      }
      else
      {
        m_comboBoxGroups.Visible = true;
        m_buttonSearch.Visible = true;
        m_textBoxSearch.Visible = true;

        m_comboBoxGroups.Size = new Size( width , m_comboBoxGroups.Height);

        m_buttonSearch.Location = new Point((Width - 8 ) - m_buttonSearch.Width , m_buttonSearch.Top); 

        width = (m_buttonSearch.Left - 10) - m_textBoxSearch.Left;

        if ( width <= 0 )
        {
          m_textBoxSearch.Visible = false;
          m_buttonSearch.Visible = false;
        }
        else
        {
          m_textBoxSearch.Size = new Size( width , m_textBoxSearch.Height);
        }

      }


      int Y = m_textBoxSearch.Top + m_textBoxSearch.Height + 16;
      m_dataGridDicom.Location = new Point(3, Y);
      m_dataGridDicom.Size = new Size(Width - 8, Height - (Y + 13)  );


    }

    private void OnFontChanged(object sender, System.EventArgs e)
    {
      m_groupBoxDicomInformation.Font = this.Font;
    }

    private void OnBackColorChange(object sender, System.EventArgs e)
    {
      m_groupBoxDicomInformation.BackColor = this.BackColor;
    }

    private void OnResize(object sender, System.EventArgs e )
    {
      m_groupBoxDicomInformation.Size = new Size(Width - 2, Height - 11);
    }

    private void OnDragEnter(object sender, System.Windows.Forms.DragEventArgs e)
    {
      Cursor = Cursors.Hand;

      
    }

    private void OnDragLeave(object sender, System.EventArgs e)
    {
      Cursor = Cursors.Default;

    }

    private void OnDragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if ( ! e.Data.GetDataPresent(System.Type.GetType("System.String")) )
      {
        e.Effect = DragDropEffects.None;
        return;
      }
      else
      {
        e.Effect = DragDropEffects.Copy;
      }

    }

    private void OnDicomGridDrapDrop(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if ( e.Effect == DragDropEffects.Copy )
      {
        string sData = e.Data.GetData("System.String").ToString();
      }

     
    }

    private void OnDicomGridDragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if ( ! e.Data.GetDataPresent(System.Type.GetType("System.String")) )
      {
        e.Effect = DragDropEffects.None;
        return;
      }
      else
      {
        e.Effect = DragDropEffects.Copy;
      }

 
    }

    private void OnSaveElementsAsXmlClick(object sender, System.EventArgs e)
    {

      if ( m_dataGridDicom.Tag == null ) return;

      vxDicom.ElementList elementList = (vxDicom.ElementList)m_dataGridDicom.Tag;

      ElementSaveAs saveAs = new ElementSaveAs();
      saveAs.Text = "Save Elements As Xml";
      saveAs.Filename = System.IO.Path.ChangeExtension(elementList.DicomFileName,Viatronix.v3D.Utilities.IO.File.XmlExt);

      if ( saveAs.ShowDialog() == DialogResult.OK )
      {
        string fileName = System.IO.Path.ChangeExtension(saveAs.Filename,Viatronix.v3D.Utilities.IO.File.XmlExt);

        elementList.Dataset.WriteXml(fileName, System.Data.XmlWriteMode.IgnoreSchema);
      }
    }

    private void OnSaveElementsAsTextClick(object sender, System.EventArgs e)
    {

      if ( m_dataGridDicom.Tag == null ) return;

      vxDicom.ElementList elementList = (vxDicom.ElementList)m_dataGridDicom.Tag;

      ElementSaveAs saveAs = new ElementSaveAs();
      saveAs.Text = "Save Elements As Text";
      saveAs.Filename = System.IO.Path.ChangeExtension(elementList.DicomFileName,Viatronix.v3D.Utilities.IO.File.TextExt);

      if ( saveAs.ShowDialog() == DialogResult.OK )
      {
        string fileName = System.IO.Path.ChangeExtension(saveAs.Filename,Viatronix.v3D.Utilities.IO.File.TextExt);

        elementList.WriteElements(fileName);
      }
 
    }


    private void OnDragDrop(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if ( e.Effect == DragDropEffects.Copy )
      {
        string sData = e.Data.GetData("System.String").ToString();
      }

    }

    private void OnDicomDataGridBackButtonClick(object sender, System.EventArgs e)
    {
      m_dataGridDicom.AllowNavigation = false;
    }



    /// <summary>
    /// Handles the click event in the data grid for navigation
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnDicomDataGridClick(object sender, System.EventArgs e)
    {

      DataView dataview = (DataView)m_dataGridDicom.DataSource;

      int rowIndex = m_dataGridDicom.CurrentRowIndex;
      
      DataRowView selectedRow = dataview[rowIndex];

      string elementVR = System.Convert.ToString(selectedRow.Row["VR"]);

      try
      {
        if ( elementVR == "SQ" )
        {
          m_dataGridDicom.AllowNavigation = true;
          m_dataGridDicom.NavigateTo(rowIndex, "SEQUENCE");
        }
      }
      catch ( Exception )
      {

      }

    }

    /// <summary>
    /// Hanles the OnSelectedIndexChanged event for the DICOM GROUP combo box.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnGroupSelectedIndexChanged(object sender, System.EventArgs e)
    {
      DataView dataView = (DataView)m_dataGridDicom.DataSource;
      m_textBoxSearch.Text = "";

      if ( m_comboBoxGroups.SelectedIndex == 0 )
      {
        m_filter = "";
        dataView.RowFilter = "";
        m_textBoxSearch.Enabled = true;
        m_buttonSearch.Enabled = true;
      }
      else
      {
        m_textBoxSearch.Enabled = false;
        m_buttonSearch.Enabled = false;

        char [] sep = { ' ' };
        string [] fields = m_comboBoxGroups.Text.Split(sep);

        dataView.RowFilter = "GROUP = '" + fields[0] + "'";
      }
    }


    /// <summary>
    /// Event handler the event fired when the search text has been changed.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnSearchTextChanged(object sender, System.EventArgs e)
    {

      m_buttonSearch.Enabled = true;
      if ( m_textBoxSearch.Text == "" )
      {
        m_filter = "";
        m_buttonSearch.Enabled = false;
       
      }
    } // OnSearchTextChanged()

    /// <summary>
    /// Event handler the event fired when search button is pressed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnSearchButtonClick(object sender, System.EventArgs e)
    {
      m_filter = "DESCRIPTION LIKE '%" + m_textBoxSearch.Text + "%'";

      PerformSearch(m_filter);
    }

    private void OnDicomDataGridPaint(object sender, System.Windows.Forms.PaintEventArgs e)
    {
      if ( ! m_centered )
      {
        System.Drawing.Graphics g = CreateGraphics();

        int width = m_dataGridDicom.Width;

        SizeF size = g.MeasureString(m_dataGridCaption, m_dataGridDicom.CaptionFont);

        float spaceWidth = width - size.Width;
        float centerWidth = spaceWidth / 2.0F;

        int nspaces = (int)(centerWidth / 3.0F);


        m_dataGridDicom.CaptionText = m_spaces.Substring(0, nspaces) + m_dataGridCaption;

        m_centered = true;
      }
    }

    private void OnDicomDataGridResize(object sender, System.EventArgs e)
    {
      m_centered = false;
    }

    #endregion

    #region methods

    /// <summary>
    /// Adds a table style for the table specified
    /// </summary>
    /// <param name="tableName"></param>
    void AddTableStyle(string tableName)
    {


      DataGridTableStyle style = new DataGridTableStyle();

      style.MappingName = tableName;
      style.AllowSorting = false;
      style.BackColor = System.Drawing.Color.Lavender;
      style.ForeColor = System.Drawing.Color.Navy;
      style.GridLineColor = System.Drawing.Color.DarkGray;
      style.HeaderBackColor = System.Drawing.Color.FromArgb(160, 161, 198);
      style.ForeColor = System.Drawing.Color.DarkBlue;
      style.LinkColor = System.Drawing.Color.Blue;
      style.SelectionBackColor = System.Drawing.Color.FromArgb(200, 200, 200);
      style.SelectionForeColor = System.Drawing.Color.Navy;
  
      style.ColumnHeadersVisible = true;
      style.RowHeadersVisible = true;
      style.ReadOnly = true;

      DataGridTextBoxColumn column = new DataGridTextBoxColumn();
      column.HeaderText = "ParentTag";
      column.MappingName = "PARENT_TAG";
      column.Width = 0;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      column = new DataGridTextBoxColumn();
      column.HeaderText = "Tag";
      column.MappingName = "TAG";
      column.Width = 0;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      column = new DataGridTextBoxColumn();
      column.HeaderText = "Group";
      column.MappingName = "GROUP";
      column.Width = 55;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      column = new DataGridTextBoxColumn();
      column.HeaderText = "Field";
      column.MappingName = "FIELD";
      column.Width = 55;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      column = new DataGridTextBoxColumn();
      column.HeaderText = "Description";
      column.MappingName = "DESCRIPTION";
      column.Width = 200;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);
    
      column = new DataGridTextBoxColumn();
      column.HeaderText = "VR";
      column.MappingName = "VR";
      column.Width = 30;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      column = new DataGridTextBoxColumn();
      column.HeaderText = "Value";
      column.MappingName = "VALUE";
      column.Width = 500;
      column.ReadOnly = true;
      column.Alignment = System.Windows.Forms.HorizontalAlignment.Left;
      style.GridColumnStyles.Add(column);

      m_dataGridDicom.TableStyles.Add(style);

    } // AddtabelStyle()

    /// <summary>
    /// Performs the search on the dicom header
    /// </summary>
    /// <param name="filter">Filter to be used on the dataset</param>
    private void PerformSearch(string filter)
    {

      if ( filter != "" )
      {
        DataView dataView = (DataView)m_dataGridDicom.DataSource;

        if ( dataView != null )
        {
          dataView.RowFilter = filter;
        }
      }

    } // PerformSearch()


    /// <summary>
    /// Set the element list using a dicom file
    /// </summary>
    /// <param name="dicomFile">Dicom file tobe read</param>
    public void SetElements(string dicomFile)
    {
      SetElements(dicomFile, false);
    }




    /// <summary>
    /// Set the element list using a dicom file
    /// </summary>
    /// <param name="dicomFile">Dicom file tobe read</param>
    public void SetElements(string dicomFile, bool savePreviousSettings)
    {
   
      vxDicom.ElementList elementList = new vxDicom.ElementList(dicomFile);

      try
      {
        SetElements(elementList, savePreviousSettings);
      }
      catch ( Exception ex)
      {
        vxUI.MessageBox.ShowError(ex);        
      }
      finally
      {
        elementList.Dispose();
      }

    }

    /// <summary>
    /// Sets the element list using the supplied element list
    /// </summary>
    /// <param name="elementList">Element list</param>
    public void SetElements(vxDicom.ElementList elementList )
    {
      SetElements(elementList, false);
    }


    /// <summary>
    /// Sets the element list using the supplied element list
    /// </summary>
    /// <param name="elementList">Element list</param>
    public void SetElements(vxDicom.ElementList elementList, bool savePreviousSettings )
    {

      DataSet dataset = elementList.Dataset;

      m_dataGridDicom.ReadOnly = true;

      m_dataGridDicom.TableStyles.Clear();

      for ( int i = 0; i < dataset.Tables.Count; i++ )
      {
        AddTableStyle(dataset.Tables[i].TableName);
      }

      m_dataGridDicom.DataSource =  dataset.Tables[elementList.TableName].DefaultView;
      m_dataGridDicom.Tag = elementList;

      vxDicom.Groups groups = elementList.Groups;

      int selectedIndex = 0;
      int currentIndex = m_comboBoxGroups.SelectedIndex;
      

      string search = m_textBoxSearch.Text;
      string currentSetting = ( currentIndex == -1 ? "" : m_comboBoxGroups.Items[currentIndex].ToString());

      m_comboBoxGroups.Items.Clear();
      m_comboBoxGroups.Items.Add("All DICOM groups");

      for ( int i = 0; i < groups.Count; i++ )
      {
        string groupName = groups.get_Group(i).ToString();
        int index = m_comboBoxGroups.Items.Add(groupName);

        if ( groupName == currentSetting )
        {
          selectedIndex = index; 
        }
          
      }
       
      m_comboBoxGroups.SelectedIndex = selectedIndex;
      m_textBoxSearch.Text = search;

      if ( search.Length != 0 )
      {
        OnSearchButtonClick(this, System.EventArgs.Empty );
      }

    }
  
    /// <summary>
    /// Set the element using the supplied volume
    /// </summary>
    /// <param name="volume">Volume to be used to extract the element list from</param>
    public void SetElements(Volume volume)
    {
      SetElements(volume, false);
    }

    /// <summary>
    /// Set the element using the supplied volume
    /// </summary>
    /// <param name="volume">Volume to be used to extract the element list from</param>
    public void SetElements(Volume volume, bool savePreviousSettings)
    {

      vxDicom.ElementList elementList = new vxDicom.ElementList(volume);

      try
      {
        SetElements(elementList, savePreviousSettings);
      }
      catch ( Exception ex)
      {   
        vxUI.MessageBox.ShowError(ex);
      }
      finally
      {
        elementList.Dispose();
      }
    } 

    /// <summary>
    /// Set the element using the supplied dataset
    /// </summary>
    /// <param name="dataset">dataset to be used to extract the element list from</param>
    public void SetElements(Dataset dataset)
    {
      SetElements(dataset, false);
    }

    /// <summary>
    /// Set the element using the supplied dataset
    /// </summary>
    /// <param name="dataset">dataset to be used to extract the element list from</param>
    public void SetElements(Dataset dataset, bool savePreviousSettings)
    {

      vxDicom.ElementList elementList = new vxDicom.ElementList(dataset.IntensityVolume);

      try
      {
        SetElements(elementList, savePreviousSettings );
      }
      catch ( Exception ex)
      {      
        vxUI.MessageBox.ShowError(ex);
      }
      finally
      {
        elementList.Dispose();
      }
    } 
    

    #endregion



    
  }

}
