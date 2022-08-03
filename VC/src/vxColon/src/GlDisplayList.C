// $Id: GlDisplayList.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille( frank@viatronix.com )


// includes
#include "StdAfx.h"
#include "GlDisplayList.h"
#include "Exception.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"

using namespace OpenGLUtils;

/**
 * constructor
 */
GlDisplayList::GlDisplayList() :
m_iListIdentifier( -1 )
{
} // GlDisplayList


/**
 * destructor
 */
GlDisplayList::~GlDisplayList()
{
  DeleteList();
} // ~GlDisplayList()


/**
 * begin definition of a new list
 */
void GlDisplayList::BeginDefinition()
{
  if ( GetValid() ) DeleteList();

  m_iListIdentifier = glGenLists( 1 );

  if ( m_iListIdentifier == 0 )
  {
    throw new ex::AbortOperationException( LogRec( "Unable to create an OpenGL display list", "GlDisplayList", "BeginDefinition" ) );
  }

  glNewList( m_iListIdentifier, GL_COMPILE_AND_EXECUTE );

} // BeginDefinition()


/**
 * end definition of a new list
 */
void GlDisplayList::EndDefinition()
{
  glEndList();
} // EndDefinition()


/**
 * call the display list
 */
void GlDisplayList::CallList()
{
  if ( ! GetValid() )
  {
    throw new ex::AbortOperationException( LogRec( "Tried to call an invalid OpenGL display list", "GlDisplayList", "CallList" ) );
  }

  glCallList( m_iListIdentifier );
} // CallList()


/**
 * gets whether the list is valid
 *
 * @return if the list is valid
 */
const bool GlDisplayList::GetValid() const
{
  return m_iListIdentifier != -1;
} // GetValid()


/**
 * invalidates the cache
 */
void GlDisplayList::Invalidate()
{
  DeleteList();
} // Invalidate()


/**
 * deletes the list
 */
void GlDisplayList::DeleteList()
{
  if ( m_iListIdentifier == -1 ) return;

  glDeleteLists( m_iListIdentifier, 1 );
} // DeleteList()


// undefines
#undef FILE_REVISION


// $Log: GlDisplayList.C,v $
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/16 21:46:06  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2005/04/06 15:45:04  frank
// Issue #3451: Used display lists to accelerate overview drawing with marking
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/GlDisplayList.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
// $Id: GlDisplayList.C,v 1.1 2005/09/13 12:59:56 geconomos Exp $
