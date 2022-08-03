// $Id: ProcessAsDialog.cs,v 1.11.2.5 2011/05/18 01:56:59 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


using System;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Windows.Forms;
using System.Xml;
using Viatronix.v3D.Core;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Console
{
  /// <summary>
  /// Provides the UI for selecting Command line parameters.
  /// </summary>
  public class ProcessAsDialog : Viatronix.UI.Dialog  //System.Windows.Forms.Form //
  {

    #region fields

    
 
    /// <summary>
    /// Current preprocess
    /// </summary>
    private Template m_template = null;

     
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// The set default button.
    /// </summary>
    private Viatronix.UI.Button m_setDefaultButton;

    /// <summary>
    /// OK Button
    /// </summary>
    private Viatronix.UI.Button m_run;

    /// <summary>
    /// Cancel Button
    /// </summary>
    private Viatronix.UI.Button m_cancel;

    /// <summary>
    /// Label Object 
    /// </summary>
    private System.Windows.Forms.Label Lock;


   /// <summary>
    /// CommandLine Panel.
    /// </summary>
    private System.Windows.Forms.Panel m_commandlinePanel;

    /// <summary>
    /// Empty Params Label.
    /// </summary>
    private System.Windows.Forms.Label m_emptyParamsLabel;

    /// <summary>
    /// Process with label
    /// </summary>
    private System.Windows.Forms.Label m_processWithLabel;
 
 
    /// <summary>
    /// Process as combo
    /// </summary>
    private System.Windows.Forms.ComboBox m_comboBoxProcessAs;

  
    /// <summary>
    /// Process As Subtype
    /// </summary>
    private string m_processAs = "Default";

    /// <summary>
    /// Template tasks
    /// </summary>
    private System.Windows.Forms.ListView m_listViewTasks;

    /// <summary>
    /// Task parameters
    /// </summary>
    private System.Windows.Forms.ListView m_listViewParameters;

    /// <summary>
    /// Group Box
    /// </summary>
    private System.Windows.Forms.GroupBox m_groupBox;

    /// <summary>
    /// File Name
    /// </summary>
    private string m_fileName = string.Empty;

    /// <summary>
    /// Combo box Preprocess
    /// </summary>
    private Viatronix.UI.ComboBox m_comboBoxPreprocess;

    
    #endregion fields

    #region construction

  

    /// <summary>
    /// COnstructor
    /// </summary>
    /// <param name="title">Title of the dialog</param>
    /// <param name="subtype"></param>
    public ProcessAsDialog( IProcessEntity entity, string title = "" ) 
    {
      Initialize();

      if (!string.IsNullOrEmpty(title))
        Text = title;

      m_comboBoxProcessAs.Enabled = entity.CanProcess;
    }//ReprocessDialog(string fileName, string patientName,string subtype)


     
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
    }//Dispose( bool disposing )


    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.Lock = new System.Windows.Forms.Label();
      this.m_comboBoxProcessAs = new System.Windows.Forms.ComboBox();
      this.m_setDefaultButton = new Viatronix.UI.Button();
      this.m_cancel = new Viatronix.UI.Button();
      this.m_run = new Viatronix.UI.Button();
      this.m_commandlinePanel = new System.Windows.Forms.Panel();
      this.m_emptyParamsLabel = new System.Windows.Forms.Label();
      this.m_processWithLabel = new System.Windows.Forms.Label();
      this.m_listViewTasks = new System.Windows.Forms.ListView();
      this.m_listViewParameters = new System.Windows.Forms.ListView();
      this.m_groupBox = new System.Windows.Forms.GroupBox();
      this.m_comboBoxPreprocess = new Viatronix.UI.ComboBox();
      this.m_commandlinePanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // Lock
      // 
      this.Lock.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.Lock.Location = new System.Drawing.Point(9, 16);
      this.Lock.Name = "Lock";
      this.Lock.Size = new System.Drawing.Size(62, 25);
      this.Lock.TabIndex = 4;
      this.Lock.Text = "Process As";
      // 
      // m_comboBoxProcessAs
      // 
      this.m_comboBoxProcessAs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.m_comboBoxProcessAs.Location = new System.Drawing.Point(83, 12);
      this.m_comboBoxProcessAs.Name = "m_comboBoxProcessAs";
      this.m_comboBoxProcessAs.Size = new System.Drawing.Size(217, 22);
      this.m_comboBoxProcessAs.TabIndex = 6;
      this.m_comboBoxProcessAs.SelectedIndexChanged += new System.EventHandler(this.OnSelectedProcessAsChanged);
       // 
      // m_cancel
      // 
      this.m_cancel.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_cancel.ForeColor = System.Drawing.Color.Black;
      this.m_cancel.Location = new System.Drawing.Point(378, 279);
      this.m_cancel.Name = "m_cancel";
      this.m_cancel.OldStockButton = false;
      this.m_cancel.Size = new System.Drawing.Size(60, 25);
      this.m_cancel.TabIndex = 2;
      this.m_cancel.Text = "Cancel";
      this.m_cancel.Click += new System.EventHandler(this.OnCancel);
      // 
      // m_run
      // 
      this.m_run.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_run.ForeColor = System.Drawing.Color.Black;
      this.m_run.Location = new System.Drawing.Point(313, 279);
      this.m_run.Name = "m_run";
      this.m_run.OldStockButton = false;
      this.m_run.Size = new System.Drawing.Size(60, 25);
      this.m_run.TabIndex = 1;
      this.m_run.Text = "Run";
      this.m_run.Click += new System.EventHandler(this.OnRun);
       // 
      // m_emptyParamsLabel
      // 
      this.m_emptyParamsLabel.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.m_emptyParamsLabel.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_emptyParamsLabel.Location = new System.Drawing.Point(37, 52);
      this.m_emptyParamsLabel.Name = "m_emptyParamsLabel";
      this.m_emptyParamsLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
      this.m_emptyParamsLabel.Size = new System.Drawing.Size(191, 54);
      this.m_emptyParamsLabel.TabIndex = 0;
      this.m_emptyParamsLabel.Text = "A";
      this.m_emptyParamsLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // m_processWithLabel
      // 
      this.m_processWithLabel.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
      this.m_processWithLabel.Location = new System.Drawing.Point(9, 50);
      this.m_processWithLabel.Name = "m_processWithLabel";
      this.m_processWithLabel.Size = new System.Drawing.Size(70, 23);
      this.m_processWithLabel.TabIndex = 9;
      this.m_processWithLabel.Text = "Process with";
      // 
      // m_listViewTasks
      // 
      this.m_listViewTasks.FullRowSelect = true;
      this.m_listViewTasks.View = View.List;
      this.m_listViewTasks.BackColor = System.Drawing.Color.White;
      this.m_listViewTasks.Font = new System.Drawing.Font("Arial", 8F);
      this.m_listViewTasks.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_listViewTasks.Location = new System.Drawing.Point(18, 102);
      this.m_listViewTasks.Name = "m_listViewTasks";
      this.m_listViewTasks.Size = new System.Drawing.Size(146, 160);
      this.m_listViewTasks.TabIndex = 12;
      this.m_listViewTasks.ItemSelectionChanged += new ListViewItemSelectionChangedEventHandler(OnSelectedTaskChanged);
      // 
      // m_listViewParameters
      // 
      this.m_listViewParameters.HeaderStyle = ColumnHeaderStyle.Nonclickable;
      this.m_listViewParameters.View = View.Details;
      this.m_listViewParameters.GridLines = true;
      this.m_listViewParameters.BackColor = System.Drawing.Color.White;
      this.m_listViewParameters.Font = new System.Drawing.Font("Arial", 8F);
      this.m_listViewParameters.Location = new System.Drawing.Point(169, 102);
      this.m_listViewParameters.Name = "m_listViewParameters";
      this.m_listViewParameters.Size = new System.Drawing.Size(262, 160);
      this.m_listViewParameters.TabIndex = 13;

      // 
      // m_groupBox
      // 
      this.m_groupBox.ForeColor = System.Drawing.SystemColors.ControlLightLight;
      this.m_groupBox.Location = new System.Drawing.Point(10, 87);
      this.m_groupBox.Name = "m_groupBox";
      this.m_groupBox.Size = new System.Drawing.Size(427, 185);
      this.m_groupBox.TabIndex = 13;
      this.m_groupBox.TabStop = false;
      this.m_groupBox.Text = "Process steps and parameters";
      // 
      // m_comboBoxPreprocess
      // 
      this.m_comboBoxPreprocess.DisplayMember = "String";
      this.m_comboBoxPreprocess.Location = new System.Drawing.Point(84, 43);
      this.m_comboBoxPreprocess.Name = "m_comboBoxPreprocess";
      this.m_comboBoxPreprocess.Size = new System.Drawing.Size(350, 22);
      this.m_comboBoxPreprocess.TabIndex = 14;
      this.m_comboBoxPreprocess.ValueMember = "Object";
      this.m_comboBoxPreprocess.SelectedIndexChanged += new System.EventHandler(this.OnSelectedTemplateChanged);
     // 
      // m_setDefaultButton
      // 
      this.m_setDefaultButton.CustomImages = new System.Drawing.Bitmap[] {
        null,
        null,
        null};
      this.m_setDefaultButton.ForeColor = System.Drawing.Color.Black;
      this.m_setDefaultButton.Location = new System.Drawing.Point(10, 279);
      this.m_setDefaultButton.Name = "m_setDefaultButton";
      this.m_setDefaultButton.OldStockButton = false;
      this.m_setDefaultButton.Size = new System.Drawing.Size(100, 25);
      this.m_setDefaultButton.TabIndex = 1;
      this.m_setDefaultButton.Text = "Set As Default";
      this.m_setDefaultButton.Click += new System.EventHandler(OnSetDefaultClickHandler);
      // 
      // ProcessAsDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(443, 310);
      this.Controls.Add(this.m_processWithLabel);
      this.Controls.Add(this.m_comboBoxProcessAs);
      this.Controls.Add(this.Lock);
      this.Controls.Add(this.m_setDefaultButton);
      this.Controls.Add(this.m_cancel);
      this.Controls.Add(this.m_run);
      this.Controls.Add(this.m_listViewParameters);
      this.Controls.Add(this.m_listViewTasks);
      this.Controls.Add(this.m_groupBox);
      this.Controls.Add(this.m_comboBoxPreprocess);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "ProcessAsDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "ProcessAsDialog";
      this.Controls.SetChildIndex(this.m_comboBoxPreprocess, 0);
      this.Controls.SetChildIndex(this.m_groupBox, 0);
      this.Controls.SetChildIndex(this.m_listViewTasks, 0);
      this.Controls.SetChildIndex(this.m_listViewParameters, 0);
      this.Controls.SetChildIndex(this.m_setDefaultButton, 0);
      this.Controls.SetChildIndex(this.m_run, 0);
      this.Controls.SetChildIndex(this.m_cancel, 0);
      this.Controls.SetChildIndex(this.Lock, 0);
      this.Controls.SetChildIndex(this.m_comboBoxProcessAs, 0);
      this.Controls.SetChildIndex(this.m_processWithLabel, 0);
      this.m_commandlinePanel.ResumeLayout(false);
      this.ResumeLayout(false);

    }
    #endregion

    #endregion

    #region properties

    /// <summary>
    /// Gets the process as subtype
    /// </summary>
    public string ProcessAs
    {
      get { return (m_processAs == "Default" ? "" : m_processAs); }
    } // ProcessAs


    /// <summary>
    /// Gets the selected template 
    /// </summary>
    public Template Template
    {
      get { return m_template; }
    } // Preprocess

    #endregion

    #region event handlers

    /// <summary>
    /// Handles a task being selected in the list box.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnSelectedTaskChanged(object sender, ListViewItemSelectionChangedEventArgs args)
    {
      if (args.IsSelected)
      {
        RefreshTaskParameters((XmlNode)args.Item.Tag);
      }
    } // OnSelectedTaskChanged( sender, args )

    
    /// <summary>
    /// OnSelectedProcessAsChanged, Load the corresponding Params on Product Selection
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnSelectedProcessAsChanged( object sender, System.EventArgs args )
    {
      m_listViewParameters.Items.Clear();
      m_listViewTasks.Items.Clear();

      /// Process As application
      m_processAs = ((Viatronix.Enterprise.Entities.Application)m_comboBoxProcessAs.SelectedItem).Id;
 
      /// Initialize the preprocessing list (combo box)
      InitializeTemplates();

    }//OnSelectedProcessAsChanged(object sender, System.EventArgs args)


    /// <summary>
    /// OnSelectedTemplateChanged handler
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnSelectedTemplateChanged( object sender, System.EventArgs args )
    {
      /// Currently selected preprocess
      m_template = (Template)((Viatronix.UI.ComboBox.ObjectString)(m_comboBoxPreprocess.SelectedItem)).Object;

       /// Inializes the preprocessing steps list
      InitializeTasks(m_template);

    }//OnSelectedTemplateChanged( object sender, System.EventArgs args )


    /// <summary>
    /// Get the Commandline string on OK button Click.
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnRun( object sender, System.EventArgs args )
    {
 

      DialogResult = DialogResult.OK;

      this.Close();

    }//OnRun(object sender, System.EventArgs args)

   
    /// <summary>
    /// OnCancel, Cancels the commandline Provider 
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnCancel( object sender, System.EventArgs args )
    {
      DialogResult = DialogResult.Cancel;
      this.Close();
      this.Dispose();
    }//OnCancel(object sender, System.EventArgs args)

       

     /// <summary>
    /// Handles the click for the Set Default button.  Sets the default processing mode for the selected application.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnSetDefaultClickHandler(object sender, EventArgs args)
    {
      Viatronix.UI.ComboBox.ObjectString item = (Viatronix.UI.ComboBox.ObjectString)m_comboBoxPreprocess.SelectedItem;
      Template template = (Template)item.Object;


      Global.DataProvider.SetAsDefault( template);

      InitializeTemplates();

    } // OnSetDefaultClickHandler( sender, args )

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization.
    /// </summary>
    private void Initialize()
    {
      InitializeComponent();

      m_listViewParameters.Columns.Add("Parameter").Width = 145;
      m_listViewParameters.Columns.Add("Value").Width = 110;

      if (!Global.CurrentUser.IsAllowed("ConfigureProcessing"))
      {
        m_setDefaultButton.Visible = false;
      }

      List<Viatronix.Enterprise.Entities.Application> apps = Global.DataProvider.GetApplications(true);
      foreach (Viatronix.Enterprise.Entities.Application app in apps)
      {
        m_comboBoxProcessAs.Items.Add(app);
      }

      InitializeTemplates();

      if (m_comboBoxProcessAs.Items.Count > 0)
        m_comboBoxProcessAs.SelectedIndex = 0;

    } // Initialize()


    /// <summary>
    /// Initializes the preprocess list combo box with the list from the database
    /// </summary>
    private void InitializeTemplates()
    {
      try
      {

        // clear the original data
        m_comboBoxPreprocess.Items.Clear();

 
        List<Template> templates = new List<Template>();

        if (m_comboBoxProcessAs.Text == "Default")
          templates.Add(Global.DataProvider.GetDefaultTemplate());
        else
          templates = Global.DataProvider.GetTemplates( m_processAs, "*");


        //////////////////////////////////////////////////////////
        // Add preprocesses to the list for user selection
        ////////////////////////////////////////////////////////
        foreach (Template template in templates)
        {
          m_comboBoxPreprocess.Items.Add( new Viatronix.UI.ComboBox.ObjectString(template, template.Description) );
        } // END ... For each preprocess


        if (templates.Count == 1) 
          m_comboBoxPreprocess.DropDownStyle = ComboBoxStyle.Simple;
        else
          m_comboBoxPreprocess.DropDownStyle = ComboBoxStyle.DropDownList;

        if (m_comboBoxPreprocess.Items.Count > 0)
          m_comboBoxPreprocess.SelectedIndex = 0;
       
      }
      catch ( Exception ex )
      {
        Viatronix.Logging.Log.Error("Failed to the get processing template information" + ex.Message, "ProcessAsDialog", "Initialize");
        throw ex;
      }

    } // InitializeTemplates()


    /// <summary>
    /// Initializes the preprocess list combo box with the list from the database
    /// </summary>
    private void InitializeTasks(Template template)
    {
      int selectedTask = m_listViewTasks.SelectedIndices.Count > 0 ? m_listViewTasks.SelectedIndices[0] : -1;
      m_listViewTasks.Items.Clear();

      try
      {
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(template.Tasks);

        foreach (XmlNode node in doc.DocumentElement.SelectNodes("task"))
        {
          ListViewItem item = new ListViewItem(node.Attributes["name"].Value);
          item.Tag = node;
          m_listViewTasks.Items.Add(item);
        }

        selectedTask = m_listViewTasks.Items.Count > 0 ? m_listViewTasks.Items.Count > selectedTask ? selectedTask : 0 : -1;
        if (selectedTask > -1)
        {
          m_listViewTasks.Items[selectedTask].Selected = true;
          RefreshTaskParameters((XmlNode)m_listViewTasks.Items[selectedTask].Tag);
        }
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to the get processing template information" + ex.Message, "ProcessAsDialog", "Initialize");
        throw ex;
      }

    } // InitializePreprocessingList()


    /// <summary>
    /// Refreshes the task parameters list.
    /// </summary>
    /// <param name="taskNode">The task node.</param>
    private void RefreshTaskParameters(XmlNode taskNode)
    {
      m_listViewParameters.Items.Clear();

      foreach (XmlNode pnode in taskNode.SelectNodes("params/param"))
      {
        ListViewItem item = new ListViewItem(pnode.Attributes["name"].Value);
        item.SubItems.Add(pnode.Attributes["value"].Value);

        m_listViewParameters.Items.Add(item);
      }
    } // RefreshTaskParameters( taskNode )

    #endregion methods

  } // class ProcessAsDialog
} // namespace Viatronix.Console

#region revision history

// $Log: ProcessAsDialog.cs,v $
// Revision 1.11.2.5  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.11.2.4  2009/09/02 19:39:52  kchalupa
// Split the processing message box to two lines.
//
// Revision 1.11.2.3  2009/09/02 15:00:08  kchalupa
// Notify the user that their changes will be lost if they enter the combo boxes and something was modified.
//
// Revision 1.11.2.2  2008/01/02 20:33:02  mkontak
// Password hints
//
// Revision 1.11.2.1  2007/03/21 00:31:07  mkontak
// Fix the issue with going back and the data entered is missing
//
// Revision 1.11  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.10  2006/10/11 19:20:46  mkontak
// Issue 5054
//
// Revision 1.9  2006/08/21 14:36:28  mkontak
// Fix problem with checkin version
//
// Revision 1.8  2006/08/21 12:36:48  mkontak
// Issue 4948: Processing volume.dicom series.
//
// Revision 1.7  2006/08/17 18:21:56  mkontak
// Issue 4948:
//
// Revision 1.6  2006/07/13 14:31:39  mkontak
// Fix for the checking of the license ids in multiple places
//
// Revision 1.5  2006/05/09 17:14:31  mkontak
// Fix for wron selection
//
// Revision 1.4  2006/05/04 02:27:44  mkontak
// Modified to work better
//
// Revision 1.3  2006/04/27 19:46:22  mkontak
// Make field non case sensitive
//
// Revision 1.2  2006/03/24 18:34:09  mkontak
// Modified for manual processing
//
// Revision 1.1  2006/03/20 15:12:29  mkontak
// New process as for the study browser
//
// Revision 1.19  2006/02/27 14:54:26  romy
// modified for License Server changes
//
// Revision 1.18  2006/02/22 19:09:08  romy
// fixed License Error
//
// Revision 1.17  2006/02/21 14:57:59  gdavidson
// New version of licensing
//
// Revision 1.16  2006/02/17 15:35:35  romy
// added null check for All Types
//
// Revision 1.15  2006/02/02 16:26:46  romy
// GUI fixes
//
// Revision 1.14  2006/02/01 20:02:50  mkontak
// Fix issue with manual start not passing the correct subtype override
//
// Revision 1.13  2006/01/25 16:17:09  romy
// bug fixes
//
// Revision 1.12  2006/01/24 20:44:53  romy
// new GUI
//
// Revision 1.10  2006/01/20 19:03:41  romy
// latest
//
// Revision 1.9  2006/01/20 05:16:10  romy
// Parameter collection from all stages
//
// Revision 1.8  2006/01/19 22:23:18  romy
// new additions
//
// Revision 1.7  2006/01/10 14:10:57  romy
// Process Params modified for CAD - intermediate
//
// Revision 1.6  2006/01/09 18:28:05  romy
// intermediate code for Process with Params
//
// Revision 1.5  2005/12/30 17:17:44  mkontak
// New DB changes
//
// Revision 1.4  2005/12/02 02:24:27  romy
// coding standards
//
// Revision 1.3  2005/11/09 16:58:19  romy
// Temp version with COmmandline params
//
// Revision 1.2  2005/08/17 17:54:53  mkontak
// Fixes in the way the processing parameters are handled
//
// Revision 1.1  2005/08/05 14:20:01  romy
// Rearranged Files
//
// Revision 1.9  2005/06/15 19:54:46  romy
// Added Message Handlers
//
// Revision 1.8  2005/06/15 19:51:58  romy
// Added Message Handlers
//
// Revision 1.7  2005/06/15 19:44:12  romy
// Removed Orientation.
//
// Revision 1.6  2005/06/08 17:34:05  romy
// Removed the Test Code.
//
// Revision 1.5  2005/06/07 18:05:55  romy
// Added Toggle UI Method
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/ProcessAsDialog.cs,v 1.11.2.5 2011/05/18 01:56:59 mkontak Exp $
// $Id: ProcessAsDialog.cs,v 1.11.2.5 2011/05/18 01:56:59 mkontak Exp $


#endregion