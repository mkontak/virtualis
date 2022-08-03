// $Id: vxHandle.h,v 1.2 2004/07/12 18:02:57 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Generic class representing a single handle. 
  This class evolved from older code after refacturing it.
*/

#ifndef vxHandle_h
#define vxHandle_h


// includes
#include "Point.h"
#include "PixelRGBA.h"
#include "GLFontGeorge.h"


// forward declarations
class vxEnvironment;
class vxViewerType;
class vxSerializer;


// class definition
class VX_VIEWER_DLL vxHandle
{
// functions
public:
  
  /// default constructor
  vxHandle();
  
  /// copy constructor
  vxHandle(const vxHandle & other);

  /// destructor
  virtual ~vxHandle();

  /// assignment operator
  const vxHandle & operator=(const vxHandle & other);

  /// reset the annotation
  virtual void Reset();

  /// get position
  Point3Df GetPosition() const { return m_position; };

  /// set position
  void SetPosition(const Point3Df & position) { m_position = position; };

  /// is it selected?
  bool IsSelected() const { return m_bSelected; };

  /// set selected
  void SetSelected(bool bSelected) { m_bSelected = bSelected; };

  /// get handle color
  PixelRGBA<uint1> GetColorHandle() const { return m_colorHandle; };

  /// set handle color
  void SetColorHandle(const PixelRGBA<uint1> & color) { m_colorHandle = color; };

  /// get selected handle color
  PixelRGBA<uint1> GetColorHandleSelected() const { return m_colorHandleSelected; };

  /// set selected handle color
  void SetColorHandleSelected(const PixelRGBA<uint1> & color) { m_colorHandleSelected = color; };

  /// get environment
  vxEnvironment * GetEnvironment() const { return m_pEnvironment; }

  /// set environment
  void SetEnvironment(vxEnvironment * pEnvironment) const { m_pEnvironment = pEnvironment; };

  /// method to prepare any computations before rendering (once per frame)
  virtual void PreRender(vxEnvironment & environment);

  /// method to render the annotation
  virtual void Render(vxEnvironment & environment);

  /// method to render the annotation text
  virtual void RenderText(vxEnvironment & environment, GLint * piViewport,
                          GLdouble * pfModelviewMatrix, GLdouble * pfProjectionMatrix) const {};

  /// returns the size of the handle in world space
  float4 GetHandleWorldSize() const { return m_fHandleWorldSize; };

  /// returns the size of the handle in screen space
  float4 GetHandleScreenSize() const { return m_fHandleScreenSize; };

private:

  /// common initialization code
  void CommonInit();

  /// returns the size of the handle in world space
  void SetHandleWorldSize(float4 fSize) { m_fHandleWorldSize = fSize; };

  /// returns the size of the handle in world space
  void SetHandleScreenSize(float4 fSize) { m_fHandleScreenSize = fSize; };

// data
private:

  /// position of handle
  Point3Df m_position;

  /// is selected?
  bool m_bSelected;

  /// associated handle color
  PixelRGBA<uint1> m_colorHandle;

  /// associated handle color if selected
  PixelRGBA<uint1> m_colorHandleSelected;

  /// size of the handle in world space
  float4 m_fHandleWorldSize;

  /// size of the handle in screen space
  static float4 m_fHandleScreenSize;

  /// environment
  mutable vxEnvironment * m_pEnvironment;

}; // vxHandle


#endif // vxHandle_h


// Revision History:
// $Log: vxHandle.h,v $
// Revision 1.2  2004/07/12 18:02:57  frank
// misplaced includes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2004/01/23 03:51:32  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxHandle.h,v 1.2 2004/07/12 18:02:57 frank Exp $
// $Id: vxHandle.h,v 1.2 2004/07/12 18:02:57 frank Exp $
