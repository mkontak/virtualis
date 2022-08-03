// $Id: vxHandlesObj.h,v 1.7 2006/08/07 18:47:48 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Base class of an object that has handle(s) that can be grabbed and 
  set by a manipulator. This class was refactured out of old code.
*/

#ifndef vxHandlesObj_h
#define vxHandlesObj_h


// includes
#include "vxISerialize.h"
#include "vxHandle.h"


// class definition
class VX_VIEWER_DLL vxHandlesObj : public vxISerialize
{
// functions
public:
  
  /// constructor
  vxHandlesObj();
  
  /// destructor
  virtual ~vxHandlesObj();

  /// reset
  virtual void Reset();

  /// add another handle
  virtual void AddHandle(const vxHandle & handle);

  /// add another handle
  virtual void RemoveHandle(const uint2 uIndex);

  /// get the indexed handle
  virtual vxHandle GetHandle(const uint2 uHandleIndex) const;

  /// set the indexed handle
  virtual void SetHandle(const uint2 uHandleIndex, const vxHandle & handle);

  /// get number of handles
  virtual uint4 GetNumHandles() const;

  /// get environment
  vxEnvironment * GetEnvironment() const { return m_pEnvironment; }

  /// set environment
  void SetEnvironment(vxEnvironment * pEnvironment) const { m_pEnvironment = pEnvironment; };

  /// prepare any computations before rendering (once per frame)
  virtual void PreRender(vxEnvironment & environment);

  /// render the annotation
  virtual void Render(vxEnvironment & environment);

  /////////////////////////////////////////////////////////////////////
  // actual vxISerialize functions

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element);

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxHandlesObj"; };

  /// copy constructor
  vxHandlesObj( const vxHandlesObj & other );             
  
  /// assignment operator
  vxHandlesObj & operator=( const vxHandlesObj & other ); 
 
// data
protected:

  /// vector of handle points
  std::vector<vxHandle> m_vHandles;

  /// environment
  mutable vxEnvironment * m_pEnvironment;
}; // vxHandlesObj

 
#endif // vxHandlesObj_h


// Revision History:
// $Log: vxHandlesObj.h,v $
// Revision 1.7  2006/08/07 18:47:48  romy
// added CCtrs
//
// Revision 1.6  2006/08/03 20:31:08  romy
// access modifier modified for derived classes
//
// Revision 1.5  2005/04/12 15:24:14  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.4  2004/07/12 18:02:57  frank
// misplaced includes
//
// Revision 1.3  2004/04/06 12:08:53  frank
// removed unused includes
//
// Revision 1.2  2004/03/11 17:39:22  frank
// named the parameters consistently
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2004/01/23 03:51:32  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxHandlesObj.h,v 1.7 2006/08/07 18:47:48 romy Exp $
// $Id: vxHandlesObj.h,v 1.7 2006/08/07 18:47:48 romy Exp $
