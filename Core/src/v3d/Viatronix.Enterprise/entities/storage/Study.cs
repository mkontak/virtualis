// $Id: Study.cs,v 1.39.2.22 2009/09/10 13:31:16 mkontak Exp $
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
using System.Collections.Specialized;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Globalization;
using System.Runtime.Serialization;
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Entities
{


  #region delegates

  /// <summary>
  /// Study delgate
  /// </summary>
  /// <param name="study"></param>
  public delegate void StudyDelegate(Study study);

  /// <summary>
  /// Study collection delegate
  /// </summary>
  /// <param name="collection"></param>
  public delegate void StudyCollectionDelegate(List<Study> collection);

  #endregion

 
  #region Study class

  /// <summary>
  /// implements a study object containing all the study information.
  /// </summary>
  public class Study : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Uid                 = "uid";
      public const string MedicalId           = "mid";
      public const string Date                = "date";
      public const string Physician           = "phys";
      public const string Id                  = "id";
      public const string Status              = "status";
      public const string State               = "state";
      public const string AcessionNumber      = "an";
      public const string Description         = "desc";
      public const string Directory           = "dir";
      public const string PatientName         = "name";
      public const string PatientWeight       = "wgt";
      public const string PatientDob          = "dob";
      public const string PatientGender       = "sex";
      public const string Modalities          = "mods";
      public const string Group               = "grp";
      public const string Applications        = "apps";
      public const string Series              = "series";
      public const string Url                 = "url";
    } // class Attributes

    #endregion

 
    #region fields

 
    /// <summary>
    /// Xml file name
    /// </summary>
    public const string XmlTag = "study";


 
    /// <summary>
    /// Directory
    /// </summary>
    private string m_dir = string.Empty;

    /// <summary>
    /// Studies Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Referring Physicians name
    /// </summary>
    private string m_physician = string.Empty;

    /// <summary>
    /// Studies Description
    /// </summary>
    private string m_description = string.Empty;


    /// <summary>
    /// Dicom Study Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Accession Number
    /// </summary>
    private string m_accessionNumber = string.Empty;

    /// <summary>
    /// Studies Date and Time
    /// </summary>
    private DateTimeOffset m_date = DateTimeOffset.Now;


    /// <summary>
    /// Series collection for this study
    /// </summary>
    private List<Series> m_series = new List<Series>();


    /// <summary>
    /// Group 
    /// </summary>
    private string m_group = string.Empty;

     /// <summary>
    /// Patients medical id
    /// </summary>
    private string m_medicalId = string.Empty;

    /// <summary>
    /// Patient weight
    /// </summary>
    private float m_weight = 0.0F;

    /// <summary>
    /// Patients gender/sex
    /// </summary>
    private Genders m_gender = Genders.Unknown;

    /// <summary>
    /// Patient name
    /// </summary>
    private PersonName m_name = new PersonName();

    /// <summary>
    /// Patient DOB
    /// </summary>
    private DateTime? m_dob = null;

    /// <summary>
    /// State of the study (Or-ing of the series states)
    /// </summary>
    private States m_state = States.None;

    /// <summary>
    /// Applications
    /// </summary>
    private List<string> m_applications = new List<string>();

    /// <summary>
    /// Modalities
    /// </summary>
    private List<String> m_modalities = new List<string>();

    /// <summary>
    /// URL ( Location of the study)
    /// </summary>
    private string m_url = string.Empty;

    #endregion

    #region constructors

    /// <summary>
    /// Creates a new instance of a Study object.
    /// </summary>
    public Study()
    {
    } // Study()

    /// <summary>
    /// Creates a new instance of a Study object.
    /// </summary>
    public Study(string uid)
    {
      m_uid = uid;
    } // Study()


 
    #endregion

    #region properties

 
    /// <summary>
    /// Gets/Sets the application list
    /// </summary>
    public List<string> Applications
    {
      get { return m_applications; }
      set { m_applications = value; }
    } // Applications


    /// <summary>
    /// Get/Set the modality list
    /// </summary>
    public List<string> Modalities
    {
      get { return m_modalities; }
      set { m_modalities = value; }
    } // Modalities

    /// <summary>
    /// Sets/Gets the medical id
    /// </summary>
    public string MedicalId
    {
      get { return m_medicalId; }
      set { SetProperty<string>("MedicalId", value, ref m_medicalId); }
    } // MedicalId


    /// <summary>
    /// Gets the patients age
    /// </summary>
    public TimeSpan Age
    {
      get { return ( m_dob.HasValue ?  DateTimeOffset.Now.Subtract(m_dob.Value) : new TimeSpan() ); }
    } // Age


    /// <summary>
    /// Sets/Gets the patients weight
    /// </summary>
    public float Weight
    {
      get { return m_weight; }
      set { SetProperty<float>("Weight", value, ref m_weight ); }
    } // Weight


    /// <summary>
    /// Sets/Gets the patients gender
    /// </summary>
    public Genders Gender
    {
      get { return m_gender; }
      set { SetProperty<Genders>("Gender", value, ref m_gender ); }
    } // Gender


    /// <summary>
    /// Sets/Gets the patient name
    /// </summary>
    public PersonName Name
    {
      get { return m_name; }
      set { SetProperty<PersonName>("Name", value, ref m_name); }
    } // Name


    /// <summary>
    /// Sets/Gets the patients DOB
    /// </summary>
    public DateTime Dob
    {
      get { return ( m_dob.HasValue ? m_dob.Value : DateTime.MinValue ); }
      set { SetProperty<DateTime?>("Dob", value, ref m_dob ); }
    } // Dob


    /// <summary>
    /// Gets the study directory name
    /// </summary>
    public string Directory
    {
      get { return m_dir; }
      set { SetProperty<string>("Directory", value, ref m_dir); }
    } // Directory


    /// <summary>
    /// Gets the study url
    /// </summary>
    public string Url
    {
      get { return m_url; }
    } // Url


    /// <summary>
    /// Sets/Gets the studies Uid
    /// </summary>
    public string Uid
    {
      get { return m_uid; }
      set { SetProperty<string>("Uid", value, ref m_uid ); }
    } //Uid


    /// <summary>
    /// Gets/Sets the referring physicians name
    /// </summary>
    public string Physician
    {
      get { return m_physician; }
      set { SetProperty<string>("Physician", value, ref m_physician); }
    } // Physician


    /// <summary>
    /// Sets/Gets the studies description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { SetProperty<string>("Description", value, ref m_description ); }
    } // Description

    /// <summary>
    /// Sets/Gets the study ID
    /// </summary>
    public string Id
    {
      get { return m_id; }
      set { SetProperty<string>("Id", value, ref m_id ); }
    } // StudyId

    /// <summary>
    /// Sets/Gets the accession number
    /// </summary>
    public string AccessionNumber
    {
      get { return m_accessionNumber; }
      set { SetProperty<string>("AccessionNumber", value, ref m_accessionNumber); }
    } // AccessionNumber

    /// <summary>
    /// Sets/gets the studies date and time
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date; }
      set { SetProperty<DateTimeOffset>("Date", value, ref m_date ); }
    } // StudyDateTime

    /// <summary>
    /// Set/Get the state
    /// </summary>
    public States State
    {
      get { return m_state;  }
      set { SetProperty<States>("State", value, ref m_state );  }
    } // State



    /// <summary>
    /// Gets the series collection for this study
    /// </summary>
    public List<Series> Series
    {
      get { return m_series; }
      set { m_series = value; } 
    } // Series

    /// <summary>
    /// Sets/Gets the studies group
    /// </summary>
    public string Group
    {
      set { SetProperty<string>("Group", value, ref m_group ); }
      get { return m_group; }
    }   // Group


    /// <summary>
    /// True if the study is processing
    /// </summary>
    public bool IsProcessing
    { get { return IsState(States.Processing); } } // IsProcessing

    /// <summary>
    /// True if the study is receiving
    /// </summary>
    public bool IsReceiving
    { get { return IsState(States.Receiving); } } // IsReceiving


    /// <summary>
    /// Returns true if the study is locked
    /// </summary>
    public bool IsLocked
    { get { return IsState(States.Locked); } }

    /// <summary>
    /// Returns true if the study has a failed series
    /// </summary>
    public bool IsFailed
    { get { return IsState(States.Failed); } }

    #endregion

    #region methods

    /// <summary>
    /// Check the state specified against the internal field case insensitive
    /// </summary>
    /// <param name="state">state to compare to</param>
    /// <returns>true if the state equals the one specified</returns>
    public bool IsState(States state)
    { return (m_state & state) > 0; } 

    /// <summary>
    /// Returns String Representation of this object (Uid)
    /// </summary>
    /// <returns></returns>
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


    /// <summary>
    /// Creates an xml representation of a Study object.
    /// </summary>
    /// <param name="includeSeries">Indicates that the series and all their xml is included</param>
    /// <returns>Xlm string representing the Study object.</returns>
    public string ToXml(bool includeSeries)
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(XmlTag);
        {


          writer.WriteAttributeString(Attributes.Uid, m_uid);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("O"));
          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.AcessionNumber, m_accessionNumber);
          writer.WriteAttributeString(Attributes.Group, m_group);

          if (!string.IsNullOrEmpty(m_dir))
            writer.WriteAttributeString(Attributes.Directory, m_dir);

          if (!string.IsNullOrEmpty(m_url))
            writer.WriteAttributeString(Attributes.Url, m_url);

          writer.WriteAttributeString(Attributes.MedicalId, m_medicalId);

          writer.WriteAttributeString(Attributes.State, (int)m_state);

          if (m_modalities.Count > 0)
            writer.WriteAttributeString(Attributes.Modalities, m_modalities.ToDelimtedString('|'));

          if (m_applications.Count > 0)
            writer.WriteAttributeString(Attributes.Applications, m_applications.ToDelimtedString('|'));

          if (m_dob.HasValue)
            writer.WriteAttributeString(Attributes.PatientDob, m_dob.Value.ToString("o"));

          writer.WriteAttributeString(Attributes.PatientGender, (m_gender.ToString()).ToUpper().Substring(0, 1));
          writer.WriteAttributeString(Attributes.PatientWeight, m_weight.ToString(CultureInfo.InvariantCulture));
          writer.WriteAttributeString(Attributes.PatientName, m_name.ToDicom());
          writer.WriteAttributeString(Attributes.Physician, m_physician.ToString());

          if (includeSeries && m_series.Count > 0)
          {
            writer.WriteStartElement(Attributes.Series);

            foreach (Series series in m_series)
            {
              writer.WriteRaw(series.ToXml());
            }

            writer.WriteEndElement();
          }

          WriteAttributeStrings(writer);
          WriteElements(writer);

        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml()


 
    #endregion

    #region static methods

    /// <summary>
    /// Converts a DICOM string to a Gender type
    /// </summary>
    /// <param name="gender">Gender string representation</param>
    /// <returns>Gender enumerator value</returns></returns>
    public static Genders ConvertToGender(string sex)
    {
      Genders gender = Genders.Unknown;

      switch (sex.ToUpper()[0])
      {
        case 'M':
          gender = Genders.Male;
          break;
        case 'F':
          gender = Genders.Female;
          break;
        case 'O':
          gender = Genders.Other;
          break;
        default:
          gender = Genders.Unknown;
          break;

      };


      return gender;
    } // ConvertToGender(string gender)


 
     /// <summary>
    /// Creates a new instance of a Study object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Study object.</param>
    /// <returns>A Study object.</returns>
    public static Study Create(XmlNode node)
    {
      Study study = new Study();

      if ( study == null )
        throw new OutOfMemoryException("Failed to allocate study");


      study.Load(node);

      return study;
    } // Create( node )

    /// <summary>
    /// Creates a new instance of a Study object from xml file.
    /// </summary>
    /// <param name="file">The xml file.</param>
    /// <returns>A Study object.</returns>
    public static Study Create(string xmlOrFile)
    {

      XmlDocument doc = new XmlDocument();

      if (!xmlOrFile.StartsWith("<"))
      {

        if (!File.Exists(xmlOrFile))
        {
          Log.Error("The specified study xml file could not be found [FILE=" + xmlOrFile + "]", "Server", "FromFile");
          throw new System.IO.FileNotFoundException("Study xml file was not found");
        }

        doc.Load(xmlOrFile);


      }
      else
        doc.LoadXml(xmlOrFile);



      return Create(doc.DocumentElement);

    } // Create(string xmlFile)

    #endregion


    #region IEntity

    /// <summary>
    /// Creates an xml representation of a Study object.
    /// </summary>
    /// <returns>Xlm string representing the Study object.</returns>
    public override string ToXml()
    {
      return ToXml(false);
    } // ToXml()

    /// <summary>
    /// Creates a new instance of a Study object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Study object.</param>
    /// <returns>A Study object.</returns>
    public override void Load(XmlNode node)
    {

      if (node.Name == "data-set")
      {

        Dicom.Element element = new Dicom.Element(0, 0, string.Empty);
        Dicom.ElementCollection elements = Dicom.ElementCollection.Create(node);


        if (elements.TryGetElement(0x0020, 0x000d, ref element))
          m_uid = element.Value;

        if (elements.TryGetElement(0x0010, 0x0020, ref element))
          m_medicalId = element.Value;

        if (elements.TryGetElement(0x0008, 0x1030, ref element))
          m_description = element.Value;


        if (elements.TryGetElement(0x0008, 0x0010, ref element))
          m_id = element.Value;

        if (elements.TryGetElement(0x0008, 0x0050, ref element))
          m_accessionNumber = element.Value;

        if (elements.TryGetElement(0x0008, 0x0090, ref element))
          m_physician = element.Value;

        if (elements.TryGetElement(0x0010, 0x0010, ref element))
          m_name = PersonName.Create(element.Value);

        if (elements.TryGetElement(0x0010, 0x0030, ref element))
          m_dob = Dicom.Utility.ToDateTime(element.Value, "");

        if (elements.TryGetElement(0x0010, 0x1030, ref element))
          m_weight = Single.Parse(element.Value);

        if (elements.TryGetElement(0x0010, 0x0040, ref element))
          m_gender = ConvertToGender(element.Value);


        string date = string.Empty;
        string time = string.Empty;

        if ( elements.TryGetElement(0x0008, 0x0020, ref element))
          date = element.Value;
        else if ( elements.TryGetElement(0x0008, 0x0021 , ref element) )
          date = element.Value;
        else if ( elements.TryGetElement(0x0008, 0x0022 , ref element) )
          date = element.Value;
        else if (elements.TryGetElement(0x0008, 0x0023 , ref element) )
          date = element.Value;

        if ( elements.TryGetElement(0x0008, 0x0030, ref element))
          time = element.Value;
        else if ( elements.TryGetElement(0x0008, 0x0031 , ref element) )
          time = element.Value;
        else if ( elements.TryGetElement(0x0008, 0x0032 , ref element) )
          time = element.Value;
        else if (elements.TryGetElement(0x0008, 0x0033 , ref element) )
          time = element.Value;


        m_date = Dicom.Utility.ToDateTime(date, time);


      }
      else
      {
        base.Load(node);

        string version = string.Empty;
        node.TryGetAttributeValue<string>(EntityAttributes.Version, ref version);

        if (ActualVersion >= 4.0)
          LoadVersion4(node);
        else if (ActualVersion >= 3.0 || ActualVersion <= 4.0)
          LoadVersion3(node);
        else if (ActualVersion >= 2.0)
          LoadVersion2(node);
        else
          LoadVersion1(node);
      }
   
    } //  Load(XmlNode node)

    /// <summary>
    /// Clones the series
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Study study = new Study();

      if (study == null)
        throw new OutOfMemoryException("Failed to allocation study");


      study.m_uid = m_uid;
      study.m_id = m_id;
      study.m_date = m_date;
      study.m_description = m_description;
      study.m_dob = m_dob;
      study.m_group = m_group;
      study.m_medicalId = m_medicalId;
      study.m_modalities = m_modalities.Clone<List<string>>();
      study.m_applications = m_applications.Clone<List<string>>(); ;
      study.m_accessionNumber = m_accessionNumber;
      study.m_name = m_name;
      study.m_physician = m_physician;
      study.m_dir = m_dir;
      study.m_state = m_state;
      study.m_weight = m_weight;
      study.m_gender = m_gender;
      study.m_state = m_state;
      study.m_url = m_url;

      m_series.ForEach(s => study.m_series.Add(s));


      return study;

    } // Clone()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Study other = obj as Study;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_uid, other.m_uid);

    } // Equals( obj )


    #region Load Methods

    /// <summary>
    /// Loads version 4.0 and great of the study object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion4(XmlNode node)
    {
      // Current version.
      m_uid = node.Attributes[Attributes.Uid].Value;
      m_medicalId = node.Attributes[Attributes.MedicalId].Value;
      m_description = node.Attributes[Attributes.Description].Value;
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_id = node.Attributes[Attributes.Id].Value;
      m_state = (States)Int32.Parse(node.Attributes[Attributes.State].Value);

      node.TryGetAttributeValue<string>(Attributes.Directory, ref m_dir);
      node.TryGetAttributeValue<string>(Attributes.Url, ref m_url);
      node.TryGetAttributeValue<string>(Attributes.AcessionNumber, ref m_accessionNumber);

      m_group = node.Attributes[Attributes.Group].Value;
      m_physician = node.Attributes[Attributes.Physician].Value;
      m_name = PersonName.Create(node.Attributes[Attributes.PatientName].Value);
      m_medicalId = node.Attributes[Attributes.MedicalId].Value;
      m_gender = ConvertToGender(node.Attributes[Attributes.PatientGender].Value);

      if (!Single.TryParse(node.Attributes[Attributes.PatientWeight].Value, out m_weight))
        Single.TryParse(node.Attributes[Attributes.PatientWeight].Value, NumberStyles.Any, CultureInfo.InvariantCulture, out m_weight);

      m_modalities.Clear();
      string modalities = string.Empty;
      if (node.TryGetAttribute<string>(Attributes.Modalities, ref modalities))
      {
        string[] list = modalities.Split(new char[] { '|' });

        // ==========================
        // Add modalities to the list
        // ==========================
        foreach (string str in list)
        {
          m_modalities.Add(str);
        } // 
      }

      m_applications.Clear();
      string applications = string.Empty;
      if (node.TryGetAttribute<string>(Attributes.Applications, ref applications))
      {
        string[] list = applications.Split(new char[] { '|' });

        // ==========================
        // Add modalities to the list
        // ==========================
        foreach (string str in list)
        {
          m_applications.Add(str);
        } // 
      }


      node.TryGetAttributeValue<DateTime>(Attributes.PatientDob, ref m_dob);

      m_series.Load<Series>(node);


    } // LoadVersion4()

    /// <summary>
    /// Loads versions >= 3.0 and < 4.0 of the study object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion3(XmlNode node)
    {
      // Current version.
      m_uid         = node.Attributes[Attributes.Uid].Value;
      m_medicalId   = node.Attributes[Attributes.MedicalId].Value;
      m_description = node.Attributes[Attributes.Description].Value;
      m_date        = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_id          = node.Attributes[Attributes.Id].Value;
      m_state       = (States)Int32.Parse(node.Attributes[Attributes.State].Value);
      

      node.TryGetAttributeValue<string>(Attributes.AcessionNumber, ref m_accessionNumber);

      m_group = node.Attributes[Attributes.Group].Value;
      m_physician = node.Attributes[Attributes.Physician].Value;
      m_name = PersonName.Create(node.Attributes[Attributes.PatientName].Value);
      m_medicalId = node.Attributes[Attributes.MedicalId].Value;
      m_gender = ConvertToGender(node.Attributes[Attributes.PatientGender].Value);

      if (!Single.TryParse(node.Attributes[Attributes.PatientWeight].Value, out m_weight))
        Single.TryParse(node.Attributes[Attributes.PatientWeight].Value, NumberStyles.Any, CultureInfo.InvariantCulture, out m_weight);

      m_modalities.Clear();
      string modalities = string.Empty;
      if (node.TryGetAttribute<string>(Attributes.Modalities, ref modalities))
      {
        string[] list = modalities.Split(new char[] { '|' });

        // ==========================
        // Add modalities to the list
        // ==========================
        foreach (string str in list)
        {
          m_modalities.Add(str);
        } // 
      }

      m_applications.Clear();
      string applications = string.Empty;
      if (node.TryGetAttribute<string>(Attributes.Applications, ref applications))
      {
        string[] list = applications.Split(new char[] { '|' });

        // ==========================
        // Add modalities to the list
        // ==========================
        foreach (string str in list)
        {
          m_applications.Add(str);
        } // 
      }


      node.TryGetAttributeValue<DateTime>(Attributes.PatientDob, ref m_dob);

      m_series.Load<Series>(node);
   

    } // LoadVersion3()


    /// <summary>
    /// Loads versions >= 2.0 and < 3.0 of the study object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion2(XmlNode node)
    {
      // Current version.
      m_uid = node.Attributes["uid"].Value;
      m_medicalId = node.Attributes["mid"].Value;
      m_description = node.Attributes["desc"].Value;
      m_date = DateTimeOffset.Parse(node.Attributes["date"].Value);
      m_id = node.Attributes["id"].Value;

      node.TryGetAttributeValue<string>("an", ref m_accessionNumber);

      m_group = node.Attributes["grp"].Value;
      m_physician = PersonName.Create(node.Attributes["phys"].Value).ToDicom();

      XmlNode patient = node.SelectSingleNode("patient");

      m_name = PersonName.Create(patient.Attributes["names"].Value);
      m_medicalId = patient.Attributes["mid"].Value;
      m_gender = (Genders)(Enum.Parse(typeof(Genders), node.Attributes["sex"].Value));
      m_dob = DateTime.Parse(patient.Attributes["dob"].Value);

    } // LoadVersion2()

    /// <summary>
    /// Loads versions >= 1.0 and < 2.0 of the study object
    /// </summary>
    /// <param name="node"></param>
    private void LoadVersion1(XmlNode node)
    {
      // Old version.
      m_uid = node.Attributes["UID"].Value;

      // What to do with this?
      //study.Patient.MedicalId = node.Attributes[StudyFields.mid.ToString()].Value;
      m_description = node.Attributes["DESCRIPTION"].Value;
      m_date = DateTimeOffset.Parse(node.Attributes["SCAN_DATE"].Value);
      m_id = node.Attributes["DICOM_STUDY_ID"].Value;
      m_accessionNumber = node.Attributes["ACCESSION_NUMBER"].Value;
      m_group = node.Attributes["GROUP"].Value;

      if (node.SelectSingleNode("PHYSICIAN_NAME") != null)
      {
        m_physician = PersonName.Create(node.SelectSingleNode("PHYSICIAN_NAME")).ToDicom();
      }
      else if (node.SelectSingleNode("referringPhysicianName") != null)
      {
        m_physician = PersonName.Create(node.SelectSingleNode("referringPhysicianName")).ToDicom();
      }
    } // LoadVersion2()

    #endregion

    #endregion
  } // class Study

  #endregion



} // namespace Viatronix.Enterprise.Entities
