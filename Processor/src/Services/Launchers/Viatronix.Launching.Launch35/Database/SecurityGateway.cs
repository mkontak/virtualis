using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Threading.Tasks;

namespace Viatronix.Launching.Database
{
  public static class SecurityGateway
  {
    private const string m_name = "security";


    public static string Login(string login, string password)
    {
      string context = string.Empty;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_acquireContext]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("LOGIN", login));
        command.Parameters.Add(new SqlParameter("PASSWORD", password));

        SqlParameter results = new SqlParameter("CONTEXT_ID", System.Data.SqlDbType.NVarChar, 256, System.Data.ParameterDirection.Output, false, 0, 0, string.Empty, System.Data.DataRowVersion.Default, context);
        command.Parameters.Add(results);

        command.ExecuteNonQuery();

        context = (string)results.Value;


      }

      return context;

    }

    public static void ReleaseContext(Session session)
    {

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        if (!session.IsSuppliedContext)
        {

          if (session.HasContext)
          {

            Logging.Log.Information(string.Format("Releasing context {0}", session.ContextId), "SecurityGateway", "ReleaseContext");

            string storedProcedure = "[v3d].[sp_releaseContext]";

            SqlCommand command = new SqlCommand(storedProcedure, connection);

            command.CommandType = System.Data.CommandType.StoredProcedure;
            command.Parameters.Add(new SqlParameter("ID", session.ContextId));
            command.Parameters.Add(new SqlParameter("FORCE", "1"));


            command.ExecuteNonQuery();

          }
          else
          {


            if (string.Compare(session.Request.UserName, "vxservice", true) == 0 || string.Compare(session.Request.UserName, "administrator", true) == 0)
              return;

            Logging.Log.Information(string.Format("Releasing user {0} context", session.Request.UserName), "SecurityGateway", "ReleaseContext");


            string sql = "DELETE FROM [v3d].[Contexts] WHERE [login] = '" + session.Request.UserName + "'";

            SqlCommand command = new SqlCommand(sql, connection);

            command.CommandType = System.Data.CommandType.Text;


            command.ExecuteNonQuery();



          }
        }
      }


    }


  }
}
