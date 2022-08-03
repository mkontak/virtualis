// $Id: Hazards.C,v 1.2 2006/02/03 19:27:53 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file 
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.net)


// includes
#include "stdafx.h"
#include "Hazards.h"
#include "V3DFile.h"
#include "File.h"
#include "utilities.h" 
#include "FileExtensions.h"
#include "FilePtrFactory.h"

// static intitialization
std::list<Hazards::HazardEnum> Hazards::m_hazardList;


/**
 * Read from disk
 *
 * @param sFilePrefix the prefix to the file
 *
 * @return success
 */
bool Hazards::Read( const std::string & sFilePrefix )
{

  std::string sFileName = sFilePrefix + vx::WarningsLogExt;

  if (! io::FilePtrFactory::Exists(sFileName))
  {
    return false;
  }
  
  V3DFile wrnFile;
  try
  {
    wrnFile.Read( sFileName, std::pair< float4, float4 >( -1.0F, -1.0F ) );
  }
  catch (ex::IOException)
  {
    throw;
  }
  
  int iNumHazards = atoi( wrnFile.GetVariable( "Hazards" ).GetValue().c_str() );
  for ( uint4 i=0; i<iNumHazards; ++i )
  {
    const std::string sHazardText = wrnFile.GetVariable( ToAscii( i ) ).GetValue();
    m_hazardList.push_back( static_cast< HazardEnum >( atoi( sHazardText.c_str())));
  }
  return true;

} // Read()


/**
 * Write to disk
 *
 * @param sFilePrefix the prefix to the filename
 *
 * @return success
 */
bool Hazards::Write( const std::string & sFilePrefix )
{
  if (m_hazardList.size() > 0)
  {  

    V3DFile wrnFile;
    
    wrnFile.SetVariable( Variable( "Hazards", ToAscii( m_hazardList.size() ), "The number of hazards found during processing" ) );
    
    std::list< HazardEnum >::iterator it = m_hazardList.begin();
    for ( uint4 j=0; it != m_hazardList.end(); j++, it++ )
    {
      wrnFile.SetVariable( Variable( ToAscii(j), ToAscii( * it ), "A hazard" ) );
    }
    
    wrnFile.Write( sFilePrefix + vx::WarningsLogExt );

  } // if any data
 
  return true;

} // Write()


/**
 * Add a warning
 *
 * @param hazard a hazard
 */
void Hazards::AddHazard( const HazardEnum hazard )
{
  m_hazardList.push_back(hazard);
} // AddHazard()


// $Log: Hazards.C,v $
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 1.2  2002/09/30 20:45:09  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 1.1  2002/03/08 22:34:04  michael
// added Hazards to 20_image to avoid forward dependencies
//
// Revision 3.3  2002/02/25 18:13:53  michael
// re-added Hazards class
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.2   Oct 01 2001 16:10:12   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.11  2001/05/21 13:00:23  frank
// Removed extra function.
//
// Revision 1.10  2001/04/19 15:53:34  jmeade
// removed unreferenced vars
//
// Revision 1.9  2001/04/13 00:31:09  jmeade
// file extensions, coding standards
//
// Revision 1.8  2001/04/12 18:51:20  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.7  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.6  2001/04/09 18:02:57  dmitry
// Removed Revision History from Top of File
//
// Revision 1.5  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.4  2001/02/26 17:00:36  binli
// if no .WRN file ...
//
// Revision 1.3  2001/02/02 16:44:19  dave
// hazards updated on v2k side
//
// Revision 1.2  2001/02/01 17:47:09  frank
// Added a conversion function to get actual strings.
//
// Revision 1.1  2001/02/01 14:24:25  frank
// Renamed from Warnings
//
// Revision 1.1  2001/01/31 19:36:10  frank
// New class for enumerated warning messages (internationalization support)
// 
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Hazards.C,v 1.2 2006/02/03 19:27:53 frank Exp $
// $Id: Hazards.C,v 1.2 2006/02/03 19:27:53 frank Exp $
