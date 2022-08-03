// $Id: v3D_SegmentationPreset.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// pragmas
#pragma once


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class SegmentationPreset
{
// functions
public:
  
  /// default constructor
  SegmentationPreset();

  /// initializer from name, low intensity, high intensity, color and resetRange flag
  SegmentationPreset( System::String ^ mpName, int iLow, int iHigh, System::Drawing::Color color, bool bResetRange );
  
  /// initializer from name and color (will not reset intensity range)
  SegmentationPreset( System::String ^ mpName, System::Drawing::Color color );
  
  /// gets the preset's name
  property System::String ^ Name
  {
    System::String ^ get() { return m_mpName; }
    void set( System::String ^ mpName ) { m_mpName = mpName; }
  }

  /// gets the preset's color
  property System::Drawing::Color Color
  {
    System::Drawing::Color get() { return m_color; }
   void set( System::Drawing::Color color ) { m_color = color; }
  }

  /// gets if the preset should reset the intensity range upon activation
  property bool IntensityRangeReset
  {
    bool get() { return m_bIntensityRangeReset; }
    void set( bool bIntensityRangeReset ) { m_bIntensityRangeReset = bIntensityRangeReset; }
  }

  /// gets the preset's low intensity
  property int IntensityRangeLow
  {
    int get() { return m_iIntensityRangeLow; }
    void set( int iIntensityRangeLow ) { m_iIntensityRangeLow = iIntensityRangeLow; }
  }

  /// gets the preset's high intensity
  property int IntensityRangeHigh
  {
    int get() { return m_iIntensityRangeHigh; }
    void set( int iIntensityRangeHigh ) { m_iIntensityRangeHigh = iIntensityRangeHigh; }
  }

  /// create from xml
  static SegmentationPreset ^ FromXml( System::Xml::XmlNode ^ mpParent );

private:

  /// name
  System::String ^ m_mpName;
  
  /// the suggested component color
  System::Drawing::Color m_color;

  /// if the intensity range should be reset
  bool m_bIntensityRangeReset;

  /// lower value of intensity range
  int m_iIntensityRangeLow;
  
  /// upper value of intensity range
  int m_iIntensityRangeHigh;

}; // class SegmentationPreset


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_SegmentationPreset.h,v $
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
// Revision 1.6  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.5  2003/07/29 11:03:01  michael
// adjusted the segmentation preset to use a volume (volume header) to put
// together the ranges for the auto segments based on modality etc.
//
// Revision 1.4  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.3  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.2  2003/05/12 19:06:50  geconomos
// Coding standards.
//
// Revision 1.1  2002/11/25 13:52:47  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SegmentationPreset.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SegmentationPreset.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
