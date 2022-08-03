// $Id: v3D_CameraFlySettings.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
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
#include "vxCameraFlySettings.h"


// namespaces
OPEN_VISUALIZATION_NS

public ref class CameraFlySettings
{
public:

  /// default constructor
  CameraFlySettings();

  /// destructor
  virtual ~CameraFlySettings();

  /// finalizer
  !CameraFlySettings();

  /// gets a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pCameraFlySettings ); }

  /// gets sets if camera flying is enabled
  property bool Enabled
  {
    bool get() { return m_pCameraFlySettings->IsEnabled(); }
    void set( bool bEnabled ) { m_pCameraFlySettings->SetEnabled( bEnabled ); }
  } // Enabled

  /// gets or set if auto flying is enabled
  property bool AutoFly
  {
    bool get() { return m_pCameraFlySettings->IsAutoFlyMode(); }
    void set( bool bAutoFly ) { m_pCameraFlySettings->SetAutoFlyMode( bAutoFly ); }
  } // AutoFly

  /// gets or sets the frames per second
  property int4 FramesPerSecond
  {
    int4 get() { return m_pCameraFlySettings->GetFramesPerSecond(); }
    void set( int4 iFramesPerSecond ) { m_pCameraFlySettings->SetFramesPerSecond( iFramesPerSecond ); }
  } // FramePerSecond

private:

  /// unmanaged settings
  vxCameraFlySettings * m_pCameraFlySettings;

}; // class CameraFlySettings


CLOSE_VISUALIZATION_NS


// $Log: v3D_CameraFlySettings.h,v $
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CameraFlySettings.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
// $Id: v3D_CameraFlySettings.h,v 1.1 2007/01/16 17:14:19 gdavidson Exp $

