using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Windows.Forms;

namespace DatabasePatch351
{
  public partial class DatabasePatcher : Form
  {

    #region enum
    public enum ServiceStates
    {
      Running,
      PendingStart,
      PendingStop,
      Stopped,
      Invalid
    }

    #endregion

    #region delegates

    /// <summary>
    /// Started flag to indicate that the service was started
    /// </summary>
    private bool m_started = false;

    /// <summary>
    /// Processing delegate
    /// </summary>
    private delegate void ProcessDelegate();

    /// <summary>
    /// Update status delegate 
    /// </summary>
    /// <param name="status"></param>
    private delegate void ShowMessageDelegate(string message, string caption, MessageBoxButtons button, MessageBoxIcon icon);

    /// <summary>
    /// Enable form delegate
    /// </summary>
    /// <param name="enable"></param>
    /// <param name="cursor"></param>
    private delegate void EnableFormDelegate(bool enable, Cursor cursor);

    /// <summary>
    ///Sets focus delegate 
    /// </summary>
    /// <param name="status"></param>
    private delegate void SetFocusDelegate(TextBox textBox);

    /// <summary>
    /// Enable progress delegate
    /// </summary>
    /// <param name="enable"></param>
    /// <param name="cursor"></param>
    private delegate void EnableProgressDelegate(bool enable);

    /// <summary>
    /// Update progress delegate
    /// </summary>
    /// <param name="enable"></param>
    /// <param name="cursor"></param>
    private delegate void UpdateProgressDelegate(string message);

    /// <summary>
    /// Close form
    /// </summary>
    private delegate void CloseFormDelegate();

    #endregion

    /// <summary>
    /// Enable conacel
    /// </summary>
    private bool m_enableCancel = true;

    /// <summary>
    /// Script path used to append to scripts in the config file
    /// </summary>
    private string m_path = string.Empty;

    /// <summary>
    /// Show scripts
    /// </summary>
    private bool m_showScripts = true;

    /// <summary>
    /// Patch completed
    /// </summary>
    private bool m_patchCompleted = false;

    /// <summary>
    /// Scripts
    /// </summary>
    private List<string> m_scripts = new List<string>();

    /// <summary>
    /// Scripts supplied
    /// </summary>
    private bool m_scriptsSupplied = false;

    /// <summary>
    /// Log file
    /// </summary>
    private string m_logFile = string.Empty;

    /// <summary>
    /// Process task
    /// </summary>
    private System.Threading.Tasks.Task m_task;

    /// <summary>
    /// Update status delegate
    /// </summary>
    private ShowMessageDelegate m_showMessage = null;

    /// <summary>
    /// Enable form delegate
    /// </summary>
    private EnableFormDelegate m_enableForm = null;

    /// <summary>
    /// Enable progress delegate
    /// </summary>
    private EnableProgressDelegate m_enableProgress = null;

    /// <summary>
    /// Close form
    /// </summary>
    private CloseFormDelegate m_closeForm = null;

    /// <summary>
    /// Set Focus
    /// </summary>
    private SetFocusDelegate m_setFocus = null;

    /// <summary>
    /// 
    /// </summary>
    private UpdateProgressDelegate m_updateProgress = null;

    public DatabasePatcher()
    {
      InitializeComponent();
 
    
      m_enableForm = new EnableFormDelegate(this.EnableForm);
      m_showMessage = new ShowMessageDelegate(this.ShowMessage);
      m_setFocus = new SetFocusDelegate(this.SetFocus);
      m_enableProgress = new EnableProgressDelegate(this.EnableProgress);
      m_closeForm = new CloseFormDelegate(this.CloseForm);
      m_updateProgress = new UpdateProgressDelegate(this.UpdateProgress);
    }

    private void ShowUsage()
    {
      StringBuilder builder = new StringBuilder();
      builder.Append("DatabasePatcher\n\n");
      builder.Append("Patches the database using the specified options.\n\n");
      builder.Append("Options:\n\n");
      builder.Append("[-a <DIRECTORY>]\tSpecifies the assemblies directory used to \n");
      builder.Append("\t\treplace <ASSEMBLIES> in the scripts files.\n\n");
      builder.Append("[-l <LOGPATH>]\tSpecfies the og file path used to perform logging.\n\n");
      builder.Append("[-d <DIRECTORY>]\tSpecifies the scripts directory.\n\n");
      builder.Append("[-p <PASSWORD>]\tSpecifies the sa password for the database\n\n");
      builder.Append("[-i <INSTANCE>]\tSpecifies the database server instance \n");
      builder.Append("\t\t( example: (local)\\V3D\n\n");
      builder.Append("[-f <FILEPATH>]\tSpecifies the xml script file path.\n\n");
      builder.Append("[-s <FILE>]\tSpecifies a single script file. May\n");
      builder.Append("\t\tcontain multiple -s options.\n");
      builder.Append("\t\t-s script1.sql -s script2.sql ...\n\n");
      builder.Append("[-c]\t\tDisables the ability to cancel the application\n\n");
      builder.Append("Description:\n\n");
      builder.Append("The database patcher will process the supplied scripts from both the\n");
      builder.Append("command line and/or the script file (-f). The order will be first \n");
      builder.Append("come first server so if a -s is specified before he -f then the script\n");
      builder.Append("from the -s qill be processed first before the scripts in the\n");
      builder.Append("file. \n");

      MessageBox.Show(builder.ToString());


    }

    /// <summary>
    /// Creates the Databse patcher
    /// </summary>
    /// <param name="args">command line arguements</param>
    /// <returns></returns>
    public static DatabasePatcher Create(string[] args)
    {
      DatabasePatcher patcher = new DatabasePatcher();

      if (patcher == null)
        throw new OutOfMemoryException("Failed to allocate database patcher");

      patcher.m_textBoxInstance.Text = System.Configuration.ConfigurationManager.AppSettings["instance"];
      patcher.m_textBoxPassword.Text = System.Configuration.ConfigurationManager.AppSettings["password"];
      patcher.m_textBoxScript.Text = System.Configuration.ConfigurationManager.AppSettings["script"];
      patcher.m_showScripts = Boolean.Parse(System.Configuration.ConfigurationManager.AppSettings["showscripts"]);


      if (patcher.m_textBoxScript.Text.Length == 0)
      {
        string[] files = System.IO.Directory.GetFiles(".", "*.sql");

        if (files.Length > 0)
        {
          patcher.m_textBoxScript.Text = files[0];
        }
      }


      try
      {

        // =================================
        // For each command line arguement
        // =================================
        for (int i = 0; i < args.Length; ++i)
        {
          string arg = args[i];

          if (arg[0] == '-' || arg[0] == '/' )
          {
            switch (arg[1])
            {
              case '?':
                {
                  patcher.ShowUsage();

                  throw new Exception("Usage");
                }
                //break;
              // ====================
              // script path 
              // ====================
              case 'd':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing scripts directory arguement");

                  patcher.m_path = args[i];
                }
                break;


              // ====================
              // Asemblies directory
              // ====================
              case 'a':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing assemblies directory arguement");

                  patcher.m_textBoxAssemblies.Text = args[i];
                }
                break;

              // ============
              // SA Password
              // ===========
              case 'p':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing password arguement");

                  patcher.m_textBoxPassword.Text = args[i];
                }
                break;

              // ===================
              // database instance
              // ===================
              case 'i':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing database server\\instance arguement");

                  patcher.m_textBoxInstance.Text = args[i];
                }
                break;

              // ==============================
              // Disables the cancel option
              // =============================
              case 'c':
                patcher.m_enableCancel = false;
                break;

              // ================
              // Sql script 
              // ==============
              case 's':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing script arguement");

                  patcher.m_scripts.Add(args[i]);

                }
                break;

              // =========================================
              // Patch file containing sql script list
              // =========================================
              case 'f':
                {
                  if (++i == args.Length)
                    throw new Exception("Missing patch file arguement");

                  if (!System.IO.File.Exists(args[i]))
                    throw new Exception(String.Format("Specified patch file {0} does not exist", args[i]));


                  XmlDocument doc = new XmlDocument();
                  doc.Load(args[i]);

                  // ====================================
                  // Load all the scripts into the list
                  // ====================================
                  foreach (XmlNode node in doc.DocumentElement.SelectNodes("script"))
                  {
                    if (node.Attributes["sql"] != null)
                    {
                      string script = node.Attributes["sql"].Value;

                      if ( ! patcher.m_scripts.Contains(script) )
                        patcher.m_scripts.Add(script);
                      
                    }
                  }   // END ... For each script

                }
                break;

              case 'l':
                {

                  if (++i == args.Length)
                    throw new Exception("Missing log file arguement");

                  patcher.m_logFile = args[i];

                  string dir = Path.GetDirectoryName(patcher.m_logFile);

                  if (!Directory.Exists(dir))
                    Directory.CreateDirectory(dir);

                  // Delete the previous file
                  File.Delete(args[i]);
               
                }

                break;
              default:
                throw new Exception("Invalid arguement");

            } // END ... swicth on argument

          } // END ... If the arguement contains a '-' swicth identifier

        } // END ... For each arguement



        if (patcher.m_scripts.Count > 0)
        {

          string [] scripts = new string [patcher.m_scripts.Count];

          patcher.m_scripts.CopyTo(scripts);
          patcher.m_scripts.Clear();

          foreach (string script in scripts)
          {

            string scriptFile = script;
            string dir = Path.GetDirectoryName(script);

            if ( string.IsNullOrEmpty(dir) && patcher.m_path.Length > 0)
              scriptFile = Path.Combine(patcher.m_path, script);

            if (!System.IO.File.Exists(scriptFile))
              throw new Exception(String.Format("Specified script file {0} does not exist", scriptFile));

            patcher.LogMessage(string.Format("Adding script {0} to be processed", scriptFile));

            patcher.m_scripts.Add(scriptFile);

          }


          // Remove controls un-needed
          patcher.m_scriptsSupplied = true;
          patcher.m_textBoxScript.Visible = false;
          patcher.m_textBoxAssemblies.Visible = false;

          patcher.m_labelScript.Visible = false;
          patcher.m_buttonBrowse.Visible = false;
          patcher.m_buttonAssembliesBrowse.Visible = false;

          patcher.m_buttonTest.Visible = false;

          patcher.m_buttonCancel.Visible = patcher.m_enableCancel;
          patcher.m_labelStatus.Visible = patcher.m_showScripts;

          // Make adjustments to form
          patcher.Height = patcher.Height - (patcher.m_textBoxScript.Height + patcher.m_textBoxAssemblies.Height + 10);
          patcher.Width = patcher.Width - 200;
          patcher.m_buttonPatch.Location = new Point(patcher.m_buttonPatch.Location.X - 200, patcher.m_buttonPatch.Location.Y);
          patcher.m_buttonCancel.Location = new Point(patcher.m_buttonCancel.Location.X - 200, patcher.m_buttonCancel.Location.Y);
          

 
        }


      } // END ... Try
      catch (Exception ex)
      {

        if (ex.Message != "Usage")
        {
          patcher.LogMessage(string.Format("Failed to create patcher : {0}", ex.Message));

          MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        throw;
      }

      return patcher;
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="control"></param>
    private void SetFocus(TextBox textBox)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_setFocus, new object[] { textBox });
      }
      else
      {
        textBox.Focus();
      }

    }

    /// <summary>
    /// Displays a message mox
    /// </summary>
    /// <param name="message"></param>
    /// <param name="caption"></param>
    /// <param name="button"></param>
    /// <param name="icon"></param>
    private void ShowMessage(string message, string caption, MessageBoxButtons button, MessageBoxIcon icon)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_showMessage, new object[] { message, caption, button, icon });
      }
      else
      {
        MessageBox.Show(this, message, caption, button, icon);
      }
    }

    /// <summary>
    /// Enables/Disables the form
    /// </summary>
    /// <param name="enable"></param>
    /// <param name="cursor"></param>
    private void EnableForm(bool enable, Cursor cursor)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_enableForm, new object[] { enable, cursor });
      }
      else
      {

        this.Cursor = cursor;
        m_textBoxPassword.Enabled = enable;
        m_textBoxInstance.Enabled = enable;
        m_textBoxScript.Enabled = enable;
        m_buttonBrowse.Enabled = enable;
        m_buttonCancel.Enabled = enable;
        m_buttonPatch.Enabled = enable;
        m_buttonTest.Enabled = enable;

      }
    }

    /// <summary>
    /// Enables/Disables the progress bar
    /// </summary>
    /// <param name="enable"></param>
    private void EnableProgress(bool enable)
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_enableProgress, new object[] { enable });
      }
      else
      {
        m_progressBar.Style = (enable ? ProgressBarStyle.Marquee : ProgressBarStyle.Blocks);
      }
    }

    /// <summary>
    /// Close form
    /// </summary>
    private void CloseForm()
    {
      if (this.InvokeRequired)
      {
        this.Invoke(m_closeForm, new object[] {  });
      }
      else
      {
        Close();
      }
    }

    /// <summary>
    /// Update progress
    /// </summary>
    /// <param name="message"></param>
    private void UpdateProgress(string message)
    {
      if (m_showScripts)
      {
        if (this.InvokeRequired)
        {
          this.Invoke(m_updateProgress, new object[] { message });
        }
        else
        {
          m_labelStatus.Text = message;
         }
      }
    }

    /// <summary>
    /// Log message 
    /// </summary>
    /// <param name="message"></param>
    private void LogMessage(string message)
    {

      try
      {
        if (!string.IsNullOrEmpty(m_logFile))
        {

          string directoryName = Path.GetDirectoryName(m_logFile);
          if (!Directory.Exists(directoryName))
            Directory.CreateDirectory(directoryName);

          using (StreamWriter writer = File.AppendText(m_logFile))
          {

            writer.WriteLine(message);
          }

        }
      }
      catch (Exception)
      {
        // Ignore
      }

    }

    protected override void OnClosing(CancelEventArgs e)
    {
      if ( ! m_enableCancel && m_scriptsSupplied && ! m_patchCompleted)
        e.Cancel = true;
      else
        base.OnClosing(e);
    }

    private void OnBrowseClick(object sender, EventArgs e)
    {

      OpenFileDialog browser = new OpenFileDialog();
      browser.Title = "Select the script file";
      browser.SupportMultiDottedExtensions = false;
      browser.ShowReadOnly = true;
      browser.Multiselect = false;
      browser.Filter = "All|*.*|SQL Script|*.sql";

      if (browser.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        m_textBoxScript.Text = browser.FileName;
    }

    private bool TestDatabaseConnection(bool displaySuccess)
    {
      
      LogMessage(string.Format("Testing database connectectivity {0}",m_textBoxInstance.Text));

      if (string.IsNullOrEmpty(m_textBoxPassword.Text))
      {
        LogMessage("No password enetered");
        MessageBox.Show(this, "No password entered", "Invalid Password", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxPassword.Text = string.Empty;
        m_textBoxPassword.Focus();
        return false;
      }
      else
      {

        string[] parts = m_textBoxInstance.Text.Split(new Char[] { '\\' });
        string serviceName = string.Format("MSSQL${0}", parts[1]);

        switch (GetServiceState(serviceName))
        {
          case ServiceStates.Invalid:
            {
              LogMessage(string.Format("Invalid database instance {0}", m_textBoxInstance.Text));
              MessageBox.Show("Invalid database instance, does not exist", "Database Test", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return false;
          case ServiceStates.Stopped:
            try
            {
              ServiceControl("start", serviceName);
              m_started = true;
            }
            catch (Exception ex)
            {
              LogMessage(String.Format("Failed to start service {0}", serviceName));

              MessageBox.Show(this, ex.Message, "Service Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
              m_textBoxInstance.Focus();
              return false;
            }

            break;

        }
 

        string connectionString = "Data Source=" + m_textBoxInstance.Text + ";Initial Catalog=master;User Id=sa;Password=" + m_textBoxPassword.Text + ";";


        using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(connectionString))
        {
          try
          {
            connection.Open();

            if ( displaySuccess )
              MessageBox.Show("Database connection was successful", "Database Test", MessageBoxButtons.OK);
          }
          catch (Exception ex)
          {
            LogMessage(String.Format("Failed to log into database : {0}", ex.Message));

            MessageBox.Show(this, ex.Message, "Login Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
            m_textBoxPassword.Text = string.Empty;
            m_textBoxPassword.Focus();
            return false;
          }

        }

      }

      return true;

    }

    private void OnTestDatabase(object sender, EventArgs e)
    {
      TestDatabaseConnection(true);
    }


    /// <summary>
    /// 
    /// </summary>
    /// <param name="filename"></param>
    /// <param name="args"></param>
    private void Process(string filename, string args)
    {
      System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();
      info.CreateNoWindow = true;
      info.UseShellExecute = false;
      info.FileName = filename;
      info.Arguments = args;
      info.RedirectStandardError = true;
      info.RedirectStandardOutput = true;

      System.Diagnostics.Process process = new System.Diagnostics.Process();

      process.StartInfo = info;

      process.Start();

      process.WaitForExit();

      string output = process.StandardOutput.ReadToEnd();

      if (process.ExitCode != 0 || output.StartsWith("Msg") )
      {

        LogMessage(string.Format("{0} failed"));
        LogMessage("Output:");
        LogMessage(output);
        LogMessage("Error:");
        LogMessage(process.StandardError.ReadToEnd());

        throw new Exception(string.Format("{0} failed"));
      }
      else
      {
        LogMessage("Output:");
        LogMessage(output);
      }


    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    private bool IsServiceExists(string name)
    {
      System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();
      info.CreateNoWindow = true;
      info.UseShellExecute = false;
      info.FileName = "sc.exe";
      info.Arguments = string.Format("query {0}", name);
      info.RedirectStandardError = true;
      info.RedirectStandardOutput = true;

      System.Diagnostics.Process process = new System.Diagnostics.Process();

      process.StartInfo = info;

      process.Start();

      process.WaitForExit();

      return ( process.ExitCode == 0 );


    }

    private  ServiceStates GetServiceState(string name)
    {

      LogMessage(string.Format("Getting service state for {0}",name));

      System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();
      info.CreateNoWindow = true;
      info.UseShellExecute = false;
      info.FileName = "sc.exe";
      info.Arguments = string.Format("query {0}", name);
      info.RedirectStandardError = true;
      info.RedirectStandardOutput = true;

      System.Diagnostics.Process process = new System.Diagnostics.Process();

      process.StartInfo = info;

      process.Start();

      process.WaitForExit();

      string output = process.StandardOutput.ReadToEnd();
      LogMessage("");
      LogMessage(String.Format("Output: {0}",output));

      if (process.ExitCode != 0)
        return ServiceStates.Invalid;
      else
      {
 
        if (output.Contains("RUNNING"))
          return ServiceStates.Running;
        else if (output.Contains("START_PENDING"))
          return ServiceStates.PendingStart;
        else if (output.Contains("STOP_PENDING"))
          return ServiceStates.PendingStop;
        else
          return ServiceStates.Stopped;
      }


    }


    /// <summary>
    /// Perform a service control operation
    /// </summary>
    /// <param name="op">start or stop</param>
    /// <param name="name">Service name</param>
    private void ServiceControl(string op, string name)
    {

      try
      {
 
        // ====================================================================================
        // Dont need to start if we are already started or stop if we are already stopped.
        // ==================================================================================
        ServiceStates state = GetServiceState(name);
        if ( ( op == "start" && (state == ServiceStates.Running || state == ServiceStates.PendingStart) ) ||
             ( op == "stop" && (state == ServiceStates.Stopped || state == ServiceStates.PendingStop)  ) )
            return;

        do
        {
          System.Threading.Thread.Sleep(10);
          state = GetServiceState(name);
        } while ((op == "start" && state != ServiceStates.Stopped) ||
                  (op == "stop" && state != ServiceStates.Running));

        LogMessage(String.Format("{0} service {1}", op.ToUpper(), name));

        Process("sc.exe", string.Format(" {0} \"{1}\"", op, name));

        System.Threading.Thread.Sleep(500);

      }
      catch (Exception)
      {
        throw new Exception(string.Format("Failed to {0} {1}", op, name));

      }
 
    }


    private void OnPatch(object sender, EventArgs e)
    {

      if (string.IsNullOrEmpty(m_textBoxInstance.Text))
      {
        MessageBox.Show(this, "No instance entered", "Invalid Instance", MessageBoxButtons.OK, MessageBoxIcon.Error);
        m_textBoxInstance.Text = string.Empty;
        m_textBoxInstance.Focus();
        return;
      }

      if (!TestDatabaseConnection(false))
        return;

      // ==============================================================
      // Only check the script text box if not scripts were supplied
      // ==============================================================
      if (!m_scriptsSupplied)
      {
        if (string.IsNullOrEmpty(m_textBoxScript.Text))
        {
          MessageBox.Show(this, "No script file entered", "Invalid script file", MessageBoxButtons.OK, MessageBoxIcon.Error);
          m_textBoxScript.Text = string.Empty;
          m_textBoxScript.Focus();
          return;
        }
        else if (!System.IO.File.Exists(m_textBoxScript.Text))
        {
          MessageBox.Show(this, "Specified file does not exist", "Invalid script file", MessageBoxButtons.OK, MessageBoxIcon.Error);
          m_textBoxScript.Text = string.Empty;
          m_textBoxScript.Focus();
          return;
        }


        m_scripts.Add(m_textBoxScript.Text);

      } // END ... If scripts were not supplied


      // Enable progress
      EnableProgress(true);

      // Disable form
      EnableForm(false, Cursors.WaitCursor);

      m_task = new System.Threading.Tasks.Task( () =>
      {



        try
        {
          
            string [] parts = m_textBoxInstance.Text .Split( new Char [] { '\\' } );

            string serviceName = "MSSQL$" + parts[1];

            if (m_started)
            {
              LogMessage(string.Format("Stopping database service instance {0}", parts[1]));
              UpdateProgress(string.Format("Stopping:  {0} database service", parts[1]));

              ServiceControl("stop", serviceName);

              LogMessage(string.Format("Starting database service instance {0}", parts[1]));
              UpdateProgress(string.Format("Starting:  {0} database service", parts[1]));

              ServiceControl("start", serviceName);
            }



            foreach (string script in m_scripts)
            {
              string filename = Path.GetFileName(script);

              LogMessage(string.Format("Processing {0}", script));

              UpdateProgress(string.Format("Processing:  {0}", filename));

              try
              {
                string updatedScript = script;

                if (!string.IsNullOrEmpty(m_textBoxAssemblies.Text))
                {
                   updatedScript = Path.Combine(Path.GetDirectoryName(script), "TMP_" + filename);

                  string[] lines = File.ReadAllLines(script);

                  using (StreamWriter writer = File.CreateText(updatedScript))
                  {

                    foreach (string line in lines)
                    {
                      writer.WriteLine(line.Replace("<ASSEMBLIES>", m_textBoxAssemblies.Text));
                    }

                  }
                }

                Process("osql.exe", string.Format("-S {0} -d master -U sa -P {1} -n -i {2}", m_textBoxInstance.Text, m_textBoxPassword.Text, updatedScript));
              }
              catch ( Exception )
              {
                throw new Exception(string.Format("Failed to process {0}",script));
              }

            }



            EnableProgress(false);

            ShowMessage("Patch was successful", "Patch Succeded", MessageBoxButtons.OK, MessageBoxIcon.Information);
        

        }
        catch (Exception ex)
        {
          EnableProgress(false);

          LogMessage(string.Format("Patch failed : {0}", ex.Message));

          ShowMessage(ex.Message, "Patch Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);

          SetFocus(m_textBoxScript);

          return;

        }
        finally
        {
          UpdateProgress("");

          EnableProgress(false);

          EnableForm(true, Cursors.Default);

          m_patchCompleted = true;

          if ( m_scriptsSupplied )
            CloseForm();
        }

      }
      );

       m_task.Start();
 
    }
  

    private void OnCancel(object sender, EventArgs e)
    {
      this.Close();
    }

    private void OnInstanceChanged(object sender, EventArgs e)
    {
      m_started = false;
    }

    private void OnBrowseAssembliesClick(object sender, EventArgs e)
    {
      FolderBrowserDialog browser = new FolderBrowserDialog();
      browser.ShowNewFolderButton = false;

      if (browser.ShowDialog() == System.Windows.Forms.DialogResult.OK)
        m_textBoxAssemblies.Text = browser.SelectedPath;

    }
  }
}
