// $Id: vxDicomRCSutilities.h, v 1.0 2012/12/06 Dongqing Exp $
//
// Copyright © 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dongqing (dongqing@viatronix.com)


// includes
#include "stdafx.h"
#include "vxDicomRCSutilities.h"

// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.0 $"


/**
 *  default constructor
 *
 *  @param header the input vxBlockVolumeHeader
 */
vxDicomRCSutilities::vxDicomRCSutilities( const vxVolumeHeader & header )
 : m_header( header ),
   m_vfOrigin( Vector3D<float4>(0.0f, 0.0f, 0.0f) )
{
} // vxDicomRCSutilities


/**
 *  calculate the origin RCS if needed
 *
 *  @param vfOrigin the container for the coordinate of the origin
 *  @return true if successful
 */
bool vxDicomRCSutilities::CalulateOriginRCS()
{
  int4 iVersion = 0; //Get enhanced DICOM and non-orthogoanl plane supported
	m_header.GetVariableVolumeHeader().GetHdrVar( "version", iVersion );

	if( iVersion < 5 )
	{
    /// get the image orientation attributes ==========================================
    std::string sImageOrientation("");
    sImageOrientation = m_header.GetDicomFieldValue( "0020", "0037", sImageOrientation );

	  if( sImageOrientation == "" )
	  {
		  LogErr( "Could not extract Image Orientation from volume header", "vxDicomRCSutilities", "CalulateOriginRCS" );
		  return false;
	  } // if

    std::string sBreaker = "/\\,:";
    int4 i, n = sImageOrientation.size();
    int4 j[5], k = 0;

	  for( i=0; i<5; i++ ) j[i] = -1;

	  /// determine location of delimiters
    for( i=0; i<n; i++ )
    {
      if( sImageOrientation[i] == sBreaker[0] || sImageOrientation[i] == sBreaker[1] || 
          sImageOrientation[i] == sBreaker[2] || sImageOrientation[i] == sBreaker[3] )
      {
        j[k] = i;
        k++;
			  if( k > 4 ) i = n;
      } // if
    } // for i

    if( j[0] < 0 || j[1] < 0 || j[2] < 0 || j[3] < 0 || j[4] < 0 )
	  {
		  LogDbg( "Image orientation consine was corrupted", "vxDicomRCSutilities", "GetRCSOriginCoordinate" );
		  return false;
	  } // if

    /// get the orientation cosines
    std::string srx, sry, srz, scx, scy, scz;
    float4      frx, fry, frz, fcx, fcy, fcz;

	  for( i=0; i<j[0]; i++ )      srx.push_back( sImageOrientation[i] );
    ToType(srx, frx);

    for( i=j[0]+1; i<j[1]; i++ ) sry.push_back( sImageOrientation[i] );
    ToType(sry, fry);

    for( i=j[1]+1; i<j[2]; i++ ) srz.push_back( sImageOrientation[i] );
	  ToType(srz, frz);

    for( i=j[2]+1; i<j[3]; i++ ) scx.push_back( sImageOrientation[i] );
    ToType(scx, fcx);

	  for( i=j[3]+1; i<j[4]; i++ ) scy.push_back( sImageOrientation[i] );
    ToType(scy, fcy);

	  for( i=j[4]+1; i<n; i++ )    scz.push_back( sImageOrientation[i] );
	  ToType(scz, fcz);

	  if( (0.0f != abs(frx) && 1.0f != abs(frx)) || (0.0f != abs(fry) && 1.0f != abs(fry)) || (0.0f != abs(frz) && 1.0f != abs(frz)) ||
		    (0.0f != abs(fcx) && 1.0f != abs(fcx)) || (0.0f != abs(fcy) && 1.0f != abs(fcy)) || (0.0f != abs(fcz) && 1.0f != abs(fcz)) )
	  {
		  LogDbg( "Imaging plane was not orthorgonal.", "vxDicomRCSutilities", "GetRCSOriginCoordinate" );
		  return false;
	  } // check if it is orthorgonal imaging plane
	 
	  /// extract image position attribute ===========================================
	  std::string sImagePosition("");
    sImagePosition = m_header.GetDicomFieldValue( "0020", "0032", sImagePosition );

	  for( i=0; i<2; i++ ) j[i] = -1;

	  n = sImagePosition.size();
	  k = 0;

	  /// determine location of delimiters
    for( i=0; i<n; i++ )
    {
      if( sImagePosition[i] == sBreaker[0] || sImagePosition[i] == sBreaker[1] || 
          sImagePosition[i] == sBreaker[2] || sImagePosition[i] == sBreaker[3] )
      {
        j[k] = i;
        k++;
			  if( k > 1 ) i = n;
      } // if
    } // for i

    if( j[0] < 0 || j[1] < 0 )
	  {
		  LogDbg( "Image position was corrupted", "vxDicomRCSutilities", "GetRCSOriginCoordinate" );
		  return false;
	  } // if

	  /// extract reference position coordinates
	  float4 fSx, fSy, fSz;

	  srx = "";
	  for( i=0; i<j[0]; i++ )      srx.push_back( sImagePosition[i] );
    ToType(srx, fSx);

	  srx = "";
    for( i=j[0]+1; i<j[1]; i++ ) srx.push_back( sImagePosition[i] );
    ToType(srx, fSy);

	  srx = "";
    for( i=j[1]+1; i<n; i++ )    srx.push_back( sImagePosition[i] );
	  ToType(srx, fSz);

	  /// extract image pixel spacing attribute ===================================================
	  std::string sImagePixelSpacing("");
    sImagePixelSpacing = m_header.GetDicomFieldValue( "0028", "0030", sImagePixelSpacing );

	  j[0] = -1;
	  n = sImagePixelSpacing.size();

	  /// determine location of delimiters
    for( i=0; i<n; i++ )
    {
      if( sImagePixelSpacing[i] == sBreaker[0] || sImagePixelSpacing[i] == sBreaker[1] || 
          sImagePixelSpacing[i] == sBreaker[2] || sImagePixelSpacing[i] == sBreaker[3] )
      {
        j[0] = i;
			  i = n;
      } // if
    } // for i

    if( j[0] < 0 )
	  {
		  LogDbg( "Image pixel spacing was corrupted", "vxDicomRCSutilities", "GetRCSOriginCoordinate" );
		  return false;
	  } // if

	  float4 fColumnUnit(0.0f), fRowUnit(0.0f);
	  srx = "";
	  for( i=0; i<j[0]; i++ )      srx.push_back( sImagePixelSpacing[i] );
    ToType( srx, fRowUnit );

	  srx = "";
	  for( i=j[0]+1; i<n; i++ )    srx.push_back( sImagePixelSpacing[i] );
    ToType( srx, fColumnUnit );

	  /// extract image dimension attribute =============================================
	  int4 iHeight(0), iWidth(0);
	  srx = "";
    srx = m_header.GetDicomFieldValue( "0028", "0010", srx );
	  ToType( srx, iHeight );

	  srx = "";
    srx = m_header.GetDicomFieldValue( "0028", "0011", srx );
	  ToType( srx, iWidth );

	  /// =========================================================================
	  /// calculation of the dicom patient RCS origin for various imaging plane orientations
    Vector3D< float4 > vCorners[3]; /// The image slice corner points.
	  /// (0, 0), (0, iHeight-1), (iWidth-1, 0), (iWidth-1, iHeight-1)

	  fcx = fcx * fRowUnit;
	  fcy = fcy * fRowUnit;
	  fcz = fcz * fRowUnit;
	  frx = frx * fColumnUnit;
	  fry = fry * fColumnUnit;
	  frz = frz * fColumnUnit;

	  iWidth  = iWidth-1;
	  iHeight = iHeight-1;

	  /// The following transformation matrix was defined in the DICOM standard
	  m_vfOrigin = Vector3D< float4 >( fSx, fSy, fSz );
	  vCorners[0].m_x = fcx * float4(iHeight) + fSx;     
	  vCorners[0].m_y = fcy * float4(iHeight) + fSy;
	  vCorners[0].m_z = fcz * float4(iHeight) + fSz;
	  vCorners[1].m_x = frx * float4(iWidth) + fSx;
	  vCorners[1].m_y = fry * float4(iWidth) + fSy;
	  vCorners[1].m_z = frz * float4(iWidth) + fSz;
	  vCorners[2].m_x = frx * float4(iWidth) + fcx * float4(iHeight) + fSx;
	  vCorners[2].m_y = fry * float4(iWidth) + fcy * float4(iHeight) + fSy;
	  vCorners[2].m_z = frz * float4(iWidth) + fcz * float4(iHeight) + fSz;

	  /// determine the origin
	  for( i=0; i<3; i++ )
	  {
		  if( m_vfOrigin.m_x >= vCorners[i].m_x && m_vfOrigin.m_y >= vCorners[i].m_y &&
          m_vfOrigin.m_z >= vCorners[i].m_z )
		  {
			  m_vfOrigin = vCorners[i];
		  } // if
	  } // for i

	} // if
	else
	{
		m_header.GetVariableVolumeHeader().GetHdrVar("volumeOrigin", m_vfOrigin);
	} // else

	return true;
} // GetRCSOriginCoordinate


/**
  *  convert volume coordinate to the dicom RCS coordinate
	*
	*  @param viVolCS   the input volume coordiantes
	*/
Vector3D< float4 > vxDicomRCSutilities::ConvertVolCoordinate2DcmRCS( const Vector3D<int4> & viVolCS )
{
  Vector3D<float4> vfWorldCoordinate(0.0f, 0.0f, 0.0f);

	vfWorldCoordinate.m_x = m_vfOrigin.m_x + m_header.GetVoxelUnits().m_x * float4(viVolCS.m_x);
	vfWorldCoordinate.m_y = m_vfOrigin.m_y + m_header.GetVoxelUnits().m_y * float4(viVolCS.m_y);
	vfWorldCoordinate.m_z = m_vfOrigin.m_z + m_header.GetVoxelUnits().m_z * float4(viVolCS.m_z);

	return vfWorldCoordinate;

} // ConvertVolCoordinate2DcmRCS()

#undef FILE_REVISION

