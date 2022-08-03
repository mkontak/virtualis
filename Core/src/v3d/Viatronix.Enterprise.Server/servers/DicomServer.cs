using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Threading.Tasks;
using Viatronix.Dicom;
using Viatronix.Enterprise.Data;

namespace Viatronix.Enterprise.Servers
{
  /// <summary>
  /// Implements a Enterprise server form of the DICOM SERVER
  /// </summary>
  public class DicomServer : Server
  {

    #region constants



    /// <summary>
    /// Machine Name
    /// </summary>
    private static string m_machineName = System.Environment.MachineName;

    #endregion

    #region fields

    /// <summary>
    ///  Dicom Server (C++ warpped)
    /// </summary>
    Dicom.Server m_server = null;


    /// <summary>
    /// Dicom Service Log Source
    /// </summary>
    private string m_logSource = "DicomService";

    /// <summary>
    /// Flag indicating that we are storing to the database
    /// </summary>
    private bool m_databaseAccess = true;

    /// <summary>
    /// Flag indicating that we are storing thumbnails
    /// </summary>
    private bool m_saveThumbnails = true;

    #endregion


    #region constructors and destructor's

    /// <summary>
    /// Constructor
    /// </summary>
    private DicomServer() : base("Dicom") { }
 

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="settings"></param>
    public DicomServer(string settings) : base("Dicom", settings) { }


    #endregion

    #region properties

    /// <summary>
    /// Gets the servers AE Title
    /// </summary>
    public string AETitle
    { get { return m_server.AETitle; } }

    /// <summary>
    /// Gets the servers port
    /// </summary>
    public uint Port
    { get { return m_server.Port; } }

    /// <summary>
    /// Gets/Sets the enable database access flag
    /// </summary>
    public bool EnableDatabaseAccess
    {
      get { return m_databaseAccess; }
      set { m_databaseAccess = value; }
    }

    /// <summary>
    /// Gets/Sets the save thumbnails flag
    /// </summary>
    public bool EnableSavingThumbnails
    {
      get { return m_saveThumbnails; }
      set { m_saveThumbnails = value; }
    }


    #endregion


    #region overrides

    /// <summary>
    /// Initializes the server the xml node supplied
    /// </summary>
    /// <param name="node">Xml initialization node</param>
    /// <returns>WorkQueueServer object</returns>
    protected override void Initialize(XmlNode node)
    {

      try
      {

        base.Initialize(node);

        // Get the log source
        node.TryGetAttribute<string>("logSource", ref m_logSource);


        /// Creates the server using the XML settings from the config
        m_server = Dicom.Server.Create(node);

        // Hook up the event handlers
        m_server.Started += new ServerEventHandler(OnServerStarted);
        m_server.ImageReceived += new ServerEventHandler(OnImageReceived);
        m_server.QueryReceived += new ServerEventHandler(OnQueryReceived);
        m_server.MoveReceived += new ServerEventHandler(OnMoveReceived);
        m_server.AssociationCompleted += new ServerEventHandler(OnAssociationCompleted);
        m_server.AssociationFailed += new ServerEventHandler(OnAssociationFailed);
        m_server.FileSaved += new ServerEventHandler(OnFileSaved); ;
        m_server.Failed += new ServerEventHandler(OnServerFailed);

 

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(m_logSource, string.Format("Invalid configuration : {0}", ex.Message), "DicomServer", "Create");
        throw;
      }


    } // Initialize(XmlNode node)

    /// <summary>
    /// Aborts the dicom server
    /// </summary>
    public override void Abort(Dictionary<string, string> parameters = null)
    {


      base.Abort();
      m_server.Stop();

 

    } // Abort()


    /// <summary>
    /// Executes the WorkQueue 
    /// </summary>
    public override void RunSynchronously()
    {



      try
      {


        // Started
        OnStarted();

        /// Cleanup receiving entries
        CleanupReceiving();

        /// Cleanup and server entries
        CleanupServer();


        m_server.Start();

        m_server.Wait();



      }
      catch (AbortException)
      {

        OnAborted();

        Viatronix.Logging.Log.Error(m_logSource, "Dicom Server is being aborted", "DicomServer", "Run");

        throw;

      }
      catch (Exception ex)
      {


        Viatronix.Logging.Log.Error(m_logSource, "Dicom Server encountered exception : " + ex.Message, "DicomServer", "RunSynchronously");


        OnFailed(ex);

        throw;

      }

    } // RunSynchronously()


    #endregion

    #region methods

    /// <summary>
    /// Creates the server using the XML node supplied
    /// </summary>
    /// <param name="node">XML initialization node</param>
    /// <returns>WorkQueueServer object</returns>
    public static DicomServer Create(XmlNode node)
    {
      DicomServer server = new DicomServer();

      if (server == null)
        throw new System.OutOfMemoryException("Failed to allocate DicomServer");

      server.Initialize(node);

      return server;




    } // Create(XmlNode node)


    /// <summary>
    /// Cleanup and receivers entries associated with this server
    /// <param name="aeTitle">AE TItle to cleanup</param>
    /// </summary>
    private void CleanupReceiving()
    {
      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        Viatronix.Logging.Log.Debug(m_logSource, "Cleaning up any receiving entries for " + m_machineName, "DicomServer", "CleanupReceiving");

        try
        {

          StorageGateway.DeleteReceivers(m_context, m_machineName, m_server.AETitle);

        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Error(m_logSource, "Failed to cleanup receiving entries for " + m_machineName + " : " + ex.Message, "DicomServer", "CleanpReceiving");

          throw;
        }

      } // END ... Make sure that we are storing to the database

    }

    /// <summary>
    /// Clean up the server in the database
    /// </summary>
    /// <param name="server">server name</param>
    /// <param name="aeTitle">AE Title of the server</param>
    void CleanupServer()
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        Viatronix.Logging.Log.Debug(m_logSource, string.Format("Cleaning up the servers (server:{0}) (aetitle:{1})", m_machineName, m_server.AETitle), "DicomServer", "CleanupServer");


        try
        {

          StorageGateway.DeleteDicomServer(m_context, m_machineName, m_server.AETitle);

        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Error(m_logSource, "Failed to cleanup servers : " + ex.Message, "DicomServer", "CleanupServer");

          throw;
        }

      } // END ... Make sure that we are stpring to the database

    } // CleanupServer(string aeTitle)


 
    #endregion

    #region event handlers


    /// <summary>
    /// Handles the receiver completed event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args">Receiver arguments</param>
    void OnAssociationCompleted(object sender, ServerEventArgs args)
    {


      string association = args.Id;

      Viatronix.Logging.Log.Debug(m_logSource, "Association completed [" + association + "]",
                                  "DicomServer",
                                  "OnAssociationCompleted");


      try
      {

        StorageGateway.AssociationCompleted(m_context, association);

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(m_logSource,
                                    "Failed to cleanup receiver [" + args.Source + "] in database  : " + ex.Message,
                                    "DicomServer", "OnAssociationCompleted");

        throw;
      }


    } // OnAssociationCompleted(object sender, ServerEventArgs args)


    /// <summary>
    /// Handles the receiver failed event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Receiver arguments</param>
    void OnAssociationFailed(object sender, ServerEventArgs args)
    {

      string association = args.Id;

      Viatronix.Logging.Log.Debug(m_logSource,
                                  "Association failed [" + association + "]",
                                   "DicomServer", "OnAssociationFailed");


      try
      {

        StorageGateway.AssociationFailed(m_context, association, args.Message);

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(m_logSource,
                                    "Failed to cleanup receiver [" + args.Source + "] in database  : " + ex.Message,
                                    "DicomServer", "OnAssociationFailed");

        throw;
      }


    } //OnReceiverFailed(object sender, ServerEventArgs args)

    /// <summary>
    /// Handles the server started event. Will call the sp_serverStarted stored procedure in the database. This will 
    /// perform differing operation based on the version of the software/database runninng.
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Receiver arguments</param>
    void OnServerStarted(object sender, ServerEventArgs args)
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        // Log information record
        Viatronix.Logging.Log.Information(m_logSource, string.Format("Server started (aetitle:{0}) (server:{1}) (port:{2})", args.CalledAETitle, m_machineName, args.Port), "DicomServer", "OnServerStarted");

        try
        {
          StorageGateway.AddDicomServer(m_context, m_machineName, args.CalledAETitle, args.Port);
        }
        catch (Exception ex)
        {

          Viatronix.Logging.Log.Error(m_logSource, "Failed to add dicom server to database : " + ex.Message, "DicomServer", "OnServerStarted");

          throw;
        }

      } // END ... Make sure that we are string to the database

    } // OnServerStarted(object sender, ServerEventArgs args)

    /// <summary>
    /// Handles the server failed event. 
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Receiver arguments</param>
    void OnServerFailed(object sender, ServerEventArgs args)
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        // Log information record
        Viatronix.Logging.Log.Error(m_logSource, string.Format("Server failed (aetitle:{0}) (server:{1}) (port:{2})", args.CalledAETitle, m_machineName, args.Port), "DicomServer", "OnServerFailed");

        try
        {
          StorageGateway.DeleteDicomServer(m_context, m_machineName, args.CalledAETitle);
        }
        catch (Exception ex)
        {

          Viatronix.Logging.Log.Error(m_logSource, "Failed to delete dicom server to database : " + ex.Message, "DicomServer", "OnServerFailed");
        }




      } // END ... Make sure that we are string to the database

      /// Get the message
      OnFailed(new ViatronixException(args.Message, ErrorCodes.None));

 

    } // OnServerStarted(object sender, ServerEventArgs args)


    /// <summary>
    /// Handles the image received event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Receiver arguments</param>
    void OnImageReceived(object sender, ServerEventArgs args)
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        Viatronix.Logging.Log.Debug(m_logSource,
                                    "Receiving image Source = " + args.CallingAETitle + "   Receiver = " + args.Source,
                                    "DicomServer", "OnImageReceived");


        try
        {
          /// Insert the image
          string results = StorageGateway.ReceiveImage(m_context, args.ToXml());


          XmlDocument doc = new XmlDocument();
          doc.LoadXml(results);

          args.FilePath = doc.DocumentElement.Attributes["dicom"].Value;
          args.ThumbnailPath = doc.DocumentElement.Attributes["thumbnail"].Value;

        }
        catch (Exception ex)
        {

          Viatronix.Logging.Log.Error(m_logSource, "Failed to insert image to database : " + ex.Message, "DicomServer", "OnImageReceived");

          throw;
        }

      } // END ... Make sure that we are storing to the database

    } // OnImageReceived(object sender, ServerEventArgs args)


    /// <summary>
    /// Abort Thumbnail handler
    /// </summary>
    /// <returns></returns>
    private bool OnGetThumbnailImageAbort()
    {

      return false;
    }

    /// <summary>
    /// Handles the file saved event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Receiver arguments</param>
    void OnFileSaved(object sender, ServerEventArgs args)
    {

      // ==========================================================================
      // If the file path and thumbnail contain data then write the thumbnail out
      // ==========================================================================
      if (m_saveThumbnails && !string.IsNullOrEmpty(args.ThumbnailPath))
      {

        Viatronix.Logging.Log.Debug(m_logSource, "Saving thumbnail image : " + args.ThumbnailPath, "DicomServer", "OnFileSaved");


        try
        {

          /// Read the DICOM file just written
          Viatronix.Dicom.IImage dataset = Viatronix.Dicom.ImageFactory.Create(args.FilePath, ParameterType.FILE);

          /// Create the bitmap from the dicom file
          System.Drawing.Image thumbnail = Viatronix.Dicom.Imaging.Renderer.RenderThumbnail(dataset, 256, 256);

          /// Save the bitmap to the thumbnail
          thumbnail.Save(args.ThumbnailPath);

        }
        catch (Exception ex)
        {

          Viatronix.Logging.Log.Error(m_logSource, "Failed to save thumbnail : " + ex.Message, "DicomServer", "OnFileSaved");

          throw;
        }

      } // END ... If the thumbnail and file path have data


    } // OnImageReceived(object sender, ServerEventArgs args)


    /// <summary>
    /// Handles the image received event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Server arguments</param>
    void OnQueryReceived(object sender, ServerEventArgs args)
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        DateTime start = DateTime.Now;


        Viatronix.Logging.Log.Debug(m_logSource,
                                    "Receiving query Source = " + args.CallingAETitle + "   Receiver = " + args.Source,
                                    "DicomServer", "OnQueryReceived");


        try
        {
          using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(m_connectionStrings["storage"]))
          {

            connection.Open();



            System.Data.SqlClient.SqlCommand command;


            if (args.QueryParameters.Level == "PATIENT")
            {
              command = new System.Data.SqlClient.SqlCommand("[v3d].[sp_queryPatients]", connection);
              command.CommandType = CommandType.StoredProcedure;


              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("QUERY", args.QueryParameters.ToXml()));
              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("RESULTS", SqlDbType.Xml, 4000));

              command.Parameters["RESULTS"].Direction = ParameterDirection.Output;


            }
            else if (args.QueryParameters.Level == "STUDY")
            {
              command = new System.Data.SqlClient.SqlCommand("[v3d].[sp_queryStudies]", connection);
              command.CommandType = CommandType.StoredProcedure;

              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("QUERY", args.QueryParameters.ToXml()));
              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("RESULTS", SqlDbType.Xml, 4000));

              command.Parameters["RESULTS"].Direction = ParameterDirection.Output;

            }
            else if (args.QueryParameters.Level == "SERIES")
            {
              command = new System.Data.SqlClient.SqlCommand("[v3d].[sp_querySeries]", connection);
              command.CommandType = CommandType.StoredProcedure;

              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("QUERY", args.QueryParameters.ToXml()));
              command.Parameters.Add(new System.Data.SqlClient.SqlParameter("RESULTS", SqlDbType.Xml, 4000));

              command.Parameters["RESULTS"].Direction = ParameterDirection.Output;


            }
            else if (args.QueryParameters.Level == "IMAGE")
            {
              return;
            }

            else
              return;


            command.ExecuteNonQuery();

            args.Results = command.Parameters["RESULTS"].Value.ToString();
          }
        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Error(m_logSource, "Failed to add image to database : " + ex.Message, "DicomService", "OnQueryReceived");

          throw;
        }
        finally
        {
          Viatronix.Logging.Log.Timing(m_logSource,
                                            "Query took " + DateTime.Now.Subtract(start).Milliseconds.ToString() + " milliseconds",
                                            "DicomServer", "OnQueryReceived");
        }

      } // END ... Make sure that we are stpring to the database

    } // OnImageReceived(object sender, ServerEventArgs args)


    /// <summary>
    /// Handles the image received event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="args">Server arguments</param>
    void OnMoveReceived(object sender, ServerEventArgs args)
    {

      // ===========================================
      // Only store to the database if we have to
      // ==========================================
      if (m_databaseAccess)
      {

        DateTime start = DateTime.Now;


        Viatronix.Logging.Log.Debug(m_logSource,
                                    "Receiving move Source = " + args.CallingAETitle + "   Receiver = " + args.Source,
                                    "DicomServer", "OnMoveReceived");


        try
        {
          using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(m_connectionStrings["storage"]))
          {

            connection.Open();


            string xml = args.QueryParameters.ToXml();

            System.Data.SqlClient.SqlCommand command;


            command = new System.Data.SqlClient.SqlCommand("[v3d].[sp_getDirectories]", connection);
            command.CommandType = CommandType.StoredProcedure;

            command.Parameters.Add(new System.Data.SqlClient.SqlParameter("QUERY", args.QueryParameters.ToXml()));
            command.Parameters.Add(new System.Data.SqlClient.SqlParameter("RESULTS", SqlDbType.Xml, 4000));

            command.Parameters["RESULTS"].Direction = ParameterDirection.Output;

            command.ExecuteNonQuery();

            args.Results = command.Parameters["RESULTS"].Value.ToString();
          }
        }
        catch (Exception ex)
        {
          Viatronix.Logging.Log.Error(m_logSource,
                                      "Failed to add image to database : " + ex.Message,
                                      "DicomServer", "OnMoveReceived");

          throw;
        }
        finally
        {
          Viatronix.Logging.Log.Timing(m_logSource,
                                            "Query took " + DateTime.Now.Subtract(start).Milliseconds.ToString() + " milliseconds",
                                            "DicomServer", "OnMoveReceived");
        }

      } // END ... Make sure that we are storing to the database

    } // OnMoveReceived(object sender, ServerEventArgs args)



    #endregion
  }
}
