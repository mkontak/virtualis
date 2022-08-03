// $Id: VolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "StdAfx.h"
#include "Volume.h"
#include "Global.h"


// namespaces
namespace VolumeUtilities
{

  /// reformat a volume from prone to supine orientation
  VX_BLOCK_VOLUME_DLL void ReformatVolumeProneToSupine( LinVolUC & volume );

  /// reformat a volume from prone to supine orientation
  VX_BLOCK_VOLUME_DLL void ReformatVolumeProneToSupineB( vxBlockVolume<uint1> & volume );


} // namespace VolumeUtilities


// $Log: VolumeUtilities.h,v $
// Revision 1.1  2006/09/13 15:11:41  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
// Added utilities to flip prone volumes to supine
//
// $Id: VolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/VolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
