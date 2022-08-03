// $Id: vxManipulatorSculpt.h,v 1.5 2006/04/24 14:14:55 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxManipulatorSculpt_h
#define vxManipulatorSculpt_h


// Includes
#include "vxManipulator.h"


// Forward declarations
class vxContour;
template <class T> class Image;
template <class T> class vxBlockVolumeIterator;
template <class T> class Box;
 

/**
 * This class inherits from Manipulator and provides the
 * interface to a lasso HandlesSet for the purpose of sculpting
 * away 3D data based on a 2D screen projection.  You first draw
 * a circular region, then you click again to choose the part
 * you want to cut.
 */
class VX_VIEWER_DLL vxManipulatorSculpt : public vxManipulator
{
// Member Functions
public:
  
  /// default constructor
  vxManipulatorSculpt( vxEnvironment & environment );
  
  /// default destructor
  ~vxManipulatorSculpt();

  /// This event handler can be reimplemented in a subclass to receive mouse move events for the viewer.     
  virtual bool OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.     
  virtual bool OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );

  /// This event handler can be reimplemented in a subclass to receive mouse up events for the viewer.     
  virtual bool OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState );
  
  /// This event handler can be reimplemented in a subclass to receive keyboard events for the viewer.     
  virtual bool OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState );

  /// Perform the actual sculpting.
  void SculptVolume( Image<bool> & maskImage );

  /// called when the mouse cursor is to be set
  virtual bool OnSetCursor();

  /// reset everything to the initial state
  void Reset();

private:
  
  /// Copy constructor
  vxManipulatorSculpt( vxManipulatorSculpt & ); // should only be public if really implemented!
  
  /// Assignment operator
  vxManipulatorSculpt & operator =( vxManipulatorSculpt & );           // should only be public if really implemented!
  
  /// Close the contour on button up event for lasso.
  void CutLasso( vxContour * pLasso, Point2D<int2> position );

  /// Clear out the lasso
  void ClearLasso();

  /// Process a subcube of the dataset deciding which voxels belong inside and outside.
  void ProcessCube( vxBlockVolumeIterator<bool> & iterVolume, 
                    vxBlockVolumeIterator<uint2> & iterLabel,
                    bool * pbLocked, 
                    const uint4 uStartZ, const uint4 uStartY, const uint4 uStartX,
                    const uint4 uStopZ , const uint4 uStopY , const uint4 uStopX,
                    const float4 mCombinedMatrix[16],
                    const int4 vViewport[4],
                    Triple<float4> volUnits,
                    Image<bool> & maskImage, bool bKeepInside,
                    Box<uint4> & visibleBounds,
                    int4 & iRemoveAllCount, int4 & iLeaveAllCount, int4 & iSlowCount,
                    bool & bAnyRemoved );

  /// Multiply two OpenGL projection matrices very quickly.
  void MatrixMul( float4 * pfProduct, const GLdouble * pMatrixA, const GLdouble * pMatrixB );

  /// Project a point in 3D to the screen given the combined (modelview and projection)
  void ProjectToScreen( const float4 fObjX, const float4 fObjY, const float4 fObjZ, const float4 combined[16],
                        const int4 viewport[4], int4 &iWinX, int4 &iWinY);

// Data Members
protected:

  /// Whether to keep the inside or the outside of the sculpt.
  bool m_bKeepInside;

  /// If we are in the state that we query the user to select a portion of the image to cut.
  bool m_bQueryUser;

  /// Whether to sculpt through the entire volume, not just the visible portion
  bool m_bFullDepth;

  /// The mask image that holds the inside/outside state of each screen pixel between the end of drawing and the cut region selection.
  Image<bool> m_maskImage;

  /// Whether to print detailed statistics in the debug log
  static const bool m_bShowTimingsInLog;

}; // class ManipulatorSculpt


#endif // ManipulatorSculpt_h


// $Log: vxManipulatorSculpt.h,v $
// Revision 1.5  2006/04/24 14:14:55  romy
// removed the 'locked voxel mask' from sculpt operation
//
// Revision 1.4  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/07/25 20:10:41  frank
// Issue #4304: Fixed problem when sculpting or contouring locked voxels
//
// Revision 1.2  2004/03/12 19:54:05  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.28  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.27  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.26  2004/01/14 16:56:05  frank
// cleared data when the manipulator is destructed
//
// Revision 1.25  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.24  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.23  2003/04/07 18:08:03  frank
// Refactored code to improve reusability and maintainability.
//
// Revision 1.22  2003/03/24 14:12:37  frank
// Fixed creation of multiple aborted lassos
//
// Revision 1.21  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.20  2003/02/05 23:08:28  frank
// Changed sculpting so you choose inside/outside after drawing.
//
// Revision 1.19.2.1  2003/02/12 22:51:02  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.19  2002/12/12 18:35:22  ingmar
// made sure that locked componentsd are handles in block increments (not only voxel increments) whenever possible
//
// Revision 1.18  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.17  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.16  2002/10/04 14:06:25  frank
// Added full depth sculpting on ctrl key.
//
// Revision 1.15  2002/10/04 12:44:42  geconomos
// Exported sculpting classes.
//
// Revision 1.14  2002/08/20 22:44:43  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.13  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.12  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.11  2002/05/17 20:42:36  frank
// Didn't sculpt into locked voxels.
//
// Revision 1.10  2002/05/10 21:02:04  frank
// Changed the name of the sculpting component.
//
// Revision 1.9  2002/05/02 22:32:45  michael
// removed unnecessary forward declarations
//
// Revision 1.8  2002/05/02 19:10:16  frank
// Changed sculpting to add a new component.
//
// Revision 1.7  2002/04/26 15:09:59  michael
// added delta volume which should be use for texture update ...
// ... once component index for sculpting is correctly included
//
// Revision 1.6  2002/04/26 02:38:05  frank
// Adding comments.
//
// Revision 1.5  2002/04/25 17:55:29  dmitry
// Cursors added.
//
// Revision 1.4  2002/04/20 18:00:51  michael
// Replaced include by forward declaration
//
// Revision 1.3  2002/04/19 21:35:38  frank
// Added outside sculpting.
//
// Revision 1.2  2002/04/19 10:41:32  frank
// Basic sculpting works on 2D.
//
// Revision 1.1  2002/04/18 18:42:03  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorSculpt.h,v 1.5 2006/04/24 14:14:55 romy Exp $
// $Id: vxManipulatorSculpt.h,v 1.5 2006/04/24 14:14:55 romy Exp $
