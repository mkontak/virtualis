// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Data;
using System.Data.Common;
using System.Linq;
using System.Text.RegularExpressions;
using System.Text;
using System.Xml;
using System.Xml.Linq;

using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;


namespace Viatronix
{
  public static class DatabaseExtensionMethods
  {



    /// <summary>
    /// Executes a db command and returns the xml
    /// </summary>
    /// <param name="db">SqlDatabase instance</param>
    /// <param name="command">DbCommand that returns xml</param>
    /// <returns>xml data</returns>
    public static string ExecuteXml(this Microsoft.Practices.EnterpriseLibrary.Data.Sql.SqlDatabase db, System.Data.Common.DbCommand command)
    {
      using (XmlReader reader = db.ExecuteXmlReader(command))
      {
        StringBuilder builder = new StringBuilder();
        while (!reader.EOF)
        {
          builder.Append(reader.ReadOuterXml());
          builder.Append(Environment.NewLine);
        }

        return builder.ToString();
      }
    } // ExecuteXml( db, command )


    /// <summary>
    /// Wraps ExecuteNonQuery by catching and rethrowing any exceptions originating in the database.
    /// </summary>
    /// <param name="db">Database</param>
    /// <param name="command">DbCommand</param>
    /// <returns>number of rows affected</returns>
    public static int ExecuteStoredProcedure(this Microsoft.Practices.EnterpriseLibrary.Data.Database db, System.Data.Common.DbCommand command)
    {
      try
      {
        return db.ExecuteNonQuery(command);
      }
      catch (Exception e)
      {
        throw ExceptionFactory.Create(e);
      }
    } // ExecuteStoredProcedure( db, command )


    /// <summary>
    /// Adds an input or output parameter.
    /// </summary>
    /// <param name="db">Database</param>
    /// <param name="command">DbCommand</param>
    /// <param name="name">parameter name</param>
    /// <param name="type">data type</param>
    /// <param name="size"></param>
    /// <param name="value"></param>
    public static void AddInOutParameter(this Microsoft.Practices.EnterpriseLibrary.Data.Database db, System.Data.Common.DbCommand command, string name, System.Data.DbType type, int size, object value)
    {
      db.AddParameter(command, name, type, size, System.Data.ParameterDirection.InputOutput, false, 0, 0, string.Empty, System.Data.DataRowVersion.Default, value);
    } // AddInOutParameter( db, command, name, type, value )



    /// <summary>
    /// Adds a return parameter
    /// </summary>
    /// <param name="db">Database</param>
    /// <param name="command">DbCommand</param>
    /// <param name="name">parameter name</param>
    /// <param name="type">data type</param>
    public static void AddReturnParameter(this Microsoft.Practices.EnterpriseLibrary.Data.Database db, System.Data.Common.DbCommand command, string name, System.Data.DbType type)
    {
      db.AddParameter(command, name, type, System.Data.ParameterDirection.ReturnValue, name, System.Data.DataRowVersion.Current, null);
    } // AddReturnParameter( db, command, name, type )



  } // class DatabaseExtensionMethods

} // namespace Viatronix.Enterprise
