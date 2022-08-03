// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

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


namespace Viatronix.Colon.Data
{
  public static class DatabaseGateway
  {

    #region fields

    /// <summary>
    /// db name
    /// </summary>
    private const string DATABASE = "ViatronixDB";

    /// <summary>
    /// security db name
    /// </summary>
    private const string SECURITY = "Security";

    /// <summary>
    /// xml data column size
    /// </summary>
    private const int XML_SIZE = 4096;

    /// <summary>
    /// user name
    /// </summary>
    private static string m_user = string.Empty;

    /// <summary>
    /// is the db enabled
    /// </summary>
    private static bool? m_isEnabled = null;

    #endregion

    #region properties


    /// <summary>
    /// Determines if the database is available
    /// </summary>
    public static bool IsEnabled
    {
      get
      {
        if( m_isEnabled == null )
        {
          m_isEnabled = false;
          Regex regex = new Regex( @"-user\s*(?<UserGroup>\w+)\b" );
          Match match = regex.Match( Environment.CommandLine );
          Group group = match.Groups[ "UserGroup" ];
          if( group.Success )
          {
            m_user = group.Value;

            foreach( System.Configuration.ConnectionStringSettings settings in System.Configuration.ConfigurationManager.ConnectionStrings )
            {
              if( settings.Name == DATABASE )
              {
                m_isEnabled = true;
                break;
              }
            }
          }
        }

        return m_isEnabled.Value;
      }
    } // IsEnabled

    #endregion

    #region methods

    /// <summary>
    /// Attempts to retrieve the profile
    /// </summary>
    /// <param name="xml">output parameter for profile xml</param>
    /// <returns>true if successful</returns>
    public static bool TryGetProfile( out string xml )
    {
      try
      {
        Database db = DatabaseFactory.CreateDatabase( DATABASE );
        using( DbConnection conn = db.CreateConnection())
        {
          DbCommand command = db.GetStoredProcCommand( "sp_getProfile" );
          db.AddInParameter( command, "LOGIN", DbType.String, m_user );
          db.AddOutParameter( command, "PROFILE", DbType.Xml, XML_SIZE );

          db.ExecuteNonQuery( command );
          xml = Convert.ToString( db.GetParameterValue( command, "PROFILE" ));
        }
      }
      catch( Exception e )
      {
        Log.Debug( e.Message, "DatabaseGateway", "TryGetProfile" );
        xml = string.Empty;
      }

      return !string.IsNullOrWhiteSpace( xml );
    } // TryGetProfile( user, xml )


    /// <summary>
    /// Sets the profile
    /// </summary>
    /// <param name="xml">profile xml</param>
    public static void SetProfile( string xml )
    {
      Database db = DatabaseFactory.CreateDatabase( DATABASE );
      using( DbConnection conn = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand( "sp_setProfile" );
        db.AddInParameter( command, "LOGIN", DbType.String, m_user );
        db.AddInParameter( command, "PROFILE", DbType.Xml, xml );

        db.ExecuteNonQuery( command );
      }
    } // SetProfile( xml )


    /// <summary>
    /// Attempts to retrieve the configuration
    /// </summary>
    /// <param name="xml">output parameter for configuration xml</param>
    /// <returns>true if successful</returns>
    public static bool TryGetConfig( string name, out string xml )
    {
      try
      {
        Database db = DatabaseFactory.CreateDatabase( DATABASE );
        using( DbConnection conn = db.CreateConnection())
        {
          DbCommand command = db.GetStoredProcCommand( "sp_getConfig" );
          db.AddInParameter( command, "NAME", DbType.String, name );
          db.AddOutParameter( command, "CONFIG", DbType.Xml, XML_SIZE );

          db.ExecuteNonQuery( command );
          xml = Convert.ToString( db.GetParameterValue( command, "CONFIG" ));
        }
      }
      catch( Exception e )
      {
        Log.Debug( e.Message, "DatabaseGateway", "TryGetProfile" );
        xml = string.Empty;
      }

      return !string.IsNullOrWhiteSpace( xml );
    } // TryGetConfig( name, xml )

    
    /// <summary>
    /// Sets the configuration
    /// </summary>
    /// <param name="name">configuration name</param>
    /// <param name="xml">configuration xml</param>
    public static void SetConfig( string name, string xml )
    {
      Database db = DatabaseFactory.CreateDatabase( DATABASE );
      using( DbConnection conn = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand( "sp_setConfig" );
        db.AddInParameter( command, "NAME", DbType.String, name );
        db.AddInParameter( command, "CONFIG", DbType.Xml, xml );

        db.ExecuteNonQuery( command );
      }
    } // SetConfig( name, xml )


    /// <summary>
    /// Attempts to retrieve the translucency
    /// </summary>
    /// <param name="xml">output parameter for translucency xml</param>
    /// <returns>true if successful</returns>
    public static bool TryGetTranslucency( out string xml )
    {
      try
      {
        Database db = DatabaseFactory.CreateDatabase( DATABASE );
        using( DbConnection conn = db.CreateConnection())
        {
          DbCommand command = db.GetStoredProcCommand( "sp_getTranslucency" );
          db.AddInParameter( command, "LOGIN", DbType.String, m_user );
          db.AddOutParameter( command, "TRANS", DbType.Xml, XML_SIZE );

          db.ExecuteNonQuery( command );
          xml = Convert.ToString( db.GetParameterValue( command, "TRANS" ));
        }
      }
      catch( Exception e )
      {
        Log.Debug( e.Message, "DatabaseGateway", "TryGetProfile" );
        xml = string.Empty;
      }

      return !string.IsNullOrWhiteSpace( xml );
    } // TryGetTranslucency( xml )


    /// <summary>
    /// Sets the translucency
    /// </summary>
    /// <param name="xml">translucency xml</param>
    public static void SetTranslucency( string xml )
    {
      Database db = DatabaseFactory.CreateDatabase( DATABASE );
      using( DbConnection conn = db.CreateConnection())
      {
        DbCommand command = db.GetStoredProcCommand( "sp_setTranslucency" );
        db.AddInParameter( command, "LOGIN", DbType.String, m_user );
        db.AddInParameter( command, "TRANS", DbType.Xml, xml );

        db.ExecuteNonQuery( command );
      }
    } // SetTranslucency( user, xml )


    /// <summary>
    /// Acquires a context
    /// </summary>
    /// <param name="name">user name</param>
    /// <param name="pwd">user password</param>
    /// <param name="id">context id</param>
    /// <param name="acquire">true if acquire; false to release </param>
    /// <returns>context id</returns>
    public static string AcquireContext( string name, string pwd, string id, bool acquire )
    {
      try
      {
        Database db = DatabaseFactory.CreateDatabase( SECURITY );
        using( DbConnection conn = db.CreateConnection())
        {
          DbCommand command = db.GetStoredProcCommand( "sp_acquireContext" );
          db.AddParameter( command, "CONTEXT_ID", DbType.String, 256, ParameterDirection.InputOutput, false, 0, 0, string.Empty, System.Data.DataRowVersion.Default, id );
          db.AddInParameter( command, "LOGIN", DbType.String, name );
          db.AddInParameter( command, "PASSWORD", DbType.String, pwd );
          db.AddInParameter( command, "ACQUIRE", DbType.Boolean, acquire );

          db.ExecuteNonQuery( command ); 
          return Convert.ToString( db.GetParameterValue( command, "CONTEXT_ID" ));
        }
      }
      catch( Exception e )
      {
        string message = e.Message.Substring( 7 ).Trim();
        if( e.Message.Substring( 0, 2 ) == "VX" )
          throw new ApplicationException( message );

        throw new DataException( "Unable to connect.", e );
      }
    } // AcquireContext( name, pwd, id, acquire )

    #endregion

  } // class DatabaseGateway
} // namespace Viatronix.Colon.Data
