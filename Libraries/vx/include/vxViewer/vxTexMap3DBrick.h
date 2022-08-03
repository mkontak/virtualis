// $Id: vxTexMap3DBrick.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Michael Meissner (michael@viatronix.com)

#ifndef TexMap3dBrick_h
#define TexMap3dBrick_h


// includes
#include "vxTexMap2dTile.h"
#include "Vector.h"
#include "GLEnv.h"


// forward declarations
template <class T> class Box;
class vxServerTexMap3d;


/**
 * Class provides definition, data, and methods for 
 * a brick of volume data (density and label). This is
 * a dersived class from vxTexMap2dTile base class.
 */
class vxTexMap3dBrick : public vxTexMap2dTile
{
  /// Need for friend: ???
  friend vxServerTexMap3d;
  
// functions
public:
  
  /// default constructor
  vxTexMap3dBrick();

  /// default destructor
  virtual ~vxTexMap3dBrick() {};

  /// get the location of vertex (0-7) in real world coordinates
  Point<float4> GetVertex(const uint1 & index) const;

  /// get the edge points of a side.
  void GetEdgeVertices(uint1 uSideIndex, Point<float4> & point1, Point<float4> & point2) const;

  /// get the texture coordinate of vertex (0-7)
  Vector<float4> GetVertexTexCoord(const uint1 & index) const;

  /// get the texture edge points of a side.
  void GetTexEdgeVertices(uint1 uSideIndex, Point<float4> & point1, Point<float4> & point2) const;

  /// get the size in texture coordinate space
  Vector<float4> GetTexCoordSize() const;
  
  /// get the side segment indices
  std::pair<uint1,uint1> GetSideSegIndices(uint1 uIndex) const;

  /// clip brick with box, if true (intersect) member data ia adjusted
  bool Clip(Box<float4> & box);
  
  /// clip brick with box, if true (intersect) member data ia adjusted
  bool Clip(vxPlaneOrtho<float4> & planeOrtho);
  
  /// assignment operator
  vxTexMap3dBrick & operator=(const vxTexMap3dBrick & brick);

private: 

  /// copy constructor, should only be public if really implemented!
  vxTexMap3dBrick(const vxTexMap3dBrick & brick);  

// data
private:

  /// side segment indices
  static const std::pair<uint1,uint1> m_vSideSegIndices[12];
}; // vxTexMap3dBrick


#endif // vxTexMap3dBrick_h


// Revision History:
// $Log: vxTexMap3DBrick.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/05 13:30:25  michael
// cosmetics
//
// Revision 1.7  2003/06/04 17:58:36  jmeade
// code review.
//
// Revision 1.6  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.5  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.4  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.3  2003/01/02 15:28:25  ingmar
// float8 -> float4 (no need to render with more than 23bits precision)
//
// Revision 1.2  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.26  2002/05/29 19:39:18  jaddonisio
// *** empty log message ***
//
// Revision 1.25  2002/04/12 02:15:34  jmeade
// Includes.
//
// Revision 1.24  2002/04/02 21:50:55  geconomos
// Updated to use vxServerTexMap2d in lieu of ServerTexMap2d.
//
// Revision 1.23  2002/03/27 17:44:21  jaddonisio
// New function to get edge vertices.
//
// Revision 1.22  2002/03/06 22:39:08  ingmar
// added template parameter for PlaneOrtho argument to CLip()
//
// Revision 1.21  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 1.20  2002/02/19 17:38:22  jaddonisio
// Return copies.
//
// Revision 1.19  2002/02/06 14:53:24  jaddonisio
// Changed TexMap3dBrick class so that it is derived from the new base class vxTexMap2dTile.
//
// Revision 1.18  2002/02/05 14:43:59  michael
// cleaned code such that Joe can now work on the 2d texture map implementation
//
// Revision 1.17  2002/02/04 20:19:37  michael
// wrong friend declaration
//
// Revision 1.16  2002/02/04 18:20:10  michael
// re-structured 3D texmap classes to serve as template for new 2D texmap classes
//
// Revision 1.15  2002/02/04 16:36:45  michael
// Cleaned the code and sorted the Get() and Set() methods in the header files
//
// Revision 1.14  2002/01/31 16:53:43  michael
// naming and coding convention stuff
//
// Revision 1.13  2001/12/31 18:44:08  ingmar
// derived from Sec_TexMap to structure doxygen documentation
//
// Revision 1.12  2001/12/21 00:10:15  michael
// 3d texture mapping done, works all fine. :-)
//
// Revision 1.11  2001/12/20 21:26:32  michael
// new 3d texture mapping works now fine with crop box. One more to go :-)
//
// Revision 1.10  2001/12/20 16:30:57  michael
// new 3d texture mapping works now fine with tags inside
//
// Revision 1.9  2001/12/20 07:25:50  michael
// 3d texture mapping with dependent texture classification does now work
//
// Revision 1.8  2001/12/18 17:19:35  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 1.7  2001/12/17 23:57:33  michael
// 3D texture mapping now works on the NVIDIA but no tags yet :-(
//
// Revision 1.6  2001/12/13 23:32:27  michael
// bricking now works nicely. Bricks are always chosen such that they do not
// waste texture memory when being larger at the volume border. :-)
//
// Revision 1.5  2001/12/11 22:14:31  michael
// new 3dTexMapRenderer compiles but still enough to do ...
//
// Revision 1.4  2001/12/11 16:52:50  michael
// added comments ...
//
// Revision 1.3  2001/12/11 16:29:59  michael
// added the sorting methods and the rendering of the plane.
//
// Revision 1.2  2001/12/10 17:22:47  michael
// added texture id and actual texture generation
//
// Revision 1.1  2001/12/07 14:12:46  michael
// Added brick class for 3D texture mapping as well as a brick server that
// generates bricks out of a given volume.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxTexMap3DBrick.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxTexMap3DBrick.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
