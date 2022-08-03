// $Id: vxISerializeFactoryXP.C,v 1.3 2005/03/08 19:09:13 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * This class implements the vxISerializeFactory for the Explorer module
 */


// includes
#include "stdafx.h"
#include "vxISerializeFactoryXP.h"
#include <list>
#include <algorithm>
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationAngle.h"
#include "vxAnnotationBox.h"
#include "vxAnnotationEllipse.h"
#include "vxAnnotationComponent.h"
#include "vxVisualizationPreset.h"


/**
 * Add a factory to the list
 * @param name
 * @return serializable object pointer
 */
vxISerialize * vxISerializeFactoryXP::Create(const std::string & sName) const
//*******************************************************************
{
  if (sName == "vxAnnotationPoint")
    return new vxAnnotationPoint;
  else if (sName == "vxAnnotationArrow")
    return new vxAnnotationArrow;
  else if (sName == "vxAnnotationDistance")
    return new vxAnnotationDistance;
  else if (sName == "vxAnnotationAngle")
    return new vxAnnotationAngle;
  else if (sName == "vxAnnotationBox")
    return new vxAnnotationBox;
  else if (sName == "vxAnnotationEllipse")
    return new vxAnnotationEllipse;
  else if (sName == "vxAnnotationComponent")
    return new vxAnnotationComponent;
  else
    return NULL;
} // Create()


// $Log: vxISerializeFactoryXP.C,v $
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.2  2004/05/17 18:53:04  frank
// moved visualization preset serialization to its own class
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2003/02/21 19:09:52  michael
// added initial version of the vxISerializeFactory classes
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxISerializeFactoryXP.C,v 1.3 2005/03/08 19:09:13 frank Exp $
// $Id: vxISerializeFactoryXP.C,v 1.3 2005/03/08 19:09:13 frank Exp $
