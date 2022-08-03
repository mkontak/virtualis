// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmCosines.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmCosines.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"



using namespace dcm;


/// Constructor
DcmCosines::DcmCosines()
{
  Initialize();
} // DcmCosines()


/**
 * Copy constructor
 *
 * @param cosines       Cosines to copy from
 */
DcmCosines::DcmCosines(const DcmCosines & cosines )
{
  DcmUtility::Copy<double>(cosines.m_cosines, m_cosines);
} // DcmCosines(const DcmCosines & cosines )


/**
 *  constructor
 *
 *  @param cosines
 */
DcmCosines::DcmCosines(std::vector<double> cosines)
{

  if ( cosines.size() != 6 )
  {
    int i(0);

    for ( std::vector<double>::iterator it = cosines.begin(); it != cosines.end(); m_cosines.push_back(*it), it++, i++ );

    for ( ; i < 6; m_cosines.push_back((double)0), i++ );
  }
  else
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Vector only contains %d values, expected 6)", cosines.size()), "DcmUtility", "ToCosines"));

} // DcmCosines(std::vector<double> cosines)


/**
 * Constructs the cosines from the dicom std::string
 *
 * @param sDicomValue     Multi-values dicom field
 */
DcmCosines::DcmCosines(const std::string & sDicomValue )
{
  Assign(sDicomValue);
} // :DcmCosines(const std::string & sDicomValue )


/**
 * Assignes the cosinse from the dicom string
 *
 */
void DcmCosines::Assign(const std::string & sCosinse)
{
  Initialize();

  // ===============================================
  // If no value them assume default of 0\0\0\0\0\0
  // ===============================================
  if ( ! sCosinse.empty() )
  {
    STRINGLIST fields = util::Strings::Split(sCosinse, "\\/" );

    if ( fields.size() == 6 )
    {
      
      for ( int i(0); i < fields.size(); i++ )
      {
        Assign(i, DcmUtility::Convert<double>(fields[i], 0.0F));
      }

    }
    else
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Dicom value is not in the proper format (only %d values detected, expected 6)", fields.size()), "DcmUtility", "ToCosines"));
  } // END ... If the cosines are blank

}


/**
 * Converts to a std::string
 *
 *  @return the string
 */
std::string DcmCosines::ToString()
{
  return DcmUtility::Merge<double>(m_cosines, '\\');
} // ToString()


/**
 * Initializes the object
 */
void DcmCosines::Initialize()
{
   m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;
  m_cosines.push_back((double)0) ;

} // Initialize()


/**
 *  Sets the value of the cosines at the specified index
 *
 * @param index     Index
 * @paran value      New value
 */
void DcmCosines::Assign(int index, double value)
{
  if ( index < 0 || index > 5 )
    throw DcmOutOfBoundsException(LogRec("Index out of bounds","DcmCosines","SetValue"));

  m_cosines[index] = value;
} // SetValue<std::string>(int index, double value)


/**
 * Gets the orientation based on the unit cosines
 */
DcmOrientations DcmCosines::GetOrientation()
{
  // ============================
  // HFP  (Head First Prone)
  //    -1\0\0  : 0\-1\0
  // =============================
  if ( m_cosines[ROW_X] < 0 && m_cosines[COL_Y] < 0  )
  {
    return O_HFP;
  }
  // =============================
  // FFP (Feet First Prone)
  //    1\0\0  : 0\-1\0   
  // =============================
  else if ( m_cosines[ROW_X] > 0 && m_cosines[COL_Y] < 0 ) 
  {
    return O_FFP;
  }  
  // ==========================
  // HFS (Head First Supine)
  //    1\0\0  : 0\1\0    
  // ==========================
  else if ( m_cosines[ROW_X] > 0 && m_cosines[COL_Y] > 0 )
  {
    return O_HFS;
  }
  // ==========================
  // FFS (Heet First Supine)
  //    -1\0\0  : 0\1\0    
  // ==========================
  else if ( m_cosines[ROW_X] < 0 && m_cosines[COL_Y] > 0 )
  {
    return O_FFS;
  }
  // =====================================
  // HFDL   (Head First Decubitus-Left)
  //    0\-1\0 : 1\0\0    
  // =====================================
  else if ( m_cosines[ROW_Y] < 0 && m_cosines[COL_X] > 0 ) 
  {
    return O_HFDL;
  }
  // =====================================
  // FFDL   (Feet First Decubitus-Left)
  //    0\1\0  : 1\0\0    
  // =====================================
  else if ( m_cosines[ROW_Y] > 0 && m_cosines[COL_X] > 0 ) 
  {
    return O_FFDL;
  }
  // =====================================
  // HFDR   (Head First Decubitus-Right)
  //    0\1\0  : -1\0\0       
  // =====================================
  else if ( m_cosines[ROW_Y] > 0 && m_cosines[COL_X] < 0 )
  {
    return O_HFDR;
  }
  // =====================================
  // FFDR   (Feet First Decubitus-Right)
  //    0\-1\0 : -1\0\0          
  // =====================================
  else if ( m_cosines[ROW_Y] < 0 && m_cosines[COL_X] < 0 )
  {
    return O_FFDR;
  }
  else
  {
    return O_NONE;
  }

} // GetOrientation()



/**
 * Gets the plane based on the unit cosines
 */
DcmPlanes DcmCosines::GetPlane()
{
 
  // =================================================
  // If all are set to 0 then there is no plane
  // =================================================
  if ( m_cosines[ROW_Z] == 0 && m_cosines[COL_Z] == 0 && 
       m_cosines[ROW_X] == 0 && m_cosines[COL_X] == 0 && 
       m_cosines[ROW_Y] == 0 && m_cosines[COL_Y] == 0 )
  {
    return P_NONE;
  }
  // =========
  // Axial - 
  // =========
  else if ( m_cosines[ROW_Z] == 0 && m_cosines[COL_Z] == 0 ) 
  {
    return P_AXIAL;
  }
  // ===========
  // Saggital
  // =============
  else if ( m_cosines[ROW_X] == 0 && m_cosines[COL_X] == 0 )
  {
    return P_SAGITTAL;
  }
  // ===========
  // Coronal
  // ==========
  else if ( m_cosines[ROW_Y] == 0 && m_cosines[COL_Y] == 0 )
  {
    return P_CORONAL;
  }
  // =========
  // Oblique
  // ==========
  else
  {
    return P_OBLIQUE;
  }

} // GetPlane()