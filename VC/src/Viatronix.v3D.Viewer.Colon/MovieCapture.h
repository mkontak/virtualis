// $Id: MovieCapture.h,v 1.1 2005/10/28 13:27:43 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

#pragma once

// include declarations
#include "GlChildView.h"
#include "MovieFrame.h"
#include "MovieFormat.h"

OPEN_MOVIES_NS

// class declaration
class MovieCapture
{
public:
  /// frames of the movie
  std::vector< Movies::MovieFrame > m_frames;
  /// movie format options
  Movies::MovieFormat m_format;
  /// which is the primary view for the movie
  ColonViews::ChildViews::GLChildView::ViewWndTypeEnum m_eType;
  /// whether or not full oblique mode is enabled for a slice
  bool m_bFullOblique;
  /// path to the avi
  std::strstream m_ssAVIFile;
  /// path to a temporary wmv
  std::strstream m_ssTempWMVFile;
}; // class MovieCapture

CLOSE_MOVIES_NS

// $Log: MovieCapture.h,v $
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieCapture.h,v 1.1 2005/10/28 13:27:43 geconomos Exp $
// $Id: MovieCapture.h,v 1.1 2005/10/28 13:27:43 geconomos Exp $
