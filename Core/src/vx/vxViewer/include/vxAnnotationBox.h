// $Id: vxAnnotationBox.h,v 1.6 2006/11/24 19:17:49 romy Exp $
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

#ifndef vxAnnotationBox_h
#define vxAnnotationBox_h


// includes
#include "vxAnnotation.h"


// class definition
class VX_VIEWER_DLL vxAnnotationBox : public vxAnnotation
{
// functions
public: 

  /// constructor
  vxAnnotationBox();
  
  /// destructor
  virtual ~vxAnnotationBox() {};

  /// copy constructor
  vxAnnotationBox( const vxAnnotationBox & other );             

	///Clone method
	virtual vxAnnotationBox * Clone() const;

  
  /////////////////////////////////////////////////////////////////////
  // actual vxHandlesObj functions

  /// reset the annotation
  virtual void Reset();

  /////////////////////////////////////////////////////////////////////
  // actual vxAnnotation functions

  /// get the number of handles
  virtual uint2 GetNumEditableHandles() const { return 9; };

  /// get the number of handles required during creation of the object
  virtual uint2 GetNumCreationHandles() const { return 2; };

  /// set the location of a given handle.
  virtual void SetHandle(const uint2 iHandleIndex, const vxHandle & handle);

  // return the handle index where the text should appear
  virtual uint2 GetTextHandleIndex() { return 2; };

  /// method to render the annotation
  virtual void Render(vxEnvironment & environment);

  /// morph from creating the object with few handles to full annotation with possibly more handles
  virtual void Morph(vxEnvironment & environment);

  /////////////////////////////////////////////////////////////////////
  // actual vxISerialize functions

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotationBox"; };

protected:

  /// update the annotation
  virtual void Update(vxEnvironment & environment);

private:

   
  /// assignment operator
  vxAnnotationBox & operator=(vxAnnotationBox & other); // should only be public if really implemented!

  /// common initialization code
  void CommonInit();

  /// is box collapsed
  bool IsCollapsed(const uint2 uHandleIndex, const Point<float4> & position);

  /// draw the arc of the angle
  void DrawBox(vxEnvironment & environment) const;

}; // vxAnnotationBox


#endif //vxAnnotationBox.h


// Revision History:
// $Log: vxAnnotationBox.h,v $
// Revision 1.6  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.5  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.4  2006/08/08 13:55:15  romy
// removed old comments which was for a private CCtr
//
// Revision 1.3  2006/08/07 20:26:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.2  2005/04/12 15:24:14  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.15  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.14.2.1  2004/02/27 21:32:47  michael
// fixed ellipse annotation creation resulting in an empty structure that would cause
// an exception when normalizing its diameter vectors. Two initial points are now
// guaranteed to be different
//
// Revision 1.14  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.13  2004/01/21 16:32:45  michael
// allows rotation now (corner handles) and scaling on edge handles
//
// Revision 1.12  2004/01/05 18:58:59  michael
// added "mailto:"
//
// Revision 1.11  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.10  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.9  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.8  2003/04/22 14:46:43  michael
// some more code reviews
//
// Revision 1.7  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.6  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.5  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.4  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.3  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.2  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.1  2003/01/19 22:26:24  michael
// Added new annotation
//
// Revision 1.4  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.3  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.2  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.1  2003/01/14 23:25:51  michael
// added initial version of angle annotation
//
// Revision 1.3  2003/01/10 14:33:15  michael
// took out redundant code
//
// Revision 1.2  2003/01/10 14:20:58  michael
// fixed compilation
//
// Revision 1.1  2003/01/10 13:39:40  michael
// added initial version of distance annotation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotationBox.h,v 1.6 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationBox.h,v 1.6 2006/11/24 19:17:49 romy Exp $
