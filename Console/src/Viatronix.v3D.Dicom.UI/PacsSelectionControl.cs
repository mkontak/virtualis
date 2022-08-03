// $Id: PacsSelectionControl.cs,v 1.8 2007/03/08 21:56:13 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

using Viatronix.Dicom;
using Viatronix.Enterprise;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Implements a control that will display all PACS defined in the host table 
	/// </summary>
  public class PacsSelectionControl : System.Windows.Forms.UserControl
  {

    #region fields

    /// <summary>
    /// Loaded flag
    /// </summary>
    private bool m_loaded = false;

    /// <summary>
    /// Service type
    /// </summary>
    private DicomServiceTypes m_serviceType = DicomServiceTypes.All;

    /// <summary>
    /// Selected Host
    /// </summary>
    private DicomHost m_selectedServer = new DicomHost();

    /// <summary>
    /// Combo box for pacs server selection
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxPacsServer;

    /// <summary> 
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// DataProvider
    /// </summary>
    private IDataProvider m_dataProvider;

    #endregion

    #region constructors and destructor's

    
    /// <summary>
    /// Constructor
    /// </summary>
    public PacsSelectionControl(IDataProvider dataProvider)
    {

      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      m_dataProvider = dataProvider;

      // Prevent drawing flicker by blitting from memory in WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.UserPaint, true);
      SetStyle(ControlStyles.SupportsTransparentBackColor, true);

      this.Font       = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.ForeColor  = System.Drawing.Color.Navy;
    } // PacsSelectionControl()

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">disposing flag</param>
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
    } // Dispose( bool disposing )

    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_comboBoxPacsServer = new System.Windows.Forms.ComboBox();
      this.SuspendLayout();
      // 
      // m_comboBoxPacsServer
      // 
      this.m_comboBoxPacsServer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_comboBoxPacsServer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxPacsServer.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_comboBoxPacsServer.ForeColor = System.Drawing.Color.Navy;
      this.m_comboBoxPacsServer.Location = new System.Drawing.Point(0, 0);
      this.m_comboBoxPacsServer.Name = "m_comboBoxPacsServer";
      this.m_comboBoxPacsServer.Size = new System.Drawing.Size(352, 23);
      this.m_comboBoxPacsServer.TabIndex = 1;
      this.m_comboBoxPacsServer.SelectedIndexChanged += new System.EventHandler(this.OnSelectedIndexChangedHandler);
      // 
      // PacsSelectionControl
      // 
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.Controls.Add(this.m_comboBoxPacsServer);
      this.Name = "PacsSelectionControl";
      this.Size = new System.Drawing.Size(352, 24);
      this.ResumeLayout(false);

    }
    #endregion

    #region properties 


    /// <summary>
    /// Sets /Gets the fore color
    /// </summary>
    public override Color ForeColor
    {
      get   {  return base.ForeColor; }
      set   
      {  
        base.ForeColor = value; 
        //m_comboBoxPacsServer.ForeColor = value;
        this.Refresh();
      }

    } // ForeColor

    /// <summary>
    /// Sets / Gets the controls font
    /// </summary>
    public override Font Font
    {
      get   {  return base.Font; }
      set   
      {  
        base.Font = value; 
        m_comboBoxPacsServer.Font = value;
        this.Refresh();
      } 

    } // Font


    /// <summary>
    /// Gets the selected server (Host)
    /// </summary>
    public DicomHost SelectedServer
    {
      get { return m_selectedServer; }
    } // SelectedServer

    /// <summary>
    /// Sets / Gets the service type
    /// </summary>
    public DicomServiceTypes ServiceType
    {
      get { return m_serviceType; }
      set 
      {
        m_serviceType = value;

        LoadComboBox(value);
      }
    } // ServiceType


    
    /// <summary>
    /// Gets the hosts associated with the specified service type
    /// </summary>
    public Host [] Hosts
    {
      get 
      { 
        Host [] list = new Host [m_comboBoxPacsServer.Items.Count];
        m_comboBoxPacsServer.Items.CopyTo(list, 0);

        return list;
      }
    } // Hosts

    /// <summary>
    /// Gets the host count
    /// </summary>
    public int Count 
    {
      get { return m_comboBoxPacsServer.Items.Count; }
    } // Count

    #endregion

    #region methods

    /// <summary>
    /// Loads the combo box for the pac servers
    /// </summary>
    /// <param name="type">servce type</param>
    private void LoadComboBox(DicomServiceTypes type)
    {

      if ( ! this.DesignMode && ! m_loaded )
      {
        m_comboBoxPacsServer.Items.Clear();

        try
        {
          foreach (DicomHost host in m_dataProvider.GetDicomHosts(XmlBuilder.Create("DicomHost", new Dictionary<string,string>() {{ "ServiceType", type.ToString() }})))
          {
            m_comboBoxPacsServer.Items.Add(host);
          }
        }
        catch (Exception e)
        {
          Logging.Log.Error("Failed to get host table: " + e.Message, "PacsSelectionControl", "LoadComboBox");
        }

        if ( m_comboBoxPacsServer.Items.Count > 0 )
        {
          m_comboBoxPacsServer.SelectedIndex = 0;
        }

        m_loaded = true;

      }

    } // LoadComboBox(string type)

    /// <summary>
    /// Inbvoke the server se;lectyed event
    /// </summary>
    protected virtual void OnServerSelected()
    {
      if (!m_loaded) return;

      try
      {
        if (this.ServerSelected != null)
          this.ServerSelected(this, EventArgs.Empty);
      }
      finally
      {
        Refresh();
      }
    } // OnServerSelected

    #endregion

    #region overrides

    /// <summary>
    /// Load event handler
    /// </summary>
    /// <param name="e">event args</param>
    protected override void OnLoad(EventArgs e)
    {
      base.OnLoad(e);


      if (!this.DesignMode)
      {
        LoadComboBox(m_serviceType);

      }


    } // OnLoad(EventArgs e)
    

    /// <summary>
    /// Handles the Resize event
    /// </summary>
    /// <param name="e">event args</param>
    protected override void OnResize(EventArgs e)
    {
      int height = m_comboBoxPacsServer.Height;

      int width = Width - 88;
      m_comboBoxPacsServer.Location = new Point(Width - width, 0);
      m_comboBoxPacsServer.Size = new Size(width, height);

      this.Size = new Size(Width, m_comboBoxPacsServer.Height);


    } // OnResize(EventArgs e)

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the selected index changed event
    /// </summary>
    /// <param name="sender">sender</param>
    /// <param name="e">evenet args</param>
    private void OnSelectedIndexChangedHandler(object sender, System.EventArgs e)
    {
      m_selectedServer = (DicomHost)m_comboBoxPacsServer.SelectedItem;
      OnServerSelected();
    } // OnSelectedIndexChangedHandler(object sender, System.EventArgs e)


    #endregion

    #region events

    /// <summary>
    /// Server selected evenet
    /// </summary>
    public event EventHandler ServerSelected = null;

    #endregion


  } // class PacsSelectionControl
 
} // namespace Viatronix.v3D.Dicom.UI


#region revision history

// $Log: PacsSelectionControl.cs,v $
// Revision 1.8  2007/03/08 21:56:13  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/PacsSelectionControl.cs,v 1.8 2007/03/08 21:56:13 mkontak Exp $
// $Id: PacsSelectionControl.cs,v 1.8 2007/03/08 21:56:13 mkontak Exp $

#endregion
