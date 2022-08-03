// $Id: StudyLoadParams.h,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#pragma once

#include "DatasetLoadParams.h"

// namespace delcaration
namespace ReaderLib 
{

// class declaration
class StudyLoadParams
{    
// enums
public:
   
  /// study load flags
  enum StudyLoadFlagsEnum{ PRIMARY_DATASET_VALID = 0x0001, SECONDARY_DATASET_VALID = 0x0002 };

// member functions
public:

  /// constructor
  StudyLoadParams();
  
  /// destructor
  virtual ~StudyLoadParams();

  /// gets the load flags
  const uint4 GetFlags() const { return m_uFlags; }

  /// sets the load flags
  void SetFlags( const uint4 uFlags ){ m_uFlags = uFlags; }

  /// gets the primary dataset parameters
  DatasetLoadParams & GetPrimaryDataset() { return m_primaryDataset; }

  /// gets the secondary dataset parameters
  DatasetLoadParams & GetSecondaryDataset() { return m_secondaryDataset; }

  /// gets the session directory
  const std::string &  GetSessionDirectory() const { return m_sSessionDirectory; }

  /// sets the session directory
  void SetSessionDirectory( const std::string & sDirectory ) ;
  
  /// copy constructor
  StudyLoadParams( const StudyLoadParams & other );

  /// assignment operator
  StudyLoadParams & operator=( const StudyLoadParams & rhs );

// member variables
private:

  /// study load flags
  uint4 m_uFlags;

  /// primary dataset parameters
  DatasetLoadParams m_primaryDataset;

  /// secondary dataset parameters
  DatasetLoadParams m_secondaryDataset;

  /// session directory
  std::string m_sSessionDirectory;

}; // class StudyLoadParams

} // namespace ReaderLib 

// $Log: StudyLoadParams.h,v $
// Revision 1.1.6.1  2007/03/30 17:12:35  romy
// temporary fix added for issue#5541
//
// Revision 1.1  2005/11/15 16:13:52  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/StudyLoadParams.h,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
// $Id: StudyLoadParams.h,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
