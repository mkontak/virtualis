// $Id: StudyLoadParams.C,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

#include "StdAfx.h"
#include "StudyLoadParams.h"

// namespaces
using namespace ReaderLib;


/**
 * Constructor
 */
StudyLoadParams::StudyLoadParams() : 
  m_primaryDataset( DatasetOrientations::ePRIMARY ),
  m_secondaryDataset( DatasetOrientations::eSECONDARY )
{
} // StudyLoadParams()


/**
 * Copy constructor
 *
 * @param     other   other StudyLoadParams instance
 */
StudyLoadParams::StudyLoadParams( const StudyLoadParams & other ) : 
  m_uFlags( other.m_uFlags),
  m_primaryDataset( other.m_primaryDataset ),
  m_secondaryDataset( other.m_secondaryDataset ),
  m_sSessionDirectory( other.m_sSessionDirectory )
{
} // StudyLoadParams( const StudyLoadParams & other )


/**
 * Destructor
 */
StudyLoadParams::~StudyLoadParams()
{
} // ~StudyLoadParams()


/**
 * Assignment operator
 *
 * @param     rhs   other StudyLoadParams instance
 * @return    reference to self
 */
StudyLoadParams & StudyLoadParams::operator=( const StudyLoadParams & rhs )
{
  if( &rhs != this )
  {
    m_uFlags = rhs.m_uFlags;
    m_primaryDataset = rhs.m_primaryDataset;
    m_secondaryDataset = rhs.m_secondaryDataset;
    m_sSessionDirectory = rhs.m_sSessionDirectory;
  }
  return *this;
} // operator=( const StudyLoadParams & rhs )

/*
 * Sets the session folder
 * @param sDirectory session folder
 */
void StudyLoadParams::SetSessionDirectory( const std::string & sDirectory ) 
{
  m_sSessionDirectory = sDirectory; 
    
  const std::string & movieFolder = m_sSessionDirectory + "\\Movies" ;
  CreateDirectory( movieFolder.c_str() , NULL );

  const std::string & pdfFolder = m_sSessionDirectory + "\\Reports" ;
  CreateDirectory( pdfFolder.c_str() , NULL );

  const std::string & snapshotsFolder = m_sSessionDirectory + "\\Snapshots" ;
  CreateDirectory( snapshotsFolder.c_str() , NULL );
}//SetSessionDirectory( const std::string & sDirectory ) 


// $Log: StudyLoadParams.C,v $
// Revision 1.1.6.1  2007/03/30 17:12:35  romy
// temporary fix added for issue#5541
//
// Revision 1.1  2005/11/15 16:14:06  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/StudyLoadParams.C,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
// $Id: StudyLoadParams.C,v 1.1.6.1 2007/03/30 17:12:35 romy Exp $
