// $Id: v3D_Cine.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once


// includes
#include "vxCine.h"
#include "v3D_Enums.h"


// namespaces
OPEN_VISUALIZATION_NS


public ref class Cine
{
public:

  /// default constructor
  Cine();

  /// destructor
  virtual ~Cine();

  /// finalizer
  !Cine();

  /// gets a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pCine ); }

  /// gets or sets the CineMode
  property CineModes CineMode
  { 
    CineModes get() { return static_cast< CineModes >( m_pCine->GetCineMode() ); }
    void set( CineModes eModes ) { m_pCine->SetCineMode( static_cast< vxCine::CineEnum >( eModes )); }
  } // CineMode

  /// gets or sets the frames per second
  property int4 FramesPerSecond
  {
    int4 get() { return m_pCine->GetFramesPerSecond(); }
    void set( int4 iFramesPerSecond ) { m_pCine->SetFramesPerSecond( iFramesPerSecond ); }
  } // FramePerSecond

private:

  /// vxCine
  vxCine * m_pCine;

}; // class Cine


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_Cine.h,v $
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Cine.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
// $Id: v3D_Cine.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $

