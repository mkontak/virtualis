// $Id: vxContourList.h,v 1.4 2006/04/30 20:48:41 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxContourList_h
#define vxContourList_h


// Includes
#include "vxElement.h"
#include "Triple.h"
#include "Vector.h"
#include "vxBlockVolume.h"
#include "vxContour.h"


/**
 * Class to contain a list of lassos and process them in order
 * to create a new component.
 */
class VX_VIEWER_DLL vxContourList : public vxElement
{
// Typedefs
public:

  // iterator for the list of lassos.
  typedef std::list< vxContour * > LassoList;

// Enumerations
public:

  /// the events that occur during its lifetime
  enum EventsEnum
  {
    EVENT_ADDED       = 0x00000001,
    EVENT_MODIFIED    = 0x00000002,  
    EVENT_COMPLETED   = 0x00000004,  
  }; // enum EventsEnum

// Member Functions
public:
  
  /// Constructor
  vxContourList();
  
  /// Destructor
  virtual ~vxContourList();

  /// Get the list of lassos
  std::list< vxContour * > & GetLassoList() { return m_vContourList; }

  /// Clear the list of lassos
  void Clear();

  /// Retrieve the mask volume
  vxBlockVolume< bool > * GetMaskVolume() const { return m_pMaskVolume; }

  /// Create a mask bool volume indicating which voxels are inside the created lassos
  void Initialize( const Triple<int4> & volDim, const Triple<float4> & volUnits );
    
  /// Get orientation of plane
  const uint1 & GetOrientation() const { return m_uOrientation; }

  /// Set the orientation of the plane
  void SetOrientation( const uint1 & uOrientation ) { m_uOrientation = uOrientation; }

  /// Fill the mask volume depending on the lassos generated in the list
  void ProcessLassoList();

  /// get the adjustment contour
  vxContour & GetAdjustmentContour() { return m_adjustmentContour; }

private:

  /// Find out if lassos need to be created on any empty slices in between the list of lassos bound
  void SetMaskVolumeWithInterpolateContours();

  /// Check if all lassos have equal numebr of points, else add points to those who do not
  uint2 ComputePoints();

  /// Find min distance between two lassos, to create a third one
  void ComputeMinMatch( const uint2 & uIndex, const uint2 & uMaxPts );

  /// Get the sum distance of all matching points
  float4 GetMatchingDistance( const uint2 & uLassoIndex );

 ///Creates a interpolate contour based on the next & prev contours
  void GetInterpolateContour( const float4 fSliceNum, const uint2 & uPos,
                                const uint1 & uOrientation, const uint2 & uMaxPts , vxContour * pPrevRoi, vxContour * pNextRoi, vxContour * pInterpolateContour  );

  ///Sets the mask volume based on the contour
  void SetMaskVolume( vxContour * pContour );
    
// Data Members
private:

  /// Orientation of plane in which the list of lassos has been created
  uint1 m_uOrientation;

  /// List to store the collection of lassos
  std::list< vxContour * > m_vContourList;

  /// A single adjustment contour
  vxContour m_adjustmentContour;

  /// Mask volume
  vxBlockVolume< bool > * m_pMaskVolume;

  /// Flag to indicate if all the lassos have been updated to contain the same number of points
  bool m_bComputePtFlag;

}; // class vxContourList


#endif // vxContourList_h


// $Log: vxContourList.h,v $
// Revision 1.4  2006/04/30 20:48:41  romy
// bug fix in contour
//
// Revision 1.3  2006/03/13 13:52:10  frank
// added adjustment contours
//
// Revision 1.2  2004/03/12 19:53:06  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1  2004/03/12 18:01:54  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/21 18:30:49  frank
// code review
//
// Revision 1.6  2003/05/13 18:28:22  frank
// code review
//
// Revision 1.5  2003/05/13 17:40:49  frank
// Issue #3215: Contour broken in some slices
//
// Revision 1.4  2003/04/28 16:42:46  frank
// Issue #3141: Fixed contour if you draw clockwise, then counterclockwise
//
// Revision 1.3  2003/01/22 21:44:03  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.2  2002/11/19 22:14:29  jmeade
// Removed unnecessary pragmas at George's request.
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.4  2002/09/30 14:36:30  dmitry
// EVENT_COMPLETED added
//
// Revision 1.3  2002/07/11 21:09:10  manju
// Speed up.
//
// Revision 1.2  2002/07/10 19:24:59  manju
// Changed CreateMaskVolume to Initialize().
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.3  2002/06/27 12:08:16  manju
// Added comments.
//
// Revision 1.2  2002/06/26 17:24:27  manju
// Changed vector of lassos to list of lassos and modified the code to get
// it working,
//
// Revision 1.1  2002/06/19 14:23:37  manju
// Restricted Lasso class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxContourList.h,v 1.4 2006/04/30 20:48:41 romy Exp $
// $Id: vxContourList.h,v 1.4 2006/04/30 20:48:41 romy Exp $
