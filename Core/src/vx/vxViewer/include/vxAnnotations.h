// $Id: vxAnnotations.h,v 1.7 2006/09/21 15:13:20 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Base class of all vxAnnotations
*/


// pragmas
#pragma once


// includes
#include "vxAnnotation.h"


// forward declarations
class vxViewerType;
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxAnnotations : public vxISerialize
{
// functions
public:

  /// default constructor
  vxAnnotations();
  
  /// copy constructor
  vxAnnotations(const vxAnnotations & other);

  /// destructor
  virtual ~vxAnnotations();// {};
  
  /// assignment operator
  const vxAnnotations & operator=(const vxAnnotations & other);

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotations"; };

  /// get the active annotation index
  int4 GetActiveIndex() const { return m_iActiveAnnotationIndex; }
  
  /// get the active annotation index
  void SetActiveIndex(int4 iIndex);

  /// get the number of annotations
  int4 GetCount() const { return m_vpAnnotations.size(); }

  /// get annotation if given index
  vxAnnotation * Get(int4 iIndex);

  /// add an annotation
  void Add(vxAnnotation * pAnnotation);

  /// remove an annotation
  void Remove(int4 iIndex);

	///Removes all annotations
	void RemoveAll( );

	/// Replaces an annotation from the list
	void Replace( int4 iIndex, vxAnnotation * pAnnotation );

	///Swaps an Annotation on a specific index
	vxAnnotation * Swap( int4 iIndex, vxAnnotation * pAnnotation );

  /// get the active annotation index;
  vxAnnotation * GetActive();

// data
private:

  /// vector of annotations
  std::vector< vxAnnotation *> m_vpAnnotations;

  /// active annotation index
  int4 m_iActiveAnnotationIndex;

}; // class vxAnnotations


// $Log: vxAnnotations.h,v $
// Revision 1.7  2006/09/21 15:13:20  romy
// added annotation removeAll
//
// Revision 1.6  2006/08/07 20:57:31  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.5  2006/08/07 20:26:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.4  2006/08/07 18:48:00  romy
// Added replace method
//
// Revision 1.3  2006/02/21 19:45:07  frank
// formatting
//
// Revision 1.2  2005/04/12 15:24:14  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/09/22 21:11:51  michael
// added "mailto:"
//
// Revision 1.12  2003/05/16 13:17:40  frank
// formatting
//
// Revision 1.11  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.10  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.9  2003/03/20 15:35:13  geconomos
// Added event EVENT_UPDATE_PLANE.
//
// Revision 1.8  2003/03/04 01:43:24  michael
// fixed 'ESC' on creating annotations
//
// Revision 1.7  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.6  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.5  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.4  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.3  2003/01/13 20:00:20  michael
// fixed bugs in creation of annotations, need to adjust the rendering
//
// Revision 1.2  2003/01/13 14:43:56  geconomos
// Set the active annotation in AddAnnotation().
//
// Revision 1.1  2003/01/09 14:54:35  michael
// added new annotations class that represents all available annotations
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotations.h,v 1.7 2006/09/21 15:13:20 romy Exp $
// $Id: vxAnnotations.h,v 1.7 2006/09/21 15:13:20 romy Exp $
