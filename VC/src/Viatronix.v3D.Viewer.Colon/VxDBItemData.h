// $Id: VxDBItemData.h,v 1.3 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxDBItemData.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_VXDBITEMDATA_H__4B24DFB9_03EC_4F3C_9B01_B85FF3424C18__INCLUDED_)
#define AFX_VXDBITEMDATA_H__4B24DFB9_03EC_4F3C_9B01_B85FF3424C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include "PersonName.h"
#include "FileExtensions.h"
#include "Date.h"
#include "DatabaseConst.h"
#include "Browser.h"

namespace ConsoleLib
{

  /**
   * Class for information on a patient study
   */
  class VxDBItemData  
  {
  public:
    /// State of a dataset item
    enum ItemStateEnum { DEFAULT, GRAYED, CHANGED, DELETED };

  public:
    /**
     * Patient information
     */
    class PatientData
    {
    public:
      /// Constructor
      PatientData() : m_iAge(0), m_fWeight(0.0f) { }

    public:
      /// Patient name
      PersonName        m_name;
      /// Date of birth
      vx::Date          m_dob;
      /// Patient age
      uint4             m_iAge;
      /// Patient gender
      std::string       m_sSex;
      /// Patient weight
      float4            m_fWeight;
    };

  public:
    /// Constructor
    VxDBItemData();

  public:
    /// Study record ID
    int8                        m_idStudyRecord;
    /// Dataset ID (if the entry is a dataset)
    int8                        m_idDataset;
    /// Map of study locations (dataset type to study loc ID)
    std::map<DatasetTypeEnum,int8> m_studyLocMap;
    /// Patient demographic information
    PatientData                 m_patient;
    /// Site ID
    int4                        m_iSiteID;
    /// Patient medical ID
    std::string                 m_sMedicalID;
    /// Type of study
    StudyTypeEnum               m_eStudyType;
    /// Study date
    SYSTEMTIME                  m_scanDate;
    /// Series date
    SYSTEMTIME                  m_seriesDate;
    /// Referring physician
    std::string                 m_sReferPhys;
    /// Path to study
    std::string                 m_sFilePath;
    /// Filename root (part of title)
    std::string                 m_sFileRoot;
    /// Series UID
    std::string                 m_sSeriesUID;
    /// Study UID
    std::string                 m_sStudyUID;
    /// Additional display information for the study
    std::string                 m_sFileAdditional;
    /// Status of datasets in study, or status of the individual entry
    Browser::StudyStateInfo     m_datasetsStatus;
    /// Type of item
    vx::FileClassificationEnum  m_eFileType;
    /// Item state
    ItemStateEnum               m_eItemState;
    ///
    std::vector<int8>           m_vLocIds;
    
    /// Stores the change notification handle (on the study directory)
    HANDLE                      m_hChangeNotify;
  }; // VxDBItemData
  /// Reference to item data
  typedef VxDBItemData& VxDBItemDataRef;
  /// Constant reference to item data
  typedef const VxDBItemData& VxDBItemDataConstRef;
  /// Pointer to item data
  typedef VxDBItemData* VxDBItemDataPtr;
  /// Constant Pointer to item data
  typedef const VxDBItemData* VxDBItemDataConstPtr;
  /// Item data vector
  typedef std::vector<VxDBItemData> VxDBItemDataVector;
  /// Item data list
  typedef std::list<VxDBItemData> VxDBItemDataList;

} // namespace ConsoleLib

#endif // !defined(AFX_VXDBITEMDATA_H__4B24DFB9_03EC_4F3C_9B01_B85FF3424C18__INCLUDED_)

// $Log: VxDBItemData.h,v $
// Revision 1.3  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.2  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3.4.2.4.1  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.3.4.2.8.1  2005/05/25 15:50:20  geconomos
// initial backup
//
// Revision 1.3.4.2  2003/07/07 20:31:53  jmeade
// Code standards.
//
// Revision 1.3.4.1  2003/06/06 23:03:24  jmeade
// code review prep.
//
// Revision 1.3  2002/10/07 23:04:55  jmeade
// Replaced MFC scan date with Win32 SYSTEMTIME.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxDBItemData.h,v 1.3 2007/03/07 22:34:05 jmeade Exp $
// $Id: VxDBItemData.h,v 1.3 2007/03/07 22:34:05 jmeade Exp $
