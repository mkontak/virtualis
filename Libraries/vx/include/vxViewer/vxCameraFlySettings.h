// $Id: vxCameraFlySettings.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
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
#include "Vector3D.h"
#include "vxSubmarine.h"


// forward declarations
class vxEnvironment;


/**
 * Data necessary for camera fly operation.
 */
class VX_VIEWER_DLL vxCameraFlySettings
{
public:

  /// default constructor
  vxCameraFlySettings();

  /// gets if the camera is in autofly
  bool IsAutoFlyMode() const { return m_bIsAutoFlyMode; }

  /// sets if the camera is autofly
  void SetAutoFlyMode( bool bIsAutoFlyMode ) { m_bIsAutoFlyMode = bIsAutoFlyMode; }

  /// gets if camera flying is enabled
  bool IsEnabled() const { return m_bEnabled; }

  /// sets if camera flying is enabled
  void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }

  /// gets if flying forward
  bool IsFlyingForward() const { return m_bIsFlyingForward; }

  /// sets if flying forward
  void SetFlyingForward( bool bFlyingForward ) { m_bIsFlyingForward = bFlyingForward; }

  /// gets the submarine
  vxSubmarine & GetSubmarine() { return m_submarine; } 

  /// gets the force vector
  Vector3Df & GetForceVector() { return m_forceVector; }

  /// sets the force vector
  void SetForceVector( const Vector3Df & vector ) { m_forceVector = vector; }

    /// gets the force vector
  Vector3Df & GetPanVector() { return m_panVector; }

  /// sets the force vector
  void SetPanVector( const Vector3Df & vector ) { m_panVector = vector; }

  /// gets the frames per second
  int4 GetFramesPerSecond() const { return m_iFramesPerSecond; } 

  /// sets the frames per second
  void SetFramesPerSecond( int4 iFramesPerSecond ) { m_iFramesPerSecond = iFramesPerSecond; }

public:

  /// intialize the submarine
  void InitializeSubmarine( vxEnvironment & environment );

  /// applies the force and pan vectors to the camera
  void UpdateSubmarine( vxEnvironment & environment );

private:

  /// if flying in automatic flight mode
  bool m_bIsAutoFlyMode;

  /// if flying enabled
  bool m_bEnabled;

  /// if flying forward
  bool m_bIsFlyingForward;

  /// force vector
  Vector3Df m_forceVector;

  /// pan vector
  Vector3Df m_panVector;

  /// frames per second
  int4 m_iFramesPerSecond;

  /// the navigation model
  vxSubmarine m_submarine;

}; // vxCameraFlySettings


// $Log: vxCameraFlySettings.h,v $
// Revision 1.2  2007/03/06 19:20:11  gdavidson
// Added class comments
//
// Revision 1.1  2007/01/16 16:20:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCameraFlySettings.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $
// $Id: vxCameraFlySettings.h,v 1.2 2007/03/06 19:20:11 gdavidson Exp $

