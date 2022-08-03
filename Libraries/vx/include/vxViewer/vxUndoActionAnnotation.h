// $Id: vxUndoActionAnnotation.h,v 1.4 2006/09/27 17:52:01 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


/**
 * Implements undo for various annotation actions
 */


// pragmas
#pragma once


// includes
#include "vxUndoAction.h"
#include "vxShareableObject.h"
#include "vxAnnotations.h"


/**
 * Undo a recently added annotation
 */
class VX_VIEWER_DLL vxUndoActionAnnotationAdd : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionAnnotationAdd( vxShareableObject< vxAnnotations > * pAnnotations, const int4 iAddedIndex );

  /// perform the undo action
  virtual void ExecuteAction();
	
	//destructor
	virtual ~vxUndoActionAnnotationAdd();

	
// member data
private:

  /// the annotations object
  vxShareableObject< vxAnnotations > * m_pAnnotations;

  /// the previously added annotation index
  int4 m_iAddedIndex;

}; // class vxUndoActionAnnotationAdd


/**
 * Undo a recently annotation selection
 */
class VX_VIEWER_DLL vxUndoActionAnnotationSelect : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionAnnotationSelect( vxShareableObject< vxAnnotations > * pAnnotations, const int4 iSelectedIndex );

  /// perform the undo action
  virtual void ExecuteAction();

	//destructor
	virtual ~vxUndoActionAnnotationSelect();

// member data
private:

  /// the annotations object
  vxShareableObject< vxAnnotations > * m_pAnnotations;

  /// the previously selected annotation index
  int4 m_iSelectedIndex;

}; // class vxUndoActionAnnotationSelect


class VX_VIEWER_DLL vxUndoActionAnnotationRemove : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionAnnotationRemove( vxShareableObject< vxAnnotations > * pAnnotations, vxAnnotation * pRemovedAnnotation );

  /// perform the undo action
  virtual void ExecuteAction();

	///destructor
	~vxUndoActionAnnotationRemove();

// member data
private:

  /// the annotations object
  vxShareableObject< vxAnnotations > * m_pAnnotations;

  /// the previously removed annotation, if any
  vxAnnotation * m_pRemovedAnnotation;

}; // class vxUndoActionAnnotationRemove


/**
 * Undo a recently modified annotation
 */
class VX_VIEWER_DLL vxUndoActionAnnotationAdjust : public vxUndoAction
{
// member functions
public:

  /// constructor
  vxUndoActionAnnotationAdjust( vxShareableObject< vxAnnotations > * pAnnotations, vxAnnotation * pAdjustedAnnotation, const int4 iAdjustedIndex );

  /// perform the undo action
  virtual void ExecuteAction();

	///destructor
	~vxUndoActionAnnotationAdjust();

// member data
private:

  /// the annotations object
  vxShareableObject< vxAnnotations > * m_pAnnotations;

	//Annotation object
	vxAnnotation * m_pAdjustedAnnotation;
	
	//Adjusted annotations index
  int4 m_iAdjustedIndex;

}; // class vxUndoActionAnnotationSelect



// $Log: vxUndoActionAnnotation.h,v $
// Revision 1.4  2006/09/27 17:52:01  romy
// added virtual destructor
//
// Revision 1.3  2006/08/07 18:49:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.2  2006/02/21 21:21:35  frank
// added more undo for annotations
//
// Revision 1.1  2006/02/21 19:44:44  frank
// added undo for annotations
//
// Revision 1.1  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxUndoActionAnnotation.h,v 1.4 2006/09/27 17:52:01 romy Exp $
// $Id: vxUndoActionAnnotation.h,v 1.4 2006/09/27 17:52:01 romy Exp $
