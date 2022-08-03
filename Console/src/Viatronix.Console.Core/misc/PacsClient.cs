using System;
using System.Data;
using System.Data.Common;
using System.Data.SqlTypes;
using System.Drawing;
using System.ComponentModel;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Dicom;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
  /// <summary>
  /// The pacs client for transfer and searching.
  /// </summary>
  public class PacsClient : IDisposable
  {

    #region events

    /// <summary>
    /// Notifies that the store has started.
    /// </summary>
    public event ClientEventHandler StoreStarted;

    /// <summary>
    /// Notifies that the store has failed.
    /// </summary>
    public event ClientEventHandler StoreFailed;

    /// <summary>
    /// Notifies that the progress has changed.
    /// </summary>
    public event ClientEventHandler StoreUpdated;

    /// <summary>
    /// Notifies that the store has completed.
    /// </summary>
    public event ClientEventHandler StoreCompleted;

    #endregion

    #region fields

    /// <summary>
    /// The CStore for sending data to the pacs.
    /// </summary>
    private CStore m_store = null;

    /// <summary>
    /// The ae title.
    /// </summary>
    private string m_aeTitle = string.Empty;

    /// <summary>
    /// The host we're connecting to.
    /// </summary>
    private DicomHost m_host = null;

    /// <summary>
    /// Keeps track of our connected state.
    /// </summary>
    private bool m_isConnected = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public PacsClient()
    {
    } // PacsClient()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="aeTitle">The aetitle of the pacs.</param>
    public PacsClient(string aeTitle)
    {
      m_aeTitle = aeTitle;
    } // PacsClient( aeTitle )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the host.
    /// </summary>
    public DicomHost Host
    {
      get { return m_host; }
      set { m_host = value; }
    } // Host


    /// <summary>
    /// GetsB the connected state.
    /// </summary>
    public bool IsConnected
    {
      get { return m_isConnected; }
    } // IsConnected

    #endregion

    #region methods

    /// <summary>
    /// Connects to the PACS.
    /// </summary>
    /// <param name="connectionType">The connection type.</param>
    public void Connect(string connectionType)
    {
      if (m_aeTitle == string.Empty)
      {
        List<DicomServer> servers = Global.DataProvider.GetDicomServers();


        if (servers.Count > 0 )
        {
          m_aeTitle = servers[0].AETitle;
        }
        else
        {
          throw new ApplicationException("Cannot connect to pacs.  Dicom AETitle wasn't specified and no dicom servers are running.");
        }
      }

      m_isConnected = true;
    } // Connect( connectionType )


    /// <summary>
    /// Disconnects from the pacs.
    /// </summary>
    public void Disconnect()
    {
      m_isConnected = false;
    } // Disconnect()


    /// <summary>
    /// Stores the series path to pacs.
    /// </summary>
    /// <param name="series">The series to store.</param>
    /// <param name="modality">The modality.</param>
    /// <param name="file">The file path.</param>
    /// <param name="wait">Should we wait for the command to come back before continuing?</param>
    public void StoreSeriesPath(Series series, string modality, string file, bool wait)
    {
      // This is used for archival.  This doesn't exist anymore in the console.
      throw new ApplicationException("PACS Archival Not Implemented.");
    } // StoreSeriesPath( series, modality, file, wait )


    /// <summary>
    /// Stores the snapshots to pacs.
    /// </summary>
    /// <param name="series"></param>
    /// <param name="modality"></param>
    /// <param name="snapshots"></param>
    /// <param name="dicomFile"></param>
    /// <param name="wait"></param>
    public void StoreSnapshots(Series series, string modality, ArrayList snapshots, string dicomFile, bool wait)
    {
      StoreBitmaps(series, modality, dicomFile, snapshots, wait);
    } // StoreSnapshots( series, modality, snapshots, dicomFile, wait )


    /// <summary>
    /// Stores the movies.
    /// </summary>
    /// <param name="subType"></param>
    /// <param name="modality"></param>
    /// <param name="frames"></param>
    /// <param name="dicomFile"></param>
    /// <param name="wait"></param>
    public void StoreMovie(Series series, string modality, ArrayList frames, string dicomFile, bool wait)
    {
      StoreBitmaps(series, modality, dicomFile, frames, wait);
    } // StoreMovie( subType, modality, frames, dicomFile, wait )


    /// <summary>
    /// Store the report.
    /// </summary>
    /// <param name="series"></param>
    /// <param name="modality"></param>
    /// <param name="frames"></param>
    /// <param name="dicomFile"></param>
    /// <param name="something"></param>
    public void StoreReport(Series series, string modality, ArrayList frames, string dicomFile, bool wait)
    {
      StoreBitmaps(series, modality, dicomFile, frames, wait);
    } // StoreReport( series, modality, frames, dicomFile, wait )


    /// <summary>
    /// Store the report as encapsulated PDF
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="modality"></param>
    /// <param name="frames"></param>
    /// <param name="dicomFile"></param>
    /// <param name="something"></param>
    public void StoreReport(Series series, string modality, string pdfFile, string dicomFile, bool wait)
    {
      if (m_store != null)
      {
        m_store.Dispose();
        m_store = null;
      }

      m_store = CStore.Create(m_aeTitle, Host.ToXml());
      m_store.Started += new ClientEventHandler(OnStoreStartedHandler);
      m_store.Progress += new ClientEventHandler(OnStoreProgressHandler);
      m_store.Completed += new ClientEventHandler(OnStoreCompletedHandler);
      m_store.Failed += new ClientEventHandler(OnStoreFailedHandler);

      string header = CreateHeader(series, modality, dicomFile);

      m_store.AddImage((Viatronix.Dicom.Image)Viatronix.Dicom.ImageFactory.Create(pdfFile, header));

      m_store.Start();

      if (wait)
      {
        m_store.Wait();
      }


    } // StoreReport( series, modality, frames, dicomFile, wait )


    /// <summary>
    /// Stores the dicom images.
    /// </summary>
    /// <param name="files"></param>
    /// <param name="wait"></param>
    public void StoreImages(string[] files, bool wait)
    {
      if (m_store != null)
      {
        m_store.Dispose();
        m_store = null;
      }

      m_store = CStore.Create(m_aeTitle, Host.ToXml());
      m_store.Started += new ClientEventHandler(OnStoreStartedHandler);
      m_store.Progress += new ClientEventHandler(OnStoreProgressHandler);
      m_store.Completed += new ClientEventHandler(OnStoreCompletedHandler);
      m_store.Failed += new ClientEventHandler(OnStoreFailedHandler);

      foreach (string dcmFile in files)
      {
        m_store.AddFile(dcmFile);
      }

      m_store.Start();

      if (wait)
      {
        m_store.Wait();
      }
    } // StoreImages( files, wait )


    /// <summary>
    /// Stores the bitmaps.
    /// </summary>
    /// <param name="dicomFile">The original file.</param>
    /// <param name="array">The list of images.</param>
    /// <param name="wait">Should wait for transfer to complete or perform asynchronously.</param>
    private void StoreBitmaps(Series series, string modality, string dicomFile, ArrayList array, bool wait)
    {
      // Generate the uid for the new series.
      string seriesUid = Viatronix.Dicom.Utility.GenerateUid();

      if (m_store != null)
      {
        m_store.Dispose();
        m_store = null;
      }

      m_store = CStore.Create(m_aeTitle, Host.ToXml());
      m_store.Started += new ClientEventHandler(OnStoreStartedHandler);
      m_store.Progress += new ClientEventHandler(OnStoreProgressHandler);
      m_store.Completed += new ClientEventHandler(OnStoreCompletedHandler);
      m_store.Failed += new ClientEventHandler(OnStoreFailedHandler);

      string header = CreateHeader(series, modality, dicomFile);


      uint instanceNumber = 1;
 
      foreach (object obj in array)
      {
        Bitmap bitmap = null;

        Bitmap bmp = obj as Bitmap;
        if (bmp != null)
        {
          bitmap = bmp;
        }
        else
        {
          string filePath = obj as string;
          if (filePath != null)
          {
            System.IO.FileStream fs = System.IO.File.OpenRead(filePath);
            bitmap = (Bitmap)Bitmap.FromStream(fs);
            fs.Close();
          }
        }

        if (bitmap != null)
        {
          Viatronix.Dicom.Image image = (Viatronix.Dicom.Image)Viatronix.Dicom.ImageFactory.Create(header, bitmap);
          image.SeriesInstanceUid = seriesUid.ToString();
          image.InstanceNumber = instanceNumber++;
          m_store.AddImage(image);
        }
        
      }

      m_store.Start();

      if (wait)
      {
        m_store.Wait();
      }
    } // StoreBitmaps( dicomFile, array, wait )

    /// <summary>
    /// Creates the header using the dicom file for the base elements and the series to generate the 
    /// reference list
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="modality">Modality</param>
    /// <param name="dicomFile">Dicom file</param>
    /// <returns></returns>
    private string CreateHeader(Series series, string modality, string dicomFile)
    {

      //// Creates the dataset using the dicom file
      //Viatronix.Dicom.IImage dataset = Viatronix.Dicom.IImage.Create(dicomFile, ParameterType.FILE);

      //// Setup value
      //dataset.SeriesInstanceUid = Viatronix.Dicom.Utility.GenerateUid();
      //dataset.SeriesNumber = string.Empty;
      //dataset.AcquisitionNumber = 1;
      //dataset.Modality = modality;

      //// Setup the series reference sequence
      //Sequence sequence = new Sequence(0x0008, 0x1115);

      //foreach (Series refSeries in series.References)
      //{
      //  ElementCollection elementList = new ElementCollection();
      //  elementList.Insert(new Element(0x0020, 0x000e, refSeries.Uid.ToString()));
      //  elementList.Insert(new Element(0x0008, 0x0060, refSeries.Modality));
      //  sequence.Insert(elementList);
      //}

      //dataset.Insert(sequence);

      ///// Returns the header as xml
      //return dataset.ToXml();

      return string.Empty;

    } // CreateHeader(Series series, string modality, string dicomFile)

    #endregion

    #region IDispose implementation

    /// <summary>
    /// Disposes of the object.
    /// </summary>
    public void Dispose()
    {
      Disconnect();
    } // Dispose()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the store's Started event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreStartedHandler(object sender, ClientEventArgs args)
    {
      if(StoreStarted != null)
      {
        StoreStarted(sender, args);
      }
    } // OnStoreStartedHandler( sender, args )


    /// <summary>
    /// Handles the store's Progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreProgressHandler(object sender, ClientEventArgs args)
    {
      if (StoreUpdated != null)
      {
        StoreUpdated(sender, args);
      }
    } // OnStoreProgressHandler( sender, args )


    /// <summary>
    /// Handles the store's Completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreCompletedHandler(object sender, ClientEventArgs args)
    {
      if (StoreCompleted != null)
      {
        StoreCompleted(sender, args);
      }
    } // OnStoreCompletedHandler( sender, args )


    /// <summary>
    /// Handles the store's Failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnStoreFailedHandler(object sender, ClientEventArgs args)
    {
      if (StoreFailed != null)
      {
        StoreFailed(sender, args);
      }
    } // OnStoreFailedHandler( sender, args )

    #endregion

  } // class PacsClient
} //  Viatronix.Console.Core
