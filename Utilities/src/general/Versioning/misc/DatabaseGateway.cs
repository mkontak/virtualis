using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;

namespace Versioning
{
  public class DatabaseGateway
  {

    private static readonly string m_connectionString = string.Format("user id=v3duser;password=plugmein65%;server={0};database=Viatronix", ConfigurationManager.AppSettings["server"]);


    public static string Get(string resource, string query = "")
    {

      try
      {
        string actualQuery = (string.IsNullOrEmpty(query) ? string.Format("<{0}/>", resource) : query);
        string storedProcedure = string.Format("sp_get{0}", resource);

        using (SqlConnection conn = new SqlConnection(m_connectionString))
        {
          conn.Open();

          SqlCommand command = new SqlCommand(storedProcedure, conn);
          command.CommandType = CommandType.StoredProcedure;

          command.Parameters.Add(new SqlParameter("QUERY", actualQuery));

          SqlParameter resultsParameter = new SqlParameter("RESULTS", SqlDbType.Xml, 20000);
          resultsParameter.Direction = ParameterDirection.Output;

          command.Parameters.Add(resultsParameter);

            // Get the results
          string results = Convert.ToString(resultsParameter.Value);

          return results;

        }
      }
      catch (Exception )
      {
        throw;
      }

    }

  }
}
