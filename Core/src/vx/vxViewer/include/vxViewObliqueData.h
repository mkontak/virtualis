// $Id: vxViewObliqueData.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxViewObliqueData_h
#define vxViewObliqueData_h


// Includes
#include "vxElement.h"
#include "vxCamera.h"


// Forward declarations
class Quadrilateral;


/**
 * A class to maintain an oblique plane
 */
class VX_VIEWER_DLL vxViewObliqueData : public vxElement
{
// enumerations
public:

  /// the modified event types
  enum EventsEnum
  {
    EVENT_MODIFIED = 0x00000001
  }; // enum EventsEnum

// member functions
public:

  /// Constructor
  vxViewObliqueData();

  /// Destructor
  virtual ~vxViewObliqueData();

  /// Get Field of view in world coordinates (mm)
  float4 GetFieldOfViewMM() const { return m_fFieldOfViewMM; }

  /// Set field of view in world coordinates (mm)
  void SetFieldOfViewMM( const float4 & fFieldOfViewMM ) { m_fFieldOfViewMM = fFieldOfViewMM; }

  /// Set the coordinate system through a camera object
  void SetCoordinateSystem( vxCamera & camera ) { m_camera = camera; }

  /// Get the coordinate system as a camera object
  vxCamera GetCoordinateSystem() const { return m_camera; }

  /// Get the four corners of the oblique plane in world coordinates.
  Quadrilateral GetQuadrilateralMM() const;

// data members
private:

  /// The camera conveninetly contains a complete coordinate system (in world coordinates)
  vxCamera m_camera;

  /// The orthographic field of view of the plane in mm (world coordinates)
  float4 m_fFieldOfViewMM;

}; // class vxViewObliqueData


#endif // vxViewObliqueData_h


// $Log: vxViewObliqueData.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/21 18:43:26  frank
// code review
//
// Revision 1.2  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.1  2002/11/13 22:08:31  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.1  2002/11/13 20:11:05  frank
// enabled oblique view renderer and manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxViewObliqueData.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxViewObliqueData.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
