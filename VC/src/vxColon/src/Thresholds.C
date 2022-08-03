// $Id: Thresholds.C,v 1.2 2006/02/03 19:27:53 frank Exp $
//
// Copyright (c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "V3DFile.h"
#include "Thresholds.h"


// namespaces
using namespace std;


/**
 * default constructor
 */
Thresholds::Thresholds()
{
} // default constructor


/**
 * copy constructor
 */
Thresholds::Thresholds(const Thresholds & src)
: m_vThresholds(src.m_vThresholds)
{
} // copy constructor


/**
 * read threshold information from disk
 */
void Thresholds::Read(const string & fileName)
{
  try 
  {
    V3DFile image;
    image.Read( fileName, std::pair< float4, float4 >( -1.0F, -1.0F ) );
    std::string vector = image.GetVariable( "Thresholds Vector" ).GetValue();
    ReadAsciiHeader(vector.c_str());
  }
  catch (ex::VException & ex)
  {
    throw ex::VException(LogRec( ex.GetLogRecord().GetTextMessage(), "Thresholds", "Read"));
  }
  catch (...)
  {
    throw ex::VException(LogRec("error loading data files in <Thresholds::Read>", "Thresholds", "Read"));
  }

  return;
} // Read


/**
 * write threshold information to disk
 */
bool Thresholds::Write(std::string fileName)
{

  // add the thresholds text chunk
  V3DFile image;
  image.SetVariable( Variable( "Thresholds Vector", BuildAsciiHeader(), "the threshold vector" ) );

  // write
  image.Write( fileName );

  return true;

} // Write


/**
 * Builds ascii header containing the Thresholds.
 */
std::string Thresholds::BuildAsciiHeader()
{
  int1 m_vEnd[3] = {13, 10, '\0'};
  const uint4 uBSize = 1000;
  int1 vBuffer[uBSize];
  std::ostrstream sHeader(vBuffer, uBSize);
  sHeader << std::endl;
  sHeader << m_vThresholds.size() << " # Number of Threshold values in the vector" << m_vEnd;
  for ( uint4 k=0; k<m_vThresholds.size(); k++ ) 
  {
    sHeader << m_vThresholds[k] << " " ;
  }
  sHeader << m_vEnd << ends;
  return (sHeader.str());
} //BuildAsciiHeader


/**
 * Reads the header containing the Thresholds as ascii text.
 */
bool Thresholds::ReadAsciiHeader(const int1 * headerPtr)
{
  std::istrstream header(headerPtr);
  unsigned long uSize;
  const int4 iBSize = 1000;
  int1 vBuffer[iBSize];
  header.getline(vBuffer, iBSize); // Get the first endline
  header >> uSize;   header.getline(vBuffer, iBSize);
  m_vThresholds.resize(uSize);
  int4 iThreshold(0);
  for ( uint8 k=0; k < m_vThresholds.size(); k++ ) 
  {
    header >> iThreshold;
    m_vThresholds[k] = iThreshold;
    std::cerr << ":" << iThreshold << ":" << m_vThresholds[k] << ",";
  }

  return (true);
} //ReadAsciiHeader


/**
 * allow to direct threshold values to an output stream.
 * @param os : reference to std::ostream output stream.
 */
ostream &  Thresholds::AsOstream(ostream & os) const
{
  os << m_vThresholds << endl;
  return (os);

} // AsOstream


// $Log: Thresholds.C,v $
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:20   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.21  2001/08/21 16:45:52  soeren
// removed some warnings
//
// Revision 1.20  2001/08/13 20:05:44  tushar
// removed errors due to the new logger
//
// Revision 1.19  2001/07/26 20:29:00  binli
// ID 89: exception handling.
//
// Revision 1.18  2001/07/25 02:47:00  tushar
// fixed exception bug
//
// Revision 1.17  2001/07/16 23:48:18  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.16  2001/07/13 19:52:16  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.15  2001/07/12 21:14:15  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.14  2001/05/25 14:50:26  lihong
// coding standard
//
// Revision 1.13  2001/05/24 22:36:57  lihong
// changed most variables for coding standard
//
// Revision 1.12  2001/04/12 18:52:39  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.11  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.10  2001/04/10 17:34:38  dmitry
// updated datatypes
//
// Revision 1.9  2001/04/05 15:45:50  dave
// updated header
//
// Revision 1.8  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.7  2001/02/05 19:16:56  lihong
// simplified AsOstream
//
// Revision 1.6  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.5  2000/11/22 06:44:22  dave
// Updated PNGImage class
//
// Revision 1.4  2000/11/13 18:06:24  ingmar
// improved operator <<
//
// Revision 1.3  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.2.16.1  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.2  2000/09/11 04:12:01  antonio
// got rid of a few more warnings
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 7     8/31/00 4:12p Lihong
// changed public variable m_vThresholds as m_thresholds
//
// 6     8/28/00 2:14p Lihong
// added Global.h for disabling the warnings
//
// 5     8/22/00 5:44p Jay
// Thresholds now reads/writes in the PNG file format and has an icon for
// the thresholds and histogram found by the thresholder
//
// 4     8/21/00 5:15p Antonio
// updated header
//
// *****************  Version 3  *****************
// User: Antonio      Date: 8/11/00    Time: 3:50p
// Updated in $/v2k/src/Thresholds
// added header and history to end of file.
//
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/28/00  Time: 12:35p
// Checked in $/v2k/src/Thresholds
// Comment:
//   added Thresholds class to hold the threshold vector
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/28/00  Time: 10:47a
// Created Thresholds.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Thresholds.C,v 1.2 2006/02/03 19:27:53 frank Exp $
// $Id: Thresholds.C,v 1.2 2006/02/03 19:27:53 frank Exp $
