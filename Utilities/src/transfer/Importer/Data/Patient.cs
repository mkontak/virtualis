// $Id: Patient.cs,v 1.32.2.6 2009/02/06 19:19:12 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Collections.Generic;
using System.Runtime.Serialization;

using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.IO;

namespace Viatronix.Utilities.Importer.Data
{

  #region enumerations

  /// <summary>
  /// Patient fields directly related to the patient table in the database
  /// </summary>
  [Serializable]
  public enum PatientFields
  {
    SITE_ID,
    lname,
    fname,
    mname,
    sfx,
    pfx,
    mid,
    sex,
    dob,
    name,
    studies
  } // enum PatientFields

  #endregion

  #region Patient class

  /// <summary>
  /// Implements a Patient object
  /// </summary>
  [Serializable]
  public class Patient : ISerializable, ICloneable
  {

    #region constants

    /// <summary>
    /// XML file name 
    /// </summary>
    private static string m_xmlFileName = "patient.xml";

    #endregion

    #region fields

    /// <summary>
    /// Patients directory
    /// </summary>
    private string m_dir = string.Empty;

    /// <summary>
    /// Patients medical id
    /// </summary>
    private string m_medicalId = string.Empty;

    /// <summary>
    /// Patient weight
    /// </summary>
    private int m_weight = 0;

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
    private DateTime m_dob = DateTime.MinValue;


    /// <summary>
    /// Patients studies
    /// </summary>
    private List<Study> m_studies = new List<Study>();

    /// <summary>
    /// Directory name
    /// </summary>
    private string m_directoryName = string.Empty;

    #endregion

    #region construction and destruction

    /// <summary>
    /// Constructor
    /// </summary>
    public Patient()
    {
    } // Patient()

    /// <summary>
    /// Deserialization constructor
    /// </summary>
    /// <param name="info">serialization info</param>
    /// <param name="context">context stream</param>
    public Patient(SerializationInfo info, StreamingContext context)  
    {
      m_medicalId     = info.GetString(PatientFields.mid.ToString());
      m_gender        = (Genders)info.GetInt32(PatientFields.sex.ToString());
      m_name          = (PersonName)info.GetValue(PatientFields.name.ToString(), typeof(PersonName));

      m_studies = (List<Study>)info.GetValue(PatientFields.studies.ToString(), typeof(List<Study>));

    } // Patient(SerializationInfo info, StreamingContext context)  

    #endregion

    #region properties
    
    /// <summary>
    /// Gets the default XML file name
    /// </summary>
    public string DefaultXmlFileName
    {
      get { return m_xmlFileName; }
    } // XmlFileName

    /// <summary>
    /// Gets the XML file name
    /// </summary>
    public string XmlFileName
    {
      get { return m_xmlFileName; }
    } // XmlFileName

    /// <summary>
    /// Directory
    /// </summary>
    public string Directory
    {
      get { return m_dir;  }
      set { m_dir = value;  }
    } // Directory

    /// <summary>
    /// Location
    /// </summary>
    public string Location
    {
      get { return m_dir; }
    } // Location

    /// <summary>
    /// Sets/Gets the medical id
    /// </summary>
    public string MedicalId
    {
      get { return m_medicalId; }
      set { m_medicalId = value; }
    } // MedicalId
    

    /// <summary>
    /// Gets the patients age
    /// </summary>
    public TimeSpan Age
    {
      get { return DateTime.Now.Subtract(m_dob); }
    } // Age


    /// <summary>
    /// Sets/Gets the patients weight
    /// </summary>
    public int Weight
    {
      get { return m_weight; }
      set { m_weight = value; }
    } // Weight


    /// <summary>
    /// Sets/Gets the patients gender
    /// </summary>
    public Genders Gender
    {
      get { return m_gender; }
      set { m_gender = value; }
    } // Gender


    /// <summary>
    /// Sets/Gets the patient name
    /// </summary>
    public PersonName Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name

    
    /// <summary>
    /// Sets/Gets the patients DOB
    /// </summary>
    public DateTime Dob
    {
      get { return m_dob; }
      set { m_dob = value; }
    } // Dob




    /// <summary>
    /// Gets/Sets the patients studies
    /// </summary>
    public List<Study> Studies
    {
      get { return m_studies; }
      set { m_studies = value; }
    } // Studies

 
    #endregion

    #region methods

    /// <summary>
    /// Returns String Representation of this object (NAME)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_name.ToString();
    } // ToString()

    /// <summary>
    /// Serializes object
    /// </summary>
    /// <param name="s">serialization info</param>
    /// <param name="c">context stream</param>
    public void GetObjectData(SerializationInfo s, StreamingContext c) 
    {
      s.SetType(this.GetType());

      s.AddValue(PatientFields.name.ToString(), m_name);
      s.AddValue(PatientFields.mid.ToString(), m_medicalId);
      s.AddValue(PatientFields.sex.ToString(), (int)m_gender);
      s.AddValue(PatientFields.dob.ToString(), m_dob);
      s.AddValue(PatientFields.studies.ToString(), m_studies, m_studies.GetType());

    } // GetObjectData(SerializationInfo s, StreamingContext c) 


    /// <summary>
    /// Creates the patient path and write the xml file
    /// </summary>
    /// <param name="basePath">base path for patient directory</param>
    /// <param name="createXml">Flag to create the xml file</param>
    /// <returns>Returns the path</returns>
    //public string CreatePath(string basePath, bool createXml)
    //{
    //  string path = PathUtilities.CombinePaths(basePath, this.Directory);

    //  if ( ! System.IO.Directory.Exists(path) )
    //  {
    //    Viatronix.Logging.Log.Debug("Creating patient path [PATH=" + path + "]", "Patient", "CreatePath");
    //    System.IO.Directory.CreateDirectory(path);
    //  } // END ... If the path does not exist
    
    //  if ( createXml )
    //  {
    //    Serialize(path);
    //  }

    //  return path;
    //} // CreatePath(string basePath, bool createXml)

    /// <summary>
    /// Serailzies to path
    /// </summary>
    /// <param name="path">Path to serialize</param>
    //public void Serialize(string path)
    //{
    
    //  string xmlFile = System.IO.Path.Combine(path,m_xmlFileName);

    //  XmlDocument doc = new XmlDocument();
    //  doc.LoadXml(this.ToXml());
    //  XmlTextWriter writer = new XmlTextWriter(xmlFile, Encoding.ASCII);
    //  writer.Formatting = Formatting.Indented;
    //  doc.Save(writer);
    //  writer.Close();
    //} // Serialize(string path)


    /// <summary>
    /// Get hash code
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode ();
    } // GetHashCode()


    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="obj">Object to check euqlaity against</param>
    /// <returns>True if equal</returns>
    public override bool Equals(object obj)
    {
      return Equals((Patient)obj);
    } // Equals()


    /// <summary>
    /// Determines equality
    /// </summary>
    /// <param name="patient">Patient to evaluate against</param>
    /// <returns>True if the patient equals this patient</returns>
    public bool Equals(Patient patient)
    {
      return (this.Name.Equals(patient.Name) && string.Compare(this.MedicalId,patient.MedicalId) == 0 );
    } // Equals()


    /// <summary>
    /// Creates an xml representation of a Patient object.
    /// </summary>
    /// <returns>Xlm string representing the Patient object.</returns>
    public string ToXml()
    {
      using( MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter( new StreamWriter( stream ));

        writer.WriteStartElement( "patient" );


        writer.WriteAttributeString( PatientFields.mid.ToString(),            MedicalId);
        writer.WriteAttributeString( PatientFields.sex.ToString(),            Gender.ToString());
        writer.WriteAttributeString( PatientFields.dob.ToString(),            Dob.ToString("o"));
 
        // write out PatientName node
        writer.WriteNode( new XmlTextReader( new StringReader( this.Name.ToXml() )), true );


        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString( stream.ToArray() );
      }
    } // ToXml()


    /// <summary>
    /// Clones this instance.
    /// </summary>
    /// <returns>Returns a clone of this instance.</returns>
    public object Clone()
    {
      Patient patient = new Patient();

      patient.m_dir = (string)m_dir.Clone();
      patient.m_directoryName = (string)m_directoryName.Clone();
      patient.m_dob = m_dob;
      patient.m_gender = ConvertToGender(m_gender.ToString());
      patient.m_medicalId = (string)m_medicalId;
      patient.m_name = (PersonName)m_name.Clone();
      patient.m_weight = m_weight;

      foreach (Study study in m_studies)
      {
        patient.Studies.Add((Study)study.Clone());
      }

      return patient;
    } // Clone()

    #endregion

    #region static methods

    /// <summary>
    /// Converts a string to a Gender type
    /// </summary>
    /// <param name="gender">Gender rstring rperesentation</param>
    /// <returns>Gender enumerator value</returns></returns>
    public static Genders ConvertToGender(string gender)
    {
      Genders g = Genders.Unknown;

      if ( string.Compare(gender,"M",true) == 0 )
      {
        g = Genders.Male;
      }
      else if (string.Compare(gender,"F",true) == 0 )
      {
        g = Genders.Female;
      }
      else if ( string.Compare(gender,"O",true) == 0 )
      {
        g = Genders.Other;
      }

      return g;
    } // ConvertToGender(string gender)

    /// <summary>
    /// Creates a new instance of a patient object from xml file.
    /// </summary>
    /// <param name="file">The xml file.</param>
    /// <returns>A patient object.</returns>
    public static Patient Create(string xmlFile)
    {

      if (xmlFile.Length == 0)
      {
        throw new System.IO.InvalidDataException("No file specified");
      }
      else if (! File.Exists(xmlFile))
      {
        Logging.Log.Error("The specified patient xml file could not be found [FILE=" + xmlFile + "]", "Patient", "FromFile");
        throw new System.IO.FileNotFoundException("Patient xml file was not found");
      }
        

      XmlDocument doc = new XmlDocument();
      doc.Load(xmlFile);
      return Create(doc.DocumentElement);
    } // Create(string xmlFile)


    /// <summary>
    /// Creates a new instance of a Patient object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Patient object.</param>
    /// <returns>A Patient object.</returns>
    public static Patient Create( XmlNode node )
    {
      Patient patient   = new Patient();

      XmlAttribute version = node.Attributes["ver"];
      if (version != null && version.Value == "2.0")
      {
        // Dealing with new version.
        patient.Name.LastName   = node.Attributes[PatientFields.lname.ToString()].Value;
        patient.Name.FirstName  = node.Attributes[PatientFields.fname.ToString()].Value;
        patient.Name.MiddleName = node.Attributes[PatientFields.mname.ToString()].Value;
        patient.Name.Prefix     = node.Attributes[PatientFields.pfx.ToString()].Value;
        patient.Name.Suffix     = node.Attributes[PatientFields.sfx.ToString()].Value;
        patient.MedicalId       = node.Attributes[PatientFields.mid.ToString()].Value;
        patient.Gender          = ConvertToGender(node.Attributes[PatientFields.sex.ToString()].Value);
        patient.Dob             = node.Attributes[PatientFields.dob.ToString()] != null ? DateTime.Parse(node.Attributes[PatientFields.dob.ToString()].Value) : DateTime.MinValue;
      }
      else
      {
        patient.Name            = PersonName.Create(node.SelectSingleNode("name"));
        patient.MedicalId       = node.Attributes["MEDICAL_ID"].Value;
        patient.Gender          = (Genders)Enum.Parse(typeof(Genders), node.Attributes["SEX"].Value, true);
        patient.Dob             = DateTime.Parse(node.Attributes["DOB"].Value);
      }

      return patient;
    } // FromXml( node )


    ///// <summary>
    ///// Creates a patient from a data row
    ///// </summary>
    ///// <param name="row">data row</param>
    ///// <returns>Returns the newly created patient</returns>
    //public static Patient Create(DataRow row)
    //{
    //  Patient patient = new Patient();



    //  if (row.Table.Columns["mid"] != null)
    //  {
    //    patient.MedicalId = row["mid"].ToString();

    //    patient.Name.FirstName = row["fname"].ToString();
    //    patient.Name.LastName = row["lname"].ToString();
    //    patient.Name.MiddleName = row["mname"].ToString();
    //    patient.Name.Prefix = row["pfx"].ToString();
    //    patient.Name.Suffix = row["sfx"].ToString();
    //    patient.Directory = row["dir"].ToString();


    //    if (row["dob"] != DBNull.Value) patient.Dob = System.Convert.ToDateTime(row["dob"]);

    //    patient.Gender = Patient.ConvertToGender(row["sex"].ToString());
    //  }
    //  else if (row.Table.Columns["patientMid"] != null)
    //  {

    //    patient.MedicalId = row["patientMid"].ToString();
    //    patient.Name.FirstName = row["patientFname"].ToString();
    //    patient.Name.LastName = row["patientLName"].ToString();
    //    patient.Name.MiddleName = row["patientMname"].ToString();
    //    patient.Name.Prefix = row["patientPfx"].ToString();
    //    patient.Name.Suffix = row["patientSfx"].ToString();
    //    patient.Directory = row["patientDir"].ToString();

    //    if (row["patientDob"] != DBNull.Value) patient.Dob = System.Convert.ToDateTime(row["patientDob"]);

    //    patient.Gender = Patient.ConvertToGender(row["patientSex"].ToString());

    //  }

    //  return patient;

    //} // ToPatient()

    #endregion

	} // class Patient

  #endregion

} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: Patient.cs,v $
// Revision 1.32.2.6  2009/02/06 19:19:12  kchalupa
// Region crashproofing.
//
// Revision 1.32.2.5  2009/01/20 17:11:49  kchalupa
// Issue #6217: Fix for Italy region.  Needed to put \\: instead of : in date format.
//
// Revision 1.32.2.4  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.32.2.3  2008/04/09 23:51:40  mkontak
// Issue 6066
//
// Revision 1.32.2.2  2007/05/30 19:22:13  mkontak
// Fixed problems with the time not specifying AM and PM
//
// Revision 1.32.2.1  2007/04/20 16:33:01  mkontak
// Fix apostrophe in name
//
// Revision 1.32  2007/03/07 19:05:53  geconomos
// code review
//
// Revision 1.31  2007/03/06 21:17:27  mkontak
// Coding standards
//
// Revision 1.30  2006/09/22 19:41:22  mkontak
// Comments
//
// Revision 1.29  2006/09/22 19:29:25  mkontak
// Added some error checking to the FromFile()
//
// Revision 1.28  2006/09/22 19:25:26  mkontak
// Added FromFile() method
//
// Revision 1.27  2006/01/16 17:50:33  mkontak
// Minor change to add a DefaultXmlFileName property that is static
//
// Revision 1.26  2005/12/30 17:17:14  mkontak
// New DB changes
//
// Revision 1.25  2005/11/02 18:07:12  mkontak
// makes use of the PathUtilities.CombinePath()
//
// Revision 1.24  2005/10/27 16:52:26  mkontak
// Chnages to ICacheObject
//
// Revision 1.23  2005/10/03 11:32:58  mkontak
// Added ICache interfaces to objects
//
// Revision 1.22  2005/08/18 13:06:47  mkontak
// Remove PatientId from series we will now use Study.Patient.Id
//
// Revision 1.21  2005/05/19 13:55:00  mkontak
// Removed age
//
// Revision 1.20  2005/05/02 19:32:58  mkontak
// Added ISerializable interface to objects
//
// Revision 1.19  2005/04/29 13:44:59  mkontak
// Serializable
//
// Revision 1.18  2005/04/28 13:21:33  mkontak
// Added series dependents, fils array and Lock
//
// Revision 1.17  2005/04/11 14:40:39  mkontak
// Added methods to create path/xml
//
// Revision 1.16  2005/04/11 14:38:22  mkontak
// Added methods to create path/xml
//
// Revision 1.15  2005/03/28 22:25:58  gdavidson
// Added enumerations for each for each database field.
//
// Revision 1.14  2005/03/24 22:27:39  mkontak
// Changes to improve usability
//
// Revision 1.13  2005/03/24 21:07:14  mkontak
// Fixes
//
// Revision 1.12  2005/03/24 05:16:46  mkontak
// Added series processes
//
// Revision 1.11  2005/03/23 19:14:17  mkontak
// Initialize most dates to the MinValue. Fix XML reading and writing with new
// dates.
//
// Revision 1.10  2005/03/22 22:16:29  mkontak
// Fixes
//
// Revision 1.9  2005/03/21 21:07:14  mkontak
// Fixes
//
// Revision 1.8  2001/01/02 09:43:56  mkontak
// Added missing fields and added funtionality
//
// Revision 1.7  2005/03/10 21:08:18  mkontak
// Minor changes
//
// Revision 1.6  2005/03/03 18:29:36  mkontak
// Added UID type
//
// Revision 1.5  2005/02/18 16:12:00  gdavidson
// Added ToXml and FromXml methods.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/Patient.cs,v 1.32.2.6 2009/02/06 19:19:12 kchalupa Exp $
// $Id: Patient.cs,v 1.32.2.6 2009/02/06 19:19:12 kchalupa Exp $

#endregion