// $Id: MovieSaveStateOptions.h,v 1.3.2.1 2009/01/21 22:02:59 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

// pragmas
#pragma once

// includes
#include "SliceChildView.h"

// namespaces
OPEN_MOVIES_NS

// class declaration
class MovieSaveStateOptions
{
// member functions
public:

  /// constructor
  MovieSaveStateOptions();

  /// destructor
  virtual ~MovieSaveStateOptions();

public:
  /// To automatically reset variables to their original values
  template <class T> class ResetVar
  {
  public:
    /// constructor
    ResetVar(T& var) : m_var(var), m_origval(var) { }
    /// destructor
    ~ResetVar() { m_var = m_origval; }

  private:
    /// variable to reset
    T& m_var;
    /// original value
    const T m_origval;
  }; // class ResetVar

public:
  /// Use to toggle off coordinate display for movie frames
  ResetVar<ColonViews::ChildViews::SliceChildView::CoordDisplay::Enum> m_coordDisplayValue;
  /// Render size
  std::pair< uint4, uint4 > m_renderSize;  
  /// Biopsy mode
  ResetVar<bool> m_bBiopsyMode;
  /// Space leaping
  ResetVar<bool> m_bSpaceLeaping;
  /// Visibility of camera location in 2D
  bool m_bShow2DCamera;
  /// Translucency window location
  std::pair< ResetVar <Point<int4> >, ResetVar <Point<int4> > > m_transWinLoc;
}; // class MovieSaveStateOptions

CLOSE_MOVIES_NS

// $Log: MovieSaveStateOptions.h,v $
// Revision 1.3.2.1  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.3  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.2  2006/09/21 19:52:45  geconomos
// Issue #4979: proper storing and restoration of primary and secondary volume render sizes
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieSaveStateOptions.h,v 1.3.2.1 2009/01/21 22:02:59 jmeade Exp $
// $Id: MovieSaveStateOptions.h,v 1.3.2.1 2009/01/21 22:02:59 jmeade Exp $
