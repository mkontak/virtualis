using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Threading.Tasks;

namespace Viatronix.Launching.Database
{
  public static class StorageGateway
  {
    private const string m_name = "storage";

    /// <summary>
    /// Gets the launch list for the session
    /// </summary>
    /// <param name="application"></param>
    /// <param name="uids"></param>
    /// <param name="user"></param>
    /// <param name="desc"></param>
    /// <param name="newSession"></param>
    /// <returns></returns>
    public static string GetLaunchList(string application, string uids, string user, string desc, bool newSession)
    {

      string launchlist = string.Empty;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_getLaunchList]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("APP", application));
        command.Parameters.Add(new SqlParameter("UIDS", uids));
        command.Parameters.Add(new SqlParameter("LOGIN", user));
        command.Parameters.Add(new SqlParameter("DESC", desc));
        command.Parameters.Add(new SqlParameter("CREATE_SESSION", (newSession ? "1" : "0")));

        SqlParameter results = new SqlParameter("RESULTS", System.Data.SqlDbType.Xml, 10000, System.Data.ParameterDirection.Output, false, 0, 0, string.Empty, System.Data.DataRowVersion.Default, launchlist);
        command.Parameters.Add(results);

        command.ExecuteNonQuery();

        launchlist = (string)results.Value;


      }

      return launchlist;

    } // GetLaunchList()

    /// <summary>
    /// Gets the launch list for the session
    /// </summary>
    /// <param name="uids"></param>
    /// <returns></returns>
    public static bool IsLocked(string uids)
    {

      bool isLocked = false;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_isLocked]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("@ID", uids));

        SqlParameter locked = new SqlParameter( "LOCKED", isLocked);
        locked.Direction = System.Data.ParameterDirection.Output;

        command.Parameters.Add(locked);

        command.ExecuteNonQuery();

        isLocked = Boolean.Parse(locked.Value.ToString());


      }


      return isLocked;

    } // isLocked()


    /// <summary>
    /// Gets the launch list for the session
    /// </summary>
    /// <param name="application"></param>
    /// <param name="uids"></param>
    /// <param name="user"></param>
    /// <param name="newSession"></param>
    /// <returns></returns>
    public static void LockSession(Session session, string user)
    {

  
      string uid = string.Empty;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_lockSession]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("UID", uid));
        command.Parameters.Add(new SqlParameter("USER", user));
        command.Parameters.Add(new SqlParameter("SECONDS", 5000));
        command.Parameters.Add(new SqlParameter("MACHINE", System.Environment.MachineName));

         command.ExecuteNonQuery();

 

      }

    } // SetLock()

    /// <summary>
    /// Gets the launch list for the session
    /// </summary>
    /// <param name="application"></param>
    /// <param name="uids"></param>
    /// <param name="user"></param>
    /// <param name="newSession"></param>
    /// <returns></returns>
    public static void UnlockSession(Session session)
    {


      string uid = string.Empty;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_unlockSession]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("UID", uid));

        command.ExecuteNonQuery();



      }

    } // SetLock()

  }
}
