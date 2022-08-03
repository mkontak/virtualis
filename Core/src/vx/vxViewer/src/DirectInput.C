// $Id: DirectInput.C,v 1.7 2007/11/23 15:05:35 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#ifndef _WIN64
#include "DirectInput.h"

#define FILE_REVISION "$Revision: 1.7 $"

/**
 * Constructor
 */
DirectInput::DirectInput() : 
 m_hInstance( NULL ),
 m_hWnd( NULL )
{
  memset( &m_selectedDeviceCaps, 0, sizeof( m_selectedDeviceCaps ) );
} // DirectInput()


/**
 * Destructor
 */
DirectInput::~DirectInput()
{
  m_spSelectedDevice = NULL;
  m_spDirectInput = NULL;
} // ~DirectInput()


/**
 * Initializes the direct input runtime and enumerates all attached input deivces
 *
 * @param     hInstance     instance handle to the application
 * @param     hWnd          window handle to be associated with the device
 * @return    true on success
 */
bool DirectInput::Initialize( HINSTANCE hInstance, HWND hWnd )
{
  HRESULT hr( S_OK );
  
  // store the initialization parameters
  m_hInstance = hInstance;
  m_hWnd = hWnd;
	
	// create an direct input instance
  if FAILED( hr = DirectInput8Create( m_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast< void** >( &m_spDirectInput ), NULL ) ) 
    return DirectInput::LogError( hr, "Unable to create direct input instance", "DirectInput", "Initialize" );

  // enumerate the attached devices
  if( FAILED( hr = m_spDirectInput->EnumDevices( DI8DEVCLASS_GAMECTRL,  &DirectInput::EnumDevices, this, DIEDFL_ATTACHEDONLY ) ) )
    return DirectInput::LogError( hr, "Error enumerating input devices", "DirectInput", "Initialize" );

  return true;
} // Initialize( HINSTANCE hInstance, HWND hWnd )


/**
 * Selects the specified input device
 *
 * @param   device    input device to select
 * @return  true on sucess
 */
bool DirectInput::SetInputDevice( const DirectInputDevice & device )
{
  HRESULT hr( S_OK );

  // has initialize been called?
  if( !m_spDirectInput )
    return DirectInput::LogError( E_FAIL, "DeviceInput is NULL. ( Have you called initialize? )", "DirectInput", "SelectInputDevice"  );

  if( m_spSelectedDevice )
  {
    m_spSelectedDevice->Unacquire();
    m_spSelectedDevice = NULL;
  }

  // create an instance of the joystick
  if( FAILED( hr = m_spDirectInput->CreateDevice( device.GetId(), &m_spSelectedDevice, NULL ) ) ) 
    return DirectInput::LogError( hr, "Unable to create input device instance", "DirectInput", "SelectInputDevice" );

  // query capabilities
  m_selectedDeviceCaps.dwSize = sizeof( m_selectedDeviceCaps );
  if( FAILED( hr = m_spSelectedDevice->GetCapabilities( &m_selectedDeviceCaps )) )
    return DirectInput::LogError( hr, "Unable to query selected device capabilities", "DirectInput", "SelectInputDevice" );

  // set the data format for the device
  if( FAILED( hr = m_spSelectedDevice->SetDataFormat( &c_dfDIJoystick2 ) ) )
    return DirectInput::LogError( hr, "Error setting input device data format", "DirectInput", "SelectInputDevice" );
    
  // set the cooperative level
  if( FAILED( hr = m_spSelectedDevice->SetCooperativeLevel( m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
    return DirectInput::LogError( hr, "Error setting the input device cooperative level", "DirectInput", "SelectInputDevice" );
    
  // enumerate the joystick objects
  if( FAILED( hr = m_spSelectedDevice->EnumObjects( &DirectInput::EnumDeviceObjectsSetRanges, this, DIDFT_ALL ) ) )
    return DirectInput::LogError( hr, "Unable to enumerate device objects", "DirectInput", "EnumDevices" );
    
  return true;

} // SetInputDevice( const DirectInputDevice & device )

/**
 * Obtains access to the input device
 * 
 * @return    true if device was obtained
 */
bool DirectInput::AcquireDevice()
{
  // have we chosen a input device?
  if( !m_spSelectedDevice )
    return DirectInput::LogError( E_FAIL, "Device is NULL", "DirectIput", "AcquireDevice"  );
  
  // acquire the device
  HRESULT hr( m_spSelectedDevice->Acquire() );
  if( hr == DI_OK  || hr == S_FALSE )
    return true;    
  
  return DirectInput::LogError( hr, "Unable to acquire input device", "DirectInput", "AcquireDevice"  );
} // AcquireDevice()


/**
 * Gets the current state of the input device
 *
 * @param   
 *
 * @return    true of success
 */
bool DirectInput::PollDevice(  DIJOYSTATE2 & state )
{
  HRESULT hr( S_OK );

  // do we have a valid device?
  if( !m_spSelectedDevice ) 
    return DirectInput::LogError( E_FAIL, "Device is NULL. ( Have you called initialize? )", "DirectIput", "PollDevice"  );

  // poll the device to read the current state
  if( FAILED( hr = m_spSelectedDevice->Poll() ) )  
  {
    // the input stream has been lost try and reacquire
    while( m_spSelectedDevice->Acquire() == DIERR_INPUTLOST ) 

    // hr may be DIERR_OTHERAPPHASPRIO or other errors, try again later 
    return true; 
  }

  // get the input device state
  if( FAILED( hr = m_spSelectedDevice->GetDeviceState( sizeof( DIJOYSTATE2 ), &state ) ) )
    return DirectInput::LogError( hr, "Error retrieving input device state", "DirectInput", "PollDevice" );

  return true;
} // PollDevice( DIJOYSTATE2 & state )


/**
 * Callback function for IDirectInput8::EnumDevices()
 *
 * @param     pDeviceInstance     describes the device instance
 * @param     pContext               application defined value
 * @return    returns DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop the enumeration
 */
BOOL __stdcall  DirectInput::EnumDevices( LPCDIDEVICEINSTANCE pDevice, void * pContext )
{
  HRESULT hr( S_OK );
  
  // blit data pointer to DirectInput instance
  DirectInput * pThis = static_cast< DirectInput * >( pContext );
  
  DirectInputDevice device;
  device.SetId( pDevice->guidInstance );
  device.SetInstanceName( pDevice->tszInstanceName );
  device.SetProductName( pDevice->tszProductName );
  
  // create an instance of the joystick
  CComPtr< IDirectInputDevice8 > spDevice;
  if( FAILED( hr = pThis->m_spDirectInput->CreateDevice( pDevice->guidInstance, &spDevice, NULL ) ) ) 
  {
    DirectInput::LogError( hr, "Unable to create input device instance", "DirectInput", "EnumDevices" );
    return DIENUM_CONTINUE;
  }
  
  // enumerate the joystick objects
  if( FAILED( hr = spDevice->EnumObjects( &DirectInput::EnumDeviceObjects, reinterpret_cast< void * >( &device ), DIDFT_ALL ) ) )
  {
    DirectInput::LogError( hr, "Unable to enumerate device objects", "DirectInput", "EnumDevices" );
    return DIENUM_CONTINUE;
  }
  
  pThis->m_devices.push_back( device );
  
  return DIENUM_CONTINUE;
} // EnumDevices( LPCDIDEVICEINSTANCE pDevice, LPVOID pData )


/**
 * Callback function for enumerating the various components of an input device
 *
 * @param     pDevice       information about a device instance
 * @param     pContext      additional data supplied to the callback
 * @return    returns DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop the enumeration
 */
BOOL __stdcall DirectInput::EnumDeviceObjects( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext )
{
  DirectInputDevice & device = *reinterpret_cast< DirectInputDevice * >( pContext );
  
  if( pDeviceObject->guidType == GUID_XAxis || pDeviceObject->guidType == GUID_YAxis || pDeviceObject->guidType == GUID_ZAxis )
  {
    Triple< bool > axes = device.GetAxes();
    axes.m_x |= pDeviceObject->guidType == GUID_XAxis? true : false;
    axes.m_y |= pDeviceObject->guidType == GUID_YAxis? true : false;
    axes.m_z |= pDeviceObject->guidType == GUID_ZAxis? true : false;
    
    device.SetAxes( axes );
  }

  else if( pDeviceObject->guidType == GUID_RxAxis || pDeviceObject->guidType == GUID_RyAxis || pDeviceObject->guidType == GUID_RzAxis )
  {
    Triple< bool > axes = device.GetRotationalAxes();
    axes.m_x |= pDeviceObject->guidType == GUID_RxAxis? true : false;
    axes.m_y |= pDeviceObject->guidType == GUID_RyAxis? true : false;
    axes.m_z |= pDeviceObject->guidType == GUID_RzAxis? true : false;
    device.SetRotationalAxes( axes );
  }
    
  else if( pDeviceObject->guidType == GUID_Slider )
  {
    device.SetNumberOfSliders( device.GetNumberOfSliders() + 1 );
  }
  
  else if( pDeviceObject->guidType == GUID_POV )
  {
    device.SetNumberOfPOV( device.GetNumberOfPOV() + 1 );
  }

  return DIENUM_CONTINUE;
} // EnumDeviceObjects( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext )


/**
 * Callback function for configuring the selected input device's objects
 *
 * @param     pDevice       information about a device instance
 * @param     pContext      additional data supplied to the callback
 * @return    returns DIENUM_CONTINUE to continue the enumeration or DIENUM_STOP to stop the enumeration
 */
BOOL __stdcall DirectInput::EnumDeviceObjectsSetRanges( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext )
{
  // blit data pointer to DirectInput instance
  DirectInput * pThis = static_cast< DirectInput * >( pContext );
  
  // set the range for the axes
  // TODO: read range from configuration
  if( pDeviceObject->dwType & DIDFT_AXIS )
  {
    DIPROPRANGE range; 
    range.diph.dwSize       = sizeof( DIPROPRANGE ); 
    range.diph.dwHeaderSize = sizeof( DIPROPHEADER ); 
    range.diph.dwHow        = DIPH_BYID; 
    range.diph.dwObj        = pDeviceObject->dwType;
    range.lMin              = -1000; 
    range.lMax              = +1000; 

    // set the range for the axis
    if( FAILED( pThis->m_spSelectedDevice->SetProperty( DIPROP_RANGE, &range.diph ) ) ) 
      return DIENUM_STOP;
  }
  return DIENUM_CONTINUE;
} // EnumDeviceObjectsSetRanges( const DIDEVICEOBJECTINSTANCE * pDeviceObject, void * pContext )




/**
 * Log the specified error mesage
 *
 * @param   hr        resultant error code
 * @param   sMsg      error message
 * @param   sClass    class name
 * @param   sFunction    function name
 */
bool DirectInput::LogError( HRESULT hr, const std::string & sMsg, const std::string & sClass, const std::string & sFunction )
{
	// TODO: Write to logger
  char s[ 255 ];
  wsprintf( s, "%s\n", sMsg.c_str() );
  return false;
} // LogError( const std::string & sMsg, HRESULT hr )

#endif
// $Log: DirectInput.C,v $
// Revision 1.7  2007/11/23 15:05:35  geconomos
// added ability to select joystick at runtime
//
// Revision 1.6  2007/11/06 16:54:27  geconomos
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
// Revision 1.1  2005/10/21 12:54:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/DirectInput.C,v 1.7 2007/11/23 15:05:35 geconomos Exp $
// $Id: DirectInput.C,v 1.7 2007/11/23 15:05:35 geconomos Exp $
