using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Utilities.IO;
using Viatronix.Utilities.Registry;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// Transfers patient information out to the device.
  /// </summary>
  public class PatientDiscTransferModel : DiscTransferModel
  {

    #region constants

    /// <summary>
    /// The header page for the disc.
    /// </summary>
    private const string PATIENT_HEADER_FILENAME = "index.html";

    /// <summary>
    /// The session page that provides easy access to session data.
    /// </summary>
    private const string SESSION_HEADER_FILENAME = "report.html";

    #endregion

    #region methods

    /// <summary>
    /// Creates a staging area containing the patient cd file system.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="stagingArea">The directory in which to create the staging area.</param>
    public override void CreateStagingArea(StudyViewModel[] studies, string stagingArea)
    {
      int dicomNumber = 0;
      int sessionNumber = 0;

      List<string> sessionDirectories = new List<string>();

      foreach(StudyViewModel study in studies)
      {
        foreach(SeriesViewModel series in study.Series)
        {
          if(series.Type == SeriesTypes.Dicom && series.IsEligibleForTransfer)
          {
            // ---------------------------
            // Set the series directories.
            // ===========================

            series.StagingArea = stagingArea;
            series.SeriesDirectory = string.Format("DICOM_{0}", dicomNumber++);

            if(!Directory.Exists(series.StagingUrl))
            {
              Directory.CreateDirectory(series.StagingUrl);
            }

            foreach(TransferItemViewModel item in series.TransferItems)
            {
              if(item.IsSelected)
              {
                string stagingItemDirectory = Path.Combine(series.StagingUrlShortPath, item.SearchInFolder);
                if (!Directory.Exists(stagingItemDirectory))
                {
                  Directory.CreateDirectory(stagingItemDirectory);
                }

                foreach(string file in item.RetrieveFiles(series.Url))
                {
                  string stagingFile = Path.Combine(stagingItemDirectory, Path.GetFileName(file));
                  if(!File.Exists(stagingFile))
                  {
                    File.Copy(file, stagingFile);
                  }
                }
              }
            }
          }
          else if(series.Type == SeriesTypes.Session && series.IsEligibleForTransfer)
          {
            // ---------------------------
            // Set the series directories.
            // ===========================

            series.StagingArea = stagingArea;
            series.SeriesDirectory = string.Format("SESSION_{0}", sessionNumber++);

            sessionDirectories.Add(series.StagingUrl);

            if(!Directory.Exists(series.StagingUrl))
            {
              Directory.CreateDirectory(series.StagingUrl);
            }

            bool transferringSnapshots = series.TransferItems.Find("Snapshots") != null && series.TransferItems.Find("Snapshots").IsSelected;
            bool transferringMovies = series.TransferItems.Find("Movies") != null && series.TransferItems.Find("Movies").IsSelected;
            bool transferringReports = series.TransferItems.Find("Reports") != null && series.TransferItems.Find("Reports").IsSelected;

            if(transferringSnapshots)
            {
              TransferItemViewModel snapshotsItem = series.TransferItems.Find("Snapshots");

              string stagingItemDirectory = Path.Combine(series.StagingUrlShortPath, snapshotsItem.SearchInFolder);
              if(!Directory.Exists(stagingItemDirectory))
              {
                Directory.CreateDirectory(stagingItemDirectory);
              }

              foreach(string file in snapshotsItem.RetrieveFiles(series.Url))
              {
                string stagingFile = Path.Combine(stagingItemDirectory, Path.GetFileName(file));
                if(!File.Exists(stagingFile))
                {
                  File.Copy(file, stagingFile);
                }
              }
            }

            if(transferringMovies)
            {
              TransferItemViewModel moviesItem = series.TransferItems.Find("Movies");

              string stagingItemDirectory = Path.Combine(series.StagingUrlShortPath, moviesItem.SearchInFolder);
              if (!Directory.Exists(stagingItemDirectory))
              {
                Directory.CreateDirectory(stagingItemDirectory);
              }

              foreach (string file in moviesItem.RetrieveFiles(series.Url))
              {
                string stagingFile = Path.Combine(stagingItemDirectory, Path.GetFileName(file));
                if (!File.Exists(stagingFile))
                {
                  File.Copy(file, stagingFile);
                }
              }
            }

            if(transferringReports)
            {
              TransferItemViewModel reportsItem = series.TransferItems.Find("Reports");

              string stagingItemDirectory = Path.Combine(series.StagingUrl, reportsItem.SearchInFolder);
              if (!Directory.Exists(stagingItemDirectory))
              {
                Directory.CreateDirectory(stagingItemDirectory);
              }

              foreach (string file in reportsItem.RetrieveFiles(series.Url))
              {
                string stagingFile = Path.Combine(stagingItemDirectory, Path.GetFileName(file));
                if (!File.Exists(stagingFile))
                {
                  File.Copy(file, stagingFile);
                }
              }
            }

            CreateSessionPage(study, series, series.StagingUrlShortPath);
          }
        }
      }

      if (studies.Length > 0)
      {
        string consoleConfigDirectory = Path.Combine(SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console");

        if (!File.Exists(Path.Combine(stagingArea, "logo.png")))
        {
          File.Copy(Path.Combine(consoleConfigDirectory, "logo.png"), Path.Combine(stagingArea, "logo.png"));
        }

        if(!File.Exists(Path.Combine(stagingArea, "autorun.ico")))
        {
          File.Copy(Path.Combine(consoleConfigDirectory, "autorun.ico"), Path.Combine(stagingArea, "autorun.ico"));
        }

        if (!File.Exists(Path.Combine(stagingArea, "autorun.inf")))
        {
          string fileContents = string.Empty;
          using (TextReader reader = new StreamReader(Path.Combine(SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console", "autorun.inf")))
          {
            fileContents = reader.ReadToEnd();
          }

          fileContents = fileContents.Replace("filename", PATIENT_HEADER_FILENAME);

          using(TextWriter writer = new StreamWriter(Path.Combine(stagingArea, "autorun.inf"), false))
          {
            writer.Write(fileContents);
          }
        }

        CreateHeaderPage(studies[0], stagingArea);
      }
    } // CreateStagingArea( studies, stagingArea )


    /// <summary>
    /// Creates the index html page.
    /// </summary>
    /// <param name="study">The study we are transferring.</param>
    /// <param name="stagingArea">The staging area.</param>
    private void CreateHeaderPage(StudyViewModel study, string stagingArea)
    {
      string pageStaging = Path.Combine(stagingArea, PATIENT_HEADER_FILENAME);
      if(!File.Exists(pageStaging))
      {
        string fileContents = string.Empty;
        using(TextReader reader = new StreamReader(Path.Combine(SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console", "ReportHeaderTemplate.htm")))
        {
          fileContents = reader.ReadToEnd();
        }

        fileContents = fileContents.Replace("<v3d_patientInfo property=\"Name\" />", study.Name);
        fileContents = fileContents.Replace("<v3d_patientInfo property=\"MedicalId\" />", study.MedicalId);
        fileContents = fileContents.Replace("<v3d_patientInfo property=\"Dob\" />", study.Dob.ToShortDateString());
        fileContents = fileContents.Replace("<v3d_patientInfo property=\"Gender\" />", study.Gender.ToString());
        
        using(TextWriter writer = new StreamWriter(pageStaging, false))
        {
          writer.Write(fileContents);
        }
      }
    } // CreateHeaderPage( study, stagingArea )


    /// <summary>
    /// Creates the session page.
    /// </summary>
    /// <param name="study"></param>
    /// <param name="series"></param>
    /// <param name="stagingArea"></param>
    private void CreateSessionPage(StudyViewModel study, SeriesViewModel series, string stagingArea)
    {
      bool transferringSnapshots = series.TransferItems.Find("Snapshots") != null && series.TransferItems.Find("Snapshots").IsSelected;
      bool transferringMovies = series.TransferItems.Find("Movies") != null && series.TransferItems.Find("Movies").IsSelected;
      bool transferringReports = series.TransferItems.Find("Reports") != null && series.TransferItems.Find("Reports").IsSelected;

      string fileContents = string.Empty;
      using (TextReader reader = new StreamReader(Path.Combine(SystemSettings.CurrentSystemSettings.ConfigDirectory, "Console", "PatientReportTemplate.htm")))
      {
        fileContents = reader.ReadToEnd();
      }

      fileContents = fileContents.Replace("<v3d_patientInfo property=\"Name\" />", study.Name);
      fileContents = fileContents.Replace("<v3d_patientInfo property=\"MedicalId\" />", study.MedicalId);
      fileContents = fileContents.Replace("<v3d_patientInfo property=\"Dob\" />", study.Dob.ToShortDateString());
      fileContents = fileContents.Replace("<v3d_patientInfo property=\"Gender\" />", study.Gender.ToString());

      if (transferringSnapshots)
      {
        TransferItemViewModel snapshots = series.TransferItems.Find("Snapshots");

        // Need to add images to the page.
        int startPosition = fileContents.IndexOf("<v3d_snapshots");
        int endPosition = fileContents.IndexOf("/>", startPosition);
        string snapshotsDefinition = startPosition > -1 && endPosition > startPosition ? fileContents.Substring(startPosition, (endPosition + 2) - startPosition) : string.Empty;

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(snapshotsDefinition);

        string tooltip = doc.DocumentElement.Attributes["alt"].Value;
        int width = System.Convert.ToInt32(doc.DocumentElement.Attributes["width"].Value);
        int height = System.Convert.ToInt32(doc.DocumentElement.Attributes["height"].Value);
        int hspace = System.Convert.ToInt32(doc.DocumentElement.Attributes["hspace"].Value);
        int vspace = System.Convert.ToInt32(doc.DocumentElement.Attributes["vspace"].Value);
        int columns = System.Convert.ToInt32(doc.DocumentElement.Attributes["columns"].Value);
        
        StringBuilder builder = new StringBuilder();
        builder.AppendLine("<table>");
        builder.AppendLine("<tr>");

        string[] snapshotsFiles = series.RetrieveFilesFromStaging("Snapshots");
        for (int i = 0; i < snapshotsFiles.Length; ++i)
        {
          if(i % columns == 0 && i > 0 && i < snapshotsFiles.Length)
          {
            builder.AppendLine("</tr>");
            builder.AppendLine("<tr>");
          }

          builder.AppendLine("<td>");

          // Add snapshots.
          builder.AppendLine(string.Format("<a href=\"Snapshots/{0}\" border=\"0\"><img src=\"Snapshots/{0}\" onclick=\"OpenDataFile(src)\" alt=\"{1}\" width=\"{2}\" " +
            "height=\"{3}\" hspace=\"{4}\" vspace=\"{5}\" border=\"1\" /></a>",
            Path.GetFileName(snapshotsFiles[i]), tooltip, width, height, hspace, vspace));

          builder.AppendLine("</td>");
        }

        builder.AppendLine("</tr>");
        builder.AppendLine("</table>");

        fileContents = fileContents.Replace(snapshotsDefinition, builder.ToString());
      }

      if (transferringMovies)
      {
        TransferItemViewModel movies = series.TransferItems.Find("Movies");

        // Need to add images to the page.
        int startPosition = fileContents.IndexOf("<v3d_movies");
        int endPosition = fileContents.IndexOf("/>", startPosition);
        string snapshotsDefinition = startPosition > -1 && endPosition > startPosition ? fileContents.Substring(startPosition, (endPosition + 2) - startPosition) : string.Empty;

        XmlDocument doc = new XmlDocument();
        doc.LoadXml(snapshotsDefinition);

        string tooltip = doc.DocumentElement.Attributes["alt"].Value;
        int width = System.Convert.ToInt32(doc.DocumentElement.Attributes["width"].Value);
        int height = System.Convert.ToInt32(doc.DocumentElement.Attributes["height"].Value);
        int hspace = System.Convert.ToInt32(doc.DocumentElement.Attributes["hspace"].Value);
        int vspace = System.Convert.ToInt32(doc.DocumentElement.Attributes["vspace"].Value);
        int columns = System.Convert.ToInt32(doc.DocumentElement.Attributes["columns"].Value);

        StringBuilder builder = new StringBuilder();
        builder.AppendLine("<table>");
        builder.AppendLine("<tr>");

        // Find matches.
        List<KeyValuePair<string, string>> moviePairs = new List<KeyValuePair<string,string>>();
        
        string[] movieFiles = series.RetrieveFilesFromStaging("Movies");
        for (int i = 0; i < movieFiles.Length; ++i)
        {
          if (Path.GetExtension(movieFiles[i]) == ".jpg" || Path.GetExtension(movieFiles[i]) == ".png")
          {
            // Need to find the matching movie file.
            for(int j = 0; j < movieFiles.Length; ++j)
            {
              if((Path.GetExtension(movieFiles[j]) == ".avi" || Path.GetExtension(movieFiles[j]) == ".wmv") && Path.GetFileNameWithoutExtension(movieFiles[i]) == Path.GetFileNameWithoutExtension(movieFiles[j]))
              {
                moviePairs.Add(new KeyValuePair<string, string>(movieFiles[i], movieFiles[j]));
              }
            }
          }
        }

        for(int i=0; i < moviePairs.Count; ++i)
        {
          if (i % columns == 0 && i > 0 && i < moviePairs.Count)
          {
            builder.AppendLine("</tr>");
            builder.AppendLine("<tr>");
          }

          builder.AppendLine("<td>");

          // Add snapshots.
          builder.AppendLine(string.Format("<a href=\"Movies/{0}\" border=\"0\"><img src=\"Movies/{1}\" alt=\"{2}\" width=\"{3}\" " +
            "height=\"{4}\" hspace=\"{5}\" vspace=\"{6}\" border=\"1\" /></a>",
            Path.GetFileName(moviePairs[i].Value), Path.GetFileName(moviePairs[i].Key), tooltip, width, height, hspace, vspace));

          builder.AppendLine("</td>");
        }

        builder.AppendLine("</tr>");
        builder.AppendLine("</table>");

        fileContents = fileContents.Replace(snapshotsDefinition, builder.ToString());
      }

      if (transferringReports)
      {
        string reportDefinition = "<v3d_reports />";

        string[] reports = series.RetrieveFilesFromStaging("Reports");
        if(reports.Length > 0)
        {
          fileContents = fileContents.Replace(reportDefinition, string.Format("<a href=\"{0}\">Click Here to View Session Report</a>", reports[0]));
        }
      }

      using(TextWriter writer = new StreamWriter(Path.Combine(series.StagingUrl, SESSION_HEADER_FILENAME)))
      {
        writer.Write(fileContents);
      }
    } // CreateSessionPage( series, stagingArea )

    #endregion

  } // class PatientDiscTransferModel
} // namespace Viatronix.Utilities.Transfer.Models
