using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;

namespace PopulateDatasetsDatabase
{
  class Program
  {

    static string m_connectionString = string.Empty;

    static void Main(string[] args)
    {

      m_connectionString = System.Configuration.ConfigurationManager.AppSettings["connectionString"];

      AnalyzeDirectory(args[0]);
    }

    static void AddImage(string file)
    {


      try
      {
        Console.WriteLine("Attempting to add image " + file);

        Viatronix.Dicom.ElementList elementList = Viatronix.Dicom.ElementList.Create(file);

        string path = System.IO.Path.GetDirectoryName(file);


        string app = string.Empty;

        if (path.Contains("\\VC") || path.ToLower().Contains("colonoscopy") )
          app = "VC";
        else if (path.Contains("\\CS") || path.ToLower().Contains("calcium") )
          app = "CS";
        else if (path.Contains("\\CAR") || path.ToLower().Contains("cardiac"))
          app = "CAR";
        else if (path.Contains("\\FN") || path.ToLower().Contains("fusion"))
          app = "FN";
        else if (path.Contains("\\LN") || path.ToLower().Contains("lung"))
          app = "LN";
        else if (path.Contains("\\VAS") || path.ToLower().Contains("vascular"))
          app = "VAS";



        char[] sep = { '\\', '/' };
        string[] fields = path.Split(sep);

        string name = string.Empty;
        bool startCapturingName = false;
        foreach (string field in fields) 
        {
          //======================================
          // Looking 'Long term' or 'Short Term'
          //======================================
          if (field.Contains(" Term"))
            startCapturingName = true;
          else if (startCapturingName)
          {
            string part = field.Replace(" ", "").Replace("Datasets", "").Replace("_Viatronix", "").Replace("Viatronix", "").Replace("_DoNotcopy", "").Replace("Dicom", "").Replace("dicom", "");

            //====================================================================================================
            // Dont use directories with '.' since they are usually uid's and no '(' since this make reading
            // the name dificult
            //===================================================================================================
            if (  !part.Contains(".") &&
                  !part.Contains("(") &&
                  part.ToLower() != "dicom" &&
                  !part.ToLower().StartsWith("dicom_") &&
                  !part.ToLower().Contains("patient") &&
                  !part.ToLower().Contains("study") &&
                  !part.StartsWith("ST") &&
                  part.ToLower() != app.ToLower() &&
                  part.ToLower() != "misc" )
              name = (name.Length == 0 ? name : name + "-") + part;
          }
        }

        if (name.Length == 0)
        {
        

          int i = 0;
          int j = 0;
          for (j = 0, i = fields.Length - 1; i > 0 && j < 2; --i)
          {
            string field = fields[i];
            string part = field.Replace(" ", "").Replace("Datasets", "").Replace("_Viatronix", "").Replace("Viatronix", "").Replace("_DoNotcopy", "").Replace("Dicom", "").Replace("dicom", "");

            if (!part.Contains(".") &&
                  !part.Contains("(") &&
                  part.ToLower() != "dicom" &&
                  !part.ToLower().StartsWith("dicom_") &&
                  !part.ToLower().Contains("patient") &&
                  !part.ToLower().Contains("study") &&
                  part.StartsWith("ST") &&
                  part.ToLower() != app.ToLower() &&
                  part.ToLower() != "misc" )
            {
              name = part +  (name.Length == 0 ? name : "-" + name);
              ++j;
            }
          }
        }


        using (System.Data.SqlClient.SqlConnection connection = new System.Data.SqlClient.SqlConnection(m_connectionString))
        {

          connection.Open();

          System.Data.SqlClient.SqlCommand command = new System.Data.SqlClient.SqlCommand("sp_addImage", connection);

          command.CommandType = CommandType.StoredProcedure;
          command.Parameters.Add(new System.Data.SqlClient.SqlParameter("NAME", name));
          command.Parameters.Add(new System.Data.SqlClient.SqlParameter("FILE", file));
          command.Parameters.Add(new System.Data.SqlClient.SqlParameter("APP", app));
          command.Parameters.Add(new System.Data.SqlClient.SqlParameter("XML", "<dicomHeader>" + elementList.ToXml() + "</dicomHeader>"));


          command.ExecuteNonQuery();

        }
      }
      catch (Exception)
      {

        throw;
      }


    }

    static void AnalyzeDirectory(string path)
    {

      Console.WriteLine("Analyzing " + path);

      string[] files = System.IO.Directory.GetFiles(path);

      foreach (string file in files)
      {
        try
        {

          AddImage(file);

        }
        catch ( Exception ex )
        {
          Console.WriteLine("Failed to add image [" + System.IO.Path.GetFileName(file) + "] : " + ex.Message);
        }
      }

      string[] dirs = System.IO.Directory.GetDirectories(path);
      
      foreach (string dir in dirs)
      {
        AnalyzeDirectory(dir);
      }

    }

   }
}
