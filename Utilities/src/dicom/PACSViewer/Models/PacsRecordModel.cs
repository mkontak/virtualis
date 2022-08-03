// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom.PACSViewer.Models
{
  /// <summary>
  /// A record coming back from the pacs.  Depending on the search type, some fields may not be filled in.
  /// </summary>
  public class PacsRecordModel
  {

    #region fields

    /// <summary>
    /// The AE Title the record originated.
    /// </summary>
    private string m_retievalAETitle = string.Empty;

    /// <summary>
    /// The patient's name.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The patient's medical id.
    /// </summary>
    private string m_medicalId = string.Empty;

    /// <summary>
    /// The patient's birth date.
    /// </summary>
    private string m_birthDate = "";

    /// <summary>
    /// The study's uid.
    /// </summary>
    private string m_studyUid = string.Empty;

    /// <summary>
    /// The study date.
    /// </summary>
    private string m_studyDate = "";

    /// <summary>
    /// The referring physician.
    /// </summary>
    private string m_physician = string.Empty;

    /// <summary>
    /// The study description.
    /// </summary>
    private string m_studyDescription = string.Empty;

    /// <summary>
    /// The study's dicom id.
    /// </summary>
    private string m_studyId = string.Empty;

    /// <summary>
    /// The series' uid.
    /// </summary>
    private string m_seriesUid = string.Empty;

    /// <summary>
    /// The series date.
    /// </summary>
    private string m_seriesDate = "";

    /// <summary>
    /// The series modality.
    /// </summary>
    private string m_modality = string.Empty;
    
    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the reteive ae title.
    /// </summary>
    public string RetrieveAETitle
    {
      get { return m_retievalAETitle; }
      set { m_retievalAETitle = value; }
    } // RetrieveAETitle


    /// <summary>
    /// Gets or sets the patient name.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name


    /// <summary>
    /// Gets or sets the medical id.
    /// </summary>
    public string MedicalId
    {
      get { return m_medicalId; }
      set { m_medicalId = value; }
    } // MedicalId


    /// <summary>
    /// Gets or sets the birth date.
    /// </summary>
    public string BirthDate
    {
      get { return m_birthDate; }
      set { m_birthDate = value; }
    } // BirthDate


    /// <summary>
    /// Gets or sets the study uid.
    /// </summary>
    public string StudyUid
    {
      get { return m_studyUid; }
      set { m_studyUid = value; }
    } // StudyUid


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public string StudyDate
    {
      get { return m_studyDate; }
      set { m_studyDate = value; }
    } // StudyDate


    /// <summary>
    /// Gets or sets the physician.
    /// </summary>
    public string Physician
    {
      get { return m_physician; }
      set { m_physician = value; }
    } // Physician


    /// <summary>
    /// Gets or sets the study description.
    /// </summary>
    public string StudyDescription
    {
      get { return m_studyDescription; }
      set { m_studyDescription = value; }
    } // StudyDescritpion


    /// <summary>
    /// Gets or sets the study id.
    /// </summary>
    public string StudyId
    {
      get { return m_studyId; }
      set { m_studyId = value; }
    } // StudyId


    /// <summary>
    /// Gets or sets the series uid.
    /// </summary>
    public string SeriesUid
    {
      get { return m_seriesUid; }
      set { m_seriesUid = value; }
    } // SeriesUid


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public string SeriesDate
    {
      get { return m_seriesDate; }
      set { m_seriesDate = value; }
    } // SeriesDate


    /// <summary>
    /// Gets or sets the modality of the series.
    /// </summary>
    public string Modality
    {
      get { return m_modality; }
      set { m_modality = value; }
    } // Modality

    #endregion

    #region methods

    /// <summary>
    /// Loads the serialized xml.
    /// </summary>
    /// <param name="xml">The results retrieved from the pacs.</param>
    public void LoadXml(string xml)
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("element"))
      {
        XmlAttribute tag = node.Attributes["tag"];
        if (tag != null)
        {
          switch (tag.Value)
          {
            case "0008,0054": RetrieveAETitle = node.InnerText; break;

            // Patient fields.
            case "0010,0010": Name = node.InnerText; break;
            case "0010,0020": MedicalId = node.InnerText; break;
            case "0010,0030": BirthDate = node.InnerText; break;

            // Study fields.
            case "0020,000d": StudyUid = node.InnerText; break;
            case "0008,0020": StudyDate = node.InnerText; break;
            case "0008,0030": StudyDate = StudyDate + " " + node.InnerText; break;
            case "0008,0090": Physician = node.InnerText; break;
            case "0080,1030": StudyDescription = node.InnerText; break;
            case "0020,0010": StudyId = node.InnerText; break;
            
            // Series fields.
            case "0020,000e": SeriesUid = node.InnerText; break;
            case "0008,0021": SeriesDate = node.InnerText; break;
            case "0008,0031": SeriesDate = SeriesDate + " " + node.InnerText; break;
            case "0008,0060": Modality = node.InnerText; break;

            // Ignore these fiels.
            case "0008,0052": break; // Query retrieval level.
            case "0020,1208": break; // Number of study related instances.
            case "0020,1209": break; // Number of series related instances.

            default: Logging.Log.Warning("An unhandled element exists in the ", "PacsRecordModel", "LoadXml"); break;
          }
        }
      }
    } // LoadXml( xml )

    #endregion

  } // class PacsRecordModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.Models
