using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Dicom;
using Viatronix.Dicom.Imaging;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Reflection;
using System.Xml;
using System.Windows;
using System.Windows.Media;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Viatronix.Dicom.MVVM.Model;
using System.Windows.Threading;



namespace Viatronix.Dicom.MVVM.ViewModels
{


  /// <summary>
  /// Dicom Header View Model
  /// </summary>
  public class HeaderViewModel : Viatronix.UI.ViewModels.ViewModelBase, IDisposable
  {

     
    #region fields


    /// <summary>
    /// Image
    /// </summary>
    private ImageViewModel m_image;

    /// <summary>
    /// Can Save
    /// </summary>
    private bool m_canSave = false;

    /// <summary>
    /// Busy flag
    /// </summary>
    private bool m_busy = false;


    /// <summary>
    /// Selected Element
    /// </summary>
    private Element m_selectedElement = null;


    /// <summary>
    /// Open file command
    /// </summary>
    private RelayCommand m_openFileCommand;


    /// <summary>
    /// Save as Xml command
    /// </summary>
    private RelayCommand m_saveAsXmlCommand;

    /// <summary>
    /// Element collection
    /// </summary>
    private ElementCollection m_elementCollection = ElementCollection.Create();

    /// <summary>
    /// Loading thread
    /// </summary>
    private BackgroundWorker m_backgroundWorker = null;

    /// <summary>
    /// Collection of elements
    /// </summary>
    private ObservableCollection<DicomElement> m_elements = new ObservableCollection<DicomElement>();


    // Track whether Dispose has been called. 
    private bool m_disposed = false;

    /// <summary>
    /// Depth
    /// </summary>
    private int m_depth = 0;
  

    #endregion


    #region constructors/destructors

    public HeaderViewModel()
    {

      DispatcherHelper.Initialize();

 
      //if (IsInDesignMode)
      //{

      //  string xml = "<data-set xfer=\"1.2.840.10008.1.2\" name=\"LittleEndianImplicit\">  <element tag=\"0008,0005\" vr=\"CS\" vm=\"1\" len=\"10\" name=\"SpecificCharacterSet\">ISO_IR 100</element>  <element tag=\"0008,0008\" vr=\"CS\" vm=\"4\" len=\"34\" name=\"ImageType\">ORIGINAL\\PRIMARY\\AXIAL\\CT_SOM5 SPI</element>  <element tag=\"0008,0016\" vr=\"UI\" vm=\"1\" len=\"26\" name=\"SOPClassUID\">1.2.840.10008.5.1.4.1.1.2</element>  <element tag=\"0008,0018\" vr=\"UI\" vm=\"1\" len=\"56\" name=\"SOPInstanceUID\">1.3.12.2.1107.5.1.4.54012.3000000410200648174680000</element>  <element tag=\"0008,0020\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"StudyDate\">20041020</element>  <element tag=\"0008,0021\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"SeriesDate\">20041020</element>  <element tag=\"0008,0022\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"AcquisitionDate\">20041020</element>  <element tag=\"0008,0023\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"ContentDate\">20041020</element>  <element tag=\"0008,0030\" vr=\"TM\" vm=\"1\" len=\"14\" name=\"StudyTime\">095437.734000</element>  <element tag=\"0008,0031\" vr=\"TM\" vm=\"1\" len=\"14\" name=\"SeriesTime\">100645.171000</element>  <element tag=\"0008,0032\" vr=\"TM\" vm=\"1\" len=\"14\" name=\"AcquisitionTime\">100317.962703</element>  <element tag=\"0008,0033\" vr=\"TM\" vm=\"1\" len=\"14\" name=\"ContentTime\">100317.962703</element>  <element tag=\"0008,0050\" vr=\"SH\" vm=\"1\" len=\"10\" name=\"AccessionNumber\">0045790677</element>  <element tag=\"0008,0060\" vr=\"CS\" vm=\"1\" len=\"2\" name=\"Modality\">CT</element>  <element tag=\"0008,0070\" vr=\"LO\" vm=\"1\" len=\"8\" name=\"Manufacturer\">SIEMENS</element>  <element tag=\"0008,0080\" vr=\"LO\" vm=\"1\" len=\"28\" name=\"InstitutionName\">Radiology</element>  <element tag=\"0008,0090\" vr=\"PN\" vm=\"1\" len=\"6\" name=\"ReferringPhysiciansName\">RAGCT</element>  <element tag=\"0008,1010\" vr=\"SH\" vm=\"1\" len=\"6\" name=\"StationName\">GRA1CV</element>  <element tag=\"0008,1030\" vr=\"LO\" vm=\"1\" len=\"30\" name=\"StudyDescription\">Abdomen^CTColonography (Adult)</element>  <element tag=\"0008,103e\" vr=\"LO\" vm=\"1\" len=\"24\" name=\"SeriesDescription\">Colo_supine  0.75  B20f</element>  <element tag=\"0008,1090\" vr=\"LO\" vm=\"1\" len=\"12\" name=\"ManufacturersModelName\">Sensation 64</element>  <sequence tag=\"0008,1140\" vr=\"SQ\" card=\"1\" name=\"ReferencedImageSequence\">    <item card=\"2\">      <element tag=\"0008,1150\" vr=\"UI\" vm=\"1\" len=\"26\" name=\"ReferencedSOPClassUID\">1.2.840.10008.5.1.4.1.1.2</element>      <element tag=\"0008,1155\" vr=\"UI\" vm=\"1\" len=\"56\" name=\"ReferencedSOPInstanceUID\">1.3.12.2.1107.5.1.4.54012.30000004102005415482800001078</element>    </item>  </sequence>  <sequence tag=\"0008,2112\" vr=\"SQ\" card=\"1\" name=\"SourceImageSequence\">    <item card=\"2\">      <element tag=\"0008,1150\" vr=\"UI\" vm=\"1\" len=\"20\" name=\"ReferencedSOPClassUID\">1.3.12.2.1107.5.9.1</element>      <element tag=\"0008,1155\" vr=\"UI\" vm=\"1\" len=\"56\" name=\"ReferencedSOPInstanceUID\">1.3.12.2.1107.5.1.4.54012.30000004102005415482800001123</element>    </item>  </sequence>  <element tag=\"0009,0010\" vr=\"LO\" vm=\"1\" len=\"20\" name=\"PrivateCreator\">SIEMENS CT VA1 DUMMY</element>  <element tag=\"0010,0010\" vr=\"PN\" vm=\"1\" len=\"20\" name=\"PatientsName\">Boniberger^Alexander</element>  <element tag=\"0010,0020\" vr=\"LO\" vm=\"1\" len=\"10\" name=\"PatientID\">0010999481</element>  <element tag=\"0010,0030\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"PatientsBirthDate\">19470608</element>  <element tag=\"0010,0040\" vr=\"CS\" vm=\"1\" len=\"2\" name=\"PatientsSex\">M</element>  <element tag=\"0010,1010\" vr=\"AS\" vm=\"1\" len=\"4\" name=\"PatientsAge\">057Y</element>  <element tag=\"0010,1020\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"PatientsSize\">1.78</element>  <element tag=\"0010,1030\" vr=\"DS\" vm=\"1\" len=\"2\" name=\"PatientsWeight\">81</element>  <element tag=\"0010,1040\" vr=\"LO\" vm=\"1\" len=\"34\" name=\"PatientsAddress\">Schwedenberg^13^85298^Scheyern^DE</element>  <element tag=\"0010,21c0\" vr=\"US\" vm=\"1\" len=\"2\" name=\"PregnancyStatus\">1</element>  <element tag=\"0018,0015\" vr=\"CS\" vm=\"1\" len=\"8\" name=\"BodyPartExamined\">ABDOMEN</element>  <element tag=\"0018,0050\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"SliceThickness\">0.75</element>  <element tag=\"0018,0060\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"KVP\">120</element>  <element tag=\"0018,0090\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"DataCollectionDiameter\">500</element>  <element tag=\"0018,1000\" vr=\"LO\" vm=\"1\" len=\"6\" name=\"DeviceSerialNumber\">54012</element>  <element tag=\"0018,1020\" vr=\"LO\" vm=\"1\" len=\"18\" name=\"SoftwareVersions\">syngo CT 2005A2.2</element>  <element tag=\"0018,1030\" vr=\"LO\" vm=\"1\" len=\"14\" name=\"ProtocolName\">CTColonography</element>  <element tag=\"0018,1100\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"ReconstructionDiameter\">357</element>  <element tag=\"0018,1110\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"DistanceSourceToDetector\">1040</element>  <element tag=\"0018,1111\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"DistanceSourceToPatient\">570</element>  <element tag=\"0018,1120\" vr=\"DS\" vm=\"1\" len=\"2\" name=\"GantryDetectorTilt\">0</element>  <element tag=\"0018,1130\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"TableHeight\">183</element>  <element tag=\"0018,1140\" vr=\"CS\" vm=\"1\" len=\"2\" name=\"RotationDirection\">CW</element>  <element tag=\"0018,1150\" vr=\"IS\" vm=\"1\" len=\"4\" name=\"ExposureTime\">500</element>  <element tag=\"0018,1151\" vr=\"IS\" vm=\"1\" len=\"4\" name=\"XRayTubeCurrent\">346</element>  <element tag=\"0018,1152\" vr=\"IS\" vm=\"1\" len=\"4\" name=\"Exposure\">123</element>  <element tag=\"0018,1160\" vr=\"SH\" vm=\"1\" len=\"2\" name=\"FilterType\">0</element>  <element tag=\"0018,1170\" vr=\"IS\" vm=\"1\" len=\"2\" name=\"GeneratorPower\">40</element>  <element tag=\"0018,1190\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"FocalSpots\">1.2</element>  <element tag=\"0018,1200\" vr=\"DA\" vm=\"1\" len=\"8\" name=\"DateOfLastCalibration\">20041020</element>  <element tag=\"0018,1201\" vr=\"TM\" vm=\"1\" len=\"14\" name=\"TimeOfLastCalibration\">074655.000000</element>  <element tag=\"0018,1210\" vr=\"SH\" vm=\"1\" len=\"4\" name=\"ConvolutionKernel\">B20f</element>  <element tag=\"0018,5100\" vr=\"CS\" vm=\"1\" len=\"4\" name=\"PatientPosition\">HFS</element>  <element tag=\"0019,0010\" vr=\"LO\" vm=\"1\" len=\"20\" name=\"PrivateCreator\">SIEMENS CT VA0  COAD</element>  <element tag=\"0019,10b0\" vr=\"DS\" vm=\"1\" len=\"4\" name=\"FeedPerRotation\">26.9</element>  <element tag=\"0020,000d\" vr=\"UI\" vm=\"1\" len=\"60\" name=\"StudyInstanceUID\">1.3.12.2.1107.5.8.3.485257.834855.80675056.2004102007195698</element>  <element tag=\"0020,000e\" vr=\"UI\" vm=\"1\" len=\"56\" name=\"SeriesInstanceUID\">1.3.12.2.1107.5.1.4.54012.30000004102006481746800001289</element>  <element tag=\"0020,0010\" vr=\"SH\" vm=\"1\" len=\"6\" name=\"StudyID\">724795</element>  <element tag=\"0020,0011\" vr=\"IS\" vm=\"1\" len=\"2\" name=\"SeriesNumber\">4</element>  <element tag=\"0020,0012\" vr=\"IS\" vm=\"1\" len=\"2\" name=\"AcquisitionNumber\">2</element>  <element tag=\"0020,0013\" vr=\"IS\" vm=\"1\" len=\"4\" name=\"InstanceNumber\">831</element>  <element tag=\"0020,0032\" vr=\"DS\" vm=\"3\" len=\"38\" name=\"ImagePositionPatient\">-196.1513671875\\-361.1513671875\\-752.5</element>  <element tag=\"0020,0037\" vr=\"DS\" vm=\"6\" len=\"12\" name=\"ImageOrientationPatient\">1\\0\\0\\0\\1\\0</element>  <element tag=\"0020,0052\" vr=\"UI\" vm=\"1\" len=\"56\" name=\"FrameOfReferenceUID\">1.3.12.2.1107.5.1.4.54012.30000004102005415482800001076</element>  <element tag=\"0020,1040\" vr=\"LO\" vm=\"0\" len=\"0\" name=\"PositionReferenceIndicator\"></element>  <element tag=\"0020,1041\" vr=\"DS\" vm=\"1\" len=\"6\" name=\"SliceLocation\">-752.5</element>  <element tag=\"0020,4000\" vr=\"LT\" vm=\"1\" len=\"6\" name=\"ImageComments\">Nativ</element>  <element tag=\"0021,0010\" vr=\"LO\" vm=\"1\" len=\"12\" name=\"PrivateCreator\">SIEMENS MED</element>  <element tag=\"0021,1011\" vr=\"DS\" vm=\"2\" len=\"6\" name=\"Target\">-18\0</element>  <element tag=\"0028,0002\" vr=\"US\" vm=\"1\" len=\"2\" name=\"SamplesPerPixel\">1</element>  <element tag=\"0028,0004\" vr=\"CS\" vm=\"1\" len=\"12\" name=\"PhotometricInterpretation\">MONOCHROME2</element>  <element tag=\"0028,0010\" vr=\"US\" vm=\"1\" len=\"2\" name=\"Rows\">512</element>  <element tag=\"0028,0011\" vr=\"US\" vm=\"1\" len=\"2\" name=\"Columns\">512</element>  <element tag=\"0028,0030\" vr=\"DS\" vm=\"2\" len=\"24\" name=\"PixelSpacing\">0.697265625\0.697265625</element>  <element tag=\"0028,0100\" vr=\"US\" vm=\"1\" len=\"2\" name=\"BitsAllocated\">16</element>  <element tag=\"0028,0101\" vr=\"US\" vm=\"1\" len=\"2\" name=\"BitsStored\">12</element>  <element tag=\"0028,0102\" vr=\"US\" vm=\"1\" len=\"2\" name=\"HighBit\">11</element>  <element tag=\"0028,0103\" vr=\"US\" vm=\"1\" len=\"2\" name=\"PixelRepresentation\">0</element>  <element tag=\"0028,0106\" vr=\"xs\" vm=\"1\" len=\"2\" name=\"SmallestImagePixelValue\">0</element>  <element tag=\"0028,0107\" vr=\"xs\" vm=\"1\" len=\"2\" name=\"LargestImagePixelValue\">2574</element>  <element tag=\"0028,1050\" vr=\"DS\" vm=\"2\" len=\"8\" name=\"WindowCenter\">60\\-600</element>  <element tag=\"0028,1051\" vr=\"DS\" vm=\"2\" len=\"8\" name=\"WindowWidth\">460\\1400</element>  <element tag=\"0028,1052\" vr=\"DS\" vm=\"1\" len=\"6\" name=\"RescaleIntercept\">-1024</element>  <element tag=\"0028,1053\" vr=\"DS\" vm=\"1\" len=\"2\" name=\"RescaleSlope\">1</element>  <element tag=\"0028,1055\" vr=\"LO\" vm=\"2\" len=\"16\" name=\"WindowCenterWidthExplanation\">WINDOW1\\WINDOW2</element>  <element tag=\"0029,0010\" vr=\"LO\" vm=\"1\" len=\"18\" name=\"PrivateCreator\">SIEMENS CSA HEADER</element>  <element tag=\"0029,0011\" vr=\"LO\" vm=\"1\" len=\"22\" name=\"PrivateCreator\">SIEMENS MEDCOM HEADER</element>  <element tag=\"0029,1008\" vr=\"CS\" vm=\"1\" len=\"6\" name=\"CSAImageHeaderType\">SOM 5</element>  <element tag=\"0029,1009\" vr=\"LO\" vm=\"1\" len=\"12\" name=\"CSAImageHeaderVersion\">VA10A 971201</element>  <element tag=\"0029,1010\" vr=\"OB\" vm=\"1\" len=\"576\" name=\"CSAImageHeaderInfo\" binary=\"hidden\"></element>  <element tag=\"0029,1131\" vr=\"LO\" vm=\"1\" len=\"12\" name=\"PMTFInformation1\">5.0.17971565</element>  <element tag=\"0029,1132\" vr=\"UL\" vm=\"1\" len=\"4\" name=\"PMTFInformation2\">524288</element>  <element tag=\"0029,1133\" vr=\"UL\" vm=\"1\" len=\"4\" name=\"PMTFInformation3\">0</element>  <element tag=\"0029,1134\" vr=\"CS\" vm=\"1\" len=\"12\" name=\"PMTFInformation4\">DB TO DICOM</element>  <sequence tag=\"0029,1140\" vr=\"SQ\" card=\"1\" name=\"ApplicationHeaderSequence\">    <item card=\"5\">      <element tag=\"0029,0010\" vr=\"LO\" vm=\"1\" len=\"22\" name=\"PrivateCreator\">SIEMENS MEDCOM HEADER</element>      <element tag=\"0029,1041\" vr=\"CS\" vm=\"1\" len=\"10\" name=\"ApplicationHeaderType\">SOM 5 TPOS</element>      <element tag=\"0029,1042\" vr=\"LO\" vm=\"1\" len=\"18\" name=\"ApplicationHeaderID\">SOM 5 NULLPOSITION</element>      <element tag=\"0029,1043\" vr=\"LO\" vm=\"1\" len=\"14\" name=\"ApplicationHeaderVersion\">VB10A 20030626</element>      <element tag=\"0029,1044\" vr=\"OB\" vm=\"1\" len=\"12\" name=\"ApplicationHeaderInfo\" binary=\"hidden\"></element>    </item>  </sequence>  <sequence tag=\"0040,0275\" vr=\"SQ\" card=\"1\" name=\"RequestAttributesSequence\">    <item card=\"2\">      <element tag=\"0040,0009\" vr=\"SH\" vm=\"1\" len=\"16\" name=\"ScheduledProcedureStepID\">049S07P000337445</element>      <element tag=\"0040,1001\" vr=\"SH\" vm=\"1\" len=\"6\" name=\"RequestedProcedureID\">724795</element>    </item>  </sequence>  <element tag=\"7fe0,0010\" vr=\"OW\" vm=\"1\" len=\"524288\" name=\"PixelData\" loaded=\"no\" binary=\"hidden\"></element></data-set>";
      //  ElementCollection collection = ElementCollection.Create(xml, ParameterType.HEADER);

      //  ObservableCollection<Element> elements = new ObservableCollection<Element>(collection);


      //  LoadGroups(collection.GetGroups(), false);
      //}
     
    }


    /// <summary>
    /// Dipose
    /// </summary>
    /// <param name="disposing"></param>
    protected virtual void Dispose(bool disposing)
    {
      // Check to see if Dispose has already been called. 
      if (!m_disposed)
      {
        m_backgroundWorker.Dispose();
        m_elementCollection.Dispose();
        m_elements.Clear();
        m_elements = null;

      }

      m_disposed = true;

    }
    
    #endregion

    #region methods

    /// <summary>
    /// Creates the header view model from the element collection
    /// </summary>
    /// <param name="collection"></param>
    /// <returns></returns>
    public static HeaderViewModel Create(ImageViewModel image, ElementCollection collection)
    {
      HeaderViewModel vm = new HeaderViewModel();

      if (vm == null)
        throw new OutOfMemoryException("Failed to allocate HeaderViewModel");

      vm.m_image = image;

      vm.m_elementCollection = collection;

      vm.LoadElements();

      return vm;
    }

    /// <summary>
    /// Creates the header view model from the file
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    public static HeaderViewModel Create(string file)
    {

      return Create(null, ElementCollection.Create(file, ParameterType.FILE));
     }


    /// <summary>
    /// Handles the loading of the files and directories by starting a background thread to perform the work.
    /// </summary>
    /// <param name="list">List of files and or directories</param>
    private void LoadHandler(string file)
    {

      if (file.Length > 0)
      {
        IsBusy = true;

        if (m_backgroundWorker != null)
          m_backgroundWorker.Dispose();

        m_backgroundWorker = new BackgroundWorker();
        m_backgroundWorker.WorkerReportsProgress = true;

        m_backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(OnWorkerCompleted);
        m_backgroundWorker.ProgressChanged += new ProgressChangedEventHandler(OnWorkerProgressChanged);
        m_backgroundWorker.DoWork += new DoWorkEventHandler(OnDoWork);
        m_backgroundWorker.RunWorkerAsync(file);
      } // END ... If there are entries in the list

    } // LoadHandler(string [] list )

    /// <summary>
    /// Loads the Sequence
    /// </summary>
    /// <param name="sequence"></param>
    /// <param name="dicomElement"></param>
    void LoadSequence(Sequence sequence, DicomElement dicomElement)
    {

      int level = dicomElement.Level + 1;
      m_depth = level + 1;

      foreach (ElementCollection col in sequence)
      {
        foreach (Element element in col)
        {
          DicomElement de = new DicomElement(level, element);

          dicomElement.Elements.Add(de);

          if (element.IsSequence)
            LoadSequence((Sequence)element, de);
        }
      }

    }

    /// <summary>
    /// Load element collection into the observable collection of elements
    /// </summary>
    void LoadElements()
    {

      m_elements.Clear();

      List<Group> groups = m_elementCollection.GetGroups();

      m_depth = 1;

      foreach (Group group in groups)
      {

        DicomElement dicomElement = new DicomElement(0, group);
        m_elements.Add(dicomElement);

        foreach (Element element in m_elementCollection)
        {
          if (element.GroupId == group.Id)
          {
            if (m_depth == 1)
              m_depth = 2;

            DicomElement de = new DicomElement(1, element);
            dicomElement.Elements.Add(de);

            if (element.IsSequence)
              LoadSequence((Sequence)element, de);
          }
        }

      }

      MaxDepth = m_depth;
      CanSave = true;

    } // LoadElements()

 
    #region event handlers

     /// <summary>
    /// Worker thread DoWork handler
    /// </summary>
    /// <param name="sender">Sender </param>
    /// <param name="args">Input arguement</param>
    void OnDoWork(object sender, DoWorkEventArgs args)
    {

      m_elementCollection = ElementCollection.Create((string)args.Argument, ParameterType.FILE);

      

    } // OnDoWork(object sender, DoWorkEventArgs args)

    /// <summary>
    /// Worker threads PorgressChanged event handler
    /// </summary>
    /// <param name="sende">Sender</param>
    /// <param name="args">Progress args</param>
    void OnWorkerProgressChanged(object sende, ProgressChangedEventArgs args)
    {

    } // OnWorkerProgressChanged(object sende, ProgressChangedEventArgs args)

    /// <summary>
    /// Worker thread completed event handler
    /// </summary>
    /// <param name="sender">Sender</param>
    /// <param name="args">Completed args</param>
    void OnWorkerCompleted(object sender, RunWorkerCompletedEventArgs args)
    {
      IsBusy = false;

      List<Group> groups = m_elementCollection.GetGroups();


      LoadElements();


    } // OnWorkerCompleted(object sender, RunWorkerCompletedEventArgs args)


    #endregion


    #endregion

    #region properties


    #region commands

    /// <summary>
    /// Gets the LoadFilesCommand
    /// </summary>
    public RelayCommand OpenFileCommand
    {
      get
      {
        if (m_openFileCommand == null)
        {
          m_openFileCommand = new RelayCommand(() =>
          {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();

            openFileDialog.Title = "Open Dicom File";
            openFileDialog.Multiselect = false;
            openFileDialog.Filter = "Dicom Files|*.dcm|All Files|*.*";
            bool? dialogResults = openFileDialog.ShowDialog();

           
            
            if (dialogResults.HasValue && dialogResults == true)
            {
              LoadHandler(openFileDialog.FileName);


            }


          }, () => true);
        }
        return m_openFileCommand;
      }
    } // OpenFileCommand

    /// <summary>
    /// Gets the LoadFilesCommand
    /// </summary>
    public RelayCommand SaveAsXmlCOmmand
    {
      get
      {
        if (m_saveAsXmlCommand == null)
        {
          m_saveAsXmlCommand = new RelayCommand(() =>
          {
            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();

            saveFileDialog.Title = "Save As Xml";
            saveFileDialog.Filter = "Xml Files|*.xml|All Files|*.*";
            saveFileDialog.CheckPathExists = true;
            saveFileDialog.DefaultExt = "xml";
            saveFileDialog.OverwritePrompt = true;
            bool? dialogResults = saveFileDialog.ShowDialog();

            if (dialogResults.HasValue && dialogResults.Value == true)
            {
              m_elementCollection.WriteXml(saveFileDialog.FileName);
            }

              


          }, () => true);
        }
        return m_openFileCommand;
      }
    } // OpenFileCommand






    #endregion


    public ImageViewModel Image
    {
      get { return m_image; }
    }

    /// <summary>
    /// Max depth of the levels in the hierarchy
    /// </summary>
    public int MaxDepth
    {
      get { return m_depth; }
      set 
      {
        m_depth = value;
        RaisePropertyChanged("MaxDepth");
      }

    }
 
    /// <summary>
    /// Stes/Gets the Elements
    /// </summary>
    public ObservableCollection<DicomElement> Elements
    {
      get { return m_elements; }
    } // Elements



    /// <summary>
    /// Sets/Gets th selected element
    /// </summary>
    public Element SelectedElement
    {
      get { return m_selectedElement; }
      set
      {

        m_selectedElement = value;
        RaisePropertyChanged("SelectedElement");

      }
    } // SelectedElement


    public bool IsBusy
    {
      get { return m_busy; }
      set
      {
        m_busy = value;
        RaisePropertyChanged("IsBusy");
      }
    }

    public bool CanSave
    {
      get { return m_canSave; }
      set
      {
        m_canSave = value;
        RaisePropertyChanged("CanSave");

      }
    }

 
    #endregion

 



    }
}
