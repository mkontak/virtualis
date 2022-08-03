// $Id: vxCine.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#pragma once

/**
 * Data defining a cine operation.
 */
class VX_VIEWER_DLL vxCine
{
public:

  /// enumeration of all cine styles
  enum CineEnum
  {
    IDLE = 0,
    SWING,
    LOOP_FORWARD,
    LOOP_BACKWARD
  }; // CineEnum

public:

  /// default constructor
  vxCine();

  /// gets the cine mode
  CineEnum GetCineMode() const { return m_eCineMode; }

  /// sets the cine mode
  void SetCineMode( CineEnum eCineMode ) { m_eCineMode = eCineMode; }

  /// gets the frames per second
  int4 GetFramesPerSecond() const { return m_iFramesPerSecond; } 

  /// sets the frames per second
  void SetFramesPerSecond( int4 iFramesPerSecond ) { m_iFramesPerSecond = iFramesPerSecond; }

private:

  /// cine mode
  CineEnum m_eCineMode;

  /// frames per second
  int4 m_iFramesPerSecond;

}; // vxCine


// $Log: vxCine.h,v $
// Revision 1.2  2007/03/06 19:20:11  gdavidson
// Added class comments
//
// Revision 1.1  2007/01/16 16:20:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCine.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
// $Id: vxCine.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $

