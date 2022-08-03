using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// A model to transfer to a file system directory.
  /// </summary>
  public class FileSystemTransferModel : TransferModel
  {

    #region fields

    /// <summary>
    /// The number of remaining files.
    /// </summary>
    private int m_remainingFiles = 0;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public FileSystemTransferModel()
    {
    } // FileSystemTransferProvider()

    #endregion

    #region methods

    /// <summary>
    /// Creates the staging area.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="stagingArea">The staging area.</param>
    public override void CreateStagingArea(StudyViewModel[] studies, string stagingArea)
    {
      base.CreateStagingArea(studies, stagingArea);

      foreach (StudyViewModel study in studies)
      {
        foreach (SeriesViewModel series in study.Series)
        {
          if (series.Type == SeriesTypes.Dicom)
          {
            // --------------------------------------------
            // Copy over any volumes that exist that apply.
            // ============================================

            string volumesStaging = Path.Combine(series.StagingUrlShortPath, "volumes");
            if (!Directory.Exists(volumesStaging))
            {
              Directory.CreateDirectory(volumesStaging);
            }

            foreach (Volume volume in series.Series.Volumes)
            {
              string volumeFilename = Path.Combine(volume.Url, volume.FileName);
              if (File.Exists(volumeFilename))
              {
                string volumeStagingDirectory = Path.Combine(volumesStaging, volume.ConversionId);
                if (!Directory.Exists(volumeStagingDirectory))
                {
                  Directory.CreateDirectory(volumeStagingDirectory);
                }

                string volumeStagingFilename = Path.Combine(volumeStagingDirectory, volume.FileName);
                File.Copy(volumeFilename, volumeStagingFilename);
              }
            }
          }
          else if (series.Type == SeriesTypes.Session)
          {
            // --------------------------------------
            // Copy all session files to the staging.
            // ======================================

            foreach (string file in Directory.GetFiles(series.Url, "*.*", SearchOption.AllDirectories))
            {
              if (Path.GetFileName(file) != Series.XmlTag)
              {
                string relativeFilePath = file.Replace(series.Url, "").TrimStart('\\').TrimEnd('\\').Trim();

                string relativeDirectoryPath = Path.GetDirectoryName(relativeFilePath);
                if (relativeDirectoryPath != string.Empty && !Directory.Exists(Path.Combine(series.StagingUrlShortPath, relativeDirectoryPath)))
                {
                  Directory.CreateDirectory(Path.Combine(series.StagingUrlShortPath, relativeDirectoryPath));
                }

                string destFileFullPath = Path.Combine(series.StagingUrlShortPath, relativeFilePath);
                if (!File.Exists(destFileFullPath))
                {
                  File.Copy(file, destFileFullPath);
                }
              }
            }
          }
        }
      }
    } // CreateStagingArea( studies, stagingArea )


    /// <summary>
    /// Transfers the data to the specified directory.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param>
    /// <param name="args">Arguments to take into account.</param>
    /// <param name="stagingArea">The staging area directory.</param>
    public override void Transfer(ViewModels.StudyViewModel[] studies, object args, string stagingArea)
    {
      string destination = Path.Combine(System.Convert.ToString(args), Guid.NewGuid().ToString("D"));

      try
      {
        foreach (string directory in Directory.GetDirectories(stagingArea))
        {
          m_remainingFiles = Viatronix.Utilities.IO.PathUtilities.GetFileCount(directory, true);
          CopyDirectory(directory, destination, false, false, true, m_remainingFiles);
        }
      }
      catch(Exception)
      {
        // Any error occurs delete the directory.
        Directory.Delete(destination, true);
      }
    } // Transfer( studies, args, stagingArea )


    /// <summary>
    /// Copies the source diretory to the destination directory.
    /// </summary>
    /// <param name="source">Source directory</param>
    /// <param name="destination">Destination directory</param>
    /// <param name="rename">
    /// Rename flag indicates whether the destination is an exact path or a parent to the sources path. If true then it means we are looking to 
    /// rename the directory if false we are looking to copy the source into the destination as a sub-directory.
    /// </param>
    /// <param name="overWrite">Flag indicating you want to overwrite the destination if it exists.</param>
    /// <param name="recursive">Copies all files and sub-directories recursivly.</param>
    /// <param name="totalFiles">The total number of files.</param>
    /// <param name="remaining">The number of remaining files.</param>
    private void CopyDirectory(string source, string destination, bool rename, bool overWrite, bool recursive, int totalFiles)
    {
      if (Directory.Exists(source))
      {
        string destPath = (rename ? destination : string.Concat(destination, Path.DirectorySeparatorChar.ToString(), Path.GetFileName(source)));

        /////////////////////////////////////////////////////
        // Make sure the destination does not already exist
        /////////////////////////////////////////////////////
        if (Directory.Exists(destPath))
        {
          /////////////////////////////////////////////
          // If overwrite is set delete the destination
          /////////////////////////////////////////////
          if (overWrite)
          {
            Viatronix.Utilities.IO.PathUtilities.DeleteDirectoryContents(destPath, true);
          } // END ... If we are to overwrite
          else
          {
            Log.Error(string.Concat("Path already exists [", destPath, "] and overwrite was not specified"), "PathUtilities", "CopyDirectory");
            throw new System.IO.IOException("Path already exists");
          } // END ... Else not overwrite
        } // END ... If the paths exists
        else
        {
          Log.Debug(string.Concat("Creating destination directory [", destPath, "]"), "PathUtilities", "CopyDirectory");
          Directory.CreateDirectory(destPath);
        }

        Log.Debug(string.Concat("Creating destination directory [", destPath, "]"), "PathUtilities", "CopyDirectory");
        Directory.CreateDirectory(destPath);

        // Get all files in the source
        string[] files = Directory.GetFiles(source);

        int filesCopied = totalFiles - m_remainingFiles;

        //////////////////
        // Copy each  file
        //////////////////
        for (int i = 0; i < files.Length; ++i)
        {
          int completed = 10 + (int)((((double)filesCopied++ / (double)totalFiles)) * 90.0);
          OnProgress(new ProgressEventArgs(string.Format("Copying file {0} of {1}.", filesCopied, totalFiles), completed, 100));

          string destFile = Path.Combine(destPath, Path.GetFileName(files[i]));
          File.Copy(files[i], destFile, true);
        } // END ... for each file

        m_remainingFiles -= files.Length;

        //////////////////////////////////////////
        // Only copy sub-directories is recursive
        //////////////////////////////////////////
        if (recursive)
        {
          string[] dirs = Directory.GetDirectories(source);

          ///////////////////////////////////////
          // For each sub-directory call ourself
          ///////////////////////////////////////
          for (int i = 0; i < dirs.Length; i++)
          {
            // Call ourself
            CopyDirectory(dirs[i], destPath, false, overWrite, recursive, totalFiles);
          } // END ... for each directory

        } // END ... If recursive
      }
    } // CopyDirectory( source, destination, rename, overWrite, recursive, totalFiles )

    #endregion

  } // class FileSystemTransferModel
} // namespace Viatronix.Utilities.Transfer.Models
