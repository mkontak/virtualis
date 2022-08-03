// $Id: vxAnnotationComponent.h,v 1.6 2006/11/24 19:17:49 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  This class represents point based annotations and has been 
  reimplemented from scratch. However, I got started with the 
  older AnnotationsPoint class originating from Manju.
*/

#ifndef vxAnnotationComponent_h
#define vxAnnotationComponent_h


// includes
#include "vxAnnotation.h"


// class definition
class VX_VIEWER_DLL vxAnnotationComponent : public vxAnnotation
{
// functions
public: 

  /// constructor
  vxAnnotationComponent();
  
  /// destructor
  virtual ~vxAnnotationComponent() {};

	/// copy constructor
  vxAnnotationComponent( const vxAnnotationComponent & other );             

	///Clone
	virtual vxAnnotationComponent * Clone() const;

  
  /////////////////////////////////////////////////////////////////////
  // actual vxHandlesObj functions

  /// reset the annotation
  virtual void Reset();

  /////////////////////////////////////////////////////////////////////
  // actual vxAnnotation functions

  /// get the number of handles
  virtual uint2 GetNumEditableHandles() const { return 1; };

  /// get the number of handles required during creation of the object
  virtual uint2 GetNumCreationHandles() const { return 1; };

  // return the handle index where the text should appear
  virtual uint2 GetTextHandleIndex() { return 0; };

  /// is update call necessary while dragging
  virtual bool IsUpdateNecessaryWhileDragging() { return false; };

  /// method to determine whether to update while dragging
  virtual bool PerformUpdateWhileDragging() { return false; };

  /////////////////////////////////////////////////////////////////////
  // actual vxISerialize functions

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotationComponent"; };

  //////////////////////////////////////////////////////////////////////
  // actual additional functions

  /// get component index
  uint2 GetComponentIndex();

protected:

  /// update the annotation
  virtual void Update(vxEnvironment & environment);

private:

   
  /// assignment operator
  vxAnnotationComponent & operator=(vxAnnotationComponent & other); // should only be public if really implemented!

  /// common initialization code
  void CommonInit();
}; // vxAnnotationComponent


#endif //vxAnnotationComponent.h


// Revision History:
// $Log: vxAnnotationComponent.h,v $
// Revision 1.6  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.5  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.4  2006/08/07 20:26:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.3  2005/04/12 15:24:14  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/17 18:10:43  michael
// Added access function to get the underlying component index (from label volume)
// so that there is one access function to use. Related to issue #3972
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.11  2004/01/05 18:58:59  michael
// added "mailto:"
//
// Revision 1.10  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.9  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.8  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.7  2003/04/22 14:46:43  michael
// some more code reviews
//
// Revision 1.6  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.5  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.4  2003/02/13 17:25:16  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.3.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/02/07 01:03:53  michael
// added annotation for components
//
// Revision 1.2  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.1  2003/02/06 14:29:51  michael
// added inital version of component annotation
//
// Revision 1.8  2003/01/19 21:07:08  michael
// added box annotation
//
// Revision 1.7  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.6  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.5  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.4  2003/01/13 14:43:35  geconomos
// Did the following:
//   (1) Added mouse cursor handling.
//   (2) Set the active annotation in AddAnnotation().
//
// Revision 1.3  2003/01/10 14:20:58  michael
// fixed compilation
//
// Revision 1.2  2003/01/09 14:55:44  michael
// some add-ons to the new annotation structure
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotationComponent.h,v 1.6 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationComponent.h,v 1.6 2006/11/24 19:17:49 romy Exp $
