using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Net;


namespace Viatronix.Utilities.Database
{
  public partial class LinkLicenseSystem : Form
  {


    #region fields

    /// <summary>
    /// Synonym defintions
    /// </summary>
    private List<Synonym> m_synonyms = (List<Synonym>)System.Configuration.ConfigurationManager.GetSection("synonyms");

    private string m_licenseServer = string.Empty;

    private string m_enterpriseServer = string.Empty;

    private string m_licensePassword = string.Empty;

    private string m_enterprisePassword = string.Empty;

    private string m_logFilePath = string.Empty;

    private bool m_displayGui = true;

    private bool m_testConnections = true;

    private string m_ip = string.Empty;

 

    #endregion


    #region constructors


    /// <summary>
    /// Constructor setting the server and password settings
    /// </summary>
    public LinkLicenseSystem(string [] args)
    {
      InitializeComponent();


      m_displayGui = false;

      for ( int i = 0; i < args.Length; ++i )
      {
        if (args[i] == "-es")
          m_enterpriseServer = (++i == args.Length ? string.Empty : args[i]);
        else if (args[i] == "-ep")
          m_enterprisePassword = (++i == args.Length ? string.Empty : args[i]);
        else if (args[i] == "-ls")
          m_licenseServer = (++i == args.Length ? string.Empty : args[i]);
        else if (args[i] == "-lp")
          m_licensePassword = (++i == args.Length ? string.Empty : args[i]);
        else if (args[i] == "-t")
          m_testConnections = false;
        else if (args[i] == "-gui")
          m_displayGui = true;
        else
        {
          Console.WriteLine("Invalid parameter " + args[i]);
          return;
        }            
      }

      if (string.IsNullOrEmpty(m_enterpriseServer))
      {
        Console.WriteLine("Missing V3D Enterprise Systems Server");
        return;
      }

      if (string.IsNullOrEmpty(m_enterprisePassword))
      {
        Console.WriteLine("Missing V3D Enterprise Systems Server Password");
        return;
      }

      if (string.IsNullOrEmpty(m_licenseServer))
      {
        Console.WriteLine("Missing V3D License Systems Server");
        return;
      }

      if (string.IsNullOrEmpty(m_licensePassword))
      {
        Console.WriteLine("Missing V3D License Systems Server Password");
        return;
      }

      string host = Dns.GetHostName();
      IPHostEntry ip = Dns.GetHostEntry(host);

      foreach (IPAddress address in ip.AddressList)
      {
        if (address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
        {
          m_ip = address.ToString();
          break;
        }
      }

      string[] paths = { System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), 
                          "Viatronix",
                          "Logs",
                          "LinkLicenseSystem.log" };

      m_logFilePath = System.IO.Path.Combine(paths);

      if (m_logFilePath.Length > 0)
        System.IO.File.Delete(m_logFilePath);

      if (m_displayGui)
      {
        m_textBoxEnterpriseServer.Text = m_enterpriseServer;
        m_textBoxEnterprisePassword.Text = m_enterprisePassword;
        m_textBoxLicenseServer.Text = m_licenseServer;
        m_textBoxLicensePassword.Text = m_licensePassword;
      }

    }

    /// <summary>
    /// Default constructor
    /// </summary>
    public LinkLicenseSystem()
    {
      InitializeComponent();

      string host = Dns.GetHostName();
      IPHostEntry ip = Dns.GetHostEntry(host);


      foreach (IPAddress address in ip.AddressList)
      {
        if (address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
        {
          m_ip = address.ToString();
          break;
        }
      }
 
      string[] paths = { System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), 
                          "Viatronix",
                          "Logs",
                          "LinkLicenseSystem.log" };

      m_logFilePath = System.IO.Path.Combine(paths);

      if (m_logFilePath.Length > 0)
        System.IO.File.Delete(m_logFilePath);

    }

    #endregion


    #region properties

    public bool DisplayGui
    {
      get { return m_displayGui; }
    }

    #endregion

    #region methods

    private void EnableForm(bool flag)
    {

      m_textBoxEnterprisePassword.Enabled = flag;
      m_textBoxEnterpriseServer.Enabled = flag;
      m_textBoxLicensePassword.Enabled = flag;
      m_textBoxLicenseServer.Enabled = flag;
      m_buttonEnterpriseTest.Enabled = flag;
      m_buttonLicenseTest.Enabled = flag;
      m_buttonLink.Enabled = flag;
      m_buttonCancel.Enabled = flag;


    }

    /// <summary>
    /// Logs message to file
    /// </summary>
    /// <param name="message"></param>
    private void LogMessage(string message)
    {

      if (m_logFilePath.Length > 0)
      {

        if (!System.IO.Directory.Exists(System.IO.Path.GetDirectoryName(m_logFilePath)))
          System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(m_logFilePath));

        StreamWriter writer = new StreamWriter(m_logFilePath, true, System.Text.ASCIIEncoding.ASCII);

        writer.WriteLine(message);

        writer.Close();
      }

    }  // LogMessage()

    /// <summary>
    /// Tests a databse connection using the servre and sa password
    /// </summary>
    /// <param name="server">Database server (SERVER\INSTANCE)</param>
    /// <param name="password">SA password</param>
    private void TestConnection(string server, string database, string password)
    {


      if (string.IsNullOrEmpty(server))
        throw new System.MissingFieldException("No server specified");


      if (string.IsNullOrEmpty(password))
        throw new System.MissingFieldException("No password specified");

      string connectionString = "Data Source=" + server + ";Initial Catalog=" +  database  + ";User Id=sa;Password=" + password + ";";

      using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(connectionString))
      {
        try
        {
          connection.Open();

          System.Data.SqlClient.SqlCommand command = new System.Data.SqlClient.SqlCommand("[v3d].[sp_checkSystem]", connection);

          command.ExecuteNonQuery();

        }
        catch (System.Data.SqlClient.SqlException)
        {
          throw;
        }
 
      }


    } // TestConnection()


    /// <summary>
    /// Links the V3D Enterprise System with the V3D Licesning System
    /// </summary>
    public void Link()
    {

      if (m_testConnections)
      {
        try
        {
          
          LogMessage("Testing V3D Enterprise Server database connection");

          /// Test the V3D Enterprise System database parameters
          TestConnection(m_enterpriseServer, "System", m_enterprisePassword);
        }
        catch (Exception ex)
        {
          throw new System.ApplicationException("V3D Enterprise System: " + ex.Message);
       }


        try
        {
          LogMessage("Testing V3D License Server database connection");

          /// Test the V3D License System database parameters
          TestConnection(m_licenseServer, "Licensing", m_licensePassword);
        }
        catch (Exception ex)
        {
          throw new System.ApplicationException("V3D Licensing System: " + ex.Message);
        }
      }

      try
      {
        CreateLink();

        CreateSynonyms();

        RegisterServer();
      }
      catch (Exception ex)
      {
        throw new System.ApplicationException("Link Failed: " + ex.Message);
      }

    }

    private void LinkLicenseServer()
    {
      try
      {

        LogMessage("Testing V3D Enterprise Server database connection");

        /// Test the V3D Enterprise System database parameters
        TestConnection(m_enterpriseServer,"System", m_enterprisePassword);

      }
      catch (Exception ex)
      {

        LogMessage("V3D Enterprise Server database connection failed : " + ex.Message);

        if (MessageBox.Show(this, "V3D Enterprise System: " + ex.Message, "Link failed", MessageBoxButtons.OKCancel, MessageBoxIcon.Error) == DialogResult.Cancel)
          return;
      }


      try
      {

        LogMessage("Testing V3D License Server database connection");

        /// Test the V3D License System database parameters
        TestConnection(m_licenseServer, "Licensing", m_licensePassword);


      }
      catch (Exception ex)
      {

        LogMessage("V3D License Server database connection failed : " + ex.Message);

        if (MessageBox.Show(this, "V3D License System: " + ex.Message, "Link failed", MessageBoxButtons.OK, MessageBoxIcon.Error) == DialogResult.Cancel)
          return;
      }

      CreateLink();

    }


    /// <summary>
    /// Creates the link to the new licnense server
    /// </summary>
    private void CreateLink()
    {


      string message = string.Format("Creating link to V3D License Server ({0}) from V3D Enterprise Server ({1})", m_licenseServer, m_enterpriseServer);
      LogMessage(System.Environment.NewLine);
      LogMessage("===============================================================================================================================================");
      LogMessage(message);
      
      string sql = string.Format("EXEC [System].[v3d].[sp_linkLicenseServer] @SERVERNAME = '{0}'", m_licenseServer);

      // =================================
      // Link new license server
      // =================================
      ProcessSql(m_enterpriseServer, "System", m_enterprisePassword, sql);


    } // CreateLink()

    /// <summary>
    /// Creates the new synonyms
    /// </summary>
    private void CreateSynonyms()
    {

      foreach (Synonym synonym in m_synonyms)
      {
        string message = string.Format("Creating synonym {0} for {1}", synonym.Destination, synonym.Source);

        LogMessage(System.Environment.NewLine);
        LogMessage("===============================================================================================================================================");
        LogMessage(message);
 
        string sql = string.Empty;

        if ( string.Compare(m_licenseServer, m_enterpriseServer, true) == 0 )
        {
          string[] args = { synonym.Destination, synonym.Destination, synonym.Destination, synonym.Source   };


          sql =  string.Format("IF (SELECT object_id('{0}')) IS NOT NULL DROP SYNONYM {1}; CREATE SYNONYM {2} FOR {3};", args);

        }
        else
        {
          string[] args = { synonym.Destination, synonym.Destination, synonym.Destination, m_licenseServer, synonym.Source };

          sql = string.Format("IF (SELECT object_id('{0}')) IS NOT NULL DROP SYNONYM {1}; CREATE SYNONYM {2} FOR [{3}].{4};", args); 
        }

        LogMessage(sql);

        // =================================
        // Delete and create new synonym
        // =================================
        ProcessSql(m_enterpriseServer, synonym.Database, m_enterprisePassword, sql);


      }

      

    } // CreateSynonyms()

    /// <summary>
    /// Registers the V3D Enterprise System with the V3D License Sytem
    /// </summary>
    private void RegisterServer()
    {

      string message = string.Format("Registering the V3D Enterprise System with the  V3D License Serve");
      LogMessage(System.Environment.NewLine);
      LogMessage("===============================================================================================================================================");
      LogMessage(message);


      string sql = string.Format("EXEC [System].[v3d].[sp_registerServer] @IP = '{0}'", m_ip);

      // =================================
      // Link new license server
      // =================================
      ProcessSql(m_enterpriseServer, "System", m_enterprisePassword, sql);

    } // RegisterServer()


    private void ProcessSql(string server, string database, string password, string sql)
    {
      Process p = new Process();
      ProcessStartInfo info = new ProcessStartInfo("sqlcmd.exe", string.Format("-S {0} -d {1} -U sa -P {2} -Q \"{3}\"", server, database, password, sql));;

 
      info.RedirectStandardOutput = true;
      info.RedirectStandardError = true;
      info.UseShellExecute = false;
      info.WindowStyle = ProcessWindowStyle.Hidden;
      info.CreateNoWindow = true;
      info.ErrorDialog = true;
      //info.ErrorDialogParentHandle = this.Handle;

      p.StartInfo = info;
      p.Start();

      //this.Focus();

      string output = p.StandardOutput.ReadToEnd();
      p.WaitForExit();

      
      LogMessage(output);

      if (!string.IsNullOrEmpty(output))
        throw new System.ApplicationException(output);

    }


    #endregion

    #region event handlers

    /// <summary>
    /// Handles the click of the Test button for the V3D Enterprise System
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTestEnterpriseClick(object sender, EventArgs e)
    {

      try
      {
        EnableForm(false);

        LogMessage("Testing V3D Enterprise Server database connection");

        TestConnection(m_textBoxEnterpriseServer.Text, "System", m_textBoxEnterprisePassword.Text);
      }
      catch (Exception ex)
      {
        LogMessage("V3D Enterprise Server database connection failed : " + ex.Message);

        MessageBox.Show(this, ex.Message, "V3D Enterprise System", MessageBoxButtons.OK, MessageBoxIcon.Error);

        if (ex.Message.Contains("password"))
          m_textBoxEnterprisePassword.Focus();
        else
          m_textBoxEnterpriseServer.Focus();
      }
      finally
      {
        EnableForm(true);
      }



    } // OnTestEnterpriseClick()

    /// <summary>
    /// Handles the click event for the V3D License System 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTestLicenseClick(object sender, EventArgs e)
    {

      try
      {
        EnableForm(false);

        TestConnection(m_textBoxLicenseServer.Text, "Licensing", m_textBoxLicensePassword.Text);
      }
      catch (Exception ex)
      {
        MessageBox.Show(this, ex.Message, "V3D License System", MessageBoxButtons.OK, MessageBoxIcon.Error);

        if (ex.Message.Contains("password"))
          m_textBoxLicensePassword.Focus();
        else
          m_textBoxLicenseServer.Focus();
      }
      finally
      {
        EnableForm(true);
      }

    } // OnTestLicenseClick()


    private void OnLinkClick(object sender, EventArgs e)
    {
      EnableForm(false);

      m_licenseServer = m_textBoxLicenseServer.Text;
      m_licensePassword = m_textBoxLicensePassword.Text;
      m_enterpriseServer = m_textBoxEnterpriseServer.Text;
      m_enterprisePassword = m_textBoxEnterprisePassword.Text;


      try
      {
        Link();

        MessageBox.Show(this, 
                        string.Format("Link ofr V3D Enterprise System ({0}) to V3D License System ({1}) was SUCESSFUL", m_enterpriseServer, m_licenseServer), 
                        "Link Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);

      }
      catch ( Exception ex )
      {
        MessageBox.Show(this, ex.Message, "Link Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }


      EnableForm(true);

    }


    #endregion

    private void OnEnterLicensePassword(object sender, EventArgs e)
    {

      // ============================================================================================
      // If the current license password is blank and the server are the same then set the password
      // =============================================================================================
      if (string.IsNullOrEmpty(m_textBoxLicensePassword.Text) && string.Compare(m_textBoxEnterpriseServer.Text, m_textBoxLicenseServer.Text, true) == 0 )
        m_textBoxLicensePassword.Text = m_textBoxEnterprisePassword.Text;
    }

    private void OnCancelClick(object sender, EventArgs e)
    {
      this.Close();
    }

    private void m_labelEnterpriseServer_Click(object sender, EventArgs e)
    {

    }


    

   




  }
}
