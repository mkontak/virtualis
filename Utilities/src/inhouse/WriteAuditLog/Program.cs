using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Sql;
using System.Data.SqlClient;

namespace Viatronix
{
  class LogBackMessage
  {
    static int Main(string[] args)
    {

      try
      {

        // Set up default values based on the parameters in the Application Setting section of the confi file
        string message = string.Empty;
        string type = System.Configuration.ConfigurationManager.AppSettings["type"];
        string connectionString = System.Configuration.ConfigurationManager.AppSettings["connectionString"];
        string file = string.Empty;
        string system = string.Empty;
        string started = string.Empty;
        string completed = string.Empty;
        string source = string.Empty;
        string status = string.Empty;
        string subtype = string.Empty;

        //======================================================
        // Parse the arguement list to override any settings
        //=====================================================
        for (int i = 0; i < args.Length; ++i)
        {

          //===========================
          // TYPE setting
          //============================
          if (args[i] == "-type")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No type specified");
              return 1;
            }
            type = args[i];
          }
          //============================
          // FILE setting
          //===========================
          else if (args[i] == "-file")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No file specified");
              return 1;
            }
            file = args[i];

          }
          //============================
          // SUBTYPE setting
          //===========================
          else if (args[i] == "-subtype")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No subtype specified");
              return 1;
            }
            subtype = args[i];

          }
          //============================
          // SYSTEM setting
          //===========================
          else if (args[i] == "-system")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No system specified");
              return 1;
            }
            system = args[i];

          }
          //============================
          // SOURCE setting
          //===========================
          else if (args[i] == "-source")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No source specified");
              return 1;
            }
            source = args[i];

          }
          //============================
          // STARTED date time setting
          //===========================
          else if (args[i] == "-started")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No start date specified");
              return 1;
            }
            started = args[i];

          }
          //============================
          // STATUS setting
          //===========================
          else if (args[i] == "-status")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No status specified");
              return 1;
            }
            status = args[i];

          }
          //============================
          // COMPLETED date time setting
          //===========================
          else if (args[i] == "-completed")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No complete date specified");
              return 1;
            }
            completed = args[i];

          }
          //============================
          // MESSAGE setting
          //===========================
          else if (args[i] == "-message")
          {
            if (++i == args.Length)
            {
              Console.WriteLine("No message specified");
              return 1;
            }

            message = args[i];
          }
          //============================
          // Invalid setting
          //===========================
          else
          {
            Console.WriteLine("Invalid parameter specified [" + args[i] + "]");
            return 1;
          } // END ... If any of the settings match


        } // END ... For each arguement

        int rc = 0;


        if (completed.Length == 0)
          completed = System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss");

        if (started.Length == 0)
          started = completed;

        using (SqlConnection connection = new SqlConnection(connectionString))
        {
          connection.Open();
          
          string sql = "INSERT INTO [AuditLog] ( type, subtype, server, system, source, [file], started, completed, message, status ) VALUES ( '" + 
                            type + "', '" +  subtype + "', '" + System.Environment.MachineName + "', '" + system + "', '" + 
                            source + "', '" + file + "', '" + started +"', '" + completed + "', '" + message + "', '" + status + "' )";

          SqlCommand command = new SqlCommand(sql, connection);

          rc = command.ExecuteNonQuery();

          
        }

        if (rc != 1)
          return 1;
        else
          return 0;

      }
      catch (Exception ex)
      {
        // Dump error message
        Console.WriteLine("Logging backup info failed : " + ex.Message);
        return 1;

      } //END ... Catch
    }
  }
}
