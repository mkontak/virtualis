// $Id: GlDisplayList.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille( frank@viatronix.com )


// pragmas
#pragma once


// includes
#include "Typedef.h"

namespace OpenGLUtils
{
  /**
   * Class to handle the creation and use of OpenGL dislay lists
   */
  class GlDisplayList
  {
  // member functions
  public:

    /// constructor
    GlDisplayList();

    /// destructor
    ~GlDisplayList();

    /// begin definition of a new list
    void BeginDefinition();

    /// end definition of a new list
    void EndDefinition();

    /// call the display list
    void CallList();

    /// gets whether the list is valid
    const bool GetValid() const;

    /// invalidates the cache
    void Invalidate();

  private:

    /// deletes the list
    void DeleteList();

  // data members
  private:

    /// the OpenGL display list identifier
    int4 m_iListIdentifier;

  }; // class GlDisplayList

} // namespace OpenGLUtils


// $Log: GlDisplayList.h,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/GlDisplayList.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: GlDisplayList.h,v 1.1 2005/09/13 13:00:10 geconomos Exp $
