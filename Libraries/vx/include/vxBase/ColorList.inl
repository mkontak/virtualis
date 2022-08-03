// $Id: ColorList.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ColorList.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


/**
 * Constructor
 */
ColorList::ColorList()
{
  Reset();
} // constructor


/**
 * Reset the data to the defaults - a list maximally unique colors
 */
void ColorList::Reset()
{
  m_colors.clear();
  m_colors.push_back( Triple<uint1>( 252,   0,   0 ) );
  m_colors.push_back( Triple<uint1>( 239,  22, 211 ) );
  m_colors.push_back( Triple<uint1>( 184,  82, 237 ) );
  m_colors.push_back( Triple<uint1>( 142,  98, 241 ) );
  m_colors.push_back( Triple<uint1>( 109, 169, 243 ) );
  m_colors.push_back( Triple<uint1>(  14,  52, 245 ) );
  m_colors.push_back( Triple<uint1>(  30, 210, 199 ) );
  m_colors.push_back( Triple<uint1>(   0, 201,  85 ) );
  m_colors.push_back( Triple<uint1>( 187, 223,  18 ) );
  m_colors.push_back( Triple<uint1>( 218, 190,   0 ) );
  m_colors.push_back( Triple<uint1>( 191, 190, 106 ) );
  m_colors.push_back( Triple<uint1>( 255, 156,   0 ) );
  m_colors.push_back( Triple<uint1>( 255,  96,   0 ) );
  m_colors.push_back( Triple<uint1>( 245, 153, 153 ) );
  m_colors.push_back( Triple<uint1>( 244, 188, 116 ) );
} // Reset()


/**
 * Retrieves next available color
 * @return next available color
 */
Triple<uint1> ColorList::GetColor()
{
  if ( m_colors.empty() == true )
    Reset();  
  
  Triple<uint1> color = m_colors.front();
  m_colors.pop_front();
  return color;
} // GetColor()


// $Log: ColorList.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/08 20:14:03  michael
// code reviews
//
// Revision 1.7  2003/05/08 20:02:54  frank
// code review
//
// Revision 1.6  2003/05/08 17:33:33  michael
// code review
//
// Revision 1.5  2003/05/06 16:18:49  frank
// code review
//
// Revision 1.4  2003/04/29 21:55:37  michael
// some initial code reviews
//
// Revision 1.3  2003/01/13 21:00:26  ingmar
// colors are not of type Triple<uint2> but Triple<uint1>
//
// Revision 1.2  2001/11/09 19:32:48  geconomos
// Updated to make color list non-static
//
// Revision 1.1  2001/10/30 16:21:58  dmitry
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ColorList.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ColorList.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
