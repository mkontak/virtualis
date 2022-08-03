// $Id: vxDicomRCSutilities.h, v 1.0 2012/12/06 Dongqing Exp $
//
// Copyright © 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dongqing (dongqing@viatronix.com)


/**
 *  This class provides tools to convert volume coordinate into patient reference coordinate.
 *  If the volume header version is 5.0 or later, the volume origin coordinate should be in 
 *  the input volume header. Otherwise, it needs to calculate the volume origin coordinate 
 *  first.
 */


#ifndef vxDicomRCSutilities_h
#define vxDicomRCSutilities_h


// includes
#include "vxVolumeHeader.h"


// class definition
class VX_BLOCK_VOLUME_DLL vxDicomRCSutilities
{
public:

  /// default constructor
  vxDicomRCSutilities( const vxVolumeHeader & header );

	/// calculate the origin RCS if needed
	bool CalulateOriginRCS();

	/// convert volume coordinate to the dicom RCS coordinate
	Vector3D< float4 > ConvertVolCoordinate2DcmRCS( const Vector3D<int4> & viVolCS );


private:

  /// the header
  vxVolumeHeader m_header;

	/// the volume origin coordinate
	Vector3D< float4 > m_vfOrigin;


}; // vxDicomRCSutilities

#endif // vxDicomRCSutilities_h