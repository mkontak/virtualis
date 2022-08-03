using System;
using System.Threading;
using System.Collections;
using System.Net.Sockets;
using System.Configuration;
using System.Data;
using System.Xml;
using System.IO;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Net;
using System.Net.Mail;
using System.Text;
using System.Data.Sql;
using System.Data.SqlClient;

using Viatronix.Operations;

namespace Viatronix
{

  /// <summary>
  /// Server to access remote/local cache. Performs synchronous operations to maintain cache coherence 
  /// on the remote server. Also can act as a router passing transactions through without saving to disk.
  /// </summary>
  public class Server : Thread
  {

    #region fields

    /// <summary>
    /// Used to watch the local cache and to update the main server
    /// </summary>
    private List<FileSystemWatcher> m_folderWatchers = new List<FileSystemWatcher>();

    /// <summary>
    /// Folders to be watched
    /// </summary>
    Dictionary<string, Folder> m_folders;

    /// <summary>
    /// disposed flag 
    /// </summary>
    private bool m_disposed = false;

    /// <summary>
    /// The operations that should be done on a regular basis.
    /// </summary>
    private List<Operations.IFileWatcherOperation> m_operations = new List<Operations.IFileWatcherOperation>();

    #endregion

    #region constructors and finalizers

    /// <summary>
    /// Constructor
    /// </summary>
    public Server(Dictionary<string, Folder> folders) 
    {

      m_folders = folders;

      foreach (KeyValuePair<string,Folder> folder in folders)
      {
        FileSystemWatcher watcher = new FileSystemWatcher(folder.Value.Path);

        watcher.IncludeSubdirectories = true;
        watcher.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName;

        watcher.Created += new FileSystemEventHandler(OnFileSystemCreated);
        watcher.Changed += new FileSystemEventHandler(OnFileSystemChanged);
        watcher.Deleted += new FileSystemEventHandler(OnFileSystemDeleted);

        watcher.EnableRaisingEvents = true;

 
        m_folderWatchers.Add(watcher);
      }

      m_operations = (List<IFileWatcherOperation>)ConfigurationManager.GetSection("operations");
    } // Server()

 

    /// <summary>
    /// Dispose of the server
    /// </summary>
    /// <param name="disposing"></param>
    protected override void Dispose(bool disposing)
    {
      base.Dispose(disposing);

      m_disposed = true;
    }

    #endregion


    #region methods

    /// <summary>
    /// Logs message to file
    /// </summary>
    /// <param name="message">Message to log</param>
    public static void LogMessage(string message)
    {
      string logFile = (System.Configuration.ConfigurationManager.AppSettings["logFile"] != null ? System.Configuration.ConfigurationManager.AppSettings["logFile"] : string.Empty);

      if (logFile.Length > 0)
      {

        DateTime now = DateTime.Now;

        logFile = logFile + "_" + now.ToString("yyyyMMddHHmmssffff") + ".log";

        FileStream fs = new FileStream(logFile, FileMode.Append, FileAccess.Write, FileShare.None);

        StreamWriter writer = new StreamWriter(fs);

        writer.WriteLine(string.Format("{0}   {1}", now.ToString("yyyy-MM-dd HH:mm:ss"), message));

        writer.Close();
      }
    } // LogMessage(string message)

    /// <summary>
    /// Logs backup message to the database
    /// </summary>
    /// <param name="type">Type of message</param>
    /// <param name="system">System</param>
    /// <param name="file">File</param>
    /// <param name="message">messafge</param>
    private void WriteAuditLog(Folder folder, string filePath )
    {
      try
      {

        string file = System.IO.Path.GetFileName(filePath);

        FileInfo info = new FileInfo(filePath);

        // Set up default values based on the parameters in the Application Setting section of the confi file
        string connectionString = System.Configuration.ConfigurationManager.AppSettings["connectionString"];

        using (SqlConnection connection = new SqlConnection(connectionString))
        {
          connection.Open();

          string sql = "INSERT INTO [AuditLog] ( type, server, system, source, started, completed, [file], message ) VALUES ( '" + 
                          folder.Type + "', '" + System.Environment.MachineName + "', '" + folder.System + "', '" + folder.Source + "', '" +
                          DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss") + "', '" + info.LastWriteTime.ToString("yyyy-MM-ddTHH:mm:ss") + "', '" +
                          file + "', '" + String.Format(folder.Body, filePath, DateTime.Now.ToString()) + "' )";
         
          SqlCommand command = new SqlCommand(sql, connection);

          if (command.ExecuteNonQuery() == 1)
          {
            LogMessage("Failed to insert backup message, 0 records updated");
          }

          
        }

        LogMessage("Backup message logged for " + file);

      }
      catch (Exception ex)
      {
        LogMessage("Failed to insert backup message : " + ex.Message);

      } //END ... Catch

    } // LogBackupMessage

    /// <summary>
    /// Send email to the the user listed in the to setting field of the config file
    /// </summary>
    /// <param name="folder">Folder</param>
    /// <param name="file">file</param>
    private void Email(Folder folder, string file)
    {

      // Set up default values based on the parameters in the Application Setting section of the confi file
      string to = System.Configuration.ConfigurationManager.AppSettings["to"];
      string server = System.Configuration.ConfigurationManager.AppSettings["server"];
      string portValue = System.Configuration.ConfigurationManager.AppSettings["port"];
      string from = System.Configuration.ConfigurationManager.AppSettings["from"];
      string user = System.Configuration.ConfigurationManager.AppSettings["user"];
      string password = System.Configuration.ConfigurationManager.AppSettings["password"];
      string ssl = System.Configuration.ConfigurationManager.AppSettings["ssl"];
      string subject = System.Configuration.ConfigurationManager.AppSettings["subject"];
      bool enableSSL = (ssl.Length > 0 ? Boolean.Parse(ssl) : false);
      int port = (portValue.Length == 0 ? 0 : int.Parse(portValue));

      string body = string.Empty;
      string attachments = string.Empty;


      try
      {
        //======================================
        // Make sure we have people to send to
        //=======================================
        if (to.Length == 0)
        {
          // Log
          return;
        } // END ... If there are no people to send to

        // Seperate list
        Char[] sep = { ',', ';',  '|' };

        // Split out the to field for multiple to
        string[] toList = to.Split(sep);

        // Mail message
        System.Net.Mail.MailMessage message = new System.Net.Mail.MailMessage();

        // Add all the To people to the list
        for ( int i = 0; i < toList.Length; ++i )
          message.To.Add(toList[i]);

        // Set the subect
        message.Subject = (folder.Subject.Length > 0 ? folder.Subject : "[CREATED] " + file);

        // Set the from 
        message.From = new MailAddress(from);

        // Set the boidy
        message.Body = String.Format(folder.Body,file,DateTime.Now.ToString());

        //======================================
        // Check if any attachments are needed
        //======================================
        if (attachments.Length > 0)
        {
          // Split out hte attachments since ther can be more then one
          string[] attachmentList = attachments.Split(sep);

          // Add all attachmenst to the mail message
          for (int i = 0; i < attachmentList.Length; ++i)
            message.Attachments.Add(new Attachment(attachmentList[i]));

        } // END ... If attachments are present


        // Create SMTP client using the server setting
        System.Net.Mail.SmtpClient smtp = new System.Net.Mail.SmtpClient(server);

        // Set Enable SSL setting
        smtp.EnableSsl = enableSSL;

        // Set the port
        smtp.Port = port;

        //====================================================================
        // Only set the credentials if a user name and password is specified.
        //====================================================================
        if (user.Length > 0 && password.Length > 0)
        {
          smtp.Credentials = new System.Net.NetworkCredential(user, password);
        } // END ... If the user and password are specified

        // Set the delivery method to network
        smtp.DeliveryMethod = System.Net.Mail.SmtpDeliveryMethod.Network;

        // Send the message
        smtp.Send(message);

        LogMessage("Email send to " + to + " for " + file);

      }
      catch (Exception ex)
      {
        // Dump error message
        LogMessage("Sending of email failed : " + ex.Message);

      } //END ... Catch

    }

    #endregion


    #region event invokers

    #region FileSystem Event Handlers

    /// <summary>
    /// Handles the created event from the FIleSystemWatcher 
    /// </summary>
    /// <param name="source">source object</param>
    /// <param name="e">event args</param>
    private void OnFileSystemCreated(object source, FileSystemEventArgs e)
    {

      string filename = System.IO.Path.GetFileName(e.FullPath);
      string path = System.IO.Path.GetDirectoryName(e.FullPath);
      Folder folder = m_folders[path.ToUpper()];

      LogMessage("Detected file creation [" + e.FullPath + "]");

      WriteAuditLog(folder, e.FullPath);

      Email(folder, e.Name);

    } // OnFileSystemCreated(object source, FileSystemEventArgs e)

    /// <summary>
    /// Handles the Changed event from the File System Watcher
    /// </summary>
    /// <param name="source">Source object</param>
    /// <param name="e">Event args</param>
    private void OnFileSystemChanged(object source, FileSystemEventArgs e)
    {
      // Do Nothing
    } // OnFileSystemChanged(object source, FileSystemEventArgs e)

    /// <summary>
    /// Handles the deleted event from the FileSystemWatcher
    /// </summary>
    /// <param name="source">file to delete</param>
    /// <param name="e">file args</param>
    private void OnFileSystemDeleted(object source, FileSystemEventArgs e)
    {
      LogMessage("Detected file deletion [" + e.FullPath + "]");
    } // OnFileSystemDeleted(object source, FileSystemEventArgs e)

    #endregion FileSystem Event Handlers


    #endregion  event handlers

    #region thread methods

    /// <summary>
    /// Abort
    /// </summary>
    public override void Abort()
    {
      System.Threading.Thread.Sleep(1000);

      base.Abort();
    }
    /// <summary>
    /// Runs the thread
    /// </summary>
    public override void Run()
    {
      LogMessage("Staring Folder Monitoring Server");

      foreach (KeyValuePair<string, Folder> folder in m_folders)
      {
        LogMessage("Monitoring " + folder.Value.Path);
      }

      //////////////////
      // Loop forever
      //////////////////
      while ( true )
      {
        foreach (IFileWatcherOperation op in m_operations)
        {
          op.Execute(m_folders);
        }

        // Sleeps a x number of hours split out over at a second a time.  If we do it this way we can handle the fs events.
        int sleepHours = Convert.ToInt32(ConfigurationManager.AppSettings["hoursBetweenOperations"]);

        int waitSeconds = sleepHours * 3600;
        for (int i = 0; i < waitSeconds; ++i)
        {
          System.Threading.Thread.Sleep(1000);
        }
      } // END ... While forever
    } // Run()


    #endregion
    

  } // class Server
}


