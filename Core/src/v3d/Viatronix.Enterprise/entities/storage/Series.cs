// $Id: Series.cs,v 1.79.2.36 2010/03/12 20:03:39 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Collections.Generic;
using System.Runtime.Serialization;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Entities
{

  #region delegates

  /// <summary>
  /// Series delgate
  /// </summary>
  /// <param name="series"></param>
  public delegate void SeriesDelegate(Series series);

  /// <summary>
  /// Series collection delegate
  /// </summary>
  /// <param name="collection"></param>
  public delegate void SeriesCollectionDelegate(List<Series> collection);

  #endregion

  #region SeriesComparer

  public class SeriesComparer : IComparer<Series>
  {
    #region fields

    private string m_sortBy = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Sort by attribute
    /// </summary>
    public string SortBy
    { get { return m_sortBy; } }

    #endregion


    #region constructor

    /// <summary>
    /// Constructors the series comparer with the sorting attribute
    /// </summary>
    /// <param name="sortAttribute"></param>
    public SeriesComparer(string sortBy)
    { m_sortBy = sortBy; }

    #endregion

    #region methods

    /// <summary>
    /// Comparer for series sorting
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    public int Compare(Series x, Series y)
    {
      if (x == null)
      {
        if (y == null)
          return 0;
        else
           return -1;
        
      }
      else
      {
        
        if (y != null)
        {
          if (m_sortBy == Series.Attributes.DateReceived)
            return (x.DateReceived == y.DateReceived ? 0 : (x.DateReceived < y.DateReceived ? -1 : 1));
          else
            return string.Compare(x.Uid, y.Uid);

        }
        else
          return 1;
  
      }
    }


    #endregion
  }

  #endregion

  #region Series class

  /// <summary>
  /// Series data type
  /// </summary>
  public class Series : ExtendedEntity, IProcessEntity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Uid                 = "uid";
      public const string StudyUid            = "suid";
      public const string Orientation         = "ort";
      public const string Date                = "date";
      public const string Modality            = "mod";
      public const string Id                  = "id";
      public const string State               = "state";
      public const string DateReceived        = "rcv";
      public const string Description         = "desc";
      public const string Directory           = "dir";
      public const string DateProcessed       = "on";
      public const string Application         = "app";
      public const string Type                = "type";
      public const string ImageCount          = "icnt";
      public const string ImagePlane          = "pln";
      public const string Number              = "num";
      public const string Url                 = "url";
      public const string Source              = "src";
      public const string Process             = "proc";
      public const string Parameters          = "parameters";
      public const string References          = "references";
      public const string Dependents          = "dependents";
      public const string Volumes             = "volumes";
      public const string ExtendedProperties  = "ext";
      public const string Message             = "msg";
        
    } // class Attributes

    #endregion

 
    #region fields

    /// <summary>
    /// Xml Node Name
    /// </summary>
    public const string XmlTag = "series";

 
    /// <summary>
    /// Message associated with the state (usually the failed or warning)
    /// </summary>
    private string m_message = string.Empty;
 
    /// <summary>
    /// Directory
    /// </summary>
    private string m_dir = string.Empty;

    /// <summary>
    /// Contents List
    /// </summary>
    private List<string> m_files = new List<string>();


    /// <summary>
    /// Process used to process this series
    /// </summary>
    private string m_process = string.Empty;

    /// <summary>
    /// Process State
    /// </summary>
    private States m_state = States.None;

    /// <summary>
    /// Dicom series number
    /// </summary>
    private string m_number = string.Empty;

    /// <summary>
    /// Series Uid
    /// </summary>
    private string m_uid = string.Empty;


    /// <summary>
    /// Series Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Series Date/Time (Same as Study Date Time if fields are not present)
    /// </summary>
    private DateTimeOffset m_date = System.DateTimeOffset.Now;


    /// <summary>
    /// Series Type
    /// </summary>
    private SeriesTypes m_type = SeriesTypes.None;

    /// <summary>
    /// Modality
    /// </summary>
    private string m_modality = "OT";


    /// <summary>
    /// Patient Orientation
    /// </summary>
    private string m_patientOrientation = string.Empty;

    /// <summary>
    /// Application
    /// </summary>
    private string m_application = string.Empty;



    /// <summary>
    /// Number of images in series
    /// </summary>
    private int m_imageCount = 0;

    /// <summary>
    /// Acquisition count
    /// </summary>
    private int m_acquisitionCount = 0;

    /// <summary>
    /// Date the series was received
    /// </summary>
    private DateTimeOffset? m_dateReceived = System.DateTimeOffset.Now;


    /// <summary>
    /// Date the series was processed
    /// </summary>
    private DateTimeOffset? m_dateProcessed = null;

    /// <summary>
    /// Image plane (Axial, Coronal, Sagittal )
    /// </summary>
    private Planes m_imagePlane = Planes.None;

    /// <summary>
    /// The source of the series
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// Full path of series
    /// </summary>
    private string m_path = string.Empty;

    /// <summary>
    /// Link to study
    /// </summary>
    private string m_studyUid = string.Empty;

    /// <summary>
    /// Referenced series
    /// </summary>
    private List<Series> m_references = new List<Series>();

    /// <summary>
    /// Dependent Series
    /// </summary>
    private List<Series> m_dependents = new List<Series>();

    /// <summary>
    /// Series parameters
    /// </summary>
    private List<Parameter> m_parameters = new List<Parameter>();

    /// <summary>
    /// Volume list
    /// </summary>
    private List<Volume> m_volumes = new List<Volume>();


    /// <summary>
    /// Extended properties
    /// </summary>
    private string m_extendedProperties = string.Empty;


    /// <summary>
    /// Url (Location or the series)
    /// </summary>
    private string m_url = string.Empty;


    #endregion


    #region properties

    /// <summary>
    /// Gets the flag to indicate the series can process
    /// </summary>
    public virtual bool CanProcess
    { get { return (m_type == SeriesTypes.Dicom); } }
 
    /// <summary>
    /// Gets/Sets the image plane
    /// </summary>
    public Planes ImagePlane
    {
      get { return m_imagePlane;  }
      set { SetProperty<Planes>("ImagePlane", value, ref m_imagePlane );  }
    }

    /// <summary>
    /// Directory
    /// </summary>
    public string Directory
    {
      get { return m_dir; }
      set { SetProperty<string>("Directory", value, ref m_dir ); }
    } // Directory

    /// <summary>
    /// Url
    /// </summary>
    public string Url
    {
      get { return m_url; }
    } // Url



    /// <summary>
    /// List of files for this series
    /// </summary>
    public List<string> Files
    {
      get { return m_files; }
      set { m_files = value; }
    } // Files


    /// <summary>
    /// Gets the message associated with the state (usually for failed or warning)
    /// </summary>
    public string Message
    {
      get { return m_message; }
    }

    /// <summary>
    /// Sets/Gets Referenced Series
    /// </summary>
    public List<Series> References
    {
      get { return m_references; }
      set { m_references = value; }
    } // References

    /// <summary>
    /// Sets/Gets Dependent Series
    /// </summary>
    public List<Series> Dependents
    {
      get { return m_dependents; }
      set { m_dependents = value; }
    } // Dependents

    /// <summary>
    /// Gets the volumes
    /// </summary>
    public List<Volume> Volumes
    {
      get { return m_volumes; }
    } // Volumes

    /// <summary>
    /// Parameters
    /// </summary>
    public List<Parameter> Parameters
    {
      get { return m_parameters; }
      set { m_parameters = value; }
    } // Parameters

    /// <summary>
    /// Sets/Gets the process name
    /// </summary>
    public string Process
    {
      get { return m_process; }
      set { SetProperty<string>("Process", value, ref m_process ); }
    } // Process

    /// <summary>
    /// Sets/Gets the process state
    /// </summary>
    public States State
    {
      get { return m_state; }
      set { SetProperty<States>("State",value, ref m_state ); }
    } // State

    /// <summary>
    /// Sets/Gets the study this series belongs to
    /// </summary>
    public string StudyUid
    {
      get { return m_studyUid; }
      set { SetProperty<string>("StudyUid", value, ref m_studyUid); }
    } // Study


    /// <summary>
    /// Gets the series full path 
    /// </summary>
    public string Path
    {
      get { return m_path; }
      set { SetProperty<string>("Path", value, ref m_path); }
    } // FullPath


    /// <summary>
    /// Sets/Gets the source
    /// </summary>
    public string Source
    {
      get { return m_source; }
      set { SetProperty<string>("Source", value, ref m_source); }
    } // Source




    /// <summary>
    /// Sets/Gets the series number
    /// </summary>
    public string Number
    {
      get { return m_number; }
      set { SetProperty<string>("Number", value, ref m_number ); }
    } // Number





    /// <summary>
    /// Sets/Gets the patient orientation
    /// </summary>
    public string PatientOrientation
    {
      get { return m_patientOrientation; }
      set
      {
        // ==========================================================================
        // Only allow the orientation to be changed if the series is not a session
        // ===========================================================================
        if (m_type != SeriesTypes.Session )
        {
          SetProperty<string>("PatientOrientation", value, ref m_patientOrientation);
        }
        else
        {
          Log.Warning("Patient orientation cannot be set to " + value + " because the series is a Session", "Series", "PatientOrientation");
          //throw new System.InvalidOperationException("Cannot set the patient orientation if the series is a Session");
        }
      }
    } // PatientOrientation


    /// <summary>
    /// Gets/Sets the Series Uid
    /// </summary>
    public string Uid
    {
      get { return m_uid; }
      set { SetProperty<string>("Uid", value, ref m_uid ); }
    } // Uid



    /// <summary>
    /// Gets/Sets the Series Description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { SetProperty<string>("Description", value, ref m_description ); }
    } // Description


    /// <summary>
    /// Gets/Sets the Series Date and Time
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date; }
      set { SetProperty<DateTimeOffset>("Date", value, ref m_date); }
    } // Date



    /// <summary>
    /// Gets/Sets the Series Modality
    /// </summary>
    public string Modality
    {
      get { return m_modality; }
      set { SetProperty<string>("Modality", value, ref m_modality ); }
    } // Modality


    /// <summary>
    /// Gets/Sets the Series Type
    /// </summary>
    public SeriesTypes Type
    {
      get { return m_type; }
      set
      {
        SetProperty<SeriesTypes>("Type", value, ref m_type);

        // If the series type is a session then the orientation is cleared (Sessions have no orientations)
        if (m_type == SeriesTypes.Session) 
            this.PatientOrientation = string.Empty;

      }
    } // Type


    /// <summary>
    /// Gets/Sets the Series Application type
    /// </summary>
    public string Application
    {
      get { return m_application; }
      set { SetProperty<string>("Application", value, ref m_application); }
    } // SubType



    /// <summary>
    /// Gets/Sets the number of images in the series
    /// </summary>
    public int ImageCount
    {
      get { return m_imageCount; }
      set { SetProperty<int>("ImageCount", value, ref m_imageCount); }
    } // ImageCOunt


    /// <summary>
    /// Gets/Sets the date the series was received
    /// </summary>
    public DateTimeOffset DateReceived
    {
      get { return m_dateReceived.Value; }
      set { m_dateReceived = value; }
    } // DateReceived



    /// <summary>
    /// Gets/Sets the date the series was marked completed
    /// </summary>
    public DateTimeOffset DateProcessed
    {
      get { return m_dateProcessed.Value; }
      set { m_dateProcessed = value; }
    } // DateProcessed


    /// <summary>
    /// Returns true if the series is processing
    /// </summary>
    public bool IsProcessing
    { get { return IsState(States.Processing); } }

    /// <summary>
    /// Returns true series is completed
    /// </summary>
    public bool IsCompleted
    { get { return IsState(States.Completed); } }

    /// <summary>
    /// Returns true if the series is being received
    /// </summary>
    public bool IsReceiving
    { get { return IsState(States.Receiving); }}

    /// <summary>
    /// Returns true if the series has a warning associated with it
    /// </summary>
    public bool IsWarning
    { get { return IsState(States.Warning); } }

    /// <summary>
    /// Returns true if the series has failed
    /// </summary>
    public bool IsFailed
    { get { return IsState(States.Failed); }   }

    /// <summary>
    /// Returns true if the series has failed
    /// </summary>
    public bool IsAborted
    { get { return IsState(States.Aborted); } }

    /// <summary>
    /// Returns true if the series is locked
    /// </summary>
    public bool IsLocked
    { get { return IsState(States.Locked); } }

    #endregion

    #region methods

    /// <summary>
    /// Creates a volume in the list and returns the reference
    /// </summary>
    /// <param name="name">Name of the volume (unique qithin series)</param>
    /// <param name="date">Date of the volume (if null then today)</param>
    /// <param name="file">Volume file name (if "" then the name.vol is used )</param>
    /// <param name="imageCount">Image count (if 0 then the series image count)</param>
    /// <returns>Newly added volume</returns>
    public Volume CreateVolume(string conversionId, string name = "", DateTimeOffset? date = null, string file = "", int imageCount = 0)
    {

      /// Sets the volume name if not supplied.
      string volumeName = (string.IsNullOrEmpty(name) ? string.Format("volume{0:D3}", (m_volumes.Count + 1)) : name);

      Volume volume = Volume.Create(conversionId, m_uid, volumeName);

      if (date.HasValue)
        volume.Date = date.Value;



      volume.FileName = (file.Length > 0 ? file : string.Format("{0}.vol", volumeName));
      volume.ImageCount = ( imageCount == 0 ? this.ImageCount : imageCount );

      if (m_volumes.Contains(volume))
        throw new AlreadyExistsException(string.Format("Volume {0} already exists in this series",name));

      m_volumes.Add(volume);

      return volume;

    } // CreateVolume(string name, DateTimeOffset? date = null, string file = "", int imageCount = 0)
  

    /// <summary>
    /// Returns true if the process as is that specified
    /// </summary>
    /// <param name="process">process as to compare to</param>
    /// <returns></returns>
    public bool IsProcess(string process)
    {
      return (string.Compare(m_process, process, true) == 0);
    } // IsProcess(string process)

    /// <summary>
    /// Returns true if the orientation as is that specified
    /// </summary>
    /// <param name="orientation">orientation as to compare to</param>
    /// <returns></returns>
    public bool IsPatientOrientation(string orientation)
    {
      return (string.Compare(m_patientOrientation, orientation, true) == 0);
    } // IsPatientOrientation(string preprocessAs)


    /// <summary>
    /// Check the subtype specified agains the internal field case insensitive
    /// </summary>
    /// <param name="application">application to compare to</param>
    /// <returns>true of the application equals the one specified</returns>
    public bool IsApplication(string application)
    {
      return (string.Compare(m_application, application, true) == 0);
    } // IsSubType(string subType)

    /// <summary>
    /// Check the state specified against the internal field case insensitive
    /// </summary>
    /// <param name="state">state toi compare to</param>
    /// <returns>true if the state equals the one specified</returns>
    public bool IsState(States state)
    { return (m_state & state) > 0; } 

    /// <summary>
    /// Check the type specified agains the internal field case insensitive
    /// </summary>
    /// <param name="type">Type toi compare to</param>
    /// <returns>true if the type equals the one specified</returns>
    //public bool IsType(string type)
    //{
    //  return (string.Compare(m_type, type, true) == 0);
    //} // IsType(string type)

    /// <summary>
    /// Check the modality specified agains the internal field case insensitive
    /// </summary>
    /// <param name="modality">modality toi compare to</param>
    /// <returns>true if the modality matches the one specified</returns>
    public bool IsModality(string modality)
    {
      return (string.Compare(m_modality.Trim(), modality, true) == 0);
    } // IsModality(string modality)

    /// <summary>
    /// Returns String Representation of this object (Uid)
    /// </summary>
    /// <returns>string representation of the object</returns>
    public override string ToString()
    {
      return m_uid.ToString();
    } // ToString()


    /// <summary>
    /// Gets the hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_uid.GetHashCode();
    } // GetHashCode()



    #endregion

    #region static methods

    /// <summary>
    /// Extracts the phase percentage from the series supplied
    /// </summary>
    /// <param name="series">Series to extract the phase percentage from</param>
    /// <returns>Returns the phase percentage (default is o%)</returns>
    public static float ExtractPhasePercentage(Series series)
    {

      float percentage = 0.0F;

      if (series.Type == SeriesTypes.Dicom)
      {
        percentage = ExtractPhasePercentage(series.Description);
      }
      else
      {

        Series dicom = null;
        
        if ( series.References.TryFindType(SeriesTypes.Dicom, ref dicom) )
          percentage = ExtractPhasePercentage(dicom);
        
      }

      return percentage;
    } // ExtractPhasePercentage(Series series)


    /// <summary>
    /// Extracts the phase percentage from a description 
    /// </summary>
    /// <param name="description">Description to extract the phase percentage from</param>
    /// <returns>Returns the phase percentage (default is o%)</returns>
    public static float ExtractPhasePercentage(string description)
    {

      float percentage = 0.0F;

      char[] percentChar = { '%' };

      int pos = description.IndexOf('%');

      if (pos > -1)
      {

        int end = pos - 1;
        for (; end >= 0 && char.IsWhiteSpace(description[end]); --end) ;

        int start = end - 1;
        for (; start >= 0 && (char.IsDigit(description[start]) || description[start] == '.'); --start) ;

        if (start < 0 || (start >= 0 && description[start] == ' '))
        {
          start++;

          try
          {
            percentage = float.Parse(description.Substring(start, (end - start) + 1));
          }
          catch (Exception ex)
          {
            Log.Debug("No valid % format : " + ex.Message, "Series", "ExtractPhasePercentage");
          }
        }
      }

      return percentage;

    } // ExtractPhasePercentage(string description)

    /// <summary>
    /// Generates a directory name for the series
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    public static string GenerateDirecrtoryName(Series series)
    {
      return (series.Type != SeriesTypes.None ? series.Type.ToString().ToUpper() + "_" : "") + DateTime.Now.ToString("yyyyMMddHHmmssff");
    } // GenerateDirecrtoryName(Series series)


 


    /// <summary>
    /// Creates a new instance of a Study object from xml file.
    /// </summary>
    /// <param name="file">The xml file.</param>
    /// <returns>A Study object.</returns>
    public static Series Create(string xmlOrFile)
    {

      XmlDocument doc = new XmlDocument();

      if (!xmlOrFile.StartsWith("<"))
      {

        if (!File.Exists(xmlOrFile))
        {
          Log.Error("The specified series xml file could not be found [FILE=" + xmlOrFile + "]", "Server", "FromFile");
          throw new System.IO.FileNotFoundException("Series xml file was not found");
        }

        doc.Load(xmlOrFile);


      }
      else
        doc.LoadXml(xmlOrFile);



      return Create(doc.DocumentElement);

    } // Create(string xmlFile)

    /// <summary>
    /// Creates a new instance of a Series object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Series object.</returns>
    public static Series Create(XmlNode node)
    {
      Series series = new Series();

      if (series == null)
        throw new OutOfMemoryException("Failed to allocation series");


      series.Load(node);



      return series;
    } // Create( node )





    #endregion


    #region IEntity

   /// <summary>
    /// Creates an xml representation of a User object.
    /// </summary>
    /// <returns>Xml string representing the User object.</returns>
    public override string ToXml()
    {

      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(XmlTag);
        {


          writer.WriteAttributeString(Attributes.Uid, m_uid);

          writer.WriteAttributeString(Attributes.StudyUid, m_studyUid);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("O"));
          writer.WriteAttributeString(Attributes.Orientation, m_patientOrientation);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Modality, m_modality);
          writer.WriteAttributeString(Attributes.Type, ((int)m_type).ToString());
          writer.WriteAttributeString(Attributes.Application, m_application);
          writer.WriteAttributeString(Attributes.Source, m_source);
          writer.WriteAttributeString(Attributes.Number, m_number);
          writer.WriteAttributeString(Attributes.Message, m_message);
          writer.WriteAttributeString(Attributes.Process, m_process);
          writer.WriteAttributeString(Attributes.State, ((int)m_state).ToString());
          writer.WriteAttributeString(Attributes.ImageCount, m_imageCount.ToString());
          writer.WriteAttributeString(Attributes.DateReceived, m_dateReceived != null ? m_dateReceived.Value.ToString("O") : DateTimeOffset.Now.ToString("O"));
          writer.WriteAttributeString(Attributes.DateProcessed, m_dateProcessed != null ? m_dateProcessed.Value.ToString("O") : DateTimeOffset.Now.ToString("O"));

          if (!string.IsNullOrEmpty(m_dir))
            writer.WriteAttributeString(Attributes.Directory, m_dir);

          if ( !string.IsNullOrEmpty(m_uid) )
            writer.WriteAttributeString(Attributes.Url, m_url);

          writer.WriteAttributeString(Attributes.ImagePlane, ((int)m_imagePlane).ToString());

          WriteAttributeStrings(writer);
          WriteElements(writer);




          if (this.Parameters.Count > 0)
          {
            writer.WriteStartElement(Attributes.Parameters);

            foreach (Parameter parameter in this.Parameters)
            {
              writer.WriteStartElement(Parameter.XmlTag);
              writer.WriteAttributeString("name", parameter.Name);
              writer.WriteAttributeString("value", parameter.Value);
              writer.WriteAttributeString("commandline", parameter.CommandLine);
              writer.WriteEndElement();
            }

            writer.WriteEndElement();

          }

          if (this.References.Count > 0)
          {
            writer.WriteStartElement(Attributes.References);

            foreach (Series referencedSeries in this.References)
            {
              writer.WriteStartElement(XmlTag);
              writer.WriteAttributeString(Attributes.Uid, referencedSeries.Uid.ToString());
              writer.WriteAttributeString(Attributes.Type, referencedSeries.Type);
              writer.WriteEndElement();

            }

            writer.WriteEndElement();
          } // END ... If there are references

          if (this.Dependents.Count > 0)
          {
            writer.WriteStartElement(Attributes.Dependents);

            foreach (Series dependentSeries in this.Dependents)
            {
              writer.WriteStartElement(XmlTag);
              writer.WriteAttributeString(Attributes.Uid, dependentSeries.Uid.ToString());
              writer.WriteAttributeString(Attributes.Type, dependentSeries.Type);
              writer.WriteEndElement();
            }


            writer.WriteEndElement();
          } // END ... If there are dependents

          if (this.Volumes.Count > 0)
          {
            writer.WriteStartElement(Attributes.Volumes);

            foreach (Volume volume in m_volumes)
            {
              writer.WriteRaw(volume.ToXml());
            }

  
            writer.WriteEndElement();
          } // END ... If there are volumes


          // ==========================
          // Write extended properties
          // =========================
          if (m_extendedProperties.Length > 0)
          {
             writer.WriteStartElement("ext");
             writer.WriteRaw(m_extendedProperties);
             writer.WriteEndElement();
          } // END .. If extended properties


        }

        writer.WriteEndElement();
      }

      return builder.ToString();

    } // ToXml()


    /// <summary>
    /// Creates a new instance of a Series object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Series object.</returns>
    public override void Load(XmlNode node)
    {

      if (node.Name == "data-set")
      {

        Dicom.Element element = new Dicom.Element(0,0,string.Empty);
        Dicom.ElementCollection elements = Dicom.ElementCollection.Create(node);

  
        if ( elements.TryGetElement(0x0020,0x000e, ref element) )
          m_uid = element.Value;

        if ( elements.TryGetElement(0x0020,0x000d, ref element) )
          m_studyUid = element.Value;

        string date = string.Empty;
        string time = string.Empty;


        if ( elements.TryGetElement(0x0008,0x0031, ref element) )
          time = element.Value;

        if (elements.TryGetElement(0x0008, 0x0021, ref element))
          date = element.Value;

        m_date = (date.Length > 0 ? Dicom.Utility.ToDateTime(date, time) : m_date);


        if ( elements.TryGetElement(0x0008,0x103e,ref element) )
          m_description = element.Value;

        if ( elements.TryGetElement(0x0008,0x0060,ref element) )
          m_modality = element.Value;

        m_type = SeriesTypes.Dicom;

        if ( elements.TryGetElement(0x0020,0x0011,ref element) )
          m_number = element.Value;

        if (elements.TryGetElement(0x0020, 0x0037, ref element))
        {
          string imageOrientation = element.Value;

          char [] del = { '\\', '.', '/', ':' };

          string [] fields = imageOrientation.Split(del);

          List<double> cosines = new List<double>();
          foreach (string field in fields)
          {
            cosines.Add(Double.Parse(field));
          }

          m_patientOrientation = Dicom.Utility.GetImageOrientation(cosines).ToString();

          m_imagePlane = (Planes)Dicom.Utility.GetImagePlane(cosines);
        }


      }
      else
      {
        base.Load(node);

        string version = string.Empty;
        node.TryGetAttributeValue<string>(EntityAttributes.Version, ref version);



        if (ActualVersion >= 4.0 )
          LoadVersion4(node);
        else if (ActualVersion >= 3.0 && ActualVersion < 4.0)
          LoadVersion3(node);
        else if (ActualVersion >= 2.0 && ActualVersion < 3.0)
          LoadVersion2(node);
        else
          LoadVersion1(node);
      }

     
    } // Load( node )


    /// <summary>
    /// Clones the series
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Series series = new Series();

      if (series == null)
        throw new OutOfMemoryException("Failed to allocation series");


      series.m_uid = m_uid;
      series.m_studyUid = m_studyUid;
      series.m_date = m_date;
      series.m_description = m_description;
      series.m_modality = m_modality;
      series.m_patientOrientation = m_patientOrientation;
      series.m_type = m_type;
      series.m_application = m_application;
      series.m_source = m_source;
      series.m_number = m_number;
      series.m_dir = m_dir;
      series.m_imageCount = m_imageCount;
      series.m_acquisitionCount = m_acquisitionCount;
      series.m_dateReceived = m_dateReceived;
      series.m_dateProcessed = m_dateProcessed;
      series.m_state  = m_state;
      series.m_imagePlane = m_imagePlane;
      series.m_path = m_path;
      series.m_type = m_type;
      series.m_message = m_message;
      series.m_url = m_url;
      series.m_extendedProperties = m_extendedProperties;
  
      m_references.ForEach(rs => series.m_references.Add(rs));
      m_dependents.ForEach(ds => series.m_dependents.Add(ds));
      m_parameters.ForEach(p => series.m_parameters.Add(p));


      return series;
 
    } // Clone()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Series other = obj as Series;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_uid, other.m_uid);

    } // Equals( obj )

    #region Load Methods

    /// <summary>
    /// Loads version 3 of this object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion4(XmlNode node)
    {
      // We're dealing with the current version.

      m_uid = node.Attributes[Attributes.Uid].Value;
      m_studyUid = (node.Attributes[Attributes.StudyUid].Value);
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_description = node.Attributes[Attributes.Description].Value;
      m_modality = node.Attributes[Attributes.Modality].Value;
      m_patientOrientation = node.Attributes[Attributes.Orientation].Value;
      m_type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), node.Attributes[Attributes.Type].Value) ;
      m_application = node.Attributes[Attributes.Application].Value;
      m_source = node.Attributes[Attributes.Source].Value;
      m_number = node.Attributes[Attributes.Number].Value;
      m_message = node.Attributes[Attributes.Message].Value;
      m_imageCount = Int32.Parse(node.Attributes[Attributes.ImageCount].Value);

      node.TryGetAttributeValue<string>(Attributes.Url, ref m_url);
      node.TryGetAttributeValue<string>(Attributes.Directory, ref m_dir);

      node.TryGetAttributeValue<DateTimeOffset>(Attributes.DateReceived, ref m_dateReceived);
      node.TryGetAttributeValue<DateTimeOffset>(Attributes.DateProcessed, ref m_dateProcessed);

      m_state = (States)Enum.Parse(typeof(States), node.Attributes[Attributes.State].Value);
      m_imagePlane = (Planes)Enum.Parse(typeof(Planes), node.Attributes[Attributes.ImagePlane].Value, true);


      node.TryGetNodeValue(Attributes.ExtendedProperties, ref m_extendedProperties);
      node.TryGetNodeValue(Attributes.Process, ref m_process);

      // ====================================
      // Extract the reference series nodes
      // ======================================
      XmlNode references = node.SelectSingleNode(Attributes.References);
      if (references != null)
      {
        XmlNodeList list = references.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series referenceSeries = new Series();
          referenceSeries.Uid = seriesNode.Attributes[Attributes.Uid].Value;
          referenceSeries.StudyUid = seriesNode.Attributes[Attributes.StudyUid].Value;
          referenceSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes[Attributes.Type].Value);

          m_references.Add(referenceSeries);
        }
      } // END ... If there is referenced series


      // ====================================
      // Extract the depenedent series nodes
      // ======================================
      XmlNode dependents = node.SelectSingleNode(Attributes.Dependents);
      if (dependents != null)
      {
        XmlNodeList list = dependents.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series dependentSeries = new Series();
          dependentSeries.Uid = seriesNode.Attributes[Attributes.Uid].Value;
          dependentSeries.StudyUid = seriesNode.Attributes[Attributes.StudyUid].Value;
          dependentSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes[Attributes.Type].Value);

          m_dependents.Add(dependentSeries);
        }
      } // END .... If there is dependent series


      // =============================
      //  Extract the volume nodes
      // ============================
      XmlNode volumes = node.SelectSingleNode(Attributes.Volumes);
      if (volumes != null)
      {
        XmlNodeList list = volumes.SelectNodes(Volume.XmlTag);
        foreach (XmlNode volumeNode in list)
        {
          Volume volume = Volume.Create(volumeNode);
          m_volumes.Add(volume);
        }
      } // END .... If there is volumes



      if (node.SelectSingleNode("parameters") != null)
      {
        XmlNodeList parameters = node.SelectSingleNode("parameters").SelectNodes(Parameter.XmlTag);
        foreach (XmlNode parameterNode in parameters)
        {
          m_parameters.Add(Parameter.Create(parameterNode));
        }
      }

    } // LoadVersion3()

    /// <summary>
    /// Loads version 3 of this object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion3(XmlNode node)
    {
      // We're dealing with the current version.

      m_uid = node.Attributes[Attributes.Uid].Value;
      m_studyUid = (node.Attributes[Attributes.StudyUid].Value);
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_description = node.Attributes[Attributes.Description].Value;
      m_modality = node.Attributes[Attributes.Modality].Value;
      m_patientOrientation = node.Attributes[Attributes.Orientation].Value;
      m_type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), node.Attributes[Attributes.Type].Value);
      m_application = node.Attributes[Attributes.Application].Value;
      m_source = node.Attributes[Attributes.Source].Value;
      m_number = node.Attributes[Attributes.Number].Value;
      m_dir = node.Attributes[Attributes.Directory].Value;
      m_imageCount = Int32.Parse(node.Attributes[Attributes.ImageCount].Value);
 
      node.TryGetAttributeValue<DateTimeOffset>(Attributes.DateReceived, ref m_dateReceived);
      node.TryGetAttributeValue<DateTimeOffset>(Attributes.DateProcessed, ref m_dateProcessed);

      m_state = (States)Enum.Parse(typeof(States), node.Attributes[Attributes.State].Value);
      m_imagePlane = (Planes)Enum.Parse(typeof(Planes), node.Attributes[Attributes.ImagePlane].Value, true);


      node.TryGetNodeValue(Attributes.ExtendedProperties, ref m_extendedProperties);
      node.TryGetNodeValue(Attributes.Process, ref m_process);

      // ====================================
      // Extract the reference series nodes
      // ======================================
      XmlNode references = node.SelectSingleNode(Attributes.References);
      if (references != null)
      {
        XmlNodeList list = references.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series referenceSeries = new Series();
          referenceSeries.Uid = seriesNode.Attributes[Attributes.Uid].Value;
          referenceSeries.StudyUid = seriesNode.Attributes[Attributes.StudyUid].Value;
          referenceSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes[Attributes.Type].Value);

          m_references.Add(referenceSeries);
        }
      } // END ... If there is referenced series


      // ====================================
      // Extract the depenedent series nodes
      // ======================================
      XmlNode dependents = node.SelectSingleNode(Attributes.Dependents);
      if (dependents != null)
      {
        XmlNodeList list = dependents.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series dependentSeries = new Series();
          dependentSeries.Uid = seriesNode.Attributes[Attributes.Uid].Value;
          dependentSeries.StudyUid = seriesNode.Attributes[Attributes.StudyUid].Value;
          dependentSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes[Attributes.Type].Value);

          m_dependents.Add(dependentSeries);
        }
      } // END .... If there is dependent series


      if (node.SelectSingleNode("parameters") != null)
      {
        XmlNodeList parameters = node.SelectSingleNode("parameters").SelectNodes(Parameter.XmlTag);
        foreach (XmlNode parameterNode in parameters)
        {
          m_parameters.Add(Parameter.Create(parameterNode));
        }
      }

    } // LoadVersion3()


    /// <summary>
    /// Loads version 2 of the object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion2(XmlNode node)
    {
      // We're dealing with the current version.

      m_uid = (node.Attributes["uid"].Value);
      m_studyUid = node.Attributes["suid"].Value;
      m_date = DateTimeOffset.Parse(node.Attributes["date"].Value);
      m_description = node.Attributes["desc"].Value;
      m_modality = node.Attributes["mod"].Value;
      m_patientOrientation = node.Attributes["ort"].Value;
      m_type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), node.Attributes["type"].Value);
      m_application = node.Attributes["subtype"].Value;
      m_source = node.Attributes["src"].Value;
      m_number = node.Attributes["num"].Value;

      if (node.Attributes["imgCnt"] != null)
        m_imageCount = Int32.Parse(node.Attributes["imgCnt"].Value);
      else
        m_imageCount = Int32.Parse(node.Attributes["icnt"].Value);

      m_dateReceived = DateTimeOffset.Parse(node.Attributes["rcv"].Value);
      m_dateProcessed = DateTimeOffset.Parse(node.Attributes["procOn"].Value);
      m_process = node.Attributes["proc"].Value;
      m_state = (States)Enum.Parse(typeof(States), node.Attributes["state"].Value, true);



      if (node.Attributes["phase"] == null)
      {

        String value;
        StringBuilder builder = new StringBuilder();

        builder.Append("<phase ");

        value = node.Attributes["ppct"].Value;
        builder.Append(string.Format(" pp=\"{0}\" ", value));

        value = node.Attributes["psize"].Value;
        builder.Append(string.Format(" ps=\"{0}\" ", value));

        value = node.Attributes["pnum"].Value;
        builder.Append(string.Format(" pn=\"{0}\" ", value));

        value = node.Attributes["pcnt"].Value;
        builder.Append(string.Format(" pc=\"{0}\" ", value));


        builder.Append(" />");

        m_extendedProperties += builder.ToString();
      }
      else
      {
        if (node.Attributes["phase"].Value != string.Empty)
        {


          StringBuilder builder = new StringBuilder();

          // handle old phase style
          string sPhase = node.Attributes["phase"].Value;

          // records the index of the OF in phase.
          int index0 = sPhase.IndexOf("Phase ");
          int index1 = sPhase.IndexOf(" of ");
          int index2 = sPhase.IndexOf(" at ");
          int index3 = sPhase.IndexOf("% size of ");

          builder.Append("<phase ");
          builder.Append(string.Format(" pp=\"{0}\" ", sPhase.Substring(index0 + 6, index1 - (index0 + 6)).Trim()));
          builder.Append(string.Format(" ps=\"{0}\" ", sPhase.Substring(index1 + 4, index2 - (index1 + 4)).Trim()));
          builder.Append(string.Format(" pn=\"{0}\" ", sPhase.Substring(index2 + 4, index3 - (index2 + 4)).Trim()));
          builder.Append(string.Format(" pc=\"{0}\" ", sPhase.Substring(index3 + 10).Trim()));
          builder.Append(" />");

          m_extendedProperties += builder.ToString();

        }
      }

      XmlNode refSeries = node.SelectSingleNode("references");
      if (refSeries != null)
      {
        XmlNodeList list = refSeries.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series referenceSeries = new Series();
          referenceSeries.Uid = seriesNode.Attributes["uid"].Value;
          referenceSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes["type"].Value);


          m_references.Add(referenceSeries);
        }
      } // END ... If there is referenced series


      XmlNode depSeries = node.SelectSingleNode("dependents");

      if (depSeries != null)
      {
        XmlNodeList list = depSeries.SelectNodes(XmlTag);
        foreach (XmlNode seriesNode in list)
        {
          Series dependentSeries = new Series();
          dependentSeries.Uid = seriesNode.Attributes["uid"].Value;
          dependentSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes["type"].Value);

          m_dependents.Add(dependentSeries);
        }
      } // END .... If there is dependent series

      if (node.SelectSingleNode("parameters") != null)
      {
        XmlNodeList parameters = node.SelectSingleNode("parameters").SelectNodes("parameter");
        foreach (XmlNode parameterNode in parameters)
        {
          m_parameters.Add(Parameter.Create(parameterNode));
        }
      }
    } // LoadVersion2()

    /// <summary>
    /// Loads version 1 of the object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion1(XmlNode node)
    {
      // We're dealing with the old version.
      if (node.Attributes["Uid"] != null)
      {
        m_uid = node.Attributes["Uid"].Value;
      }
      else if(node.Attributes["UID"] != null)
      {
        m_uid = node.Attributes["UID"].Value;
      }

      // What to do here?
      //series.Study.Uid = new Uid(node.Attributes["suid".Value);

      m_date = DateTimeOffset.Parse(node.Attributes["SERIES_DATE"].Value);
      m_description = node.Attributes["DESCRIPTION"].Value;
      m_modality = node.Attributes["MODALITY"].Value;
      m_patientOrientation = node.Attributes["ORIENTATION"].Value;
      m_type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), node.Attributes["TYPE"].Value);
      m_application = node.Attributes["SUB_TYPE"].Value;
      m_source = node.Attributes["SOURCE"].Value;
      m_number = node.Attributes["DICOM_SERIES_NUMBER"].Value;
      m_imageCount = Int32.Parse(node.Attributes["IMAGE_COUNT"].Value);
      m_dateReceived = DateTimeOffset.Parse(node.Attributes["DATE_RECEIVED"].Value);
      m_dateProcessed = DateTimeOffset.Parse(node.Attributes["DATE_PROCESSED"].Value);
      m_process = node.Attributes["PROCESS"].Value;

      if (node.Attributes["STATE"].Value != string.Empty)
      {
        m_state = (States)Enum.Parse(typeof(States), node.Attributes["STATE"].Value, true);
      }
      else
      {
        m_state = States.None;
      }

      // What to do here?
      //series.m_dir = node.Attributes["dir");

      if (node.Attributes["PHASE"] == null)
      {
        String value;
        StringBuilder builder = new StringBuilder();

        builder.Append("<phase ");

        value = node.Attributes["PHASE_PERCENT"].Value;
        builder.Append(string.Format(" pp=\"{0}\" ", value));

        value = node.Attributes["PHASE_SIZE"].Value;
        builder.Append(string.Format(" ps=\"{0}\" ", value));

        value = node.Attributes["PHASE_NUMBER"].Value;
        builder.Append(string.Format(" pn=\"{0}\" ", value));

        value = node.Attributes["TOTAL_PHASES"].Value;
        builder.Append(string.Format(" pc=\"{0}\" ", value));


        builder.Append(" />");

        m_extendedProperties += builder.ToString();

      }
      else
      {
        if (node.Attributes["PHASE"].Value != string.Empty)
        {
          // handle old phase style
          string sPhase = node.Attributes["PHASE"].Value;

          // records the index of the OF in phase.
          int index0 = sPhase.IndexOf("Phase ");
          int index1 = sPhase.IndexOf(" of ");
          int index2 = sPhase.IndexOf(" at ");
          int index3 = sPhase.IndexOf("% size of ");

          StringBuilder builder = new StringBuilder();

          builder.Append("<phase ");
          builder.Append(string.Format(" pp=\"{0}\" ", sPhase.Substring(index0 + 6, index1 - (index0 + 6)).Trim()));
          builder.Append(string.Format(" ps=\"{0}\" ", sPhase.Substring(index1 + 4, index2 - (index1 + 4)).Trim()));
          builder.Append(string.Format(" pn=\"{0}\" ", sPhase.Substring(index2 + 4, index3 - (index2 + 4)).Trim()));
          builder.Append(string.Format(" pc=\"{0}\" ", sPhase.Substring(index3 + 10).Trim()));
          builder.Append(" />");

          m_extendedProperties += builder.ToString();
        }
      }

      XmlNode refSeries = node.SelectSingleNode("REFERENCED_SERIES");
      if (refSeries != null)
      {
        XmlNodeList list = refSeries.SelectNodes("series");
        foreach (XmlNode seriesNode in list)
        {
          Series referenceSeries = new Series();

          if (seriesNode.Attributes["Uid"] != null)
          {
            referenceSeries.Uid = seriesNode.Attributes["Uid"].Value;
          }
          else if(seriesNode.Attributes["UID"] != null)
          {
            referenceSeries.Uid = seriesNode.Attributes["UID"].Value;
          }

          referenceSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes["TYPE"].Value);

          m_references.Add(referenceSeries);
        }
      } // END ... If there is referenced series

      //Dependent series are new only.
      XmlNode depSeries = node.SelectSingleNode("DEPENDENTS");

      if (depSeries != null)
      {
        XmlNodeList list = depSeries.SelectNodes("series");
        foreach (XmlNode seriesNode in list)
        {
          Series dependentSeries = new Series();
          dependentSeries.Uid = seriesNode.Attributes["uid"].Value;
          dependentSeries.Type = (SeriesTypes)Enum.Parse(typeof(SeriesTypes), seriesNode.Attributes["TYPE"].Value);

          m_dependents.Add(dependentSeries);
        }
      } // END .... If there is dependent series

      XmlNodeList parameters = node.SelectNodes("PARAMETERS/parameter");
      foreach (XmlNode parameterNode in parameters)
      {
        m_parameters.Add(Parameter.Create(parameterNode));
      }

    } // LoadVersion1()

    #endregion

    #endregion

  } // class Series

  #endregion


} // namespace Viatronix.Enterprise.Entities


