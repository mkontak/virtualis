using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// An interface to define an entity for transferring data.
  /// </summary>
  public abstract class TransferModel
  {

    #region events

    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    public event ProgressEventHandler Progress;

    #endregion

    #region methods

    /// <summary>
    /// Performs the transfer.
    /// </summary>
    /// <param name="studies">The list of studies.</param>
    /// <param name="args">The arguemnt that the transfer needs to be provided.</param>
    /// <param name="stagingArea">The staging area directory.</param>
    public abstract void Transfer(StudyViewModel[] studies, object args, string stagingArea);


    /// <summary>
    /// Creates a default staging area.
    /// </summary>
    /// <param name="studies">The studies to be transferred.</param>
    /// <param name="stagingArea">The pre-created directory in which the staging area will be housed.</param>
    public virtual void CreateStagingArea(StudyViewModel[] studies, string stagingArea)
    {
      foreach(StudyViewModel study in studies)
      {
        study.StagingArea = stagingArea;
        Directory.CreateDirectory(study.StagingUrl);

        foreach(SeriesViewModel series in study.Series)
        {
          if (series.IsEligibleForTransfer || series.IsRequiredForTransfer)
          {
            series.StagingArea = stagingArea;
            Directory.CreateDirectory(series.StagingUrl);

            foreach(TransferItemViewModel item in series.TransferItems)
            {
              if(item.IsSelected)
              {
                string stagingItemDirectory = Path.Combine(series.StagingUrl, item.SearchInFolder);
                if(!Directory.Exists(stagingItemDirectory))
                {
                  Directory.CreateDirectory(stagingItemDirectory);
                }

                // Item was selected, transfer full contents.
                foreach(string file in item.RetrieveFiles(series.Url))
                {
                  string stagingItemFilename = Path.Combine(stagingItemDirectory, Path.GetFileName(file));
                  if (!File.Exists(stagingItemFilename))
                  {
                    File.Copy(file, stagingItemFilename);
                  }
                }
              }
              else if(item.IsRequired)
              {
                // Item is required, transfer first file.

                string stagingItemDirectory = Path.Combine(series.StagingUrl, item.SearchInFolder);
                if(!Directory.Exists(stagingItemDirectory))
                {
                  Directory.CreateDirectory(stagingItemDirectory);
                }

                string[] files = item.RetrieveFiles(series.Url);
                if(files.Length > 0)
                {
                  string stagingItemFilename = Path.Combine(stagingItemDirectory, Path.GetFileName(files[0]));
                  if(!File.Exists(stagingItemFilename))
                  {
                    File.Copy(files[0], stagingItemFilename);
                  }
                }
              }
            }

            // Copy all files in the series directory if they haven't been copied over already.
            foreach(string file in Directory.GetFiles(series.Url))
            {
              string stagingFilename = Path.Combine(series.Url, Path.GetFileName(file));
              if(!File.Exists(Path.Combine(stagingFilename)))
              {
                File.Copy(file, stagingFilename);
              }
            }

            series.Serialize(series.StagingUrl);
          }
        }

        study.Serialize(study.StagingUrl);
      }
    } // CreateStagingArea( studies, directory )


    /// <summary>
    /// Builds the Files table for the studies for transfer.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="staging">The staging directory.</param>
    public virtual void BuildFileList(StudyViewModel[] studies, string staging)
    {
      foreach(StudyViewModel study in studies)
      {
        foreach(SeriesViewModel series in study.Series)
        {
          if (Directory.Exists(series.StagingUrl))
          {
            string[] files = Directory.GetFiles(series.StagingUrl, "*.*", SearchOption.AllDirectories);
            foreach (string file in files)
            {
              series.Series.Files.Add(file);
            }
          }
        }
      }
    } // BuildFileList( studies, staging )


    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    protected virtual void OnProgress(ProgressEventArgs args)
    {
      if(Progress != null)
      {
        Progress(this, args);
      }
    } // OnProgress( args )

    #endregion

  } // class TransferModel
} // namespace Viatronix.Utilities.Transfer.Models
