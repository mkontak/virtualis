// $Id: vxOverlayStrings.h,v 1.2 2004/09/17 12:47:48 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <list>
#include <string>
#include "vxElement.h"
#include "Triple.h"
#include "Point2D.h"


/**
 * The overlays strings
 */
class VX_VIEWER_DLL vxOverlayStrings : public vxElement
{
// inner classes
public:

  class VX_VIEWER_DLL vxOverlayElement
  {
  // enumerations
  public:

    /// the possible locations the text string can be rendered
    enum LocationEnum
    {
      UpperLeft,
      UpperMiddle,
      UpperRight,
      MiddleLeft,
      MiddleMiddle,
      MiddleRight,
      LowerLeft,
      LowerMiddle,
      LowerRight,
      Random,
    }; // enum LocationEnum

    /// the string alignments
    enum AlignmentEnum
    {
      AlignLeft,
      AlignCenter,
      AlignRight
    }; // enum AlignmentEnum

  // member functions
  public:

    /// convenience constructor
    vxOverlayElement();

    /// convenience constructor
    vxOverlayElement( const std::string & sText, const LocationEnum & eLocation, const Triple<float4> & colorTriple = Triple<float4>( 1, 1, 1 ) );

    /// copy constructor
    vxOverlayElement( const vxOverlayElement & other );

    /// assignment operator
    vxOverlayElement & operator =( const vxOverlayElement & other );

    /// access the text string
    const std::string & GetText() const { return m_sText; }

    /// modify the text string
    void SetText( const std::string & sText ) { m_sText = sText; }

    /// access the location
    const LocationEnum & GetLocation() const { return m_eLocation; }

    /// modify the location
    void SetLocation( const LocationEnum & eLocation ) { m_eLocation = eLocation; }

    /// access the random screen position
    Point2D<float4> GetRandomScreenPosition() const { return m_randomScreenPosition; }

    /// modify the random screen position
    void SetRandomScreenPosition( const Point2D<float4> & randomScreenPosition ) { m_randomScreenPosition = randomScreenPosition; }

    /// access the color
    const Triple<float4> & GetColor() const { return m_colorTriple; }

    /// modify the color
    void SetColor( const Triple<float4> & colorTriple ) { m_colorTriple = colorTriple; }

    /// access the alignment for randomly placed strings
    const AlignmentEnum & GetAlignment() const { return m_alignment; }

    /// modify the alignment for randomly placed strings
    void SetAlignment( const AlignmentEnum & alignment ) { m_alignment = alignment; }

// data members
private:

    /// text string to render
    std::string m_sText;

    /// location it should be displayed
    LocationEnum m_eLocation;

    /// pixel location, if not one of the standard locations
    Point2D<float4> m_randomScreenPosition;

    /// render color
    Triple<float4> m_colorTriple;

    /// the horizontal alignment
    AlignmentEnum m_alignment;

  }; // class vxOverlayElement

// enumerations and typedefs
public:

  /// the events that can occur
  enum EventsEnum
  {
    EVENT_MODIFIED = 0x00000001,
  }; // enum EventsEnum

  /// overlay element list
  typedef std::list<vxOverlayElement> OverlayElementList;

  /// iterator for the overlay element list
  typedef OverlayElementList::iterator OverlayElementListIterator;

// member functions
public:
  
  /// constructor
  vxOverlayStrings() {}

  /// copy constructor
  vxOverlayStrings( const vxOverlayStrings & other );

  /// assignment operator
  vxOverlayStrings & operator =( const vxOverlayStrings & other );

  /// clear all strings
  void Clear() { m_elementList.clear(); }

  /// add an element to the list
  void AddElement( const vxOverlayElement & element ) { m_elementList.push_back( element ); }
    
  /// return a list containing just those elements at the specified location
  std::list< vxOverlayElement > GetFilteredList( const vxOverlayElement::LocationEnum & eLocationFilter );

// Data Members
public:

  /// List of overlay elements to display
  std::list< vxOverlayElement > m_elementList;

}; // class vxOverlayStrings


// $Log: vxOverlayStrings.h,v $
// Revision 1.2  2004/09/17 12:47:48  frank
// fixed copy constructor
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.12  2003/08/25 15:52:08  frank
// typo fixed
//
// Revision 1.11  2003/08/14 16:59:33  frank
// formatting
//
// Revision 1.10  2003/08/13 13:06:44  michael
// added default constructor since compiler does not add it automatically (weired!)
//
// Revision 1.8  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.7  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.6  2003/05/01 13:34:58  frank
// Added default color to overlay elements
//
// Revision 1.5  2003/04/29 15:10:45  frank
// Added random screen locations
//
// Revision 1.4  2003/04/11 18:33:50  frank
// Got overlay strings working
//
// Revision 1.3  2003/04/10 14:57:25  frank
// Moved code to inline so that it properly links
//
// Revision 1.2  2003/04/10 14:47:02  frank
// Fixed implementation
//
// Revision 1.1  2003/04/10 02:48:49  frank
// Initial version
//
// Revision 1.3  2003/01/22 21:44:03  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.2  2002/11/19 22:14:29  jmeade
// Removed unnecessary pragmas at George's request.
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.4  2002/09/30 14:36:30  dmitry
// EVENT_COMPLETED added
//
// Revision 1.3  2002/07/11 21:09:10  manju
// Speed up.
//
// Revision 1.2  2002/07/10 19:24:59  manju
// Changed CreateMaskVolume to Initialize().
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.3  2002/06/27 12:08:16  manju
// Added comments.
//
// Revision 1.2  2002/06/26 17:24:27  manju
// Changed vector of lassos to list of lassos and modified the code to get
// it working,
//
// Revision 1.1  2002/06/19 14:23:37  manju
// Restricted Lasso class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxOverlayStrings.h,v 1.2 2004/09/17 12:47:48 frank Exp $
// $Id: vxOverlayStrings.h,v 1.2 2004/09/17 12:47:48 frank Exp $
