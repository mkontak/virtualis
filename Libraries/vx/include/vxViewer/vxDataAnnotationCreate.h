// $Id: vxDataAnnotationCreate.h,v 1.2 2006/06/28 20:54:08 frank Exp $
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

#ifndef vxDataAnnotationCreate_h
#define vxDataAnnotationCreate_h


// includes
#include "vxElement.h"


// forward declarations
class vxEnvironment;
class vxAnnotation;


// class declaration 
class VX_VIEWER_DLL vxDataAnnotationCreate :
  public vxElement
{
// enums
public:

  enum CreationModeEnum
  {
    WHILE_MOUSE_DOWN     = 0, // continuous point insertion from OnMouseDown until OnMouseUp
    ON_MOUSE_DOWN        = 1, // point insertion OnMouseDown and finalizing OnMouseUp
    ON_MOUSE_DOWN_AND_UP = 2, // point insertion OnMouseDown and OnMouseUp
  }; // CreationModeEnum

// functions
public:

  /// default constructor
  vxDataAnnotationCreate();

  /// destructor
  ~vxDataAnnotationCreate() {}

private:

  /// copy constructor
  vxDataAnnotationCreate(const vxDataAnnotationCreate & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxDataAnnotationCreate & operator=(const vxDataAnnotationCreate & other); // should only be public if really implemented!

// data
public:

  /// fixed size rendering
  CreationModeEnum m_eCreationMode;

  /// index of handle being dragged
  vxAnnotation * m_pAnnotation;

}; // vxDataAnnotationCreate


#endif


// Revision History:
// $Log: vxDataAnnotationCreate.h,v $
// Revision 1.2  2006/06/28 20:54:08  frank
// Issue #4842: fixed bad dynamic cast in annotation create
//
// Revision 1.1  2005/05/23 20:24:31  michael
// Introduced concept of data objects to start with vxData....
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataAnnotationCreate.h,v 1.2 2006/06/28 20:54:08 frank Exp $
// $Id: vxDataAnnotationCreate.h,v 1.2 2006/06/28 20:54:08 frank Exp $
