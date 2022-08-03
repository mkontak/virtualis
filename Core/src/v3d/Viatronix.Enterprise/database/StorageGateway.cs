// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontakn@viatronix.com )

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.Common;
using System.Xml;
using System.Threading;
using Viatronix.Dicom;
using System.Text.RegularExpressions;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Data
{

  public static class StorageGateway
  {

    #region fields

    /// <summary>
    /// security db name
    /// </summary>
    private const string DATABASE = "storage";

    /// <summary>
    /// xml data column size
    /// </summary>
    private const int XML_SIZE = 8000;

    #endregion



    #region methods

    /// <summary>
    /// Determines if we can connect to the database
    /// </summary>
    /// <param name="source">Source connectiion strings cource</param>
    /// <returns>True if available, false otherwise</returns>
    public static bool IsAvailable(string source = "default")
    {

      bool available = false;

      try
      {
        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE, source);


        DbConnection connection = db.CreateConnection();

        connection.Open();

        connection.Close();

        available = true;
      }
      catch (Exception ex)
      {
        Log.Critical("Storage is unavailable : " + ex.Message, "StorageGateway", "IsAvaiable");
      }

      return available;


    } // IsAvailable
 
    #region basic database methods

    #region GET

    /// <summary>
    /// Performs the appropriate GET in the security database
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="xmlBuilder">XML Builder</param>
    /// <returns>Results of the query</returns>
    public static string Get(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    {
      return DatabaseGateway.Get(DATABASE, context, resource, xmlBuilder.ToXml(), source);
    } // Get(string context, string resource, string query)



    /// <summary>
    /// Performs the appropriate GET in the security database
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="query">Query</param>
    /// <returns>Results of the query</returns>
    public static string Get(string context, string resource, string query, string source = "default")
    {
      return DatabaseGateway.Get(DATABASE, context, resource, query, source);
    } // Get(string context, string resource, string query)

    #endregion

    #region INSERT

    /// <summary>
    /// Performs the appropriate Update in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="xmlBuilder">Xml Builder</param>
    /// <returns></returns>
    public static void Insert(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    {
      Insert(context, resource, xmlBuilder.ToXml(), source);
    } // Insert(string context, string resource, IEntity entity)

    /// <summary>
    /// Performs the appropriate Update in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="entity"></param>
    /// <returns></returns>
    public static void Insert(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    {
      Insert(context, resource, entity.ToXml(), source);
    } // Insert(string context, string resource, IEntity entity)

    /// <summary>
    /// Performs the appropriate INSERT in the security database
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="query">Query</param>
    /// <returns>Results of the query</returns>
    public static void Insert(string context, string resource, string record, string source = "default")
    {
      DatabaseGateway.Insert(DATABASE, context, resource, record, source);
    } // Insert(striung context, string resource, string query)

    #endregion

    #region UPDATE

    /// <summary>
    /// Performs the appropriate Update in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="xmlBuilder">Xml Builder</param>
    /// <returns></returns>
    public static string Update(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    {
      return Update(context, resource, xmlBuilder.ToXml(), source);
    } // Update(string context, string resource, IEntity entity)

    /// <summary>
    /// Performs the appropriate Update in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="entity"></param>
    /// <returns></returns>
    public static string Update(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    {
      return Update(context, resource, entity.ToXml(), source);
    } // Update(string context, string resource, IEntity entity)

    /// <summary>
    /// Performs the appropriate Update in the security database
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="record">Record xml</param>
    /// <returns>Results of the query</returns>
    public static string Update(string context, string resource, string record, string source = "default")
    {
      return DatabaseGateway.Update(DATABASE, context, resource, record, source);
    } // Update(string context, string resource, string query)

    #endregion

    #region DELETE

    /// <summary>
    /// Performs the appropriate Delete in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="xmlBuilder">Xml Builder</param>
    /// <returns></returns>
    public static void Delete(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    {
      Delete(context, resource, xmlBuilder.ToXml(), source);
    } // Delete(string context, string resource, IEntity entity)

    /// <summary>
    /// Performs the appropriate Delete in the security database for the entity
    /// </summary>
    /// <param name="context">Context Id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="entity"></param>
    /// <returns></returns>
    public static void Delete(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    {
      Delete(context, resource, entity.ToXml(), source);
    } // Delete(string context, string resource, IEntity entity)

    /// <summary>
    /// Handles the update of a record for the specified resource.  Executes the appropriate get stored procedure in the database using the resource name. If 
    /// the store procedure does not exists a UnsupportedException is thrown.
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="resource">Resource name</param>
    /// <param name="record">Record to delete</param>
    /// <returns></returns>
    public static void Delete(string context, string resource, string record, string source = "default")
    {
      DatabaseGateway.Delete(DATABASE, context, resource, record, source);
    } // Delete(string context, string resource, string record)

    #endregion

    #endregion


    #region specialized database methods

    #region Work Queue Service Methods

    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public static List<Viatronix.Enterprise.Entities.Job> GetPendingJobs(string context)
    {

      List<Viatronix.Enterprise.Entities.Job> jobs = new List<Viatronix.Enterprise.Entities.Job>();

      XmlBuilder builder = new XmlBuilder("job");

      builder.AddAttribute("state", "Pending|Receiving");

      string xml = Get(context, "job", builder.ToXml());


      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("job"))
      {
        jobs.Add(Viatronix.Enterprise.Entities.Job.Create(node));
      }
      return jobs;


    } // GetPendingJobs(string context)

    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public static Viatronix.Enterprise.Entities.Job GetNextJob(string context, string system)
    {

      string xml = string.Empty;

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_getNextJob]");

        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID
        db.AddInParameter(command, "SYS", DbType.String, system);          //  System
        db.AddOutParameter(command, "RESULTS", DbType.Xml, XML_SIZE);       // RESULTS Xml returned

        // Execute
        db.ExecuteStoredProcedure(command);

        // Get the results
        xml = Convert.ToString(db.GetParameterValue(command, "RESULTS"));


      }


      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      // No Attributes means no job so we return null
      return (doc.DocumentElement.Attributes.Count == 0 ? null : Viatronix.Enterprise.Entities.Job.Create(doc.DocumentElement));

    } // GetNextJob(string context)


    /// <summary>
    /// Resets the JOBS/TASKS back to pending
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void ResetJobs(string context)
    {

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_resetJobs]");
        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID

        // Execute
        db.ExecuteStoredProcedure(command);

      }

    } // ResetJobs( string context ) 

    /// <summary>
    /// Starts the job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to start</param>
    public static void StartJob(string context, Viatronix.Enterprise.Entities.Job job)
    {

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_startJob]");
        db.AddInParameter(command, "JOBID", DbType.String, job.Id);           // Job Id

        // Execute
        db.ExecuteStoredProcedure(command);

      }

    } // StartJob( string context ) 

    /// <summary>
    /// Completes the job
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void CompleteJob(string context, Viatronix.Enterprise.Entities.Job job)
    {

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_completeJob]");
        db.AddInParameter(command, "JOBID", DbType.String, job.Id);           // Job Id

        // Execute
        db.ExecuteStoredProcedure(command);

      }

    } // CompleteJob(string context, Task task) 

    /// <summary>
    /// Fails the job
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void FailJob(string context, Viatronix.Enterprise.Entities.Job job, string message = "")
    {

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_failJob]");
        db.AddInParameter(command, "JOBID", DbType.String, job.Id);           // Job Id
        db.AddInParameter(command, "MESSAGE", DbType.String, message);        // Message

        // Execute
        db.ExecuteStoredProcedure(command);

      }

    } // FailJob(string context, Task task) 

    /// <summary>
    /// Aborts the task
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void AbortJob(string context, Viatronix.Enterprise.Entities.Job job, string message = "")
    {

      try
      {
        job.State = States.Aborted;
        job.Message = message;

        Update(context, "job", job.ToXml());
      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed to abort job {0}: {1}", job.Id, ex.Message), "StorageGateway", "AbortJob");
        throw;
      }

    } // AbortJob(string context, Task task) 


    /// <summary>
    /// Starts the task
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void UpdateJob(string context, Viatronix.Enterprise.Entities.Job job)
    {
      try
      {
        Update(context, "job", job.ToXml());
      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed to update job {0}: {1}", job.Id, ex.Message), "StorageGateway", "UpdateJob");
        throw;
      }

    } // UpdateJob(string context, Task task)

    /// <summary>
    /// Creates the job using the entity objects
    /// </summary>
    /// <param name="context">Authentication token</param>
    /// <param name="series">Source series</param>
    /// <param name="template">Processing template</param>
    public static void CreateJob(string context, Viatronix.Enterprise.Entities.Series series, Viatronix.Enterprise.Entities.Template template, bool paused = false)
    {
      CreateJob(context, series.Uid, template.Id, template.Application, paused);
    } // UpdateJob(string context, Task task)

    /// <summary>
    /// Creates a new job to be processed
    /// </summary>
    /// <param name="context">Authentication token</param>
    /// <param name="uid">source series uid</param>
    /// <param name="tid">template id</param>
    /// <param name="app">application id</param>
    /// <param name="paused">pause state flag</param>
    public static void CreateJob(string context, string uid, string tid, string app, bool paused = false )
    {
      try
      {
        // Create the database
        Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

        using (DbConnection connection = db.CreateConnection())
        {

          DbCommand command = db.GetStoredProcCommand("[v3d].[sp_createJob]");
          db.AddInParameter(command, "ID", DbType.String, context);                // Context ID

          if ( ! string.IsNullOrEmpty(tid) )
            db.AddInParameter(command, "TID", DbType.String, tid);                // Template Id

          if (!string.IsNullOrEmpty(app))
            db.AddInParameter(command, "APP", DbType.String, app);                // Application Code

          db.AddInParameter(command, "UID", DbType.String, uid);                  // Series Uid
          db.AddInParameter(command, "PAUSED_STATE", DbType.Boolean, paused);     // Flag indicating that the job is to be in a paused state



          // Execute
          db.ExecuteStoredProcedure(command);

        }
      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed to create job  for series {0} and template {1}: {2}", uid, tid, ex.Message), "StorageGateway", "CreateJob");
        throw;
      }

    } // UpdateJob(string context, Task task)

    #endregion


    #region Session

    /// <summary>
    /// Coses the specified session 
    /// </summary>
    /// <param name="context">autrhentication token</param>
    /// <param name="uid">session uid</param>
    public static void CloseSession(string context, string uid)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using( DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_closeSession]");
        db.AddInParameter( command, "ID", DbType.String, context );
        db.AddInParameter( command, "UID", DbType.String, uid );

        db.ExecuteStoredProcedure( command );
      }
    }

    /// <summary>
    /// Completes the sepcified session
    /// </summary>
    /// <param name="context">autrhentication token</param>
    /// <param name="uid">session uid</param>
    public static void CompleteSession(string context, string uid)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using( DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_completeSession]");
        db.AddInParameter( command, "ID", DbType.String, context );
        db.AddInParameter( command, "UID", DbType.String, uid );

        db.ExecuteStoredProcedure( command );
      }

    }


    #endregion

    #region Pacs Queue Service Methods


    /// <summary>
    /// Gets the pending jobs
    /// </summary>
    /// <param name="context"></param>
    /// <returns></returns>
    public static Viatronix.Enterprise.Entities.PacsJob GetPacsNextJob(string context, string system)
    {
      string xml = string.Empty;

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_getNextPacsJob]");

        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID
        db.AddOutParameter(command, "RESULTS", DbType.Xml, XML_SIZE);       // RESULTS Xml returned

        // Execute
        db.ExecuteStoredProcedure(command);

        // Get the results
        xml = Convert.ToString(db.GetParameterValue(command, "RESULTS"));


      }


      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      // No Attributes means no job so we return null
      return (doc.DocumentElement.Attributes.Count == 0 ? null : Viatronix.Enterprise.Entities.PacsJob.Create(doc.DocumentElement));

    } // GetNextJob(string context)


    /// <summary>
    /// Resets the JOBS/TASKS back to pending
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void ResetPacsJobs(string context)
    {

        // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_resetPacsJobs]");
        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID

        // Execute
        db.ExecuteStoredProcedure(command);

      }

    } // ResetJobs( string context ) 

    /// <summary>
    /// Starts the job
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="job">Job to start</param>
    public static void StartJob(string context, ref Viatronix.Enterprise.Entities.PacsJob job)
    {


      job.State = Viatronix.Enterprise.Entities.PacsJob.States.Processing;

      string xml = Update(context, "pacsjob", job.ToXml());

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      job = Viatronix.Enterprise.Entities.PacsJob.Create(doc.DocumentElement);

    } // StartJob( string context ) 

    /// <summary>
    /// Completes the task
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void CompleteJob(string context, ref Viatronix.Enterprise.Entities.PacsJob job)
    {

      job.State = Viatronix.Enterprise.Entities.PacsJob.States.Completed;

      string xml = Update(context, "pacsjob", job.ToXml());

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      job = Viatronix.Enterprise.Entities.PacsJob.Create(doc.DocumentElement);

    } // CompletePacsJob(string context, Task task) 

    /// <summary>
    /// Completes the task
    /// </summary>
    /// <param name="context">Auth token</param>
    public static void FailJob(string context, ref Viatronix.Enterprise.Entities.PacsJob job, string message)
    {

      job.State = Viatronix.Enterprise.Entities.PacsJob.States.Failed;
      job.Message = message;

      string xml = Update(context, "pacsjob", job.ToXml());

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      job = Viatronix.Enterprise.Entities.PacsJob.Create(doc.DocumentElement);

    } // FailPacsJob(string context, Task task) 


    #endregion


    #region Dicom Service Methods

    #region Association methods

    /// <summary>
    /// Performs the operations necessary when an association completes
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="association">Association Id</param>
    public static void AssociationCompleted(string context, string association)
    {
      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_associationCompleted]");


        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID
        db.AddInParameter(command, "ASSOCIATION", DbType.String, association);

        // Execute
        db.ExecuteStoredProcedure(command);


      }

    } // AssociationCompleted(string context, string association)

    /// <summary>
    /// Performs the operations necessary when an association completes
    /// </summary>
    /// <param name="context">Auth token</param>
    /// <param name="association">Association Id</param>
    public static void AssociationFailed(string context, string association, string message)
    {
      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_associationFailed]");


        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID
        db.AddInParameter(command, "ASSOCIATION", DbType.String, association);
        db.AddInParameter(command, "MESSAGE", DbType.String, message);

        // Execute
        db.ExecuteStoredProcedure(command);


      }

    } // AssociationCompleted(string context, string association)

    #endregion


    #region Receiving methods

    /// <summary>
    /// 
    /// </summary>
    /// <param name="context"></param>
    /// <param name="record"></param>
    /// <returns></returns>
    public static string ReceiveImage(string context, string record )
    {
      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_receiveImage]");

        db.AddInParameter(command, "ID", DbType.String, context);       // Context ID
        db.AddInParameter(command, "RECORD", DbType.Xml, record);      // DICOM Header record
        db.AddOutParameter(command, "PATHS", DbType.Xml, 4000);        // Paths returns ()

        // Execute
        db.ExecuteStoredProcedure(command);

        return db.GetParameterValue(command, "PATHS").ToString();
      }
    } // ReceiveImage(string resource, string query)

    /// <summary>
    /// Deletes the receiving
    /// </summary>
    /// <param name="context"></param>
    /// <param name="server"></param>
    /// <param name="aeTitle"></param>
    public static void DeleteReceivers(string context, string server = "", string aeTitle = "")
    {
      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_deleteReceiver]");


        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID

        if (!string.IsNullOrEmpty(aeTitle))
          db.AddInParameter(command, "AETITLE", DbType.String, aeTitle);

        if (!string.IsNullOrEmpty(server))
          db.AddInParameter(command, "SERVER", DbType.String, server);

        // Execute
        db.ExecuteStoredProcedure(command);


      }

    }


 
    #endregion


    /// <summary>
    /// Deletes the receiving
    /// </summary>
    /// <param name="context"></param>
    /// <param name="server"></param>
    /// <param name="aeTitle"></param>
    public static void DeleteDicomServer(string context, string server = "", string aeTitle = "")
    {
      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_deleteDicomServer]");

        db.AddInParameter(command, "ID", DbType.String, context);           // Context ID

        if (!string.IsNullOrEmpty(aeTitle))
          db.AddInParameter(command, "AETITLE", DbType.String, aeTitle);

        if (!string.IsNullOrEmpty(server))
          db.AddInParameter(command, "SERVER", DbType.String, server);

        // Execute
        db.ExecuteStoredProcedure(command);


      }
 

    }


    /// <summary>
    /// Adds a dicom server entry to the database
    /// </summary>
    /// <param name="context">Authorization token</param>
    /// <param name="server">Dicom server name</param>
    /// <param name="aeTitle">Dicom server's AE Title</param>
    public static void AddDicomServer(string context, string server, string aeTitle, int port)
    {

      XmlBuilder builder = new XmlBuilder("dicomserver");
      builder.AddAttribute("aetitle", aeTitle);
      builder.AddAttribute("server", server);
      builder.AddAttribute("port", port.ToString());

      Insert(context, "dicomserver", builder.ToXml());

 
    }

    #endregion


    #region File System Resource Provider Methods

    /// <summary>
    /// Handles the update of a record for the specified resource.  Executes the appropriate get stored procedure in the database using the resource name. If 
    /// the store procedure does not exists a UnsupportedException is thrown.
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="uid">Series uid</param>
    /// <param name="location">local or remote</param>
    /// <returns></returns>
    public static string GetSeriesPath(string context, string uid, string location)
    {

      // Create the database
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetSqlStringCommand(string.Format("SELECT [v3d].[fn_getSeriesPath]('{0}','{1}')", location, uid));

        // Execute
        string path = (string)db.ExecuteScalar(command);

        return path;

      }
 

    } // GetSeriesPath(string context, string uid, string location)

    #endregion


    #endregion

    #endregion

  } // class StorageGateway
} // namespace Viatronix.Enterprise.Data
