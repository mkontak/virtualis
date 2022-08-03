// $Id: v3D_SegmentationPreset.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "v3d_SegmentationPreset.h"
#include "v3D_IntensityConverter.h"
#include <string>


// namespaces
using namespace System::Xml;
USING_VISUALIZATION_NS


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Default constructor
 */
 SegmentationPreset::SegmentationPreset() :
  m_mpName( System::String::Empty ),
  m_color( Color.Red ),
  m_bIntensityRangeReset( false ),
  m_iIntensityRangeLow( 0 ),
  m_iIntensityRangeHigh( 0 )
{
} // SegmentationPreset()


/**
 * Constructor from name, low and high intensity
 * @param mpName      preset name
 * @param iLow        lower value of intensity range
 * @param iHigh       upper value of intensity range
 * @param color       color of component made out of preset
 * @param bResetRange reset the intensity range
 */
SegmentationPreset::SegmentationPreset( System::String ^ mpName, int iLow, int iHigh, System::Drawing::Color color, bool bResetRange ) :
  m_mpName( mpName ),
  m_color( color ),
  m_bIntensityRangeReset( bResetRange ),
  m_iIntensityRangeLow( iLow ),
  m_iIntensityRangeHigh( iHigh )
{
} // SegmentationPreset( name, low, high, color )


/**
 * initializer from name and color (will not reset intensity range)
 *
 * @param mpName preset name
 * @param color  color of component made out of preset
 */
SegmentationPreset::SegmentationPreset( System::String ^ mpName, System::Drawing::Color color ) :
  m_mpName( mpName ),
  m_color( color ),
  m_bIntensityRangeReset( false ),
  m_iIntensityRangeLow( 0 ),
  m_iIntensityRangeHigh( 0 )
{
} // SegmentationPreset( name, color )


/**
 * create from xml
 *
 * @param parent the parent xml node
 *
 * @return a new segmentation preset
 */
SegmentationPreset ^ SegmentationPreset::FromXml( System::Xml::XmlNode ^ mpParent )
{

  try
  {
    //
    // check the version
    //
    XmlNode ^ mpVersionNode = mpParent->Attributes->GetNamedItem( "version" );
    if ( mpVersionNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "version missing from SegmentationPreset node", "SegmentationPreset", "FromXml" ) );
    }
    System::String ^ mpVersion = mpVersionNode->Value;
    if ( mpVersion->CompareTo( "1.0" ) != 0 )
    {
      throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "SegmentationPreset", "FromXml" ) );
    }

    //
    // get name
    //
    XmlNode ^ mpNameNode = mpParent->Attributes->GetNamedItem( "name" );
    if ( mpNameNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "name attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    System::String ^ mpName = mpNameNode->Value;

    //
    // get range reset
    //
    XmlNode ^ mpResetNode = mpParent->Attributes->GetNamedItem( "reset_range" );
    if ( mpResetNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "reset_range attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    bool bResetRange = System::Boolean::Parse( mpResetNode->Value );

    //
    // get low range
    //
    XmlNode ^ mpLowNode = mpParent->Attributes->GetNamedItem( "low" );
    if ( mpLowNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "low attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    int4 iLow = System::Int32::Parse( mpLowNode->Value );

    //
    // get high range
    //
    XmlNode ^ mpHighNode = mpParent->Attributes->GetNamedItem( "high" );
    if ( mpHighNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "high attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    int4 iHigh = System::Int32::Parse( mpHighNode->Value );

    //
    // get red color (0-255)
    //
    XmlNode ^ mpRedNode = mpParent->Attributes->GetNamedItem( "red" );
    if ( mpRedNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "red attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    uint1 uRed = System::Byte::Parse( mpRedNode->Value );

    //
    // get green color (0-255)
    //
    XmlNode ^ mpGreenNode = mpParent->Attributes->GetNamedItem( "green" );
    if ( mpGreenNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "green attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    uint1 uGreen = System::Byte::Parse( mpGreenNode->Value );

    //
    // get blue color (0-255)
    //
    XmlNode ^ mpBlueNode = mpParent->Attributes->GetNamedItem( "blue" );
    if ( mpBlueNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "blue attribute missing", "SegmentationPreset", "FromXml" ) );
    }
    uint1 uBlue = System::Byte::Parse( mpBlueNode->Value );

    //
    // make the segmentation preset
    //
    System::Drawing::Color color = System::Drawing::Color::FromArgb( uRed, uGreen, uBlue );
    SegmentationPreset ^ mpNewSegmentationPreset = gcnew SegmentationPreset( mpName, iLow, iHigh, color, bResetRange );
    return mpNewSegmentationPreset;
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in SegmentationPreset::FromXml" );
  }

} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_SegmentationPreset.cpp,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.1.1.12.3  2005/08/02 16:32:23  frank
// Issue #4327: Added custom segmentation preset that leaves the intensity range as it was
//
// Revision 1.1.1.1.12.2  2005/07/07 13:04:25  frank
// Implemented a segmentation preset collection; moved CT presets to XML
//
// Revision 1.1.1.1.12.1  2005/07/05 17:55:37  frank
// added names and colors to the segmentation presets
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.7  2003/07/29 11:03:01  michael
// adjusted the segmentation preset to use a volume (volume header) to put
// together the ranges for the auto segments based on modality etc.
//
// Revision 1.6  2003/06/04 18:36:17  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.5  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.4  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.3  2003/05/12 19:06:50  geconomos
// Coding standards.
//
// Revision 1.2  2002/11/25 23:20:15  geconomos
// Corrected default values for MR.
//
// Revision 1.1  2002/11/25 13:52:47  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SegmentationPreset.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SegmentationPreset.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
