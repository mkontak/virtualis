// $Id: v3D_SegmentationPresetCollection.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille( mailto:frank@viatronix.com )


// includes
#include "stdafx.h"
#include "v3D_SegmentationPresetCollection.h"
#include "v3D_IntensityConverter.h"


// namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Xml;
USING_VISUALIZATION_NS

/**
 * Default constructor
 */
 SegmentationPresetCollection::SegmentationPresetCollection() : 
 m_mpSelectedPreset( nullptr )
{
} // SegmentationPresetCollection

/// <summary>
/// Adds the specified SegmentationPreset to the collection.
/// </summary>
/// <param name="segmentationPreset">SegmentationPreset object being added.</param>
void SegmentationPresetCollection::Add( SegmentationPreset ^ mpSegmentationPreset )
{
  this->List->Add( mpSegmentationPreset );
} // Add( mpSegmentationPreset )


/// <summary>
/// Removes the specified SegmentationPreset from the collection.
/// </summary>
/// <param name="mpSegmentationPreset">SegmentationPreset object being removed.</param>
void SegmentationPresetCollection::Remove( SegmentationPreset ^ mpSegmentationPreset )
{
  this->List->Remove( mpSegmentationPreset );
} // Remove( segmentationPreset )


/**
 * Returns the specified preset
 *
 * @param     iIndex      index of segemntaion preset tp reurn
 * @return    SegmentationPreset reference
 */
SegmentationPreset ^ SegmentationPresetCollection::GetPreset( int iIndex )
{
  return safe_cast< SegmentationPreset ^ >( this->List[ iIndex ]);
} //GetPreset( int iIndex )


/// <summary>
/// Validates that the specified object has the correct type.
/// </summary>
/// <param name="mpSegmentationPreset">SegmentationPreset object being validated.</param>
void SegmentationPresetCollection::OnValidate( System::Object ^ mpSegmentationPreset )
{
  System::Collections::CollectionBase::OnValidate( mpSegmentationPreset );
  if ( dynamic_cast< SegmentationPreset ^ >( mpSegmentationPreset ) != nullptr )
    return;

  throw gcnew System::ArgumentException( "The specified parameter is not of type SegmentationPreset." );
} // OnValidate( mpSegmentationPreset )


/**
 * Return the default presets for a CT volume
 * @return array of segmentation presets
 */
SegmentationPresetCollection ^ SegmentationPresetCollection::GetRanges(Volume ^ mpVolume)
{
  std::string sModality = ss(mpVolume->GetHeaderVariable(HeaderVariables::DataModality));
  int min = IntensityConverter::GetMinValue(mpVolume->Header, true);
  int max = IntensityConverter::GetMaxValue(mpVolume->Header, true);
  int subwidth(( max - min )  / 4);
  
  if (sModality == "CT")
  {
    if(m_CtPresets->Count > 0 )
    {
      m_CtPresets->SelectedPreset = safe_cast< Viatronix::Visualization::SegmentationPreset ^ >( m_CtPresets->List[ 0 ] );
      return m_CtPresets;
    }
    else
    {
      return nullptr;
    }
  }
  else if (sModality == "MR")
  {
    SegmentationPreset ^ mpCustom  = gcnew SegmentationPreset( "Custom",  min,                               max,                                         System::Drawing::Color::FromArgb( 184, 184, 4   ), false );
    SegmentationPreset ^ mpLow     = gcnew SegmentationPreset( "Low",     min,                               min + subwidth,                              System::Drawing::Color::FromArgb( 227, 135, 15  ), true  );
    SegmentationPreset ^ mpMedLow  = gcnew SegmentationPreset( "MedLow",  mpLow->IntensityRangeHigh + 1,     mpLow->IntensityRangeHigh + subwidth - 1,    System::Drawing::Color::FromArgb( 40 , 221, 252 ), true  );
    SegmentationPreset ^ mpMedHigh = gcnew SegmentationPreset( "MedHigh", mpMedLow->IntensityRangeHigh + 1,  mpMedLow->IntensityRangeHigh + subwidth - 1, System::Drawing::Color::FromArgb( 39 , 62 , 228 ), true  );
    SegmentationPreset ^ mpHigh    = gcnew SegmentationPreset( "High",    mpMedHigh->IntensityRangeHigh + 1, max,                                         System::Drawing::Color::FromArgb( 72 , 121, 166 ), true  );

    SegmentationPresetCollection ^ mpRanges = gcnew SegmentationPresetCollection();
    mpRanges->Add( mpCustom );
    mpRanges->Add( mpLow );
    mpRanges->Add( mpMedLow );
    mpRanges->Add( mpMedHigh );
    mpRanges->Add( mpHigh );
    mpRanges->SelectedPreset = mpCustom;
    return mpRanges;
  }
  else // not yet different but once we revisit MR it will be needed
  {
    SegmentationPreset ^ mpCustom  = gcnew SegmentationPreset( "Custom" , min                              , max,                                         System::Drawing::Color::FromArgb( 184, 184, 4   ), false );
    SegmentationPreset ^ mpLow     = gcnew SegmentationPreset( "Low"    , min                              , min + subwidth,                              System::Drawing::Color::FromArgb( 227, 135, 15  ), true  );
    SegmentationPreset ^ mpMedLow  = gcnew SegmentationPreset( "MedLow" , mpLow->IntensityRangeHigh + 1    , mpLow->IntensityRangeHigh + subwidth - 1,    System::Drawing::Color::FromArgb( 40 , 221, 252 ), true  );
    SegmentationPreset ^ mpMedHigh = gcnew SegmentationPreset( "MedHigh", mpMedLow->IntensityRangeHigh + 1 , mpMedLow->IntensityRangeHigh + subwidth - 1, System::Drawing::Color::FromArgb( 39 , 62 , 228 ), true  );
    SegmentationPreset ^ mpHigh    = gcnew SegmentationPreset( "High"   , mpMedHigh->IntensityRangeHigh + 1, max,                                         System::Drawing::Color::FromArgb( 72 , 121, 166 ), true  );

    SegmentationPresetCollection ^ mpRanges = gcnew SegmentationPresetCollection();
    mpRanges->Add( mpCustom );
    mpRanges->Add( mpLow );
    mpRanges->Add( mpMedLow );
    mpRanges->Add( mpMedHigh );
    mpRanges->Add( mpHigh );
    mpRanges->SelectedPreset = mpCustom;
    return mpRanges;
  }

  return nullptr;

} // GetRanges()


// $Log: v3D_SegmentationPresetCollection.cpp,v $
// Revision 1.5  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2005/11/22 21:18:04  geconomos
// sanity check for null CT preset collection
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_SegmentationPresetCollection.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SegmentationPresetCollection.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
