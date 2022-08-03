// $Id: vxMomentum.h,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: george economos (george@viatronix.com)
#pragma once

class VX_VIEWER_DLL vxMomentum
{
// member functions
public:
  
  /// default consturctor
  vxMomentum();

  /// destructor
  virtual ~vxMomentum();

  /// resets the momentum instance to a deafult state
  void Reset();

  /// gets if momentum is enabled
  bool GetEnabled() const { return m_bEnabled; }
  
  /// sets if momentum is enabled
  void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }

  /// gets if momentum is enabled
  bool GetInitiated() const { return m_bInitiated; }
  
  /// sets if momentum is enabled
  void SetInitiated( bool bInitiated ) { m_bInitiated = bInitiated; }

  /// gets the right angle
  float GetRightAngle() const { return m_fRightAngle; }
  
  /// sets the right angle
  void SetRightAngle( float fAngle ) { m_fRightAngle = fAngle; }

  /// gets the right angle for the camera's view direction
  float GetViewDirectionRightAngle() const { return m_fViewDirectionRightAngle; }
  
  /// sets the right angle for the camera's view direction
  void SetViewDirectionRightAngle( float fAngle ) { m_fViewDirectionRightAngle = fAngle; }

  /// gets the right angle
  float GetUpAngle() const { return m_fUpAngle; }
  
  /// sets the right angle
  void SetUpAngle( float fAngle ) { m_fUpAngle = fAngle; }

  /// gets the right angle for the camera's view direction
  float GetViewDirectionUpAngle() const { return m_fViewDirectionUpAngle; }
  
  /// sets the right angle for the camera's view direction
  void SetViewDirectionUpAngle( float fAngle ) { m_fViewDirectionUpAngle = fAngle; }

  /// gets the frames per second
  int4 GetFramesPerSecond() const { return m_iFramesPerSecond; } 

  /// sets the frames per second
  void SetFramesPerSecond( int4 iFramesPerSecond ) { m_iFramesPerSecond = iFramesPerSecond; }

// member variables
private:

  /// inidicates if momentum is enabled
  bool m_bEnabled;

  /// indicates if momentum is initiated
  bool m_bInitiated;

  /// camera's right angle
  float4 m_fRightAngle;

  /// the right angle for the camera's view direction
  float4 m_fViewDirectionRightAngle;

  /// camera's up angle
  float4 m_fUpAngle;

  /// the up angle for the camera's view direction
  float4 m_fViewDirectionUpAngle;

  /// frames per second
  int4 m_iFramesPerSecond;

}; // vxMomentum

// $Log: vxMomentum.h,v $
// Revision 1.3  2007/01/16 17:03:48  gdavidson
// Moved angle information from manuipulator to data object
//
// Revision 1.2  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.2.2  2005/08/15 15:28:58  geconomos
// Added Reset method to return instance to default state
//
// Revision 1.1.2.1  2005/08/12 17:20:07  geconomos
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMomentum.h,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
// $Id: vxMomentum.h,v 1.3 2007/01/16 17:03:48 gdavidson Exp $
