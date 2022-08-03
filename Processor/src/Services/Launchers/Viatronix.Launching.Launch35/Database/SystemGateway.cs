using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Sql;
using System.Data.SqlClient;
using System.Threading.Tasks;

namespace Viatronix.Launching.Database
{
  public static class SystemGateway
  {
    private const string m_name = "system";


    /// <summary>
    /// Makes sure that we have at least service pack 4
    /// </summary>
    /// <returns></returns>
    public static bool IsServicePack4Installed()
    {
      bool isServicePack4Installed = false;

      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string sql = "SELECT [version] FROM [v3d].[Versions] WHERE [object] = 'Storage'";

        SqlCommand command = new SqlCommand(sql, connection);

        command.CommandType = System.Data.CommandType.Text;

        SqlDataReader reader = command.ExecuteReader();

        if ( reader.HasRows )
        {
          // Call Read before accessing data. 
          while (reader.Read())
          {

            float version = 0.0F;
            string column = reader.GetString(0);

            isServicePack4Installed = (Single.TryParse(column, out version) && version > 4.0F);

          }

        }
        reader.Close();

      }

      return isServicePack4Installed;

    }

    public static void ReleaseContext(string context)
    {
      using (SqlConnection connection = new SqlConnection(Database.ConnectionStrings[m_name]))
      {

        connection.Open();

        string storedProcedure = "[v3d].[sp_releaseContext]";

        SqlCommand command = new SqlCommand(storedProcedure, connection);

        command.CommandType = System.Data.CommandType.StoredProcedure;
        command.Parameters.Add(new SqlParameter("ID", context));
        command.Parameters.Add(new SqlParameter("FORCE", "1"));


        command.ExecuteNonQuery();



      }


    }


  }
}
