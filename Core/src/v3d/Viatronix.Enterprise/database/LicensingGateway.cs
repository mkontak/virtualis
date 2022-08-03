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
using System.Text.RegularExpressions;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Data
{

  public static class LicensingGateway
  {

    #region fields

    /// <summary>
    /// security db name
    /// </summary>
    private const string DATABASE = "licensing";


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

    //#region INSERT

    ///// <summary>
    ///// Performs the appropriate Update in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="xmlBuilder">Xml Builder</param>
    ///// <returns></returns>
    //public static void Insert(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    //{
    //  throw new NotSupportedException("Insert operations are not supported for the Licesning Database");
    //} // Insert(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Performs the appropriate Update in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="entity"></param>
    ///// <returns></returns>
    //public static void Insert(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    //{
    //  throw new NotSupportedException("Insert operations are not supported for the Licesning Database");
    //} // Insert(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Performs the appropriate INSERT in the security database
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="query">Query</param>
    ///// <returns>Results of the query</returns>
    //public static void Insert(string context, string resource, string record, string source = "default")
    //{
    //  throw new NotSupportedException("Insert operations are not supported for the Licesning Database");
    //} // Insert(striung context, string resource, string query)

    //#endregion

    //#region UPDATE

    ///// <summary>
    ///// Performs the appropriate Update in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="xmlBuilder">Xml Builder</param>
    ///// <returns></returns>
    //public static string Update(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    //{
    //  throw new NotSupportedException("Update operations are not supported for the Licesning Database");
    //} // Update(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Performs the appropriate Update in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="entity"></param>
    ///// <returns></returns>
    //public static string Update(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    //{
    //  throw new NotSupportedException("Update operations are not supported for the Licesning Database");
    //} // Update(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Performs the appropriate Update in the security database
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="record">Record xml</param>
    ///// <returns>Results of the query</returns>
    //public static string Update(string context, string resource, string record, string source = "default")
    //{
    //  throw new NotSupportedException("Update operations are not supported for the Licesning Database");
    //} // Update(string context, string resource, string query)

    //#endregion

    //#region DELETE

    ///// <summary>
    ///// Performs the appropriate Delete in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="xmlBuilder">Xml Builder</param>
    ///// <returns></returns>
    //public static void Delete(string context, string resource, XmlBuilder xmlBuilder, string source = "default")
    //{
    //  throw new NotSupportedException("Delete operations are not supported for the Licesning Database");
    //} // Delete(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Performs the appropriate Delete in the security database for the entity
    ///// </summary>
    ///// <param name="context">Context Id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="entity"></param>
    ///// <returns></returns>
    //public static void Delete(string context, string resource, Viatronix.Enterprise.Entities.IEntity entity, string source = "default")
    //{
    //  throw new NotSupportedException("Delete operations are not supported for the Licesning Database");
    //} // Delete(string context, string resource, IEntity entity)

    ///// <summary>
    ///// Handles the update of a record for the specified resource.  Executes the appropriate get stored procedure in the database using the resource name. If 
    ///// the store procedure does not exists a UnsupportedException is thrown.
    ///// </summary>
    ///// <param name="context">Context id</param>
    ///// <param name="resource">Resource name</param>
    ///// <param name="record">Record to delete</param>
    ///// <returns></returns>
    //public static void Delete(string context, string resource, string record, string source = "default")
    //{
    //  throw new NotSupportedException("Delete operations are not supported for the Licesning Database");
    //} // Delete(string context, string resource, string record)

    //#endregion

    /// <summary>
    /// Enables the system
    /// </summary>
    /// <param name="context">autrhentication token</param>
    public static void EnableSystem(string context)
    {
      Database db = Viatronix.Enterprise.Data.DatabaseFactory.CreateDatabase(DATABASE);
      using (DbConnection connection = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand("[v3d].[sp_enableSystem]");
        db.ExecuteStoredProcedure(command);
      }
    }


    #endregion


    #endregion

  } // class LicensingGateway
} // namespace Viatronix.Enterprise.Data
