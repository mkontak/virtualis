// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Utilities.Dicom.PACSViewer.Models;

namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
{
  /// <summary>
  /// The corresponding view model for the pacs result model.
  /// </summary>
  public class PacsRecordViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The model we're based off of.
    /// </summary>
    private PacsRecordModel m_model = new PacsRecordModel();

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the retrieval ae title.
    /// </summary>
    public string RetrievalAETitle
    {
      get { return m_model.RetrieveAETitle; }
      set 
      {
        m_model.RetrieveAETitle = value;
        RaisePropertyChanged("RetrievalAETitle");
      }
    } // RetrievalAETitle


    /// <summary>
    /// Gets or sets the name of te patient.
    /// </summary>
    public string Name
    {
      get { return m_model.Name; }
      set
      {
        m_model.Name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the medical id.
    /// </summary>
    public string MedicalId
    {
      get { return m_model.MedicalId; }
      set
      {
        m_model.MedicalId = value;
        RaisePropertyChanged("MedicalId");
      }
    } // MedicalId


    /// <summary>
    /// Gets or sets the patient's birth date.
    /// </summary>
    public string BirthDate
    {
      get { return m_model.BirthDate; }
      set
      {
        m_model.BirthDate = value;
        RaisePropertyChanged("BirthDate");
      }
    } // BirthDate


    /// <summary>
    /// Gets or sets the study uid.
    /// </summary>
    public string StudyUid
    {
      get { return m_model.StudyUid; }
      set
      {
        m_model.StudyUid = value;
        RaisePropertyChanged("StudyUid");
      }
    } // StudyUid


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public string StudyDate
    {
      get { return m_model.StudyDate; }
      set
      {
        m_model.StudyDate = value;
        RaisePropertyChanged("StudyDate");
      }
    } // StudyDate


    /// <summary>
    /// Gets or sets the physician.
    /// </summary>
    public string Physician
    {
      get { return m_model.Physician; }
      set
      {
        m_model.Physician = value;
        RaisePropertyChanged("Physician");
      }
    } // Physician


    /// <summary>
    /// Gets or sets the study description.
    /// </summary>
    public string StudyDescription
    {
      get { return m_model.StudyDescription; }
      set
      {
        m_model.StudyDescription = value;
        RaisePropertyChanged("StudyDescription");
      }
    } // StudyDescription


    /// <summary>
    /// Gets or sets the study id.
    /// </summary>
    public string StudyId
    {
      get { return m_model.StudyId; }
      set
      {
        m_model.StudyId = value;
        RaisePropertyChanged("StudyId");
      }
    } // StudyId


    /// <summary>
    /// Gets or sets the series uid.
    /// </summary>
    public string SeriesUid
    {
      get { return m_model.SeriesUid; }
      set
      {
        m_model.SeriesUid = value;
        RaisePropertyChanged("SeriesUid");
      }
    } // SeriesUid


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public string SeriesDate
    {
      get { return m_model.SeriesDate; }
      set
      {
        m_model.SeriesDate = value;
        RaisePropertyChanged("SeriesDate");
      }
    } // SeriesDate


    /// <summary>
    /// Gets or sets the modality.
    /// </summary>
    public string Modality
    {
      get { return m_model.Modality; }
      set
      {
        m_model.Modality = value;
        RaisePropertyChanged("Modality");
      }
    } // Modality

    #endregion

    #region methods

    /// <summary>
    /// Loads the xml and fills out the object.
    /// </summary>
    public void LoadXml(string xml)
    {
      m_model.LoadXml(xml);
    } // LoadXml( xml )

    #endregion

  } // class PacsRecordViewModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
