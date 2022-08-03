// $Id: vxAnnotationAngle.h,v 1.5 2006/11/24 19:17:49 romy Exp $
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

#ifndef vxAnnotationAngle_h
#define vxAnnotationAngle_h


// includes
#include "vxAnnotation.h"


// class definition
class VX_VIEWER_DLL vxAnnotationAngle : public vxAnnotation
{
// functions
public: 

  /// default constructor
  vxAnnotationAngle();
  
  /// destructor
  virtual ~vxAnnotationAngle() {};

  /// copy constructor
  vxAnnotationAngle( const vxAnnotationAngle & other);

	///Clone method
	virtual vxAnnotationAngle * Clone() const;
	

  /////////////////////////////////////////////////////////////////////
  // actual vxHandlesObj functions

  /// reset the annotation
  virtual void Reset();

  /////////////////////////////////////////////////////////////////////
  // actual vxAnotation functions

  /// get the number of handles
  virtual uint2 GetNumEditableHandles() const { return 3; };

  /// get the number of handles required during creation of the object
  virtual uint2 GetNumCreationHandles() const { return 3; };

  /// method to render the annotation
  virtual void Render(vxEnvironment & environment);

  /////////////////////////////////////////////////////////////////////
  // actual vxISerialize functions

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotationAngle"; };

protected:

  /// update the annotation
  virtual void Update(vxEnvironment & environment);

  /// transform the handle point into the proper coordinate system
  Point3Df GetTransformedHandle(const unsigned int iHandleIndex, vxEnvironment & environment) const;

private:
  
  /// assignment operator
  vxAnnotationAngle & operator=(vxAnnotationAngle & other); // should only be public if really implemented!

  /// common initialization code
  void CommonInit();

  /// draw the arc of the angle
  void RenderArc(vxEnvironment & environment) const;

  /// get maximum radius for arc
  float4 GetArcRadius() const { return m_fArcRadius; };

  /// set maximum radius for arc
  void SetArcRadius(float4 fArcRadius) { m_fArcRadius = fArcRadius; };

// data:
private:

  /// maximum radius for arc
  float4 m_fArcRadius;
}; // vxAnnotationAngle


#endif //vxAnnotationAngle.h


// Revision History:
// $Log: vxAnnotationAngle.h,v $
// Revision 1.5  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.4  2006/11/23 06:23:15  romy
// cloning modified
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
// Revision 1.16  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.15  2004/01/06 13:56:09  frank
// Fixed the angle display on curved and luminal
//
// Revision 1.14  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.13  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.12  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.11  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.10  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.9  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.8  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.7  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.6  2003/02/18 15:43:30  michael
// made serializer class a friend
//
// Revision 1.5  2003/01/19 21:07:09  michael
// added box annotation
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotationAngle.h,v 1.5 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationAngle.h,v 1.5 2006/11/24 19:17:49 romy Exp $
