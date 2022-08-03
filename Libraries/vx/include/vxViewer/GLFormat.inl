// $Id: GLFormat.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george (geconomos@viatronix.com)
//
// Complete history at bottom of file.


GLFormat::GLFormat()
{
  m_uOptions = DOUBLEBUFFER | DEPTHBUFFER | RGBA | DIRECTRENDERING;
}

bool GLFormat::GetDoubleBuffer() const
{
  return TestOption(DOUBLEBUFFER);
}

void GLFormat::SetDoubleBuffer(const bool bEnable)
{
  SetOption(bEnable ? DOUBLEBUFFER : SINGLEBUFFER);
}

bool GLFormat::GetDepthBuffer() const
{
  return TestOption(DEPTHBUFFER);
}

void GLFormat::SetDepthBuffer(const bool bEnable)
{
  SetOption(bEnable ? DEPTHBUFFER : NODEPTHBUFFER);
}

bool GLFormat::GetAlpha() const
{
  return TestOption(ALPHACHANNEL);
}

void GLFormat::SetAlpha(const bool bEnable)
{
  SetOption(bEnable ? ALPHACHANNEL : NOALPHACHANNEL);
}

bool GLFormat::GetAccumulationBuffer() const
{
  return TestOption(ACCUMBUFFER);
}

void GLFormat::SetAccumulationBuffer(const bool bEnable)
{
  SetOption(bEnable ? ACCUMBUFFER : NOACCUMBUFFER);
}

bool GLFormat::GetStencilBuffer() const
{
  return TestOption(STENCILBUFFER);
}

void GLFormat::SetStencilBuffer(const bool bEnable)
{
  SetOption( bEnable ? STENCILBUFFER: NOSTENCILBUFFER );
}

bool GLFormat::GetStereoBuffer() const
{
  return TestOption(STEREOBUFFERS);
}

void GLFormat::SetStereoBuffer(const bool bEnable)
{
  SetOption(bEnable ? STEREOBUFFERS : NOSTEREOBUFFERS);
}

bool GLFormat::GetDirectRendering() const
{
  return TestOption(DIRECTRENDERING);
}

void GLFormat::SetDirectRendering(const bool bEnable)
{
  SetOption(bEnable ? DIRECTRENDERING : INDIRECTRENDERING);
}

void GLFormat::SetOption(const FormatOptionEnum eOption)
{
  if ( eOption & 0xffff )
  m_uOptions |= eOption;
  else
    m_uOptions &= ~( eOption >> 16 );
}



bool GLFormat::TestOption(const FormatOptionEnum eOption) const
{
  if ( eOption & 0xffff )
     return ( m_uOptions & eOption ) != 0;
  else
     return ( m_uOptions & ( eOption >> 16 ) ) == 0;
}




// $Log: GLFormat.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.1  2003/05/16 11:45:25  michael
// code review
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLFormat.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: GLFormat.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
