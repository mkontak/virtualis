// $Id: vxVesselArray.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

/**
 * vxVesselArray Class contains a list of vessels and functions to handle it.
 */


// includes
#include "StdAfx.h"
#include <queue>
#include <stack>
#include "vxVesselArray.h"
#include "Serialization.h"
#include "vxUtils.h"
#include "vxSerializer.h"


// Defines
#define FILE_REVISION "$Revision: 1.1 $"


// namespaces
using namespace std;
using namespace Serialization;


// Static member initialization
const std::string vxVesselArray::m_sDelimiter( "vxVesselArray" );
const float4 vxVesselArray::m_fVersion( 1.0F );


/**
 * Constructor.
 */
vxVesselArray::vxVesselArray()
{
  Reset();
} // vxVesselArray()


/**
  * Destructor.
  */
vxVesselArray::~vxVesselArray()
{

} // ~vxVesselArray()


/**
 * Retrieves the number of bytes of the serialized data.
 */
uint4 vxVesselArray::GetSerializedDataSize() const
{
  uint4 uSum(0);

  // start delimiter
  uSum += m_sDelimiter.length() + 1;

  // version
  uSum += sizeof(m_fVersion);

  // vector<VesselSection> size
  uSum += sizeof(uint4);

  // size of individual vessels
  for( int4 i(0); i<m_vVesselList.size(); ++i )
    uSum += m_vVesselList[i].GetSerializedDataSize();

  // current selection
  uSum += sizeof(m_iCurrentVesselIndex);

  // end delimiter
  uSum += m_sDelimiter.length() + 1;

  return uSum;
} // GetSerializedDataSize()


/** 
 * Read the vessel from a file
 * @param sFilename input file name
 */
void vxVesselArray::Read(const std::string & sFilename)
{
  try
  {
    V3DFile img;
    img.Read( sFilename, std::make_pair( 0.0F, 100.0F ) );
    MemoryBuffer memBuf( img.GetDataBuffer() );
    uint4 iBufSize( memBuf.GetSize() );
    int1 * pBuffer = reinterpret_cast<int1 *>( memBuf.GetDataPtr() );
    ReadSerializedData( &pBuffer, pBuffer + iBufSize );
  
  }
  catch (ex::VException & e )
  {
    LogErr("File Read Error : " + e.GetLogRecord().GetTextMessage(),"vxVessel", "Read");
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec("An unspecified error caught", "vxVessel", "Read"));
  }

  LogFyi("Reading " + sFilename + " done", "vxVessel", "Read");

} // Read()




/**
 *  Reads in all the data from serialized form.
 *
 *  @param hiBuffer    handle to the memory buffer which stores the vessels.
 *  @param piBufferEnd pointer to the end of the memory buffer.
 */
void vxVesselArray::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try
  {
    // verify the stream by reading start delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sDelimiter )
      throw ex::DataCorruptionException( LogRec( "data corruption: bad starting delimiter not '" + m_sDelimiter + "'", "vxVesselArray", "ReadSerializedData" ) );
 
    // version
    float4 fVersion;
    DeserializeSimpleData( hiBuffer, piBufferEnd, fVersion );
    if( fVersion != m_fVersion )
      throw ex::DataCorruptionException( LogRec( "Old versions not supported (" + ToAscii(fVersion) + ")", "vxVesselArray", "ReadSerializedData" ) );

    // retrieve the number of vessels
    uint4 uSize(0);
    DeserializeSimpleData( hiBuffer, piBufferEnd, uSize );
    
    m_vVesselList.resize( uSize );
    for( int4 i(0); i<m_vVesselList.size(); ++i )
      m_vVesselList[i].ReadSerializedData( hiBuffer, piBufferEnd );
  
    // read the current selection
    DeserializeSimpleData( hiBuffer, piBufferEnd, m_iCurrentVesselIndex );
    
    // sanitize the selection
    if( m_vVesselList.size() == 0 )
      m_iCurrentVesselIndex = -1;
    else if( m_iCurrentVesselIndex < 0 )
      m_iCurrentVesselIndex = 0;
    else if( m_iCurrentVesselIndex >= m_vVesselList.size() )
      m_iCurrentVesselIndex = 0;

    // verify the stream again by reading end delimiter.
    if( DeserializeStringData( hiBuffer, piBufferEnd ) != m_sDelimiter )
      throw ex::DataCorruptionException( LogRec( "data corruption: bad ending delimiter not '" + m_sDelimiter + "'", "vxVesselArray", "ReadSerializedData" ) );

  }
  catch (ex::OutOfMemoryException)
  {
    throw ex::OutOfMemoryException( LogRec( "Out of memory", "vxVesselArray", "ReadSerializedData" ) );
  }
  catch (ex::VException & error)
  {
    throw ex::VException( LogRec( error.GetLogRecord().GetTextMessage(), "VxVesselArray", "ReadSerializedData" ) );
  }

} // ReadSerializedData()


/**
 * Equality operator for a set of vessels
 * @param other is the vessel array we are to compare to
 * @return true if the vessel arrays are equal
 */
bool vxVesselArray::operator ==(const vxVesselArray & other) const
{
  if( m_vVesselList.size() != other.m_vVesselList.size() )
    return false;

  // check each vessel
  for( int4 i=0; i<m_vVesselList.size(); i++ )
  {
    if ( m_vVesselList[i] != other.m_vVesselList[i] )
      return false;
  } // for each vessels

  return true;
} // operator ==


/**
 * Inequality operator for a set of vessels
 * @param other is the vessel array we are to compare to
 * @return true if the vessel arrays are not equal
 */
bool vxVesselArray::operator !=(const vxVesselArray & other) const
{
  return ! (*this == other);
} // operator !=




/**
 * Compact the vessel array so the label indices contain no holes
 */
void vxVesselArray::Compact()
{

  LogFyi( "Compacting vxVesselArray", "vxVesselArray", "Compact" );

  // determine the maximum label of all the vessels and easy-selected components
  uint4 uMaxLabel(0);
  for ( uint4 i=0; i<m_vVesselList.size(); ++i )
  {
    if ( m_vVesselList[i].GetLabel() > uMaxLabel ) uMaxLabel = m_vVesselList[i].GetLabel();
  }
  for ( int i(0); i<m_vEasySelectedVesselLabelList.size(); ++i )
  {
    if ( m_vEasySelectedVesselLabelList[i] > uMaxLabel ) uMaxLabel = m_vEasySelectedVesselLabelList[i];
  }

  // go through the lists enough times to decrement all the labels
  for ( uint4 uThreshold(1); uThreshold<=uMaxLabel; ++uThreshold )
  {

    // see if either the vessels or the easy-selected components conains the current threshold
    bool bLabelFound( false );
    for ( vector< vxVessel >::iterator iter( m_vVesselList.begin() );
          iter != m_vVesselList.end();
          ++iter )
    {
      if ( iter->GetLabel() == uThreshold ) bLabelFound = true;
    }

    // then the easy-selected components
    for ( vector< uint2 >::iterator iter( m_vEasySelectedVesselLabelList.begin() );
      iter != m_vEasySelectedVesselLabelList.end();
      ++iter )
    {
      if ( * iter == uThreshold ) bLabelFound = true;
    }

    // reduce the label value of each item over the current threshold
    // if the current label is found
    if ( bLabelFound == false )
    {

      // first the vessels
      int count(0);
      for ( vector< vxVessel >::iterator iter( m_vVesselList.begin() );
        iter != m_vVesselList.end();
        ++iter, ++count )
      {
        LogDbg( "count: " + ToAscii(count) + " label:" + ToAscii( iter->GetLabel() ), "", "" );
        if ( iter->GetLabel() > uThreshold ) iter->SetLabel( iter->GetLabel() - 1 );
        LogDbg( "   newlabel:" + ToAscii( iter->GetLabel() ), "", "" );
      }

      // then the easy-selected components
      for ( vector< uint2 >::iterator iter( m_vEasySelectedVesselLabelList.begin() );
        iter != m_vEasySelectedVesselLabelList.end();
        ++iter )
      {
        if ( * iter > uThreshold ) ( * iter )--;
      }

    } // if label is not found
    
  } // loop over all labels

} // Compact()


/**
 * output to a string
 *
 * @return object as a string
 */
std::string vxVesselArray::ToString() const
{
  std::string output( "vxVesselArray:\n" );
  for ( uint4 i(0); i<m_vVesselList.size(); ++i )
  {
    output += "  vessel " + ToAscii(i) + " name='" + m_vVesselList[i].GetName();
    output += "' label=" + ToAscii( m_vVesselList[i].GetLabel() ) + "\n";
  }
  return output;
  
} // ToString()


/**
 * Reset all the data
 */
void vxVesselArray::Reset()
{
  m_pComponentInfo      = NULL;
  m_iCurrentVesselIndex = -1;
} // Reset()


/**
 * Add a AddVessel to the list.
 * @param vessel AddVessel & 
 * @return index of component
 */
void vxVesselArray::AddVessel( const vxVessel & vessel, const uint2 & uLabel )
{
  m_vVesselList.push_back( vessel );
  m_pComponentInfo->ArrayByLabel()[ uLabel ]->SetColor( PixelRGBA<uint1>( 255, 0, 0, 64 ) );
  m_pComponentInfo->ArrayByLabel()[ uLabel ]->SetLabelVisible( false );
  m_pComponentInfo->Modified( vxModIndInfo( vxComponentArray, uLabel, vxUtils::COMPONENTARRAY_MODIFIED ) ); 
  m_iCurrentVesselIndex = m_vVesselList.size() - 1;
  GetCurrentVessel()->SetLabel( uLabel );
} // AddVessel()


void vxVesselArray::PushVessel( const vxVessel & vessel, const uint2 & uLabel )
{
  m_vVesselList.push_back( vessel );
  m_pComponentInfo->ArrayByLabel()[ uLabel ]->SetColor( PixelRGBA<uint1>( 255, 0, 0, 64 ) );
  m_pComponentInfo->ArrayByLabel()[ uLabel ]->SetLabelVisible( false );
  
  m_iCurrentVesselIndex = m_vVesselList.size() - 1;
  GetCurrentVessel()->SetLabel( uLabel );
} // AddVessel()


/**
 * Remove a vessel from the list.
 *
 * @param iVesselIndex         the index of the vessel to remove
 * @param bRemoveComponentAlso if the associated component should also be removed
 */
void vxVesselArray::RemoveVessel( uint4 iVesselIndex, const bool bRemoveComponentAlso )
{
  // remove the associated component
  if ( bRemoveComponentAlso )
    m_pComponentInfo->RemoveComponent( GetVessel( iVesselIndex ).GetLabel() );

  // remove the vessel
  m_vVesselList.erase(m_vVesselList.begin() + iVesselIndex);

  // change the currently selected vessel
  if( m_iCurrentVesselIndex >= iVesselIndex )
    --m_iCurrentVesselIndex;

} // RemoveVessel()


/**
 * Returns a vessel with the specified index
 * @param uNumber index of vxVessel in list
 * @return Reference to a vxVessel.
 */
vxVessel & vxVesselArray::GetVessel(const uint2 uNumber)
{
  int4 size(m_vVesselList.size());
  if(uNumber >= m_vVesselList.size())
  {
    throw ex::IndexOutOfBoundsException(LogRec("invalid index", "vxVesselArray", "GetVessel"));
  }

  return m_vVesselList[uNumber];

} // GetVessel()


/**
 * Returns the index of the vessel given a label (-1 if not found)
 * @param uLabel the lable of the vessel
 * @return the index of the vessel
 */
int4 vxVesselArray::GetIndexFromLabel( uint2 uLabel ) const
{
  int4 uIndex(-1);
  for( int4 i(0); i<m_vVesselList.size(); ++i )
  {
    if( m_vVesselList[i].GetLabel() == uLabel )
    {
      uIndex = i;
    }
  }

  return uIndex;
} // GetIndexFromLabel()


/**
 * Recalculate the < min, max > diameter points from the contour.
 */
void vxVesselArray::RecalculateDiameterPoints()
{
  for( int i(0); i<m_vVesselList.size(); ++i )
    m_vVesselList[i].RecalculateDiameterPoints();
} // RecalculateDiameterPoints()


/**
 * generate Xml node
 * @param element the XML node
 */
void vxVesselArray::ToXml( vxDOMElement & element ) const
{

  const std::vector< vxVessel > vessels( m_vVesselList );
  const int4 iCurrentVesselIndex( m_iCurrentVesselIndex );

  vxSerializer::SetAttribute( element, "version", "2.0" );
  
  vxSerializer::Write( vxSerializer::AddElement( element, vxSerializer::GetElementName( vessels ), "vessels" ), vessels );
  vxSerializer::Write( vxSerializer::AddElement( element, vxSerializer::GetElementName( iCurrentVesselIndex ), "current vessel index" ), iCurrentVesselIndex );

} // ToXml()


/**
 * Initialize from XML node
 * @param element the XML node
 * @return success
 */
bool vxVesselArray::FromXml( vxDOMElement & element )
{

  if ( element.IsNull() ) return false;

  bool bSuccess( true );

  std::string sVersion("0.0");
  bSuccess &= vxSerializer::GetAttribute( element, "version", sVersion );
  if ( atof( sVersion.c_str() ) == 2.0 )
  {

    std::vector< vxVessel > vessels;
    int4 iCurrentVesselIndex( -1 );

    bSuccess &= vxSerializer::Read( vxSerializer::FindElement( element, vxSerializer::GetElementName( vessels ), "vessels" ), vessels );
    bSuccess &= vxSerializer::Read( vxSerializer::FindElement( element, vxSerializer::GetElementName( iCurrentVesselIndex ), "current vessel index" ), iCurrentVesselIndex );

    m_vVesselList = vessels;
    m_iCurrentVesselIndex = iCurrentVesselIndex;
    Compact();

  }
  else
  {
    LogDbg( "Unknown version number for XML: " + sVersion, "vxVesselArray", "FromXml" );
  }

  return bSuccess;
  
} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: vxVesselArray.C,v $
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.6  2006/06/05 15:08:12  frank
// updated for visual studio 2005
//
// Revision 1.5  2005/06/08 19:07:15  vxconfig
// completed task
//
// Revision 1.4  2005/06/08 15:02:41  vxconfig
// formatting
//
// Revision 1.3  2005/06/07 18:14:38  vxconfig
// removed the vessel array
//
// Revision 1.2  2005/04/12 15:23:45  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.5  2004/05/14 11:15:26  frank
// fixed compacting the vessel array
//
// Revision 1.4  2004/04/14 14:01:21  frank
// compacted the vessel array to match component and label volume compaction during reading
//
// Revision 1.3  2004/03/30 21:52:17  frank
// added XML serialization
//
// Revision 1.2  2004/03/06 00:38:51  wenli
// PushVessel is added
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:29  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.2.1  2004/02/25 15:00:40  frank
// formatting
//
// Revision 1.5  2004/01/21 16:27:09  frank
// removed dead code
//
// Revision 1.4  2004/01/07 21:46:20  wenli
// Add  m_vEasySelectedVesselLabelList  for easy selected vessels
//
// Revision 1.3  2003/12/22 19:18:45  frank
// why call return at the end of a void function?
//
// Revision 1.2  2003/11/03 16:34:05  frank
// formatting
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.18  2003/04/30 15:21:19  frank
// Issue #3151: Used component handler to create new components so that they can be handled properly, e.g., grow, shrink, etc.
//
// Revision 1.17  2003/04/19 16:24:40  frank
// Added progress bar parameter
//
// Revision 1.16  2003/04/11 18:39:10  frank
// Set the label after adding a vessel
//
// Revision 1.15  2003/04/08 15:16:45  frank
// Properly serialized the section interval
//
// Revision 1.14  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.13  2003/03/26 19:45:32  frank
// Improved vessel removal
//
// Revision 1.12  2003/03/07 20:42:14  geconomos
// Commented out call to Win32 MessageBox method.
//
// Revision 1.11  2003/01/14 16:16:41  dongqing
// Code review
//
// Revision 1.10  2002/11/27 23:30:06  frank
// set label visible false when creating a new vessel
//
// Revision 1.9  2002/11/27 16:30:17  frank
// Reduced alpha of vessels.
//
// Revision 1.8  2002/11/22 20:47:58  frank
// Blended away vessel label to half opacity
//
// Revision 1.7  2002/11/22 03:33:55  frank
// fixed comment
//
// Revision 1.6  2002/11/20 17:08:52  frank
// removed dead code
//
// Revision 1.5  2002/11/13 02:15:59  frank
// Moved unique component name function into vxUtils.
//
// Revision 1.4  2002/11/13 00:00:37  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.3  2002/11/07 22:20:04  frank
// Cleaned up the vessel name
//
// Revision 1.2  2002/11/04 14:54:38  frank
// dead code removed
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.5  2002/10/25 17:11:39  frank
// Added versioning and current selection to (de)serialization.
//
// Revision 1.4  2002/10/25 16:07:03  frank
// Got percent stenosis to be live.
//
// Revision 1.3  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.2  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.1  2002/09/20 18:54:48  wenli
// v3D vessel component list
//
// Revision 1.53  2002/09/06 18:53:06  frank
// Working on flashing plaques.
//
// Revision 1.52  2002/09/05 22:54:32  frank
// Swapped volume and agatston columns in report table.
//
// Revision 1.51  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.50  2002/09/03 19:59:21  geconomos
// Cleaned up code for UnassignPlaque()
//
// Revision 1.49  2002/08/29 04:29:54  frank
// Cleaning up CSV file.
//
// Revision 1.48  2002/08/29 04:12:47  frank
// Fixed lesion count in scores table.
//
// Revision 1.47  2002/08/29 03:46:49  frank
// Moved marking code out of vxCardiacPlaques.
//
// Revision 1.46  2002/08/29 03:30:00  frank
// Preparing to move marking code to other class.
//
// Revision 1.45  2002/08/28 23:17:37  frank
// Moved mask volumes up more. Working on speeding up plaques.
//
// Revision 1.44  2002/08/28 19:52:38  frank
// Fixed "other" row in report table
//
// Revision 1.43  2002/08/28 17:57:05  frank
// Removed m_preferences and m_pMaskVolume
//
// Revision 1.42  2002/08/28 17:47:43  frank
// Moved mask volumes up. Working on speeding up plaques.
//
// Revision 1.41  2002/08/28 16:19:40  frank
// Added FixColors().
//
// Revision 1.40  2002/08/28 14:20:40  frank
// whitespace
//
// Revision 1.39  2002/08/27 22:28:26  frank
// Fixed volume units.
// Fixed "jump to selected plaque slice"
//
// Revision 1.38  2002/08/27 19:15:09  geconomos
// Added AreAllLocationsDetailed method.
//
// Revision 1.37  2002/08/27 18:44:31  frank
// Hilight -> Highlight
//
// Revision 1.36  2002/08/27 16:29:07  dmitry
// SetColor method added.
//
// Revision 1.35  2002/08/27 09:43:41  frank
// Working on bone tagging.
//
// Revision 1.34  2002/08/27 08:40:06  frank
// Working on jumping to selected plaque slice.
//
// Revision 1.33  2002/08/26 19:28:52  frank
// Moved mutex from vxPlaques.
//
// Revision 1.32  2002/08/26 17:02:22  frank
// Moved code from preferences into cardiac plaques.
//
// Revision 1.31  2002/08/23 21:25:14  dmitry
// GetThresholdShort added.
//
// Revision 1.30  2002/08/22 22:32:47  uday
// Color for GetArtery
//
// Revision 1.29  2002/08/22 18:32:32  sgagliardo
// fix string table one last time.
//
// Revision 1.28  2002/08/21 20:09:23  frank
// Working on noise tagging.
//
// Revision 1.27  2002/08/21 19:50:52  frank
// Cleaned out dirty bounding boxes.
//
// Revision 1.26  2002/08/21 19:39:16  frank
// Set homogenous flag in mask04Volume for iterator histogram.
//
// Revision 1.25  2002/08/21 19:36:59  frank
// Reflected change in iterator histogram.
//
// Revision 1.24  2002/08/21 15:40:48  frank
// Sped up region growing by 30% - not yet enough.
//
// Revision 1.23  2002/08/20 13:32:49  sgagliardo
// fix sTable, add caret.
//
// Revision 1.22  2002/08/19 23:23:09  frank
// Cleaned up tagging bone.
//
// Revision 1.21  2002/08/19 22:01:47  sgagliardo
// change sTable header.
//
// Revision 1.20  2002/08/19 20:31:53  frank
// Unified connectivity enumeration with vxBase.
//
// Revision 1.19  2002/08/19 20:06:42  frank
// Removed dead code.
//
// Revision 1.18  2002/08/19 19:30:39  frank
// Improved the speed of plaque serialization & deserialization.
//
// Revision 1.17  2002/08/16 18:23:40  uday
// Update Assign, Reassign & Unassign plaques.
//
// Revision 1.16  2002/08/15 19:10:11  frank
// Added table as string.
//
// Revision 1.15  2002/08/14 21:59:46  frank
// Fixed unassignment.
//
// Revision 1.14  2002/08/14 19:15:19  geconomos
// Worked on plaque selection.
//
// Revision 1.13  2002/08/12 19:55:42  frank
// Added plaque histogram renderer.
//
// Revision 1.12  2002/07/23 20:36:55  frank
// Cleaned up function naming.
//
// Revision 1.11  2002/07/23 19:39:19  frank
// Working on plaque selection and more.
//
// Revision 1.10  2002/07/19 16:23:24  manju
// Modified remove, reassign methods.
//
// Revision 1.9  2002/07/19 15:49:01  frank
// Put noise tagging back in.
//
// Revision 1.8  2002/07/19 15:46:40  frank
// Cleaned up tagging.
//
// Revision 1.7  2002/07/18 20:40:17  frank
// Resized volume instead of creating new one.
//
// Revision 1.6  2002/07/15 23:01:25  frank
// Added Lesion Number function.
//
// Revision 1.5  2002/07/12 12:20:09  geconomos
// no message
//
// Revision 1.4  2002/07/11 21:11:46  manju
// Speed up.
// Moved firing of events to the manipulator.
//
// Revision 1.3  2002/07/10 15:07:51  manju
// Removed warning.
//
// Revision 1.2  2002/07/10 11:42:01  geconomos
// no message
//
// Revision 1.1  2002/07/08 21:14:47  geconomos
// Initial revision.
//
// Revision 1.7  2002/06/21 18:35:37  jenny
// fixed an error, was missing type definition of a variable
//
// Revision 1.5  2002/06/18 22:03:17  geconomos
// Jenny changed iterator from the wrong one!
//
// Revision 1.4  2002/06/18 19:48:45  jenny
// moved creating of m_pMaskVolume up above
//
// Revision 1.3  2002/06/18 17:25:56  geconomos
// Working on createing the interpolated volume.
//
// Revision 1.2  2002/06/12 22:00:11  jenny
// incorporated changes from other people
//
// Revision 1.1  2002/06/12 21:42:04  jenny
// new files
//
// Revision 1.1  2002/06/07 15:02:23  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxVesselArray.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
// $Id: vxVesselArray.C,v 1.1 2006/08/17 14:01:12 gdavidson Exp $
