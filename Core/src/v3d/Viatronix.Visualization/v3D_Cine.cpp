// $Id: v3D_Cine.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "v3D_Cine.h"
#include "vxCine.h"


USING_VISUALIZATION_NS


/**
 * default constructor
 */
Cine::Cine() :
  m_pCine( new vxCine )
{
} // Cine()


/**
 * destructor
 */
Cine::~Cine()
{
  this->!Cine();
} // ~Cine()


/**
 * finalizer
 */
Cine::!Cine()
{
  if( m_pCine != NULL )
    delete m_pCine;

  m_pCine = NULL;
} // !Cine()


// $Log: v3D_Cine.cpp,v $
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Cine.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
// $Id: v3D_Cine.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $

