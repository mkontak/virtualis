// $Id: vxAnnotationEllipse.h,v 1.5 2006/11/24 19:17:49 romy Exp $
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

#ifndef vxAnnotationEllipse_h
#define vxAnnotationEllipse_h


// includes
#include "vxAnnotation.h"
#include "Vector.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxAnnotationEllipse : public vxAnnotation
{
// functions
public: 

  /// default onstructor
  vxAnnotationEllipse();
  
  /// destructor
  virtual ~vxAnnotationEllipse() {};

	/// copy constructor
  vxAnnotationEllipse( const vxAnnotationEllipse & other );            
  
  ///Clone method
	virtual vxAnnotationEllipse * Clone() const;

  
  /// reset the annotation
  virtual void Reset();

  /// get the number of handles
  virtual uint2 GetNumEditableHandles() const { return 9; };

  /// get the number of handles required during creation of the object
  virtual uint2 GetNumCreationHandles() const { return 2; };

  /// set the location of a given handle.
  virtual void SetHandle(const uint2 iHandleIndex, const vxHandle & handle);

  /// return the handle index where the label should appear
  virtual uint2 GetLabelHandleIndex() { return 1; };

  /// return the handle index where the text should appear
  virtual uint2 GetTextHandleIndex() { return 5; };

  /// method to render the annotation
  virtual void Render(vxEnvironment & environment);

  /// morph from creating the object with few handles to full annotation with possibly more handles
  virtual void Morph(vxEnvironment & environment);

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotationEllipse"; };

protected:

  /// update the annotation
  virtual void Update(vxEnvironment & environment);

private:

  /// assignment operator
  vxAnnotationEllipse & operator=(vxAnnotationEllipse & other); // should only be public if really implemented!

  /// common initialization code
  void CommonInit();

  /// compute corner points
  void ComputeEllipseParams(vxEnvironment & environment, const Point<float4> & topleft, const Point<float4> & bottomright,
                            Point<float4> & center, Vector<float4> & horizontal, Vector<float4> & vertical,
                            float4 & fRadiusHorizontal,
                            float4 & fRadiusVertical) const;

  /// create a point on the ellipse for the given angle
  Point<float4> vxAnnotationEllipse::CreatePointOnEllips(const Point<float4> & center,
                                                         const Vector<float4> & horizontal, const Vector<float4> & vertical,
                                                         float4 fRadiusHorizontal, float4 fRadiusVertical, float4 fAngle);

  /// draw the ellipse at the given location with given orientation and radius
  void DrawEllipse(const Point<float4> & center, 
                   const Vector<float4> & horizontal, const Vector<float4> & vertical,
                   float4 fRadiusHorizontal, float4 fRadiusVertical) const;
}; // vxAnnotationEllipse


#endif //vxAnnotationEllipse.h


// Revision History:
// $Log: vxAnnotationEllipse.h,v $
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
// Revision 1.12  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.11  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.10  2003/05/16 13:17:40  frank
// formatting
//
// Revision 1.9  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.8  2003/04/22 14:46:43  michael
// some more code reviews
//
// Revision 1.7  2003/03/04 01:43:06  michael
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
// Revision 1.3  2003/01/30 00:36:29  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.2  2003/01/28 23:41:38  michael
// fixed ellipse scaling ... dual scaling (corner handles) still to be done ...
//
// Revision 1.1  2003/01/20 19:38:24  michael
// added ellipse annotation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotationEllipse.h,v 1.5 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationEllipse.h,v 1.5 2006/11/24 19:17:49 romy Exp $
