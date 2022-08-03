// $Id: vxBlockVolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
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
#include <string>
#include "vxBlockVolumeLibrary.h"
#include "Global.h"
#include "vxBlockVolume.h"


// namespaces
namespace vxBlockVolumeUtilities
{

  /// reformat a block volume from a random body position to supine orientation
  template <class T> VX_BLOCK_VOLUME_DLL void ReformatVolumeToSupine( vxBlockVolume<T> & vol, const std::string & sBodyPosition );

} // namespace vxBlockVolumeUtilities


// $Log: vxBlockVolumeUtilities.h,v $
// Revision 1.1  2006/09/13 15:11:41  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
// Added utilities to flip prone volumes to supine
//
// $Id: vxBlockVolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxBlockVolumeUtilities.h,v 1.1 2006/09/13 15:11:41 frank Exp $
