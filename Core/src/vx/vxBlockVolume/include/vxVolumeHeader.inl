// $Id: vxVolumeHeader.inl,v 1.15 2007/09/19 15:30:34 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// defines
#define FILE_REVISION "$Revision: 1.15 $"


/** 
 * Returns the variable VarMap volume header for volume semantics info for reading
 *
 * @param eEntry the desired header entry
 * @param value  the templatized output value
 */
template <class Type> 
void vxVolumeHeader::GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum eEntry, 
                                    Type & value) const
//*******************************************************************
{
  try
  {
    std::string sResult;
 
    // can we find the value in the cache?
    ////////////////////////////////////////
    if (m_vCache[eEntry].first == true)
    {
      sResult = m_vCache[eEntry].second;
    }
    else
    {
      if (m_varHeader.GetHdrVar(m_vsVarHeaderEntries[eEntry], sResult) == false)
      {
        // not existing in varmap? --> go expensive route
        ////////////////////////////////////////////////////
        switch(eEntry)
        {
          case vxVolumeHeader::DataModality:
            sResult = GetDicomFieldValue("0008", "0060");
            if (sResult.empty())
            {
              if (GetFileVersion() != std::pair<int4,int4>(1, 0))
              {
                sResult = "CT";
              }
              else
              {
                // there happen to be a few volumes floating around that do not
                // contain a DICOM header yet should be considered CT data
                // 8-bit data should be considered raw, otherwise CT
                if (GetVoxelTypeEnum() == vxVolumeHeader::UINT1 ||
                  GetVoxelTypeEnum() == vxVolumeHeader::INT1)
                {
                  sResult = "RAW";
                }
                else
                {
                  sResult = "CT";
                }
              }
              break;
          case vxVolumeHeader::StudyDate:
            sResult = GetDicomFieldValue("0008", "0020");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0022");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0021");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0023");
            break;
          case vxVolumeHeader::StudyTime:
            sResult = GetDicomFieldValue("0008", "0030");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0032");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0031");
            if (sResult.empty())
              sResult = GetDicomFieldValue("0008", "0033");
            break;
          case vxVolumeHeader::StudyDescription:
            sResult = GetDicomFieldValue("0008", "1030");
            break;
          case vxVolumeHeader::PhysicianName:
						sResult = ::PersonName(GetDicomFieldValue("0008", "0090")).GetFullName();
            break;        
          case vxVolumeHeader::PatientName:
						sResult = ::PersonName(GetDicomFieldValue("0010", "0010")).GetFullName();
            break;
          case vxVolumeHeader::PatientID:
            sResult = GetDicomFieldValue("0010", "0020");
            break;
          case vxVolumeHeader::PatientBirthdate:
            sResult = GetDicomFieldValue("0010", "0030");
            break;
          case vxVolumeHeader::PatientAge:
            sResult = GetDicomFieldValue("0010", "1010");
            break;
          case vxVolumeHeader::PatientSex:
            sResult = GetDicomFieldValue("0010", "0040");
            break;
          case vxVolumeHeader::Voltage:
            sResult = GetDicomFieldValue("0018", "0060");
            break;
          case vxVolumeHeader::Current:
            sResult = GetDicomFieldValue("0018", "1151");
            break;
          case vxVolumeHeader::ScanningSequence:
            sResult = GetDicomFieldValue("0018", "0020");
            break;
          case vxVolumeHeader::AcquisitionType:
            sResult = GetDicomFieldValue("0018", "0023");
            break;
          case vxVolumeHeader::EchoTime:
            sResult = GetDicomFieldValue("0018", "0081");
            break;
          case vxVolumeHeader::RepetitionTime:
            sResult = GetDicomFieldValue("0018", "0080");
            break;
          case vxVolumeHeader::ReconstructionInterval:
            sResult = GetDicomFieldValue("0018", "0088");
            break;
          case vxVolumeHeader::Tilt:
            sResult = GetDicomFieldValue("0018", "1120");
            break;
          case vxVolumeHeader::ExposureTime:
            sResult = GetDicomFieldValue("0018", "1150");
            break;
          case vxVolumeHeader::TransducerFrequency:
            sResult = GetDicomFieldValue("0018", "6030");
            break;
          case vxVolumeHeader::Contrast:
            sResult = GetDicomFieldValue("0018", "1040") + " " + GetDicomFieldValue("0018", "0010");
            break;
          case vxVolumeHeader::FieldOfView:
            sResult = GetDicomFieldValue("0018", "1100");
            break;
          case vxVolumeHeader::BodyPart:
            sResult = GetDicomFieldValue("0018", "0015");
            break;
          case vxVolumeHeader::SliceThickness:
            sResult = GetDicomFieldValue("0249", "xx36");
            break;
          case vxVolumeHeader::BitStored:
            sResult = GetDicomFieldValue("0028", "0101");
            break;
          case vxVolumeHeader::Scale:
            sResult = "1.0F"; 
            break;
          case vxVolumeHeader::Bias:
            {
              // since it is NOT in the var map, older CT volumes need to be 1000, for all others ("raw") 0
              std::string sModality("");
              GetHeaderEntry(vxVolumeHeader::DataModality, sModality);

              std::string sCacheValue;
              bool bFound = GetVariableVolumeHeader().GetHdrVar("bias", sCacheValue); 
              sResult = (bFound ? sCacheValue : ((sModality == "CT")? "1000.0F" : "0.0F"));
            }
            break;
          default:
            break;
          }
        }

        // special case handling
        //////////////////////////
        //f (eEntry == vxVolumeHeader::PhysicianName)
        //{
        //  value = sResult;
        //}
        //else if (sResult.empty() == false)
        //{
        //  std::istrstream is(sResult.c_str());
        //  is >> value; 
        //}
      } // not in variable header?
      else if (eEntry == vxVolumeHeader::PatientName)
      {
				sResult = ::PersonName(sResult).GetLastFirstMidSuffix();
      }

      // now do special case handling
      switch(eEntry)
      {
      case vxVolumeHeader::StudyDate:
        {
          // fix up the study date so it's readable
          std::string sTmp(sResult);
          if (sTmp.size() == 8)
          {
            vx::Date studyDate;
            studyDate.SetDateFromDicom(sTmp);
            sResult = studyDate.GetDate();
          }
        } // StudyDate?
        break;
      case vxVolumeHeader::PatientBirthdate:
        {
          // fix up the patient PatientBirthdate so it's readable
          std::string sTmp(sResult);
          if (sTmp.size() == 8)
          {
            vx::Date patientBirthdate;
            patientBirthdate.SetDateFromDicom(sTmp);
            sResult = patientBirthdate.GetDate();
          }
        } // PatientBirthdate?
      default:
        break;
      }

      // update cached value
      ////////////////////////
      m_vCache[eEntry].second = sResult;
      m_vCache[eEntry].first  = true;
    }

    if ( eEntry != vxVolumeHeader::DicomHeader && eEntry != vxVolumeHeader::PatientName )
    {
      // convert from string to template parameter using operator >> of iststream
      std::istrstream is(sResult.c_str());
      is >> value; 
    }
    else
    {
      value = sResult;
    }

  }
  catch (...)
  {
    LogDbg("unhandled exception", "vxVolumeHeader", "GetHeaderEntry");
  }
} // GetHeaderEntry()


/** 
 * Sets the variable VarMap volume header
 * @param varMap variable map
 */
void vxVolumeHeader::SetVariableVolumeHeader(const VarMap & varMap)
//*******************************************************************
{
  ClearCachedValues();
  m_varHeader = varMap; 
}


/**
 * Invalidate all cached values
 */
void vxVolumeHeader::ClearCachedValues()
//*******************************************************************
{
  // invalidate cached value(s)
  ///////////////////////////////
  //m_bCachedBitsStoredValid = false;
  //m_bCachedBiasValid       = false;

  for ( unsigned int i(0); i < m_vCache.size(); ++i )
  {
    m_vCache[i].first = false;
  }
} // ClearCachedValues()


/** 
 * Returns bias
 * @param value to be returned
 * @return whether or not it exists in the var map
 * /
bool vxVolumeHeader::GetCachedBias(std::string & sValue) const
//*******************************************************************
{
  if (m_bCachedBiasValid == true)
  {
    if (m_bCachedBiasInMap == true)
    {
      sValue = m_sCachedBias;
    }
    return m_bCachedBiasInMap;
  }
  else // slow path of going through VarMap and DICOM fields
  {
    std::string sValueTmp;
    m_bCachedBiasInMap = GetVariableVolumeHeader().GetHdrVar("bias", sValueTmp);
    if (m_bCachedBiasInMap == true)
    {
      m_sCachedBias = sValueTmp;
    }
    m_bCachedBiasValid = true;
    sValue = sValueTmp;
    return m_bCachedBiasInMap;
  }
} // GetCachedBias()


/** 
 * Returns stored bits
 * @param default value
 * @return result
 * /
std::string vxVolumeHeader::GetCachedBitsStored(const std::string & sDefaultValue) const
//*******************************************************************
{
  std::string sResult;
  GetHeaderEntry(vxVolumeHeader::VarVolHeaderEntryEnum::BitStored, sResult);
  return sResult;
}
*/


#undef FILE_REVISION


// Revision History:
// $Log: vxVolumeHeader.inl,v $
// Revision 1.15  2007/09/19 15:30:34  gdavidson
// Specified a namespace for PersonName
//
// Revision 1.14  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.13  2005/10/26 17:27:47  dongqing
// move the Bias set code into GetHeaderEntry() function
// This should fix the viewer HU value shift bug
//
// Revision 1.12  2005/07/22 10:50:22  mkontak
// Fixed unsigned/signed warning
//
// Revision 1.11  2005/05/23 11:15:38  michael
// Reworked Scale and Bias to always be returned
//
// Revision 1.10  2005/05/10 20:00:24  michael
// Made BitStored part of the enum of header vars instead of having a dedicated
// access function.
//
// Revision 1.9  2005/04/28 15:10:40  michael
// Fixed problem of local variable
//
// Revision 1.8  2005/03/08 19:09:33  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.7.2.1  2005/02/01 13:50:43  frank
// the bias-in-map member variable was being shadowed by a local declaration
//
// Revision 1.7  2004/12/15 20:38:58  frank
// fixed patient name in pane (any spaces were ending the string)
//
// Revision 1.6  2004/11/19 00:37:10  mkontak
// Fix for dicom header
//
// Revision 1.5  2004/11/02 18:10:36  michael
// changed caching so that it works for ANY entry. Got 33% speed-up for simple
// axial MPRs!!!
//
// Revision 1.3  2004/09/01 17:03:50  frank
// fixed the modality computation for old calcium scoring volumes
//
// Revision 1.2  2004/08/31 15:55:29  michael
// Adjusted code to correclty load old 8 bit .sld/.slc into vxBlockVolume
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.17  2003/09/04 17:29:44  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.16.2.1  2003/08/11 11:07:43  mkontak
// moved Date into vx namespace
//
// Revision 1.16  2003/07/17 14:10:19  frank
// Caught exceptions in GetHeaderEntry
//
// Revision 1.15  2003/05/17 16:22:37  michael
// reformatting
//
// Revision 1.14  2003/05/17 16:19:41  michael
// reformatting
//
// Revision 1.13  2003/05/06 19:36:44  dongqing
// add field number for DICOM field StudyDescription
//
// Revision 1.12  2003/05/06 18:51:47  frank
// Added Study Description header entry, but it still needs the actual DICOM lookup values from Dongqing.
//
// Revision 1.11  2003/05/02 18:29:12  michael
// initial code review
//
// Revision 1.10  2002/07/02 20:44:13  geconomos
// Exporting classes .
//
// Revision 1.9  2002/06/05 11:29:35  soeren
// removed non const GetVolDim
//
// Revision 1.8  2002/05/22 23:41:55  ingmar
// added non-const access to VolDim
//
// Revision 1.7  2002/05/01 21:10:55  soeren
// added data modality
//
// Revision 1.6  2002/04/17 19:50:52  ingmar
// added GetWorlDim
//
// Revision 1.5  2002/04/16 19:45:48  ingmar
// added more functionality
//
// Revision 1.4  2002/04/12 23:04:10  ingmar
// GetVolumeDimension()  -> GetVolDim()  ;-)
//
// Revision 1.3  2002/04/10 20:59:09  ingmar
// added more Get functions to VolumeHeader
//
// Revision 1.2  2002/04/10 18:07:46  ingmar
// worked on new header volume class
//
// Revision 1.1  2002/04/04 14:02:38  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxVolumeHeader.inl,v 1.15 2007/09/19 15:30:34 gdavidson Exp $
// $Id: vxVolumeHeader.inl,v 1.15 2007/09/19 15:30:34 gdavidson Exp $
