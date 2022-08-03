// $Id: MovieFrame.h,v 1.3.2.1 2009/01/21 06:02:34 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net


// pragma declarations
#pragma once


// include declarations
#include "Camera.h"
#include "StudyLoad.h"
#include "MeasureLine.h"


// namespaces
OPEN_MOVIES_NS


/// one frame of a movie
class MovieFrame
{
// member functions
public:

  /// constructor
  MovieFrame();

  /// destructor
  virtual ~MovieFrame();

// member variables
public:

  /// Identifies the type of movie frame
  enum FrameTypeEnum { FLIGHT, TRANSLUCENCY, MEASURE } m_eFrameType;

  /// Dataset position
  ReaderLib::DatasetOrientations::TypeEnum m_eDatasetOrientation;

  /// Current Position
  Camera m_camera;

  /// Current measure line
  MeasureLine m_line;

  /// Current location of the translucency window
  Point2D<int4> m_ptTransLoc;

  /// Whether or not space leaping is active
  bool m_bSpaceLeaping;

  /// Whether or not the 2D camera is visible
  bool m_bShow2DCamera;

  /// sample time in milliseconds
  uint4 m_uSampleTime;

}; // class MovieFrame


CLOSE_MOVIES_NS


// $Log: MovieFrame.h,v $
// Revision 1.3.2.1  2009/01/21 06:02:34  jmeade
// show/hide position camera in 2d movie.
//
// Revision 1.3  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.2  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieFrame.h,v 1.3.2.1 2009/01/21 06:02:34 jmeade Exp $
// $Id: MovieFrame.h,v 1.3.2.1 2009/01/21 06:02:34 jmeade Exp $
