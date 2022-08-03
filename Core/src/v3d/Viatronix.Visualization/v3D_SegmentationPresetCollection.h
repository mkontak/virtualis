// $Id: v3D_SegmentationPresetCollection.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille( mailto:frank@viatronix.com )


// pragmas
#pragma once


// includes
#include "v3D_SegmentationPreset.h"
#include "v3D_Volume.h"


// namespaces
using namespace System::Collections;
OPEN_VISUALIZATION_NS


// class definition
[ System::Reflection::DefaultMemberAttribute( L"Item" ) ]
public ref class SegmentationPresetCollection : public CollectionBase
{

// member functions
public:

  /// default constructor
  SegmentationPresetCollection();

  /// Adds the specified SegmentationPreset to the collection.
  void Add( SegmentationPreset ^ mpSegmentationPreset );

  /// Removes the specified SegmentationPreset from the collection.
  void Remove( SegmentationPreset ^ mpSegmentationPreset );

  /// returns the specified preset
  SegmentationPreset ^ GetPreset( int iIndex );

  /// gets the preset range settings
  static SegmentationPresetCollection ^ GetRanges( Volume ^ mpVolume );

  /// gets the selected preset
  property SegmentationPreset ^ SelectedPreset
  {
    SegmentationPreset ^ get() { return m_mpSelectedPreset; }
    void set( SegmentationPreset ^ mpSelectedPreset ) { m_mpSelectedPreset = mpSelectedPreset; }
  }

  /// gets the segmentation presets for CT modality
  property static SegmentationPresetCollection ^ CtPresets
  {
    SegmentationPresetCollection ^ get() { return m_CtPresets; }
    void set( SegmentationPresetCollection ^ ctPresets ) { m_CtPresets = ctPresets; }
  }

protected:

  /// Validates that the specified object has the correct type.
  virtual void OnValidate( System::Object ^ mpSegmentationPreset ) override;

// data members
private:

  /// the segmentation presets for CT modality
  static SegmentationPresetCollection ^ m_CtPresets = gcnew SegmentationPresetCollection();

  /// selected preset 
  SegmentationPreset ^ m_mpSelectedPreset;

}; // class SegmentationPresetCollection


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_SegmentationPresetCollection.h,v $
// Revision 1.4  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.2  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.2.3  2005/08/11 19:49:09  geconomos
// Added SelectedPreset property.
//
// Revision 1.1.2.2  2005/08/02 16:32:23  frank
// Issue #4327: Added custom segmentation preset that leaves the intensity range as it was
//
// Revision 1.1.2.1  2005/07/07 13:04:56  frank
// Implemented a segmentation preset collection; moved CT presets to XML
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SegmentationPresetCollection.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SegmentationPresetCollection.h,v 1.4 2006/10/04 18:29:05 gdavidson Exp $
