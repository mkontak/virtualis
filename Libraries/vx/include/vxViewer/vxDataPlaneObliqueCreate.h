// $Id: vxDataPlaneObliqueCreate.h,v 1.2 2006/07/10 13:53:40 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/*
  Encapsulate the data needed to create an oblique plane
*/

#ifndef vxDataPlaneObliqueCreate_h
#define vxDataPlaneObliqueCreate_h


// includes
#include "Point3D.h"
#include "vxCamera.h"
#include "vxPlaneOblique.h"
#include "vxHandle.h"
#include "vxElement.h"


// forward declarations
template <class T> class vxBlockVolume;
template <class T> class vxShareableObject;
class vxViewerType;
class vxEnvironment;


// class declaration 
class VX_VIEWER_DLL vxDataPlaneObliqueCreate :
	public vxElement
{
// functions
public:

  /// default constructor
  vxDataPlaneObliqueCreate();

  /// destructor
  ~vxDataPlaneObliqueCreate() {}

private:

  /// copy constructor
  vxDataPlaneObliqueCreate(const vxDataPlaneObliqueCreate & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxDataPlaneObliqueCreate & operator=(const vxDataPlaneObliqueCreate & other); // should only be public if really implemented!

// data
public:

  /// fixed size rendering
  bool m_bCreating;

  /// number of points set
  uint1 m_uNumPointsSet;

  /// original camera
  vxCamera m_cameraOriginal;

  /// original plane oblique
  vxPlaneOblique<float4> m_planeObliqueOriginal;

  /// environment of the plane to be manipulated
  vxEnvironment * m_pEnvironment;

  /// environment in which the plane is manipulated
  vxEnvironment * m_pManipulationEnvironment;

  /// renderer for handles
  vxHandle m_vHandles[2];

  /// index of handle being dragged
  int4 m_iDraggingIndex;
}; // vxDataPlaneObliqueCreate


#endif


// Revision History:
// $Log: vxDataPlaneObliqueCreate.h,v $
// Revision 1.2  2006/07/10 13:53:40  dongqing
// add public claim to the class definition
//
// Revision 1.1  2005/05/23 20:24:31  michael
// Introduced concept of data objects to start with vxData....
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/01/23 03:59:34  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.5  2003/09/23 22:08:56  michael
// added a dragging index to know which handle is being dragged
//
// Revision 1.4  2003/09/22 21:12:34  michael
// changed points to two handles that can be rendered
//
// Revision 1.3  2003/08/20 14:19:28  michael
// added pointer to environment we are manipulating it in
//
// Revision 1.2  2003/08/13 19:24:54  michael
// plane oblique creating, added environment to dynamic environment data
//
// Revision 1.1  2003/08/13 12:46:24  michael
// added initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataPlaneObliqueCreate.h,v 1.2 2006/07/10 13:53:40 dongqing Exp $
// $Id: vxDataPlaneObliqueCreate.h,v 1.2 2006/07/10 13:53:40 dongqing Exp $
