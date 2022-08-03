// $Id: DirectInput.h,v 1.6 2007/11/06 16:54:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george Economos (mailto:george@viatronix.com)

// pragma declarations

#ifndef _WIN64

#pragma once
#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\directx\\lib\\dxguid.lib" )
#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\directx\\lib\\dinput8.lib" )
#pragma comment( lib, "..\\..\\..\\..\\Libraries\\libs\\directx\\lib\\dxerr9.lib" )


// define declarations
#define DIRECTINPUT_VERSION 0x0800

// include declarations
#include <dinput.h>
#include <atlbase.h>
#include "DirectInputDevice.h"

// class declarations
class VX_VIEWER_DLL DirectInput
{
// member functions
public:
  
  /// constructor
  DirectInput();
  
  /// destructor
  virtual ~DirectInput();
  
  /// initializes the direct input runtime and enumerates all attached input deivces
  bool Initialize( HINSTANCE hInstance, HWND hWnd );
  
  /// selects the specified input device
  bool SetInputDevice( const DirectInputDevice & device );

  /// gets the selected input device
  CComPtr< IDirectInputDevice8 > & GetSelectedDevice() { return m_spSelectedDevice; }

  /// gets the selected input device capabilities
  DIDEVCAPS & GetSelectedDeviceCaps() { return m_selectedDeviceCaps; }
  
  /// obtains access to the device
  bool AcquireDevice();
  
  /// gets the current state of the input device
  bool PollDevice( DIJOYSTATE2 & state );

  /// gets the available input devices attached to the computer
  const std::vector< DirectInputDevice > & GetAvailableDevices() const { return m_devices; }

  /// logs the specified error message
  static bool LogError( HRESULT hr, const std::string & sMsg, const std::string & sClass, const std::string & sFunction );

private:

  /// callback function for enumerating input devices
  static BOOL __stdcall  EnumDevices( LPCDIDEVICEINSTANCE pDevice, void * pContext );
  
  /// callback function for enumerating device instance objects
  static BOOL __stdcall EnumDeviceObjects( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext );
  
  /// callback function for configuring the selected input device's objects
  static BOOL __stdcall EnumDeviceObjectsSetRanges( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext );


// member variables
private:

  // direct input 
  CComPtr< IDirectInput8 > m_spDirectInput;
  
  // available devices 
  std::vector< DirectInputDevice > m_devices;

  // input device
  CComPtr< IDirectInputDevice8 > m_spSelectedDevice;

  // device capabilities;
  DIDEVCAPS m_selectedDeviceCaps;
  
  // instance handle to application
  HINSTANCE m_hInstance;
  
  // window handle to be associated with the device
  HWND m_hWnd;

}; // class DirectInput

#endif

// $Log: DirectInput.h,v $
// Revision 1.6  2007/11/06 16:54:04  geconomos
// updated for force feedback
//
// Revision 1.5  2007/11/02 13:29:20  geconomos
// added query for device capabilities
//
// Revision 1.4  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.3  2006/02/09 14:05:19  geconomos
// added #pragma comment for directx related libs
//
// Revision 1.2  2005/10/21 13:02:20  geconomos
// code cleanup
//
// Revision 1.1  2005/10/21 12:54:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/DirectInput.h,v 1.6 2007/11/06 16:54:04 geconomos Exp $
// $Id: DirectInput.h,v 1.6 2007/11/06 16:54:04 geconomos Exp $
