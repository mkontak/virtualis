// $Id: GLUpdateBuffer.C,v 1.2 2005/08/16 21:45:06 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


// includes
#include "StdAfx.h"
#include "GLUpdateBuffer.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define FILE_REVISION "$Revision: 1.2 $"

using namespace OpenGLUtils;

/**
 * Constructor
 */
GLUpdateBuffer::GLUpdateBuffer() : m_pBuffer(NULL), m_size(0,0), m_uBitsPerPixel(0)
{
} // GLUpdateBuffer()


/**
 * Destructor
 */
GLUpdateBuffer::~GLUpdateBuffer()
{
  Reset();
} // ~GLUpdateBuffer()


/**
 * Allocates a GLUpdateBuffer
 *
 * @param eDatatype       Data type of pixels
 * @param size            Buffer dimension size (i.e. allocate a sizeXsize buffer)
 * @param uBitsPerPixel   Number of bits per buffer pixel
 * @return                Success
 */
bool GLUpdateBuffer::Allocate(const GLenum eDatatype, const Point2D<uint4>& size, const uint4 uBitsPerPixel)
{
  if ((m_uBitsPerPixel == uBitsPerPixel) && (m_size.m_x == size.m_x) && (m_size.m_y == size.m_y))
  {
    return true;
  }
  
  Reset();
  switch (eDatatype)
  {
  case GL_UNSIGNED_BYTE:
  case GL_BYTE:
    m_pBuffer = new BYTE[size.m_x*size.m_y*uBitsPerPixel];
    break;
    
  case GL_UNSIGNED_SHORT:
  case GL_SHORT:
    m_pBuffer = new int2[size.m_x*size.m_y*uBitsPerPixel];
    break;
    
  case GL_UNSIGNED_INT:
  case GL_INT:
    m_pBuffer = new int4[size.m_x*size.m_y*uBitsPerPixel];
    break;
    
  case GL_FLOAT:
    m_pBuffer = new float4[size.m_x*size.m_y*uBitsPerPixel];
    break;
    
  case GL_BITMAP:
  default:
    return false;
  }
  
  m_uBitsPerPixel = uBitsPerPixel;
  m_size = size;
  return true;
} // Allocate()


/**
 * Clears the buffer
 */
void GLUpdateBuffer::Reset()
{
  if (m_pBuffer)
  {
    delete []m_pBuffer;
  }
  
  m_pBuffer=NULL;
  m_size.m_x=m_size.m_y=m_uBitsPerPixel=0;
} // Reset()


// $Log: GLUpdateBuffer.C,v $
// Revision 1.2  2005/08/16 21:45:06  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2005/04/13 19:24:16  frank
// cleaned up
//
// Revision 1.1.2.1  2005/04/13 18:50:19  frank
// split out GL update buffer class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/GLUpdateBuffer.C,v 1.2 2005/08/16 21:45:06 jmeade Exp $
// $Id: GLUpdateBuffer.C,v 1.2 2005/08/16 21:45:06 jmeade Exp $
