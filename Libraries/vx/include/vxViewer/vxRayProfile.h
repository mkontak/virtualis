// $Id: vxRayProfile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRayProfile_h
#define vxRayProfile_h


// includes
#include "vxElement.h"


// class definition
class vxRayProfile : public vxElement
{
// functions
public:

  /// default constructor
  vxRayProfile() {};

  /// get ray opacity profile
  std::vector<float4> & GetRayOpacityProfile() { return m_vRayOpacityProfile; }

// data
private:

  /// opacity profile
  std::vector<float4> m_vRayOpacityProfile;
}; // class vxRayProfile


#endif // #ifndef vxRayProfile_h


// Revision History:
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRayProfile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxRayProfile.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
