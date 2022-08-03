// $Id: StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using Microsoft.SqlServer.Server;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Security.Principal;
using System.Xml;
using System.Text;
using Viatronix.Database.Sql;




  /// <summary>
  /// Stored procedures assembly
  /// 
  /// Contains the stored procedure used within the ViatronixDB database
  /// 
  /// The following SQL commands are to be run seperatly in order to utilize this assembly in the SQL server. SQL server must be version 2005 or up
  ///
  /// 1. CREATE ASSEMBLY [System.Management] FROM 'C:\Windows\Microsoft.NET\Framework\v2.0.50727\System.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// 2. CREATE ASSEMBLY [Viatronix.v3D.Management] FROM 'C:\CVS\V3D\bin\vxUtilities\Viatronix.v3D.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// 3. CREATE PROCEDURE sp_getsystemid @SYSTEM_ID NVARCHAR(256) OUTPUT  AS EXTERNAL NAME [Viatronix.v3D.Management].StoredProcedures.GetSystemId
  /// 
  /// Updates that do not change the stored procedures contract can be done with the following command
  /// 
  /// ALTER ASSEMBLY [Viatronix.v3D.Management] FROM 'C:\CVS\V3D\bin\vxUtilities\Viatronix.v3D.Management.dll' WITH PERMISSION_SET = UNSAFE;
  /// 
  /// IMPORTANT:
  /// 
  ///     Any change to the DLL will require all procedures and assemblies to be dropped an re-added...
  /// 
  /// </summary>
  public partial class StoredProcedures
  {

    #region fields

        /// <summary>
    /// Log Path
    /// </summary>
    private static string m_logPath = System.IO.Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Viatronix\\Logs\\Database");

    /// <summary>
    /// Log file name
    /// </summary>
    private static string m_logfilename = "sql_security.log";

    #endregion

    #region Security Database Procedures

    /// <summary>
    /// Decrypts the value
    /// </summary>
    /// <param name="value"></param>
    /// <param name="decryptedValue"></param>
    /// <returns></returns>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_decrypt(string value, out string decryptedValue)
    {


      decryptedValue = Viatronix.Sql.Helper.Decrypt(value);

      return 1;

    } // Encrypt(string value, out string encryptedValue )

    /// <summary>
    /// Encrypts the value supplied
    /// </summary>
    /// <param name="value"></param>
    /// <param name="encryptedValue"></param>
    /// <returns></returns>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_encrypt(string value, out string encryptedValue)
    {


      encryptedValue = Viatronix.Sql.Helper.Encrypt(value);

      return 1;

    } // Encrypt(string value, out string encryptedValue )


    /// <summary>
    /// Windows authentication
    /// </summary>
    /// <param name="domain">Domain name</param>
    /// <param name="login">Login</param>
    /// <param name="password">Password</param>
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static int sp_windowsAuthentication(string domain, string login, string password)
    {

      int rc = 1;

      // Set the local domain
      string localDomain = (domain.Length > 0 ? domain : ".");

      IntPtr token = IntPtr.Zero;
      
      //==========================================
      // Attemot to login into the windows system
      //=========================================
      if ( Win32.LogonUser(login, localDomain, password, (uint)Win32.LogonTypes.Interactive, (uint)Win32.LogonProviders.Default, out token) == true )
      {

        try
        {
         
          // Windows identity
          WindowsIdentity identity = new WindowsIdentity(token, "NTLM", WindowsAccountType.Normal );

          char [] sep = { '\\' };

          string [] fields = identity.Name.Split(sep);

          string  domainName = ( domain.Length > 0 ? domain : System.Environment.MachineName );

          if ( ( fields.Length > 1 && 
                  ( string.Compare(fields[0], domainName, true) != 0 || string.Compare(fields[1], login, true) != 0 ) ) ||
               ( fields.Length == 1 && 
               ( domain.Length != 0 || string.Compare(fields[0], login, true) != 0 ) ) )
          {
            throw new System.Security.Authentication.AuthenticationException("901212");
          }

        }
        catch ( Exception ex  )
        {
          throw new System.Security.Authentication.AuthenticationException("901200: " + ex.Message);

        }
        finally
        {
          // Close the handle
          if ( token != IntPtr.Zero )
            Win32.CloseHandle(token);
        }

     }
     else 
     {

       string message = string.Empty;
       int errorCode = System.Runtime.InteropServices.Marshal.GetLastWin32Error();

        switch ( errorCode )
        {
          case 1326:    // Logon Failure
            message = "901200";
            break;

          case 1327:  // Account restricted
            message = "901207";
            break;

          case 1328:  // Invalid login hours
            message = "901208";
            break;

          case 1329:  // Cannot log into current workstation
            message = "901209";
            break;

          case 1330:    // Password Expired
            message = "900993|Password";
            break;

          case 1331:    // Account: Disabled
            message = "900997|Account";
            break;

          case 1385:    // Login type not granted
            message = "901211";
            break;

          case 1909:  // Profile does not exist
            message = "901225";
            break;

          default:
            {

              Win32.NetJoinStatus status = Win32.NetJoinStatus.Unknown;
              IntPtr pDomain = IntPtr.Zero;

              try
              {
                int result = Win32.NetGetJoinInformation(null, out pDomain, out status);
                string domainName = System.Runtime.InteropServices.Marshal.PtrToStringUni(pDomain);

                if (domain.Length > 0 && (int)status != (int)Win32.NetJoinStatus.DomainName)
                {
                  message = "901200: Domain specified but system is not on a domain. ";
                }
                else if (domain.Length > 0 && (int)status == (int)Win32.NetJoinStatus.DomainName && string.Compare(domainName.ToString(), domain, true) != 0)
                {
                  message = "901200: Domain specified " + domain + " is not the same as the domain this system is on " + domainName;
                }
                else
                {
                  message = "901200";
                }

              }
              finally
              {
                if (pDomain != IntPtr.Zero)
                   Win32.NetApiBufferFree(pDomain);
              }

            }
            break;


        };

        rc = 0;

        throw new System.Security.Authentication.AuthenticationException(message);


      }

      return rc;

    } // WindowsAuthenticate(string domain, string login, string password)




    #endregion

    #region Helper Methods


   /// <summary>
    /// Logs a message to the specified file
    /// </summary>
    /// <param name="path">path</param>
    /// <param name="message">Message to log</param>
    private static void LogMessage(string method, string message)
    {
      Viatronix.Sql.Helper.LogMessage(m_logfilename, method, message);
    }


 
    #endregion


  };  // class StoredProcedures


#region revision history

// $Log: StoredProcedures.cs,v $
// Revision 1.5  2010/02/16 20:03:42  mkontak
// Changed the way the hashing works
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.4  2010/02/12 14:14:52  mkontak
// Updates
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3  2010/02/01 16:07:49  mkontak
// Added windows authentication
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1  2009/09/25 19:00:19  mkontak
// Initial check in
//
// Revision 1.1  2009/08/05 13:40:25  mkontak
// Moved/Renamed
//
// Revision 1.3  2009/08/04 12:10:40  mkontak
// no message
//
// Revision 1.2  2009/07/31 19:14:36  mkontak
// Added stored procedures for file system syncing
//
// Revision 1.1  2009/07/15 16:32:06  mkontak
// Moved Management Project
//
// Revision 1.2  2009/04/28 12:24:57  mkontak
// Cleanup unneeded usings
//
// Revision 1.1  2008/10/21 13:31:50  mkontak
// Fix for sp_addSeries
//
// Revision 1.1  2008/05/06 20:50:20  mkontak
// Initial check in
//
// $Header: /cvs/cvsRepository/V3D/src/Viatronix.SqlServer.Security/StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $
// $Id: StoredProcedures.cs,v 1.5 2010/02/16 20:03:42 mkontak Exp $

#endregion
