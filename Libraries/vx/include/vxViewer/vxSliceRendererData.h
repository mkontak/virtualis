// $Id: vxSliceRendererData.h,v 1.14 2006/09/26 16:01:16 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)


// pragmas
#pragma once


// includes
#include "vxShareableObject.h"
#include "vxBlockVolume.h"
#include "vxColormap.h"
#include "vxComponentArray.h"


// forward declarations


// defines
#ifndef VX_VIEWER_DLL
#define VX_VIEWER_DLL _declspec( dllimport )
#endif


// class declaration
class VX_VIEWER_DLL vxSliceRendererData
{
// member functions
public:
  
  /// default constructor
  vxSliceRendererData();
  
 
  /// desttuctor
  virtual ~vxSliceRendererData();  
  
  /// loads the specified volume files
  bool Load( const std::string & sEcvVolume, const std::string & sDiffVolume, const bool & bRotateProneToSupine );
  
  /// loads the specified volume files
  bool Load2( const std::string & sOriginalVolume, const std::string & sCleansedDiffVolume, const bool & bRotateProneToSupine );
  
  /// loads the specified volume files
  bool Load3( vxBlockVolume<uint2> * pOriginalVol, vxBlockVolume<uint2> * pCDV );

  /// Loads the original volume only
  bool vxSliceRendererData::LoadVolOnly( vxBlockVolume<uint2> * pOriginalVol );

  /// Loads the CDV volume
  bool vxSliceRendererData::LoadCDV( uint2 * pCDV, const vxVolumeHeader & vHeader );

	/// sets the biopsy colormap 
  void SetBiopsyColormap( const std::vector< std::pair< float4, float4 > > & vRed, 
                          const std::vector< std::pair< float4, float4 > > & vGreen, 
                          const std::vector< std::pair< float4, float4 > > & vBlue, 
                          const std::vector< std::pair< float4, float4 > > & vAlpha );

  /// gets the voxel's modality value
  int GetVoxel( int x, int y, int z, bool bUncleansed );

                          
  /// resets the renderer to a default state
  void Reset();
  
  /// indicates if the data is valid
  bool IsValid() const { return m_bDataValid; }

  /// gets the cleansed volume
  vxShareableObject< vxBlockVolume< uint2 > > * GetCleansedVolume() const { return m_pCleansedVolume ; }

  /// gets the uncleansed volume
  vxShareableObject< vxBlockVolume< uint2 > > * GetUncleansedVolume() const { return m_pUncleansedVolume ; }

  /// gets the label volume
  vxShareableObject< vxBlockVolume< uint2 > > * GetLabelVolume() const { return m_pLabelVolume ; }

  /// gets the default colormap
  vxShareableObject< vxColormap > * GetDefaultColormap() const { return m_pDefaultColormap; }
  
  /// gets the biopsy colormap
  vxShareableObject< vxColormap > * GetBiopsyColormap() const { return m_pBiopsyColormap; }

  /// gets the biopsy colormap
  vxShareableObject< vxComponentArray > * GetComponents() const { return m_pComponents; }

  /// assignment operator
  vxSliceRendererData & operator=( const vxSliceRendererData & rhs );


private:

  /// copy constructor
  vxSliceRendererData( const vxSliceRendererData & other ) { }

 
// member variables
private:

  /// cleansed volume
  vxShareableObject< vxBlockVolume< uint2 > > * m_pCleansedVolume;
  
  /// uncleansed volume
  vxShareableObject< vxBlockVolume< uint2 > > * m_pUncleansedVolume;
  
  /// label volume
  vxShareableObject< vxBlockVolume< uint2 > > * m_pLabelVolume;
  
  /// default colormap
  vxShareableObject< vxColormap > * m_pDefaultColormap;

  /// biopsy colormap
  vxShareableObject< vxColormap > * m_pBiopsyColormap;

  /// components
  vxShareableObject< vxComponentArray > * m_pComponents;
  
  /// indicates if the data is valid
  bool m_bDataValid;
  
}; // class vxSliceRendererData


// $Log: vxSliceRendererData.h,v $
// Revision 1.14  2006/09/26 16:01:16  geconomos
// removed unused vxEnvironment
//
// Revision 1.13  2006/09/26 15:59:08  geconomos
// removed pimpl classes
//
// Revision 1.12  2006/09/25 13:59:05  geconomos
// exposed label volume
//
// Revision 1.11  2006/09/13 15:41:57  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.10  2006/06/22 14:02:20  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.9  2006/06/08 18:20:50  frank
// exposed the uncleansed volume
//
// Revision 1.8  2005/11/01 14:49:18  frank
// Issue #4503: Read new .cdv and .vol files instead of _hq files
//
// Revision 1.7  2004/11/17 13:14:03  geconomos
// Added GetVoxel method
//
// Revision 1.6  2004/09/16 15:53:31  geconomos
// Moved default colormap from renderer data to renderer
//
// Revision 1.5  2004/05/19 13:04:39  geconomos
// Added window/level and colormap manipulation methods.
//
// Revision 1.4  2004/04/21 18:33:39  geconomos
// Continued effort integrating 2D views within vc.
//
// Revision 1.3  2004/04/06 13:37:46  geconomos
// Fixed spelling of supine.
//
// Revision 1.2  2004/04/05 18:25:56  geconomos
// Initial effort to get volume files to load into vc in the block volume format.
//
// Revision 1.1  2004/04/05 15:44:44  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSliceRendererData.h,v 1.14 2006/09/26 16:01:16 geconomos Exp $
// $Id: vxSliceRendererData.h,v 1.14 2006/09/26 16:01:16 geconomos Exp $