// $Id: vxSliceRendererData.C,v 1.22 2008/04/24 21:55:35 jmeade Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

// includes
#include "StdAfx.h"
#include "vxSliceRendererData.h"
#include "Logger.h"
#include "File.h"
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"
#include "vxEnvironment.h"
#include "vxIntensityConverter.h"
#include "Timer.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxHistogramUpdaterLabel.h"
#include "vxBlockVolumeUtilities.h"



#define FILE_REVISION "$Revision: 1.22 $"

/**
 * Default constructor.
 */
vxSliceRendererData::vxSliceRendererData()
{
  static bool bInit = false;
  if( bInit == false )
  {
    bInit = true;
  }

  m_pCleansedVolume   = new vxShareableObject< vxBlockVolume< uint2 > >();
  m_pUncleansedVolume = new vxShareableObject< vxBlockVolume< uint2 > >();
  m_pLabelVolume      = new vxShareableObject< vxBlockVolume< uint2 > >();  
  m_pComponents       = new vxShareableObject< vxComponentArray >();  

  m_pDefaultColormap = new vxShareableObject< vxColormap >();
  m_pDefaultColormap->SetActiveRange( false );
  
  m_pBiopsyColormap  = new vxShareableObject< vxColormap >();
  m_pBiopsyColormap->SetWindowLevel( vxWindowLevel( 1.0F, 0.5F ) );
  
  m_pBiopsyColormap->SetActiveRange( false );
  
  m_bDataValid = false;

} // vxSliceRendererData


/**
 * Destructor.
 */
vxSliceRendererData::~vxSliceRendererData()
{
  /**
   * This is really bad. For one reason or another the following causes VC to crash
   * while exiting. Luckily no leak is created from this due to the vc implementation.
   *
   * george  08-04-2005
  
    delete m_pCleansedVolume;
    delete m_pUncleansedVolume;
    delete m_pLabelVolume;
    delete m_pDefaultColormap;  
    delete m_pBiops
    delete m_pComponents;
    */
} // ~vxSliceRendererData()


/**
 * Makes a copy of the specified slice renderer.
 *
 * @param   other   Source data.
 */
vxSliceRendererData & vxSliceRendererData::operator=( const vxSliceRendererData & rhs )
{
  if( this != &rhs )
  {
    *m_pCleansedVolume = *( rhs.m_pCleansedVolume );
    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pCleansedVolume );

    *m_pUncleansedVolume = *( rhs.m_pUncleansedVolume );
    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pUncleansedVolume );
    
    *m_pLabelVolume = *( rhs.m_pLabelVolume );
    vxHistogramUpdaterLabel<uint2>::Update( *m_pLabelVolume );
    
    *m_pDefaultColormap = *(rhs.m_pDefaultColormap );
    *m_pBiopsyColormap = *(rhs.m_pBiopsyColormap );
  
  }
  return *this;
} // operator=( const vxSliceRendererData & rhs )


/**
 * Loads the specified volume files.
 *
 * @param sEcvVolume           filename of the electronically cleansed volume
 * @param sDiffVolume          filename of the difference volume
 * @param bRotateProneToSupine if the volumes should be rotated after loading
 * @return true if successful
 */
bool vxSliceRendererData::Load( const std::string & sEcvVolume, const std::string & sDiffVolume, const bool & bRotateProneToSupine )
{ 
 bool bReturn( true );
  
  try
  {
    Timer timer, timerAll;
    m_pCleansedVolume->Read( sEcvVolume );
    if ( bRotateProneToSupine )
      vxBlockVolumeUtilities::ReformatVolumeToSupine( * m_pCleansedVolume, "FFP" );
    vxHistogramUpdaterIntensity<uint2>::Update( *m_pCleansedVolume);

    
    m_pLabelVolume->GetHeader().SetVolDim( m_pCleansedVolume->GetHeader().GetVolDim() );
    m_pLabelVolume->GetHeader().SetVoxelUnits( m_pCleansedVolume->GetHeader().GetVoxelUnits() );
    vxHistogramUpdaterLabel<uint2>::Update( *m_pLabelVolume );

    *m_pUncleansedVolume = *m_pCleansedVolume;
    if( vx::File( sDiffVolume ).Exists() )
    {
      vxBlockVolume< uint2 > diffVolume( sDiffVolume );
      if ( bRotateProneToSupine )
        vxBlockVolumeUtilities::ReformatVolumeToSupine( diffVolume, "FFP" );

      vxBlockVolumeIterator< uint2 > diffIter( diffVolume );
      
      vxBlockVolumeIterator< uint2 > uncleansedIter( *m_pUncleansedVolume );
      for( ; diffIter.IsNotAtEnd(); diffIter.NextNonBackgroundBlockZYX() )
      {      
        uncleansedIter.SetPos( diffIter.GetPos() );
        for( ; diffIter.IsNotAtEndOfBlock(); diffIter.NextZYXinsideBlock(), uncleansedIter.NextZYXinsideBlock() )
        {
          if( diffIter.GetVoxel() != 0 )
            uncleansedIter.SetVoxel( diffIter.GetVoxel() );
        }
      }      
    }
    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pUncleansedVolume );


    {
      vxBlockVolumeIterator< uint2 > uncleansedIter( *m_pUncleansedVolume );
      for( ; uncleansedIter.IsNotAtEnd(); uncleansedIter.NextZYX())
      {
        if (uncleansedIter.GetVoxel() > 4095)
          uncleansedIter.SetVoxel(4095);
      }      
      vxBlockVolumeIterator< uint2 > cleansedIter( *m_pCleansedVolume );
      for( ; cleansedIter.IsNotAtEnd(); cleansedIter.NextZYX() )
      {
        if (cleansedIter.GetVoxel() > 4095)
          cleansedIter.SetVoxel(4095);
      }
    }

    m_pUncleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    m_pCleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    
    m_bDataValid = true;
    
    LogDbg("### Overall loading time was: " + ToAscii(timerAll.ElapsedTime()), "", ""); timer.Reset();

  } // try
  catch( ... )
  {    
    bReturn = false;
  } // catch( ... )  
  return bReturn;
} // Load()


/**
 * Loads the specified volume files.
 *
 * @param sOriginalVolume      filename of the original volume
 * @param sCleansedDiffVolume  filename of the cleansed difference volume (converts from original to cleansed)
 * @param bRotateProneToSupine if the volumes should be rotated after loading
 * @return true if successful
 */
bool vxSliceRendererData::Load2( const std::string & sOriginalVolume, const std::string & sCleansedDiffVolume, const bool & bRotateProneToSupine )
{ 
  bool bReturn( true );
  
  try
  {
    Timer timer, timerAll;
    m_pUncleansedVolume->Read( sOriginalVolume );
    if ( bRotateProneToSupine )
      vxBlockVolumeUtilities::ReformatVolumeToSupine( * m_pUncleansedVolume, "FFP" );
    vxHistogramUpdaterIntensity<uint2>::Update( *m_pUncleansedVolume );
    
    m_pLabelVolume->GetHeader().SetVolDim( m_pUncleansedVolume->GetHeader().GetVolDim() );
    m_pLabelVolume->GetHeader().SetVoxelUnits( m_pUncleansedVolume->GetHeader().GetVoxelUnits() );
    vxHistogramUpdaterLabel<uint2>::Update( *m_pLabelVolume );

    * m_pCleansedVolume = * m_pUncleansedVolume;
    if( vx::File( sCleansedDiffVolume ).Exists() )
    {
      vxBlockVolume< uint2 > cleansedDiffVolume( sCleansedDiffVolume );
      if ( bRotateProneToSupine )
        vxBlockVolumeUtilities::ReformatVolumeToSupine( cleansedDiffVolume, "FFP" );

      vxBlockVolumeIterator< uint2 > cleansedDiffIter( cleansedDiffVolume );
      
      vxBlockVolumeIterator< uint2 > cleansedIter( * m_pCleansedVolume );
      for( ; cleansedDiffIter.IsNotAtEnd(); cleansedDiffIter.NextNonBackgroundBlockZYX() )
      {      
        cleansedIter.SetPos( cleansedDiffIter.GetPos() );
        for( ; cleansedDiffIter.IsNotAtEndOfBlock(); cleansedDiffIter.NextZYXinsideBlock(), cleansedIter.NextZYXinsideBlock() )
        {
          if( cleansedDiffIter.GetVoxel() != 0 )
            cleansedIter.SetVoxel( cleansedDiffIter.GetVoxel() );
        }
      }      
    }
    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pCleansedVolume);

    // clamp both volumes
    {
      vxBlockVolumeIterator< uint2 > uncleansedIter( * m_pUncleansedVolume );
      for( ; uncleansedIter.IsNotAtEnd(); uncleansedIter.NextBlockZYX() )
      {
        for( ; uncleansedIter.IsNotAtEndOfBlock(); uncleansedIter.NextZYXinsideBlock() )
        {
          if ( uncleansedIter.GetVoxel() > 4095 )
            uncleansedIter.SetVoxel( 4095 );
        }
      }      
      vxBlockVolumeIterator< uint2 > cleansedIter( * m_pCleansedVolume );
      for( ; cleansedIter.IsNotAtEnd(); cleansedIter.NextBlockZYX() )
      {
        for( ; cleansedIter.IsNotAtEndOfBlock(); cleansedIter.NextZYXinsideBlock() )
        {
          if ( cleansedIter.GetVoxel() > 4095 )
            cleansedIter.SetVoxel( 4095 );
        }
      }
    }
    m_pUncleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    m_pCleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    
    m_bDataValid = true;

  } // try
  catch( ... )
  {    
    bReturn = false;
  } // catch( ... )  
  return bReturn;
} // Load2()


/**
 * Loads the specified volume files.
 *
 * @param   pOriginalVolume
 * @param   pCDV
 * @return  True if successful; false otherwise.
 */
bool vxSliceRendererData::Load3( vxBlockVolume<uint2> * pOriginalVol, vxBlockVolume<uint2> * pCDV )
{ 
  bool bReturn( true );
  
  try
  {
    Timer timer, timerAll;
    * m_pUncleansedVolume = * pOriginalVol;
    m_pLabelVolume->GetHeader().SetVolDim( m_pUncleansedVolume->GetHeader().GetVolDim() );
    m_pLabelVolume->GetHeader().SetVoxelUnits( m_pUncleansedVolume->GetHeader().GetVoxelUnits() );
    vxHistogramUpdaterLabel<uint2>::Update( *m_pLabelVolume );
    m_pUncleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);

    * m_pCleansedVolume = * m_pUncleansedVolume;
    vxBlockVolumeIterator< uint2 > cleansedDiffIter( *pCDV );
    vxBlockVolumeIterator< uint2 > cleansedIter( * m_pCleansedVolume );
    for( ; cleansedDiffIter.IsNotAtEnd(); cleansedDiffIter.NextNonBackgroundBlockZYX() )
    {      
      cleansedIter.SetPos( cleansedDiffIter.GetPos() );
      for( ; cleansedDiffIter.IsNotAtEndOfBlock(); cleansedDiffIter.NextZYXinsideBlock(), cleansedIter.NextZYXinsideBlock() )
      {
        if( cleansedDiffIter.GetVoxel() != 0 )
          cleansedIter.SetVoxel( cleansedDiffIter.GetVoxel() );
      }
    }      
    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pCleansedVolume );
    m_pCleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    m_pCleansedVolume->Compact();
    
    m_bDataValid = true;
  } // try
  catch( ... )
  {    
    bReturn = false;
  } // catch( ... )  
  return bReturn;
} // Load2( const std::string & sOriginalVolume, const std::string & sCleansedDiffVolume )


/**
 * Loads the original volume only
 *
 * @param   pOriginalVolume the pointer to the original block volume
 * @return  True if successful; false otherwise.
 */
bool vxSliceRendererData::LoadVolOnly( vxBlockVolume<uint2> * pOriginalVol )
{ 
	bool bReturn( true );
  try
  {
    m_pUncleansedVolume = reinterpret_cast< vxShareableObject<vxBlockVolume<uint2>> * >(pOriginalVol);

    m_pLabelVolume->GetHeader().SetVolDim( m_pUncleansedVolume->GetHeader().GetVolDim() );
    m_pLabelVolume->GetHeader().SetVoxelUnits( m_pUncleansedVolume->GetHeader().GetVoxelUnits() );
    vxHistogramUpdaterLabel<uint2>::Update( *m_pLabelVolume );
    m_pUncleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);

		* m_pCleansedVolume = * pOriginalVol;

    m_bDataValid = true;
  } // try
  catch( ... )
  {    
    bReturn = false;
  } // catch( ... )  

  return bReturn;
} // LoadPNVC


/**
 * Load the CDV volume
 *
 * @param   pCDV the pointer to the CDV linear volume
 * @param   vHeader the updated block volume header containing preprocessing parameters
 * @return  True if successful; false otherwise.
 */
bool vxSliceRendererData::LoadCDV( uint2 * pCDV, const vxVolumeHeader & vHeader )
{ 
	bool bReturn( true );
  try
  {
		m_pCleansedVolume->SetHeader( vHeader );

		const uint2 u = vxIntensityConverter::ModalityToRaw( -1000, vHeader );
		const int4 iX = vHeader.GetVolDim().m_x;
		const int4 iY = vHeader.GetVolDim().m_y;
		const int4 iZ = vHeader.GetVolDim().m_z;
		const int4 iS = iX*iY;

    vxBlockVolumeIterator< uint2 > cleansedIter( * m_pCleansedVolume );

		register int4 x, y, z, i, j, k;
		i = 0;
		for( z=0; z<iZ; z++ )
		{
			j = i;
			for( y=0; y<iY; y++ )
			{
				k = j;
				for( x=0; x<iX; x++ )
				{
					if( pCDV[k] > u )
					{
            cleansedIter.SetPos(x,y,z);
            cleansedIter.SetVoxel( pCDV[k] );
					} // if

					k++;
				} // x

				j = j + iX;
			} // for y

			i = i + iS;
		} // for z

    vxHistogramUpdaterIntensity< uint2 >::Update( *m_pCleansedVolume );
    m_pCleansedVolume->GetHeader().SetSignificantBitsPerVoxel(12);
    m_pCleansedVolume->Compact();
  } // try
  catch( ... )
  {    
    bReturn = false;
  } // catch( ... )  

  return bReturn;
} // LoadCDVINS



/**
 * Resets the renderer to a default state.
 */
void vxSliceRendererData::Reset()
{
} // Reset()


/**
 * Sets the colormap
 *
 * @param   pfRed     Red channel values.
 * @param   pfGreen   Green channel values.
 * @param   pfBlue    Blue channel values.
 * @param   pfAlpha   Alpha channel values.
 * @param   iSize     Number of values contained in channel arrays.
 */
void vxSliceRendererData::SetBiopsyColormap(  const std::vector< std::pair< float4, float4 > > & vRed, 
                                              const std::vector< std::pair< float4, float4 > > & vGreen, 
                                              const std::vector< std::pair< float4, float4 > > & vBlue, 
                                              const std::vector< std::pair< float4, float4 > > & vAlpha )

{
  std::vector<Point2D<ZeroOne> > vRed2;
  for( int i( 0 ); i < vRed.size(); ++i )
    vRed2.push_back( Point2D< ZeroOne >( ZeroOne( vRed[ i ].first ), ZeroOne( vRed[ i ].second ) ) );
  m_pBiopsyColormap->SetRedList( vRed2 );

  std::vector<Point2D<ZeroOne> > vGreen2;
  for( int i( 0 ); i < vGreen.size(); ++i )
    vGreen2.push_back( Point2D< ZeroOne >( ZeroOne( vGreen[ i ].first ), ZeroOne( vGreen[ i ].second ) ) );
  m_pBiopsyColormap->SetGreenList( vGreen2 );
  
  std::vector<Point2D<ZeroOne> > vBlue2;
  for( int i( 0 ); i < vBlue.size(); ++i )
    vBlue2.push_back( Point2D< ZeroOne >( ZeroOne( vBlue[ i ].first ), ZeroOne( vBlue[ i ].second ) ) );
  m_pBiopsyColormap->SetBlueList( vBlue2 );

  std::vector<Point2D<ZeroOne> > vAlpha2;
  for( int i( 0 ); i < vAlpha.size(); ++i )
    vAlpha2.push_back( Point2D< ZeroOne >( ZeroOne( vAlpha[ i ].first ), ZeroOne( vAlpha[ i ].second ) ) );
  m_pBiopsyColormap->SetAlphaList( vAlpha2 );
  
  m_pBiopsyColormap->SetDirty();
} // SetColormap( ... )


/**
 * Gets the voxel's modality value.
 *
 * @param   x               x component
 * @param   y               y component
 * @param   z               z component
 * @param   bUncleansed     true if the uncleansed value should be used; false to use uncleansed volume
 * @return                  Volume instensity converted based on modality.
 */
int vxSliceRendererData::GetVoxel( int x, int y, int z, bool bUncleansed )
{
  vxBlockVolume< uint2 > * pVolume = bUncleansed? m_pUncleansedVolume : m_pCleansedVolume;
  vxBlockVolumeIterator< uint2 > iter( * pVolume  );
  iter.SetPos(  Vector3D< int >( x, y, z ) );
  return vxIntensityConverter::RawToModality( iter.GetVoxel(), pVolume->GetHeader() );
} // GetVoxel()



// $Log: vxSliceRendererData.C,v $
// Revision 1.22  2008/04/24 21:55:35  jmeade
// Merge from CON_3-0 on 080424
//
// Revision 1.21  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.20.2.2  2008/01/07 16:28:20  dongqing
// fix the bug that label volume was initialized without setting voxel units
//
// Revision 1.20.2.1  2007/04/03 20:43:06  jmeade
// comments.
//
// Revision 1.20  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.19  2006/09/26 19:35:01  dongqing
// comments
//
// Revision 1.18  2006/09/26 16:01:16  geconomos
// removed unused vxEnvironment
//
// Revision 1.17  2006/09/26 15:59:16  geconomos
// removed pimpl classes
//
// Revision 1.16  2006/09/25 13:59:05  geconomos
// exposed label volume
//
// Revision 1.15  2006/09/13 15:41:57  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.14  2006/06/22 14:02:20  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.13  2006/06/08 18:20:50  frank
// exposed the uncleansed volume
//
// Revision 1.12  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.11  2005/12/20 15:09:13  romy
// removed the hard coded log path
//
// Revision 1.10.2.1  2006/05/19 14:14:37  geconomos
// removed logger initialization
//
// Revision 1.10  2005/11/01 14:49:18  frank
// Issue #4503: Read new .cdv and .vol files instead of _hq files
//
// Revision 1.9  2005/07/01 16:26:52  vxconfig
// removed the .txt extension from the Log Level
//
// Revision 1.8  2005/07/01 12:32:02  vxconfig
// Added new Logger.
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
// Revision 1.1  2004/04/05 15:45:03  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSliceRendererData.C,v 1.22 2008/04/24 21:55:35 jmeade Exp $
// $Id: vxSliceRendererData.C,v 1.22 2008/04/24 21:55:35 jmeade Exp $
