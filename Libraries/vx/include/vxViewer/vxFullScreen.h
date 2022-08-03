// $Id: vxFullScreen.h,v 1.1.2.2 2009/07/28 15:15:15 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once


// includes
#include "vxElement.h"


// forward declaration
class vxEnvironment;


/**
 * Definition for the full screen behavior
 */
class VX_VIEWER_DLL vxFullScreen : public vxElement
{
// construction
public:

  /// constructor
  vxFullScreen() : m_pEnvironment( NULL ), m_iProportion( 10 ) {}

  /// returns value
  const vxEnvironment * GetEnvironment() const { return m_pEnvironment; }

// member functions
public:

  /// sets value
  void SetEnvironment( vxEnvironment * pEnvironment ) { m_pEnvironment = pEnvironment; }

  /// gets the proportion
  const int4 GetProportion() const { return m_iProportion; }

  /// sets the proportion
  void SetProportion( const int iProportion ) { m_iProportion = iProportion; }

// member variables
private:

  /// value
  vxEnvironment * m_pEnvironment;

  /// proportion
  int4 m_iProportion;

}; // class vxFullScreen


// $Log: vxFullScreen.h,v $
// Revision 1.1.2.2  2009/07/28 15:15:15  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2008/11/24 17:11:03  gdavidson
// Issue #6171: Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxFullScreen.h,v 1.1.2.2 2009/07/28 15:15:15 kchalupa Exp $
// $Id: vxFullScreen.h,v 1.1.2.2 2009/07/28 15:15:15 kchalupa Exp $
