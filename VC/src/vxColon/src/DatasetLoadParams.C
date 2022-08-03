// $Id: DatasetLoadParams.C,v 1.7 2006/10/10 21:20:59 gdavidson Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

// includes
#include "StdAfx.h"
#include "DatasetLoadParams.h"
#include "DirPtrFactory.h"

//defines
#define FILE_REVISION "$Revision: 1.7 $"

// namespaces
using namespace ReaderLib;

/**
 * Constructor
 */
DatasetLoadParams::DatasetLoadParams( DatasetOrientations::TypeEnum eOrientation ) :
  m_eOrientation( eOrientation )
{
} // DatasetLoadParams()


/**
 * Copy constructor
 *
 * @param   other   other DatasetLoadParams
 */
DatasetLoadParams::DatasetLoadParams( const DatasetLoadParams & other ) : 
 m_sPrefix( other.m_sPrefix ),
 m_sVolumeDirectory( other.m_sVolumeDirectory ),
 m_sPreprocessedDirectory( other.m_sPreprocessedDirectory ),
 m_sCadDirectory( other.m_sCadDirectory ),
 m_eOrientation( other.m_eOrientation )
{
} // DatasetLoadParams( const DatasetLoadParams & other )


/**
 * Destructor
 */
DatasetLoadParams::~DatasetLoadParams()
{
} // ~DatasetLoadParams()


/*
 * Assignment operator
 *
 * @param   rhs   other DatasetLoadParams
 * @return  reference to self
 */
DatasetLoadParams & DatasetLoadParams::operator=( const DatasetLoadParams & rhs )
{
  if( this != &rhs )
  {
    m_sPrefix = rhs.m_sPrefix;
    m_sVolumeDirectory = rhs.m_sVolumeDirectory;
    m_sPreprocessedDirectory = rhs.m_sPreprocessedDirectory;
    m_sCadDirectory = rhs.m_sCadDirectory;
  }
  return *this;
} // operator=( const DatasetLoadParams & rhs )


/**
 * Helper function thats appends a directory and the prefix
 *
 * @param     sDirectory    directory name
 * @param     sPrefix       prefix
 * @return    formatted prefix
 */
std::string DatasetLoadParams::FormatPrefixPath( const std::string & sDirectory, const std::string & sPrefix )
{
  std::string sFormattedPrefix = sDirectory;
  if( sFormattedPrefix.length() > 0 )
  {  
    char lastchar = sFormattedPrefix.at( sFormattedPrefix.length() - 1 );
    if( lastchar != '\\' && lastchar != '/' )
      sFormattedPrefix += "\\";
  }
  sFormattedPrefix += sPrefix;
  return sFormattedPrefix;
} // FormatPrefixPath( const std::string & sDirectory, const std::string & sPrefix )


/**
 * Helper function tat generates the prefix for the dataset
 *
 * @param     sVolumeDirectory    directory used to generate prfix
 * @param     sHint               indicates which volume to use as the base prefix
 * @return    prefix
 */
std::string DatasetLoadParams::GeneratePrefix( const std::string & sVolumeDirectory, const std::string & sHint )
{
  // now we need to obtain the prefix which is used to form all file names
  // first we try and grab the .ecv file first, if that does not exist we will grab the .vol file
  std::string sPrefix;
  vx::FileExtension * pExts[] = { &vx::ElecCleansedVolExt, &vx::VolumeExt, &vx::CleansedDifferenceVolExt, &vx::SegmentListExt };

  for (int4 i(0), iCount(sizeof(pExts)/sizeof(*pExts)); i < iCount; i++)
  {

    if( FindFilename( sVolumeDirectory, std::string("*") + /*sHint + "." +*/ (*(pExts[i])), sPrefix ) )
    {
      int4 position = sPrefix.rfind( "_hq" );
      if( position != sPrefix.npos && position == sPrefix.length() - 3 )
        return sPrefix.substr( 0, position );

      return sPrefix;
    }
  }

  LogErr( "Unable to determine dataset prefix", "DatasetLoadParams", "GeneratePrefix" );
  return std::string();

} // GeneratePrefix( const std::string & sVolumeDirectory )


/**
 * Helper function tat generates the prefix for the dataset
 *
 * @param     sVolumeDirectory    directory used to generate prfix
 * @param     fileExt             indicates which volume to use as the base prefix
 * @return    prefix
 */
std::string DatasetLoadParams::GeneratePrefix( const std::string & sVolumeDirectory, const vx::FileExtension & fileExt )
{
  // now we need to obtain the prefix which is used to form all file names
  // first we try and grab the .ecv file first, if that does not exist we will grab the .vol file
  std::string sPrefix;

  if( FindFilename( sVolumeDirectory, std::string("*") + fileExt, sPrefix ) )
    return sPrefix;

  LogErr( "Unable to determine dataset prefix", "DatasetLoadParams", "GeneratePrefix" );
  return std::string();

} // GeneratePrefix( const std::string & sVolumeDirectory )


/**
 * Finds the specified file and returns the filename minus the extension
 *
 * @param     sDirectory      directory to search for file
 * @param     sExtension      file extension of file to find
 * @param     sFilename       upon return contains the filename minus the extension
 * @return    true if found; false otherwise
 */
bool DatasetLoadParams::FindFilename( const std::string & sDirectory, const std::string & sExtension, std::string & sFilename )
{
  
  
  std::vector<std::string> files = io::DirPtrFactory::GetFiles(sDirectory, sExtension );

  if ( files.size() > 0 )
  {
    sFilename = util::IO::GetFilenameWithoutExt(files[0]);
    return true;
  }

  //WIN32_FIND_DATA fd;
  //HANDLE hFile = FindFirstFile( DatasetLoadParams::FormatPrefixPath( sDirectory, sExtension ).c_str(), &fd );
  //if( hFile != INVALID_HANDLE_VALUE )
  //{
  //  char cFilename[ _MAX_FNAME ];
  // _splitpath( fd.cFileName, NULL, NULL, cFilename, NULL );
  //  sFilename = cFilename;
  //  return true;
  //}
  return false;
} // FindFilename( const std::string & sDirectory, const std::string & sExtension, std::string & sFilename )



// $Log: DatasetLoadParams.C,v $
// Revision 1.7  2006/10/10 21:20:59  gdavidson
// Remove _hq from prefix
//
// Revision 1.6  2006/10/10 15:52:08  gdavidson
// Issue #4949: Used .seg file to determine the preprocessed prefix for preprocessed directories that do not contain .cdv files
//
// Revision 1.5  2006/09/13 15:17:57  frank
// Fixed some log messages to print out the proper class
//
// Revision 1.4  2006/05/12 20:29:20  jmeade
// Issue 4795: use proper prefix for cad xml file.
//
// Revision 1.3  2006/04/24 17:45:41  jmeade
// new study directory structure.
//
// Revision 1.2  2005/11/16 14:28:07  geconomos
// added the ability to hint at which volume ( .ecv or .vol ) to base the dataset prefix on
//
// Revision 1.1  2005/11/15 16:14:06  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/DatasetLoadParams.C,v 1.7 2006/10/10 21:20:59 gdavidson Exp $
// $Id: DatasetLoadParams.C,v 1.7 2006/10/10 21:20:59 gdavidson Exp $
