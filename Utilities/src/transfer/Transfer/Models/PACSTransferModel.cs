using System;
using System.IO;
using System.Drawing;
using System.Configuration;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading;
using System.Threading.Tasks;

using Viatronix.Pdf;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Utilities;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer.Models
{
  /// <summary>
  /// Performs transfers to PACS.
  /// </summary>
  public class PACSTransferModel : TransferModel
  {

    #region fields

    /// <summary>
    /// The AE Title to 
    /// </summary>
    private string m_dicomDeviceAETitle = string.Empty;

    /// <summary>
    /// An event to send only one image at a time.
    /// </summary>
    private AutoResetEvent m_arevent = new AutoResetEvent(true);

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public PACSTransferModel()
    {
      m_dicomDeviceAETitle = ConfigurationManager.AppSettings["AETitle"];
    } // PACSTransferModel()

    #endregion

    #region methods

    /// <summary>
    /// Performs the transfer.
    /// </summary>
    /// <param name="studies">The studies to transfer.</param
    /// <param name="args">Arguments to take into account.</param>
    /// <param name="stagingArea">The staging area directory.</param>
    public override void Transfer(StudyViewModel[] studies, object args, string stagingArea)
    {
      DicomHost host = (DicomHost)args;

      foreach(StudyViewModel study in studies)
      {
        foreach(SeriesViewModel series in study.Series)
        {
          if(series.Type == SeriesTypes.Dicom && series.IsEligibleForTransfer)
          {
            TransferDicomSeries(host, study, series);
          }
          else if(series.Type == SeriesTypes.Session && series.IsEligibleForTransfer)
          {
            TransferSessionSeries(host, study, series);
          }
        }
      }
    } // Transfer( studies, args, stagingArea )


    /// <summary>
    /// Transfers the dicom series.
    /// </summary>
    /// <param name="host">The host to transfer to.</param>
    /// <param name="study">The study that applies to the series.</param>
    /// <param name="series">The series that we wish to transfer.</param>
    private void TransferDicomSeries(DicomHost host, StudyViewModel study, SeriesViewModel series)
    {
      CStore store = CStore.Create(m_dicomDeviceAETitle, host.AETitle, host.Server, host.Port);
      foreach (string file in series.RetrieveFilesFromStaging("Dicom"))
      {
        store.AddFile(file);
      }

      store.Start();
      store.Wait();
    } // TransferDicomSeries( host, study, series )


    /// <summary>
    /// Transfers a session series.
    /// </summary>
    /// <param name="host">The host to transfer to.</param>
    /// <param name="study">The study that applies to the series.</param>
    /// <param name="series">The series that we wish to transfer.</param>
    private void TransferSessionSeries(DicomHost host, StudyViewModel study, SeriesViewModel series)
    {
      bool transferSnapshots = series.TransferItems.Find("Snapshots").IsSelected;
      bool transferMovies = series.TransferItems.Find("Movies").IsSelected;
      bool transferReports = series.TransferItems.Find("Reports").IsSelected;

      if(transferSnapshots)
      {
        uint instanceNumber = 0;
        Dicom.Image image = FindDicomImage(study, series);
        image.SeriesInstanceUid = Dicom.Utility.GenerateUid();

        foreach (string file in series.RetrieveFilesFromStaging("Snapshots"))
        {
          string ext = Path.GetExtension(file).ToLower();
          if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
          {
            using (Bitmap bitmap = (Bitmap)Bitmap.FromFile(file))
            {
              TransferBitmap(host, bitmap, image, instanceNumber++);
            }
          }
        }
      }

      if (transferMovies)
      {
        foreach (string file in series.RetrieveFilesFromStaging("Movies"))
        {
          if (MovieExtracter.Supports(file))
          {
            Dicom.Image image = FindDicomImage(study, series);
            image.SeriesInstanceUid = Dicom.Utility.GenerateUid();
            TransferMovie(host, file, image);
          }
        }
      }

      if (transferReports)
      {
        foreach (string file in series.RetrieveFilesFromStaging("Reports"))
        {
          if (Path.GetExtension(file).ToLower() == ".pdf")
          {
            uint instanceNumber = 0;
            Dicom.Image image = FindDicomImage(study, series);

            PdfReader reader = PdfReader.Create(file);
            foreach(Bitmap page in reader.Pages)
            {
              TransferBitmap(host, page, image, instanceNumber++);
            }
          }
        }
      }
    } // TransferSessionSeries( host, study, series )


    /// <summary>
    /// Transfers a movie file.
    /// </summary>
    /// <param name="host">The host to transfer to.</param>
    /// <param name="file"></param>
    private void TransferMovie(DicomHost host, string movieFile, Viatronix.Dicom.Image image)
    {
      uint instanceNumber = 0;
      AutoResetEvent arevent = new AutoResetEvent(true);

      MovieExtracter extractor = new MovieExtracter();
      extractor.ExtractionStarted += new ExtractionEventHandler((sender, args) =>
      {
        OnProgress(new ProgressEventArgs("Starting Movie Transfer...", 0, 100));
      });

      extractor.ExtractionCompleted += new ExtractionEventHandler((sender, args) =>
      {
        OnProgress(new ProgressEventArgs("Movie Transfer was successful!", 100, 100));
      });

      extractor.ExtractionFailed += new ExtractionEventHandler((sender, args) =>
      {
        Log.Error("Movie frame extraction has failed.", "PACSTransferModel", "TransferMovie");
        OnProgress(new ProgressEventArgs("Movie Transfer has failed.", 100, 100));

        throw new AbortException(args.Message);
      });

      extractor.FrameExtracted += new ExtractionEventHandler((sender, args) =>
      {
        OnProgress(new ProgressEventArgs(string.Format("Transferring frame {0} of {1}.", args.FrameNumber, args.TotalFrames), (int)(((double)args.FrameNumber / (double)args.TotalFrames) * 100.0), 100));

        using (Bitmap bitmap = new Bitmap(args.Frame))
        {
          TransferBitmap(host, args.Frame, image, instanceNumber++);
        }
      });
      
      extractor.ExtractFrames(movieFile);
    } // TransferMovie( host, movieFile, image )


    /// <summary>
    /// Transfers the frame.
    /// </summary>
    /// <param name="host">The host to transfer to.</param>
    /// <param name="bitmap">The pixel dataset.</param>
    /// <param name="image">The dicom image to seed.</param>
    /// <param name="instanceNumber">The instance number.</param>
    private void TransferBitmap(DicomHost host, Bitmap bitmap, Viatronix.Dicom.Image image, uint instanceNumber)
    {
      try
      {
        m_arevent.WaitOne();
        m_arevent.Reset();

        CStore store = CStore.Create(m_dicomDeviceAETitle, host.AETitle, host.Server, host.Port);

        image.InstanceUid = Viatronix.Dicom.Utility.GenerateUid();
        image.InstanceNumber = instanceNumber;
        image.SetPixelData(bitmap);

        store.AddImage(image);

        store.Start();

        store.Wait();
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Error occurred during transfer...  [ERROR=\"{0}\"]", e.Message), "PACSTransferModel", "TransferBitmap");
        throw;
      }
      finally
      {
        m_arevent.Set();
      }
    } // TransferBitmap( host, bitmap, image, instanceNumber )


    /// <summary>
    /// Finds the dicom image for the session to transfer any session items.
    /// </summary>
    /// <param name="study">The study.</param>
    /// <param name="sessionSeries">The session series.</param>
    private Viatronix.Dicom.Image FindDicomImage(StudyViewModel study, SeriesViewModel sessionSeries)
    {
      SeriesViewModel dicomSeries = null;
      foreach(SeriesViewModel seriesViewModel in study.Series)
      {
        if (seriesViewModel.Series.Type == SeriesTypes.Dicom)
        {
          dicomSeries = seriesViewModel;
          break;
        }
      }

      if (dicomSeries != null)
      {
        string[] dcmFiles = dicomSeries.RetrieveFilesFromStaging("Dicom");
        if (dcmFiles.Length > 0)
        {
          // Creates the dataset using the dicom file
          IImage seed = ImageFactory.Create(dcmFiles[0], ParameterType.FILE);
          Viatronix.Dicom.Image dataset = new Viatronix.Dicom.Image();

          string value = string.Empty;

          // Image Type is set when setting PixelData.
          //if (seed.TryGetValue(0x0008, 0x0008, ref value))
          //  dataset.SetValue<String>(0x0008, 0x0008, value);

          // Study Date
          if (seed.TryGetValue(0x0008, 0x0020, ref value))
            dataset.SetValue<String>(0x0008, 0x0020, value);

          // Study Time
          if (seed.TryGetValue(0x0008, 0x0030, ref value))
            dataset.SetValue<String>(0x0008, 0x0030, value);

          dataset.SetValue<String>(0x0008, 0x0021, DateTime.Now.ToString("yyyymmdd"));
          dataset.SetValue<String>(0x0008, 0x0031, DateTime.Now.ToString("HHMMss"));

          if (string.IsNullOrEmpty(sessionSeries.Series.Modality))
          {
            if (seed.TryGetValue(0x0008, 0x0060, ref value))
              dataset.SetValue<String>(0x0008, 0x0060, value);
          }
          else
            dataset.SetValue<String>(0x0008, 0x0060, sessionSeries.Series.Modality);

          // Manufacturer
          dataset.SetValue<String>(0x0008, 0x0070, "Viatronix Inc.");

          // Institution Name
          if (seed.TryGetValue(0x0008, 0x0080, ref value))
            dataset.SetValue<String>(0x0008, 0x0080, value);

          // Patient Name
          if (seed.TryGetValue(0x0010, 0x0010, ref value))
            dataset.SetValue<String>(0x0010, 0x0010, value);

          // Patient Id
          if (seed.TryGetValue(0x0010, 0x0020, ref value))
            dataset.SetValue<String>(0x0010, 0x0020, value);

          // Patient Sex
          if (seed.TryGetValue(0x0010, 0x0040, ref value))
            dataset.SetValue<String>(0x0010, 0x0040, value);

          // Patient Age
          if (seed.TryGetValue(0x0010, 0x1010, ref value))
            dataset.SetValue<String>(0x0010, 0x1010, value);

          // Study Description
          if (seed.TryGetValue(0x0008, 0x1030, ref value))
            dataset.SetValue<String>(0x0008, 0x1030, value);

          // Study Uid
          if (seed.TryGetValue(0x0020, 0x000d, ref value))
            dataset.SetValue<String>(0x0020, 0x000d, value);

          // Study Id
          if (seed.TryGetValue(0x0020, 0x0010, ref value))
            dataset.SetValue<String>(0x0020, 0x0010, value);

          // Accession Number
          if (seed.TryGetValue(0x0008, 0x0050, ref value))
            dataset.SetValue<String>(0x0008, 0x0050, value);

          // Series UID
          dataset.SetValue<String>(0x0020, 0x000e, Viatronix.Dicom.Utility.GenerateUid());

          // Setup the series reference sequence
          Sequence sequence = new Sequence(0x0008, 0x1115);

          if (dicomSeries.Series.Type == SeriesTypes.Dicom)
          {
            ElementCollection elementList = ElementCollection.Create();
            elementList.Add(new Element(0x0020, 0x000e, dicomSeries.Uid));
            elementList.Add(new Element(0x0008, 0x0060, dicomSeries.Series.Modality));
            sequence.Add(elementList);
          }

          dataset.Insert(sequence);

          /// Returns the header as xml
          return dataset;
        }
      }

      return null;
    } // FindDicomImage( study, series )

    #endregion

  } // class PACSTransferModel
} // namespace Viatronix.Utilities.Transfer.Models
