using System;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.ServiceProcess;

namespace Viatronix.Utilities.Database
{

	/// <summary>
	/// Create Databases GUI front-end
	/// </summary>
	public class CreateDatabases : System.Windows.Forms.Form
  {

    #region enums


    public enum NodeType 
    {
      System,
      Database,
      Command,
      Message
    };

    #endregion

    #region delegates

    
    /// <summary>
    /// Processing delegate
    /// </summary>
    private delegate void ProcessDelegate();


    /// <summary>
    /// Databse tree node
    /// </summary>
    private TreeNode m_dbNode = null;

    /// <summary>
    /// Command tree node
    /// </summary>
    private TreeNode m_cmdNode = null;

    /// <summary>
    /// System node
    /// </summary>
    private TreeNode m_sysNode = null;
    

    /// <summary>
    /// License server name SERVER\INSTANCE
    /// </summary>
    private string m_licenseServer = string.Empty;

    /// <summary>
    /// License server password
    /// </summary>
    private string  m_licensePassword = string.Empty;

    /// <summary>
    /// Update status delegate 
    /// </summary>
    /// <param name="status"></param>
    private delegate void UpdateStatusDelegate(NodeType type, string status);

    /// <summary>
    /// Enable form delegate
    /// </summary>
    /// <param name="enable"></param>
    /// <param name="cursor"></param>
    private delegate void EnableFormDelegate(bool enable, Cursor cursor);

    #endregion

    #region fields


    /// <summary>
    /// Databases
    /// </summary>
    private List<V3DSystem> m_systems = (List<V3DSystem>)System.Configuration.ConfigurationManager.GetSection("systems");

    /// <summary>
    /// Closing flag to prevent the reading of the processes stdout prior to the killing of it
    /// </summary>
    private bool m_closing = false;


    /// <summary>
    /// Registration TTL
    /// </summary>
    private int m_ttl = 10;


    /// <summary>
    /// Registration ttl units
    /// </summary>
    private string m_units = "day";


    /// <summary>
    /// Update status delegate
    /// </summary>
    private UpdateStatusDelegate m_updateStatus = null;

    /// <summary>
    /// Enable form delegate
    /// </summary>
    private EnableFormDelegate m_enableForm = null;
 
    /// <summary>
    ///  Gets the setting from the config file
    /// </summary>
    private Hashtable m_settings = (Hashtable)System.Configuration.ConfigurationManager.GetSection("settings");

    /// <summary>
    /// Server name
    /// </summary>
    private string m_server = System.Windows.Forms.SystemInformation.ComputerName;

    /// <summary>
    /// Database Instance
    /// </summary>
    private string m_instance = "V3DENT";


    /// <summary>
    /// Log file
    /// </summary>
    private string m_logFilePath = "C:\\ProgramData\\Viatronix\\Logs\\CreateDatabase.log";


    /// <summary>
    /// V3D Core Systems sa password
    /// </summary>
    private string m_enterprisePassword = string.Empty;


    /// <summary>
    /// V3D Core Systems server (SERVER\INSTANCE)
    /// </summary>
    private string m_enterpriseServer = string.Empty;

    /// <summary>
    /// System id
    /// </summary>
    private string m_systemId = string.Empty;

    /// <summary>
    /// ASQL script location (May be relative)
    /// </summary>
    private string m_sql = string.Empty;


    /// <summary>
    /// Processing delegate
    /// </summary>
    private ProcessDelegate m_processDelegate = null;

    /// <summary>
    /// Callback to help determine when the process is complete
    /// </summary>
    private AsyncCallback m_processCallback = null;


    #region Form Fields (System Generated)

    private System.Windows.Forms.Label m_labelServer;
    private System.Windows.Forms.Label m_labelInstance;
    private System.Windows.Forms.TextBox m_textBoxServer;
    private System.Windows.Forms.TextBox m_textBoxInstance;
    private System.Windows.Forms.FolderBrowserDialog m_folderBrowserDialog;
    private Button m_buttonOk;
    private Button m_buttonCancel;
    private Label label1;
    private TextBox m_textBoxSQLLocation;
    private Button m_buttonSQLLocation;

    #endregion

    private ToolTip m_toolTip;
    private ImageList m_imageListDatabases;
    private GroupBox m_groupBoxSystems;
    private TreeView m_treeViewSystems;
    private TreeView m_treeViewLog;
    private Label label3;
    private NumericUpDown m_numericUpDownTtl;
    private ComboBox m_comboBoxUnits;
    private IContainer components;

    #endregion


    #region constructors and destructors


    /// <summary>
    /// Constrcutor
    /// </summary>
    public CreateDatabases()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent( );


      m_processDelegate = new ProcessDelegate(StartCreateDatabase);

      //Create Call Backs
      m_processCallback = new AsyncCallback(ProcessingCallback);

 
      m_updateStatus = new UpdateStatusDelegate(this.UpdateStatus);


      m_enableForm      = new EnableFormDelegate(this.EnableForm);

 
      //System.IO.File.Delete( m_logFile );

		} // CreateDatabases()


		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		} // Displose( bool bDisposing )

    #endregion


    #region Windows Form Designer generated code
    /// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CreateDatabases));
      this.m_labelServer = new System.Windows.Forms.Label();
      this.m_labelInstance = new System.Windows.Forms.Label();
      this.m_textBoxServer = new System.Windows.Forms.TextBox();
      this.m_textBoxInstance = new System.Windows.Forms.TextBox();
      this.m_folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
      this.m_buttonOk = new System.Windows.Forms.Button();
      this.m_buttonCancel = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.m_textBoxSQLLocation = new System.Windows.Forms.TextBox();
      this.m_buttonSQLLocation = new System.Windows.Forms.Button();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_numericUpDownTtl = new System.Windows.Forms.NumericUpDown();
      this.m_comboBoxUnits = new System.Windows.Forms.ComboBox();
      this.m_imageListDatabases = new System.Windows.Forms.ImageList(this.components);
      this.m_groupBoxSystems = new System.Windows.Forms.GroupBox();
      this.m_treeViewSystems = new System.Windows.Forms.TreeView();
      this.m_treeViewLog = new System.Windows.Forms.TreeView();
      this.label3 = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownTtl)).BeginInit();
      this.m_groupBoxSystems.SuspendLayout();
      this.SuspendLayout();
      // 
      // m_labelServer
      // 
      this.m_labelServer.Location = new System.Drawing.Point(15, 16);
      this.m_labelServer.Name = "m_labelServer";
      this.m_labelServer.Size = new System.Drawing.Size(100, 23);
      this.m_labelServer.TabIndex = 0;
      this.m_labelServer.Text = "Server:";
      // 
      // m_labelInstance
      // 
      this.m_labelInstance.Location = new System.Drawing.Point(15, 45);
      this.m_labelInstance.Name = "m_labelInstance";
      this.m_labelInstance.Size = new System.Drawing.Size(100, 23);
      this.m_labelInstance.TabIndex = 1;
      this.m_labelInstance.Text = "Instance:";
      // 
      // m_textBoxServer
      // 
      this.m_textBoxServer.Location = new System.Drawing.Point(168, 16);
      this.m_textBoxServer.Name = "m_textBoxServer";
      this.m_textBoxServer.Size = new System.Drawing.Size(119, 21);
      this.m_textBoxServer.TabIndex = 1;
      this.m_toolTip.SetToolTip(this.m_textBoxServer, "Enter the server name for the database");
      // 
      // m_textBoxInstance
      // 
      this.m_textBoxInstance.Location = new System.Drawing.Point(168, 43);
      this.m_textBoxInstance.Name = "m_textBoxInstance";
      this.m_textBoxInstance.Size = new System.Drawing.Size(119, 21);
      this.m_textBoxInstance.TabIndex = 2;
      this.m_toolTip.SetToolTip(this.m_textBoxInstance, "Enter the database instance name");
      // 
      // m_buttonOk
      // 
      this.m_buttonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_buttonOk.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonOk.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonOk.Location = new System.Drawing.Point(707, 12);
      this.m_buttonOk.Name = "m_buttonOk";
      this.m_buttonOk.Size = new System.Drawing.Size(75, 23);
      this.m_buttonOk.TabIndex = 7;
      this.m_buttonOk.Text = "Create";
      this.m_toolTip.SetToolTip(this.m_buttonOk, "Click to create the databases");
      this.m_buttonOk.UseVisualStyleBackColor = false;
      this.m_buttonOk.Click += new System.EventHandler(this.OnOkClick);
      // 
      // m_buttonCancel
      // 
      this.m_buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.m_buttonCancel.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.m_buttonCancel.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonCancel.Location = new System.Drawing.Point(707, 41);
      this.m_buttonCancel.Name = "m_buttonCancel";
      this.m_buttonCancel.Size = new System.Drawing.Size(75, 23);
      this.m_buttonCancel.TabIndex = 8;
      this.m_buttonCancel.Text = "Exit";
      this.m_toolTip.SetToolTip(this.m_buttonCancel, "Click to exit");
      this.m_buttonCancel.UseVisualStyleBackColor = false;
      this.m_buttonCancel.Click += new System.EventHandler(this.OnCancelClick);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(15, 73);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(104, 21);
      this.label1.TabIndex = 19;
      this.label1.Text = "SQL Location:";
      // 
      // m_textBoxSQLLocation
      // 
      this.m_textBoxSQLLocation.Location = new System.Drawing.Point(168, 70);
      this.m_textBoxSQLLocation.Name = "m_textBoxSQLLocation";
      this.m_textBoxSQLLocation.ReadOnly = true;
      this.m_textBoxSQLLocation.Size = new System.Drawing.Size(454, 21);
      this.m_textBoxSQLLocation.TabIndex = 3;
      this.m_toolTip.SetToolTip(this.m_textBoxSQLLocation, "Enter the SQL scripts location");
      // 
      // m_buttonSQLLocation
      // 
      this.m_buttonSQLLocation.BackColor = System.Drawing.SystemColors.ControlLight;
      this.m_buttonSQLLocation.ForeColor = System.Drawing.SystemColors.ControlText;
      this.m_buttonSQLLocation.Location = new System.Drawing.Point(628, 70);
      this.m_buttonSQLLocation.Name = "m_buttonSQLLocation";
      this.m_buttonSQLLocation.Size = new System.Drawing.Size(32, 21);
      this.m_buttonSQLLocation.TabIndex = 21;
      this.m_buttonSQLLocation.TabStop = false;
      this.m_buttonSQLLocation.Text = "...";
      this.m_toolTip.SetToolTip(this.m_buttonSQLLocation, "Cick to browse for the SQL scripts location");
      this.m_buttonSQLLocation.UseVisualStyleBackColor = false;
      this.m_buttonSQLLocation.Click += new System.EventHandler(this.OnBrowseSQLLocation);
      // 
      // m_numericUpDownTtl
      // 
      this.m_numericUpDownTtl.Location = new System.Drawing.Point(168, 99);
      this.m_numericUpDownTtl.Maximum = new decimal(new int[] {
            2000,
            0,
            0,
            0});
      this.m_numericUpDownTtl.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
      this.m_numericUpDownTtl.Name = "m_numericUpDownTtl";
      this.m_numericUpDownTtl.Size = new System.Drawing.Size(49, 21);
      this.m_numericUpDownTtl.TabIndex = 5;
      this.m_toolTip.SetToolTip(this.m_numericUpDownTtl, "Select the time to live when registering the V3D Licesning and V3D System");
      this.m_numericUpDownTtl.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
      // 
      // m_comboBoxUnits
      // 
      this.m_comboBoxUnits.FormattingEnabled = true;
      this.m_comboBoxUnits.Items.AddRange(new object[] {
            "minute",
            "day",
            "month",
            "year"});
      this.m_comboBoxUnits.Location = new System.Drawing.Point(223, 98);
      this.m_comboBoxUnits.Name = "m_comboBoxUnits";
      this.m_comboBoxUnits.Size = new System.Drawing.Size(97, 23);
      this.m_comboBoxUnits.TabIndex = 6;
      this.m_toolTip.SetToolTip(this.m_comboBoxUnits, "Select the unit type for the time to live");
      // 
      // m_imageListDatabases
      // 
      this.m_imageListDatabases.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_imageListDatabases.ImageStream")));
      this.m_imageListDatabases.TransparentColor = System.Drawing.Color.Transparent;
      this.m_imageListDatabases.Images.SetKeyName(0, "datas.png");
      this.m_imageListDatabases.Images.SetKeyName(1, "data.png");
      this.m_imageListDatabases.Images.SetKeyName(2, "tables.png");
      this.m_imageListDatabases.Images.SetKeyName(3, "table_remove.png");
      // 
      // m_groupBoxSystems
      // 
      this.m_groupBoxSystems.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.m_groupBoxSystems.Controls.Add(this.m_treeViewSystems);
      this.m_groupBoxSystems.Location = new System.Drawing.Point(4, 127);
      this.m_groupBoxSystems.Name = "m_groupBoxSystems";
      this.m_groupBoxSystems.Size = new System.Drawing.Size(778, 190);
      this.m_groupBoxSystems.TabIndex = 35;
      this.m_groupBoxSystems.TabStop = false;
      this.m_groupBoxSystems.Text = "Systems";
      // 
      // m_treeViewSystems
      // 
      this.m_treeViewSystems.BackColor = System.Drawing.SystemColors.ScrollBar;
      this.m_treeViewSystems.CheckBoxes = true;
      this.m_treeViewSystems.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_treeViewSystems.HotTracking = true;
      this.m_treeViewSystems.ImageIndex = 0;
      this.m_treeViewSystems.ImageList = this.m_imageListDatabases;
      this.m_treeViewSystems.Location = new System.Drawing.Point(3, 17);
      this.m_treeViewSystems.Name = "m_treeViewSystems";
      this.m_treeViewSystems.SelectedImageIndex = 0;
      this.m_treeViewSystems.Size = new System.Drawing.Size(772, 170);
      this.m_treeViewSystems.TabIndex = 34;
      this.m_treeViewSystems.BeforeCheck += new System.Windows.Forms.TreeViewCancelEventHandler(this.OnBeforeCheck);
      this.m_treeViewSystems.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.OnAfterCheck);
      this.m_treeViewSystems.NodeMouseHover += new System.Windows.Forms.TreeNodeMouseHoverEventHandler(this.OnNodeMouseHover);
      // 
      // m_treeViewLog
      // 
      this.m_treeViewLog.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.m_treeViewLog.ImageIndex = 0;
      this.m_treeViewLog.ImageList = this.m_imageListDatabases;
      this.m_treeViewLog.Location = new System.Drawing.Point(0, 320);
      this.m_treeViewLog.Name = "m_treeViewLog";
      this.m_treeViewLog.SelectedImageIndex = 0;
      this.m_treeViewLog.Size = new System.Drawing.Size(790, 439);
      this.m_treeViewLog.TabIndex = 36;
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(15, 99);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(115, 21);
      this.label3.TabIndex = 38;
      this.label3.Text = "Registration:";
      // 
      // CreateDatabases
      // 
      this.AcceptButton = this.m_buttonOk;
      this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
      this.BackColor = System.Drawing.Color.SlateGray;
      this.CancelButton = this.m_buttonCancel;
      this.ClientSize = new System.Drawing.Size(790, 759);
      this.Controls.Add(this.m_comboBoxUnits);
      this.Controls.Add(this.m_numericUpDownTtl);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.m_treeViewLog);
      this.Controls.Add(this.m_groupBoxSystems);
      this.Controls.Add(this.m_buttonSQLLocation);
      this.Controls.Add(this.m_textBoxSQLLocation);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.m_buttonCancel);
      this.Controls.Add(this.m_buttonOk);
      this.Controls.Add(this.m_textBoxInstance);
      this.Controls.Add(this.m_textBoxServer);
      this.Controls.Add(this.m_labelInstance);
      this.Controls.Add(this.m_labelServer);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ForeColor = System.Drawing.SystemColors.ControlLight;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "CreateDatabases";
      this.Text = "Create Databases";
      this.Load += new System.EventHandler(this.OnLoad);
      ((System.ComponentModel.ISupportInitialize)(this.m_numericUpDownTtl)).EndInit();
      this.m_groupBoxSystems.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion


    #region Main

    /// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new CreateDatabases());
		}


    #endregion

    #region methods


    /// <summary>
    /// Enables/Disables all controls on the form except the status box.
    /// </summary>
    /// <param name="flag"></param>
    private void EnableForm(bool flag, Cursor cursor)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_enableForm, new object[] { flag, cursor });
      }
      else
      {
        this.Cursor = cursor;
        this.m_buttonCancel.Enabled = flag;
        this.m_buttonOk.Enabled = flag;
        this.m_textBoxInstance.Enabled = flag;
        this.m_textBoxServer.Enabled = flag;
        this.m_textBoxSQLLocation.Enabled = flag;
        this.m_numericUpDownTtl.Enabled = flag;
        this.m_comboBoxUnits.Enabled = flag;
      }
    } // EnableForm(bool flag)

    /// <summary>
    /// Logs Delimiter
    /// </summary>
    private void LogDelimiter()
    {
      LogMessage(String.Format("==================================================================================================================={0}", System.Environment.NewLine));
    }

    /// <summary>
    /// Updates the status box
    /// </summary>
    /// <param name="status"></param>
    private void LogMessage(string status)
    {

      if (m_logFilePath.Length > 0)
      {

        if (!System.IO.Directory.Exists(System.IO.Path.GetDirectoryName(m_logFilePath)))
          System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(m_logFilePath));

        StreamWriter writer = new StreamWriter(m_logFilePath, true, System.Text.ASCIIEncoding.ASCII);

        writer.WriteLine(status);

        writer.Close();
      }


    } // LogMessage(string status)

 
    /// <summary>
    /// Updates the tree view log 
    /// </summary>
    /// <param name="status"></param>
    private void UpdateStatus(NodeType type, string status)
    {
      //=================================================================================================
      // Only update if we are not closing otherwise we error because the resource has been disposed
      // and the process thread has not terminated yet.
      //================================================================================================
      if (!m_closing)
      {

        if (this.InvokeRequired)
          this.Invoke(m_updateStatus, new object[] { type, status });
        else
        {

          switch (type)
          {
            case NodeType.System:
             {
                TreeNode sys = new TreeNode(status);
                sys.ImageIndex = 0;
                sys.SelectedImageIndex = 0;
                sys.ForeColor = Color.DarkBlue;
                sys.Tag = "System";
                m_treeViewLog.Nodes.Add(sys);
                m_sysNode = sys;
              }
              break;

            case NodeType.Database:
              {
                TreeNode db = new TreeNode(status);
                db.ImageIndex = 1;
                db.SelectedImageIndex = 1;
                db.ForeColor = Color.DarkBlue;
                db.Tag = "Database";
                m_sysNode.Nodes.Add(db);
                m_dbNode = db;
              }
              break;

            case NodeType.Command:
              {
                TreeNode cmd = new TreeNode(status);
                cmd.ImageIndex = 2;
                cmd.SelectedImageIndex = 2;
                cmd.ForeColor = Color.DarkOliveGreen;
                cmd.Tag = "Command";
                m_dbNode.Nodes.Add(cmd);
                m_cmdNode = cmd;
              }
              break;

            case NodeType.Message:
              {
                TreeNode msg = new TreeNode(status);
                msg.ImageIndex = 4;
                msg.SelectedImageIndex = 4;
                msg.Tag = "Message";
                m_cmdNode.Nodes.Add(msg);
              }
              break;
          };
        
        }
      }
    } // LogMessage(string status)



    /// <summary>
    /// Processes the SQL using the osql command
    /// </summary>
    /// <param name="status"></param>
    /// <param name="arguements"></param>
    private void ProcessCommand(Command command)
    {

      string filePath = System.IO.Path.Combine(m_sql, command.File);
      string ext = System.IO.Path.GetExtension(command.File).ToLower();


      try
      {
        if (System.IO.File.Exists(filePath))
        {


          if (ext == ".sql" && string.Compare(m_instance, "V3DENT", true) != 0)
          {
            string oldtext = System.IO.File.ReadAllText(filePath);
            string newText = oldtext.Replace("V3DENT", m_instance);


            filePath = System.IO.Path.Combine(m_sql, "_" + command.File);
            System.IO.File.WriteAllText(filePath, newText);
          }




          Process p = new Process();
          ProcessStartInfo info;

          if (System.IO.Path.GetExtension(command.File) == ".sql")
          {
            info = new ProcessStartInfo("sqlcmd.exe", string.Format("-S {0} -d {1} -U sa -P {2} -i {3}", m_enterpriseServer, command.Database, m_enterprisePassword, filePath));
          }
          else
          {
            string filename = System.IO.Path.GetFileName(command.File);
            if ( string.Compare(filename, "Register.bat", true) ==  0)
            {
              string[] args = new string[] { command.File, m_enterpriseServer, command.Database, m_enterprisePassword, m_ttl.ToString(), m_units };

              info = new ProcessStartInfo("cmd.exe", string.Format(" /C {0} {1} {2} {3} {4} {5} ", args));

            }
            else
            {
              LogMessage("Not supported" + System.Environment.NewLine);
              UpdateStatus(NodeType.Message, "Not Supported");
              return;
            }

          }

          info.RedirectStandardOutput = true;
          info.RedirectStandardError = true;
          info.UseShellExecute = false;
          info.WindowStyle = ProcessWindowStyle.Hidden;
          info.CreateNoWindow = true;
          info.ErrorDialog = true;
          info.WorkingDirectory = m_sql;
          //info.ErrorDialogParentHandle = this.Handle;

          p.StartInfo = info;
          p.Start();

          //this.Focus();

          string output = p.StandardOutput.ReadToEnd();
          p.WaitForExit();


          string[] sep = { System.Environment.NewLine };

          string[] lines = output.Split(sep, StringSplitOptions.RemoveEmptyEntries);

          foreach (string line in lines)
          {

            UpdateStatus(NodeType.Message, line);
          }

          LogMessage(output);

        }
        else
        {


          LogMessage("File not found" + System.Environment.NewLine);
          UpdateStatus(NodeType.Message, "File not found");
        }
      }
      finally
      {
        if (System.IO.Path.GetFileName(filePath)[0] == '_')
          System.IO.File.Delete(filePath);

      }

      LogDelimiter();

    } // ProcessSql()

    /// <summary>
    /// Loads the systems tree view
    /// </summary>
    private void LoadSystemsTreeView()
    {

      bool allCommandFilesFound = true;

      m_treeViewSystems.Nodes.Clear();

      foreach (V3DSystem system in m_systems)
      {

        TreeNode sys = new TreeNode(system.Name);
        sys.Checked = true;
        sys.ImageIndex = 0;
        sys.SelectedImageIndex = 0;
        sys.Tag = system;

        if (!system.Selectable)
          sys.ForeColor = Color.DarkGreen;


        foreach (Database database in system.Databases)
        {

          TreeNode db = new TreeNode(database.Name);
          db.Checked = true;
          db.ImageIndex = 1;
          db.SelectedImageIndex = 1;
          db.Tag = database;

          if (!database.Selectable)
            db.ForeColor = Color.DarkGreen;


          foreach (Command command in database.Commands)
          {
            TreeNode cmd = new TreeNode(command.Description);
            cmd.Checked = true;
            cmd.ImageIndex = 2;
            cmd.SelectedImageIndex = 2;
            cmd.Tag = command;


            string sql = System.IO.Path.Combine(m_sql, command.File);

            if (!command.Selectable)
            {
              cmd.ForeColor = Color.DarkGreen;

              if (!System.IO.File.Exists(sql))
              {
                cmd.Checked = false;
                cmd.ImageIndex = 3;
              }
          

            }
            else
            {
 
              if (!System.IO.File.Exists(sql))
              {

                if ( ! command.Optional )
                  allCommandFilesFound = false;

                cmd.ForeColor = Color.DarkRed;
                cmd.Checked = false;
                cmd.ImageIndex = 3;
                cmd.SelectedImageIndex = 3;
              }
            }

            db.Nodes.Add(cmd);
          }

          sys.Nodes.Add(db);

        }

        m_treeViewSystems.Nodes.Add(sys);

      }

      // If not all command files were found then
      if (!allCommandFilesFound)
        MessageBox.Show("Not all command file were found, check to make sure you have the correct path", "Missing Files", MessageBoxButtons.OK, MessageBoxIcon.Warning);
    }

    #endregion

    #region event handlers


    /// <summary>
    /// OK click event handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnOkClick(object sender, System.EventArgs e)
    {

      if (m_textBoxSQLLocation.Text.Length == 0)
      {
        MessageBox.Show(this, "You must enter a location for your SQL scripts", "Data Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxSQLLocation.Focus();
        return;

      }


      m_enterpriseServer = string.Format("{0}\\{1}", m_textBoxServer.Text, m_textBoxInstance.Text);
      LoginForm login = new LoginForm(m_enterpriseServer);
     



      if (login.ShowDialog(this) == DialogResult.Cancel)
      {
        m_textBoxServer.Focus();
        return;
      }




      m_instance            = m_textBoxInstance.Text;
      m_server              = m_textBoxServer.Text;
      m_sql                 = m_textBoxSQLLocation.Text;
      m_enterprisePassword        = login.Password;
      m_ttl                 = (int)m_numericUpDownTtl.Value;
      m_units               = m_comboBoxUnits.SelectedItem.ToString();

      m_treeViewLog.Nodes.Clear(); ;

      // ===============================================
      // Delete the olf file so it is instance based
      // ==============================================
      if (System.IO.File.Exists(m_logFilePath))
        System.IO.File.Delete(m_logFilePath);

      EnableForm(false, Cursors.WaitCursor);

      m_processDelegate.BeginInvoke(m_processCallback, null);

    } // OnOkClick()

    /// <summary>
    /// Cancel click event handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnCancelClick(object sender, System.EventArgs e)
    {
      this.Close();
    } // OnCancelClick(object sender, System.EventArgs e)


    private void OnBrowseSQLLocation(object sender, EventArgs e)
    {
      m_folderBrowserDialog.Description = "SQL Scripts Location";
      m_folderBrowserDialog.ShowNewFolderButton = true;
      if (m_folderBrowserDialog.ShowDialog(this) != DialogResult.Cancel)
      {
        m_sql = m_textBoxSQLLocation.Text = m_folderBrowserDialog.SelectedPath;

        LoadSystemsTreeView();
      }

    }



    /// <summary>
    /// Form Load Event Handler
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnLoad(object sender, EventArgs e)
    {
      m_instance      = (m_settings["instance"] != null ? m_settings["instance"].ToString() : string.Empty);
      m_logFilePath   = (m_settings["logFilePath"] != null ? m_settings["logFilePath"].ToString() : m_logFilePath);
      m_sql           = (m_settings["sql"] != null ? m_settings["sql"].ToString() : System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), "sql"));



      if (m_systems.Count == 0)
      {
        MessageBox.Show(this, "No systems defined", "Invalid configuration", MessageBoxButtons.OK, MessageBoxIcon.Error);
        EnableForm(false, Cursors.Default);
      }
      else
      {

        foreach (V3DSystem system in m_systems)
        {
          if ( system.Databases.Count == 0 )
          {
            MessageBox.Show(this, "No databases defined for " + system.Name, "Invalid configuration", MessageBoxButtons.OK, MessageBoxIcon.Error);
            EnableForm(false, Cursors.Default);
          }
 
        }
      }

      m_comboBoxUnits.SelectedIndex = 1;


      LoadSystemsTreeView();
 
      m_textBoxServer.Text = m_server;
      m_textBoxInstance.Text = m_instance;
      m_textBoxSQLLocation.Text = m_sql;


    }


    #endregion event handlers

    #region processing


    /// <summary>
    /// Links the license system
    /// </summary>
    private void LinkLicenseSystems()
    {


      m_licenseServer = (string.IsNullOrEmpty(m_licenseServer) ? m_enterpriseServer : m_licenseServer);
      m_licensePassword = (string.IsNullOrEmpty(m_licensePassword) ? m_enterprisePassword : m_licensePassword);


      string args = string.Format("-es \"{0}\" -ep \"{1}\" -ls \"{2}\" -lp \"{3}", m_enterpriseServer, m_enterprisePassword, m_licenseServer, m_licensePassword);
      
      Process process = new Process();
      ProcessStartInfo info = new ProcessStartInfo("LinkLicenseSystem.exe", args);

      info.RedirectStandardOutput = true;
      info.RedirectStandardError = true;
      info.UseShellExecute = false;
      info.WindowStyle = ProcessWindowStyle.Hidden;
      info.CreateNoWindow = true;
      info.ErrorDialog = true;
      info.WorkingDirectory = m_sql;
      //info.ErrorDialogParentHandle = this.Handle;

      process.StartInfo = info;
      process.Start();

      //this.Focus();

      string output = process.StandardOutput.ReadToEnd();
      process.WaitForExit();

      if (process.ExitCode != 0)
        throw new System.ApplicationException(output);


    }

    /// <summary>
    /// Start the create databases executable
    /// </summary>
    public void StartCreateDatabase()
    {

      try
      {

        LogMessage("Starting database creation");
        LogMessage("");
        LogMessage("Instance: " + m_instance);
        LogMessage("SQL Location: " + m_sql);
        LogMessage("");

        // ================================
        // Loop through all the systems
        // ================================
        foreach (TreeNode systemNode in m_treeViewSystems.Nodes )
        {

          V3DSystem system = (V3DSystem)systemNode.Tag;

          LogDelimiter();

          List<V3DSystem> installed = new List<V3DSystem>();

          // =========================================================
          // If the system was checked the process all the dtaabases
          // ==========================================================
          if (systemNode.Checked)
          {

            LogMessage(String.Format("System: {0}{1}", system.Name, System.Environment.NewLine));
            UpdateStatus(NodeType.System, system.Name);
            
            // ===============================================
            // Loop through all the databases in the system
            // ===============================================
            foreach (TreeNode databaseNode in systemNode.Nodes)
            {

              Database database = (Database)databaseNode.Tag;

              LogDelimiter();

              // ================================================================
              // If the databse node was checked then process all the command
              // ===============================================================
              if (databaseNode.Checked)
              {

                LogMessage(String.Format("Database: {0}{1}", database.Name, System.Environment.NewLine));
                UpdateStatus(NodeType.Database, database.Name);

                // ===========================================
                // Loop through all the database commands
                // ===========================================
                foreach (TreeNode commandNode in databaseNode.Nodes)
                {

                  Command command = (Command)commandNode.Tag;

                  // =======================================================================================
                  // If the command is checked we are processing it otherwise log amd display appropriate 
                  // message.
                  // ========================================================================================
                  if (commandNode.Checked)
                  {
                    LogMessage(String.Format("Command: {0}{1}", command.File, System.Environment.NewLine));

                    UpdateStatus(NodeType.Command, command.File);

                    ProcessCommand(command);
                  } // END ... If the command was selected
                  else
                  {

                    // ===========================================================================
                    // If the command is DarkRed then the file was not found. Otherwise it is not 
                    // selected.
                    // =============================================================================
                    if (commandNode.ForeColor == Color.DarkRed)
                    {
                      LogMessage(String.Format("Command: {0}(Not Found){1}", command.File, System.Environment.NewLine));
                      UpdateStatus(NodeType.Command, command.File + "(Not Found)");
                    } 
                    else
                    {
                      LogMessage(String.Format("Command: {0}(Not Selected){1}", command.File, System.Environment.NewLine));
                      UpdateStatus(NodeType.Command, command.File + "(Not Selected)");

                    }
                  } // END ... Else the command wsas not selected

                } // END ... For each database command

              } // END ... If the database was selcted
              else
              {
                LogMessage(String.Format("Database: {0}(Not Selected){1}", database.Name, System.Environment.NewLine));
                UpdateStatus(NodeType.Database, database.Name + "(Not Selected)");
              } // END ... Else the database was not selected

            } // END ... For each database in tree

          } // END ... If the system was selected
          else
          {
            LogMessage(String.Format("System: {0}(Not Selected){1}", system.Name, System.Environment.NewLine));
            UpdateStatus(NodeType.System, system.Name + "(Not Selected)");
          } // END ... Else the system was not selected

           
        } // END ... For each system in tree


        LinkLicenseSystems();

        LogMessage("");
        LogMessage("Database Creation Done");

        MessageBox.Show("Done!", "Database Creation", MessageBoxButtons.OK, MessageBoxIcon.Information);

      }
      catch ( Exception ex )
      {
        LogMessage("Database creation failed : " + ex.Message);
        MessageBox.Show(ex.Message, "Database Creation Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);

      }
 
    } // StartCreateDatabase()


    /// <summary>
    /// Processing callback 
    /// </summary>
    /// <param name="ar"></param>
    public void ProcessingCallback( IAsyncResult ar )
    {
      if(ar.IsCompleted)
      {
        //Ends the Asynchronous Call.
        m_processDelegate.EndInvoke( ar );

        EnableForm(true, Cursors.Default);

      }
    }



    #endregion 

    /// <summary>
    /// Handles the after check event for the tree node
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnAfterCheck(object sender, TreeViewEventArgs e)
    {
      foreach (TreeNode child in e.Node.Nodes)
      {
        if (child.ForeColor != Color.DarkRed && child.ForeColor != Color.DarkGreen)
          child.Checked = e.Node.Checked;
      }

      if (e.Node.Tag is V3DSystem)
      {
        V3DSystem system = (V3DSystem)e.Node.Tag;


        if (system.Id == "Licensing")
        {

          // Clear the license databse server name
          m_licenseServer = string.Empty;
          m_licensePassword = string.Empty;

          // ==================================================================
          // If the node is not checked ask for the license server information
          // ==================================================================
          if (!e.Node.Checked)
          {

            LicenseSystem licenseSystem = new LicenseSystem();

            // =============================================================
            // If OK was pressed then set the license database server name
            // =============================================================
            if (licenseSystem.ShowDialog(this) == DialogResult.OK)
            {
              m_licenseServer   = licenseSystem.Server;
              m_licensePassword = licenseSystem.Password;
            }
            else
              MessageBox.Show("No V3D License System has been setup, you will need to link to a license server after the install", "License Server", MessageBoxButtons.OK, MessageBoxIcon.Warning);


          } // END ... If the license system is not checked

        } // END ... If the system is Licensing

      } // END ... If the Node is a V3DSystem node

    } // OnAfterCheck


    /// <summary>
    /// Handles the before check event for the tree node
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnBeforeCheck(object sender, TreeViewCancelEventArgs e)
    {

      if (e.Node.ForeColor == Color.DarkRed || e.Node.ForeColor == Color.DarkGreen)
        e.Cancel = true;

    }


    private void OnNodeMouseHover(object sender, TreeNodeMouseHoverEventArgs e)
    {
      
      

      if (e.Node.Tag is Command)
      {
        string filePath = Path.Combine(m_sql, ((Command)e.Node.Tag).File);
        this.m_toolTip.SetToolTip(m_treeViewSystems, filePath);
      }

    }

    

 
 
	}
}

