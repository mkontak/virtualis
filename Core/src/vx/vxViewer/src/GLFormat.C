// $Id: GLFormat.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)


// includes
#include "stdafx.h"
#include "GlFormat.h"


/**
 * Constructor
 */
GLFormat::GLFormat()
{
  m_uOptions = DOUBLEBUFFER | DEPTHBUFFER | RGBA | DIRECTRENDERING;
} // GLFormat()


/**
 * Is double buffering enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetDoubleBuffer() const
{
  return TestOption( DOUBLEBUFFER );
}


/**
 * Set double buffering
 * @param bEnable true to enable; false otherwise.
 */
void GLFormat::SetDoubleBuffer( const bool bEnable )
{
  SetOption( bEnable ? DOUBLEBUFFER : SINGLEBUFFER );
}


/**
 * Is depth buffer enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetDepthBuffer() const
{
  return TestOption( DEPTHBUFFER );
} // GetDepthBuffer()


/**
 * Set depth buffer
 * @param bEnable true to enable; false otherwise.
 */
void GLFormat::SetDepthBuffer( const bool bEnable )
{
  SetOption( bEnable ? DEPTHBUFFER : NODEPTHBUFFER );
} // SetDepthBuffer()


/**
 * Is alpha channel enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetAlpha() const
{
  return TestOption( ALPHACHANNEL );
} // GetAlpha()


/**
 * Set alpha channel
 * @param bEnable true to enable; false otherwise.
 */
void GLFormat::SetAlpha( const bool bEnable )
{
  SetOption( bEnable ? ALPHACHANNEL : NOALPHACHANNEL );
} // SetAlpha()


/**
 * Is accumulation buffer enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetAccumulationBuffer() const
{
  return TestOption( ACCUMBUFFER );
} // GetAccumulationBuffer()


/**
 * Set accumulation buffer
 * @param bEnable true to enable; false otherwise.
 */
void GLFormat::SetAccumulationBuffer( const bool bEnable )
{
  SetOption( bEnable ? ACCUMBUFFER : NOACCUMBUFFER );
} // SetAccumulationBuffer()


/**
 * Is stencil buffer enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetStencilBuffer() const
{
  return TestOption( STENCILBUFFER );
} // GetStencilBuffer() 


/**
 * Set stencil buffer
 * @param bEnable true to enable; false otherwise. 
 */
void GLFormat::SetStencilBuffer( const bool bEnable )
{
  SetOption( bEnable ? STENCILBUFFER: NOSTENCILBUFFER );
} // SetStencilBuffer()


/**
 * Is stereo buffer enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetStereoBuffer() const
{
  return TestOption( STEREOBUFFERS );
} // GetStereoBuffer() 


/**
 * Set stereo buffer
 * @param bEnable true to enable; false otherwise. 
 */
void GLFormat::SetStereoBuffer( const bool bEnable )
{
  SetOption( bEnable ? STEREOBUFFERS : NOSTEREOBUFFERS );
} // SetStereoBuffer()


/**
 * Is direct rendering enabled
 * @return true if enabled; false otherwise
 */
bool GLFormat::GetDirectRendering() const
{
  return TestOption( DIRECTRENDERING );
} // GetDirectRendering()


/**
 * Set direct rendering
 * @param bEnable true to enable; false otherwise. 
 */
void GLFormat::SetDirectRendering( const bool bEnable )
{
  SetOption( bEnable ? DIRECTRENDERING : INDIRECTRENDERING );
} // SetDirectRendering()


/**
 * Sets the specified option.
 * @param eOption Option to set.
 */
void GLFormat::SetOption( const FormatOptionEnum eOption )
{
  if( eOption & 0xffff )
  m_uOptions |= eOption;
  else
    m_uOptions &= ~( eOption >> 16 );
} // SetOption()


/**
 * Tests if the specified option is set.
 * @param eOption Option to to test.
 * @return True if option is set; false otherwise.
 */
bool GLFormat::TestOption( const FormatOptionEnum eOption ) const
{
  if( eOption & 0xffff )
     return( m_uOptions & eOption ) != 0;
  else
     return ( m_uOptions & ( eOption >> 16 ) ) == 0;
}// TestOption()


// $Log: GLFormat.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/14 13:26:59  geconomos
// Coding standards.
//
// Revision 1.2  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 1.1  2002/07/02 20:28:54  geconomos
// Initial revision.
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:34   ingmar
// Initial revision.
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.4  2001/06/27 19:15:11  geconomos
// updated formatting
//
// Revision 1.3  2001/06/27 18:51:09  geconomos
// no message
//
// Revision 1.2  2001/06/22 18:22:07  geconomos
// Initial OpenGL implementation.
//
// Revision 1.1  2001/06/18 21:52:14  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/GLFormat.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: GLFormat.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
