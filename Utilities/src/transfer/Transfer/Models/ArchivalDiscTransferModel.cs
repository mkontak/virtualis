using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// The archival disc model.
  /// </summary>
  public class ArchivalDiscTransferModel : DiscTransferModel
  {

    #region methods

    /// <summary>
    /// Creates the staging area.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="stagingArea">The staging area.</param>
    public override void CreateStagingArea(StudyViewModel[] studies, string stagingArea)
    {
      base.CreateStagingArea(studies, stagingArea);

      foreach(StudyViewModel study in studies)
      {
        foreach(SeriesViewModel series in study.Series)
        {
          if(series.Type == SeriesTypes.Dicom)
          {
            // --------------------------------------------
            // Copy over any volumes that exist that apply.
            // ============================================
            
            string volumesStaging = Path.Combine(series.StagingUrlShortPath, "volumes");
            if (!Directory.Exists(volumesStaging))
            {
              Directory.CreateDirectory(volumesStaging);
            }

            foreach(Volume volume in series.Series.Volumes)
            {
              string volumeFilename = Path.Combine(volume.Url, volume.FileName);
              if(File.Exists(volumeFilename))
              {
                string volumeStaging = Path.Combine(volumesStaging, volume.ConversionId);
                if (!Directory.Exists(volumeStaging))
                {
                  Directory.CreateDirectory(volumeStaging);
                }

                File.Copy(volume.Url, Path.Combine(volumeStaging, volume.FileName));
              }
            }
          }
          else if(series.Type == SeriesTypes.Session)
          {
            // --------------------------------------
            // Copy all session files to the staging.
            // ======================================

            foreach(string files in Directory.GetFiles(series.StagingUrlShortPath, "*.*", SearchOption.AllDirectories))
            {
              string relativeFilePath = files.Replace(series.Url, "").TrimEnd('\\').Trim();

              string relativeDirectoryPath = Path.GetDirectoryName(relativeFilePath);
              if(relativeDirectoryPath != string.Empty && !Directory.Exists(Path.Combine(series.StagingUrlShortPath, relativeDirectoryPath)))
              {
                Directory.CreateDirectory(Path.Combine(series.StagingUrlShortPath, relativeDirectoryPath));
              }

              File.Copy(files, Path.Combine(series.StagingUrlShortPath, relativeFilePath));
            }
          }
        }
      }
    } // CreateStagingArea( studies, stagingArea )

    #endregion

  } // class ArchivalDiscTransferModel
} // namespace Viatronix.Utilities.Transfer.Models
