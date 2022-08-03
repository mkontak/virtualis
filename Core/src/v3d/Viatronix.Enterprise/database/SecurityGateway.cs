// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: SecurityGateway.cs
//
// Description: 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)



using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.Common;
using System.Xml;
using System.Threading;
using System.Text.RegularExpressions;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Viatronix.Enterprise.Logging;
using Viatronix;

namespace Viatronix.Enterprise.Data
{


  /// <summary>
  /// Gateway to the Security database
  ///   User
  ///   Role
  ///   License
  ///   Context
  /// </summary>
  public static class SecurityGateway
  {

    #region fields

    /// <summary>
    /// security db name
    /// </summary>
    private const string DATABASE = "security";

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
      if (resource == "user")
      {
        Viatronix.Enterprise.Entities.User user = Viatronix.Enterprise.Entities.User.Create(record);
        DeleteUser(context, user.Login, source);
      }
      else
        DatabaseGateway.Delete(DATABASE, context, resource, record, source);
    } // Delete(string context, string resource, string record)

    #endregion

    #endregion


    #region specialized methods

    #region User methods

    /// <summary>
    /// Deletes the specified user from the database
    /// </summary>
    /// <param name="login"></param>
    public static void DeleteUser(string context, string login, string source = "default")
    {
      try
      {
        Database db = DatabaseFactory.CreateDatabase(DATABASE, source);
        using (DbConnection connection = db.CreateConnection())
        {
          DbCommand command = db.GetStoredProcCommand("[v3d].[sp_deleteUser]");
          db.AddInParameter(command, "ID", DbType.String, context);
          db.AddInParameter(command, "LOGIN", DbType.String, login);

          db.ExecuteStoredProcedure(command);
        }
      }
      catch (DbException ex)
      {
        throw ExceptionFactory.Create(ex);
      }
      catch (Exception ex)
      {
        throw ExceptionFactory.Create(ex);
      }
    } // DeleteUser( string context, string login )

 


    #endregion


    #region Context methods

 
    /// <summary>
    /// Acquires a context
    /// </summary>
    /// <param name="context">context id</param>
    /// <param name="login">user login</param>
    /// <param name="pwd">user password</param>
    /// <param name="acquire">true if acquire; false to release </param>
    /// <returns>context id</returns>
    public static string AcquireContext(string context, string login, string pwd)
    {
      Database db = DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_acquireContext]");
        db.AddInOutParameter(command, "CONTEXT_ID", DbType.String, 256, context);
        db.AddInParameter(command, "LOGIN", DbType.String, login);
        db.AddInParameter(command, "PASSWORD", DbType.String, pwd);

        db.ExecuteStoredProcedure(command);
        return Convert.ToString(db.GetParameterValue(command, "CONTEXT_ID"));
      }
     } // AcquireContext( name, pwd, id, acquire )


    /// <summary>
    /// Acquires a context
    /// </summary>
    /// <param name="name">user name</param>
    /// <param name="pwd">user password</param>
    /// <param name="id">context id</param>
    /// <param name="acquire">true if acquire; false to release </param>
    /// <returns>context id</returns>
    public static void RenewContext( string id )
    {
      Database db = DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_renewContext]");
        db.AddInParameter(command, "ID", DbType.String, id);

        db.ExecuteStoredProcedure(command);
      }
    } // RenewContext( id )


    /// <summary>
    /// Release the context
    /// </summary>
    /// <param name="context">context id</param>
    /// <param name="login">User name to release context</param>
    /// <returns>context id</returns>
    public static void ReleaseContext( string context, string login = "" )
    {
      Database db = DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_releaseContext]");

        db.AddInParameter(command, "ID", DbType.String, context);

        if (login.Length > 0)
          db.AddInParameter(command, "LOGIN", DbType.String, login);


        db.ExecuteStoredProcedure(command);
      }
     } // ReleaseContext( string id, string login = "" )


    public static void Authenticate(string context, string login, string pwd)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_isAccessAuthorized]");
        db.AddInParameter(command, "ID", DbType.String, context);
        db.AddInParameter(command, "LOGIN", DbType.String, login);
        db.AddInParameter(command, "PASSWORD", DbType.String, pwd);

        db.ExecuteStoredProcedure(command);
      }
    }



    #endregion


    #region Config

    /// <summary>
    /// Gets the user profile
    /// </summary>
    /// <param name="login"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static string GetUserProfile(string context, string login, string name)
    {

      Database db = DatabaseFactory.CreateDatabase(DATABASE);

      using (DbConnection connection = db.CreateConnection())
      {

        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_getUsersConfig]");
        db.AddInParameter(command, "ID", DbType.String, context);
        db.AddInParameter(command, "LOGIN", DbType.String, login);
        db.AddInParameter(command, "NAME", DbType.String, name);
        db.AddOutParameter(command, "RESULTS", DbType.Xml, XML_SIZE);

        db.ExecuteStoredProcedure(command);
        return Convert.ToString(db.GetParameterValue(command, "RESULTS"));
      }
    } // GetConfig( name, type, xml )


    /// <summary>
    /// Sets the configuration
    /// </summary>
    /// <param name="name">configuration name</param>
    /// <param name="xml">configuration xml</param>
    public static void SetConfig(string name, string type, string xml)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_setConfig]");
        db.AddInParameter(command, "NAME", DbType.String, name);
        db.AddInParameter(command, "TYPE", DbType.String, type);
        db.AddInParameter(command, "CONFIG", DbType.Xml, xml);

        db.ExecuteNonQuery(command);
      }
    } // SetConfig( name, type, xml )

    #endregion


    #region Profiles

    public static string GetProfile(string user)
    {
      Database db = DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_getProfile]");
        db.AddInParameter(command, "LOGIN", DbType.String, user);
        db.AddOutParameter(command, "PROFILE", DbType.Xml, XML_SIZE);

        db.ExecuteStoredProcedure(command);
        return Convert.ToString(db.GetParameterValue(command, "PROFILE"));
      }
    } // TryGetProfile( user, xml )


    /// <summary>
    /// Sets the profile
    /// </summary>
    /// <param name="xml">profile xml</param>
    public static void SetProfile(string user, string xml)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_setProfile]");
        db.AddInParameter(command, "LOGIN", DbType.String, user);
        db.AddInParameter(command, "PROFILE", DbType.Xml, xml);

        db.ExecuteNonQuery(command);
      }
    } // SetProfile( xml )


    /// <summary>
    /// Gets the users translucency setting from the database
    /// </summary>
    /// <param name="login">Users login</param>
    /// <returns></returns>
    public static string GetTranslucency(string login)
    {
      Database db = DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d][sp_getTranslucency]");
        db.AddInParameter(command, "LOGIN", DbType.String, login);
        db.AddOutParameter(command, "TRANS", DbType.Xml, XML_SIZE);

        db.ExecuteStoredProcedure(command);
        return Convert.ToString(db.GetParameterValue(command, "TRANS"));
      }
    } // GetTranslucency( xml )


    /// <summary>
    /// Sets the translucency
    /// </summary>
    /// <param name="xml">translucency xml</param>
    public static void SetTranslucency(string login, string xml)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_setTranslucency]");
        db.AddInParameter(command, "LOGIN", DbType.String, login);
        db.AddInParameter(command, "TRANS", DbType.Xml, xml);

        db.ExecuteNonQuery(command);
      }
    } // SetTranslucency( user, xml )

    #endregion



    #endregion


    #endregion

  } // class SecurityGateway
} // namespace Viatronix.Enterprise.Data
