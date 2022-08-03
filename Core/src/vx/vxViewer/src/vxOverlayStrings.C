// $Id: vxOverlayStrings.C,v 1.3 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxOverlayStrings.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
 * Constructor
 */
vxOverlayStrings::vxOverlayElement::vxOverlayElement()
{
  m_colorTriple = Triple<float4>( 1, 1, 1 );
  m_alignment = vxOverlayElement::AlignCenter;
} // OverlayElement()


/**
 * convenience constructor
 *
 * @param sText       the text
 * @param eLocation   the location
 * @param colorTriple the color
 */
vxOverlayStrings::vxOverlayElement::vxOverlayElement( const std::string & sText,
                                                      const LocationEnum & eLocation,
                                                      const Triple<float4> & colorTriple ) :
m_sText( sText ),
m_eLocation( eLocation ),
m_colorTriple( colorTriple ),
m_alignment( vxOverlayElement::AlignCenter )
{
} // vxOverlayElement()


/**
 * copy constructor
 *
 * @param other the other element
 */
vxOverlayStrings::vxOverlayElement::vxOverlayElement( const vxOverlayElement & other ) :
m_sText( other.m_sText ),
m_eLocation( other.m_eLocation ),
m_colorTriple( other.m_colorTriple ),
m_randomScreenPosition( other.m_randomScreenPosition ),
m_alignment( other.m_alignment )
{
} // vxOverlayElement( other )


/**
 * Assignment operator
 *
 * @param other the other object
 */
vxOverlayStrings::vxOverlayElement & vxOverlayStrings::vxOverlayElement::operator =( const vxOverlayElement & other )
{
  m_sText = other.m_sText;
  m_eLocation = other.m_eLocation;
  m_colorTriple = other.m_colorTriple;
  m_randomScreenPosition = other.m_randomScreenPosition;
  m_alignment = other.m_alignment;
  return * this;
} // operator =()


/**
 * Copy constructor
 *
 * @param other the original object
 */
vxOverlayStrings::vxOverlayStrings( const vxOverlayStrings & other )
{
  * this = other;
} // Copy constructor


/**
 * Assignment operator
 *
 * @param other the other object
 */
vxOverlayStrings & vxOverlayStrings::operator =( const vxOverlayStrings & other )
{
  if ( & other != this )
    m_elementList = other.m_elementList;

  return * this;
} // operator =()


/**
 * Return a list containing just those elements at the specified location
 *
 * @param eLocationFilter the location on which to filter
 *
 * @return a new list as described above
 */
std::list< vxOverlayStrings::vxOverlayElement > vxOverlayStrings::GetFilteredList( const vxOverlayElement::LocationEnum & eLocationFilter )
{
  std::list<vxOverlayElement> filteredElementList;

  for ( std::list<vxOverlayElement>::iterator it( m_elementList.begin() ); it != m_elementList.end(); ++it )
  {
    if ( it->GetLocation() == eLocationFilter )
    {
      filteredElementList.push_back( * it );
    }
  }

  return filteredElementList;
} // GetFilteredList()


#undef FILE_REVISION 


// $Log: vxOverlayStrings.C,v $
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2  2004/09/17 12:47:48  frank
// fixed copy constructor
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.5  2003/05/22 14:57:30  dongqing
// code review
//
// Revision 1.4  2003/04/29 15:10:45  frank
// Added random screen locations
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
// $Id: vxOverlayStrings.C,v 1.3 2006/01/18 20:39:26 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxOverlayStrings.C,v 1.3 2006/01/18 20:39:26 geconomos Exp $
