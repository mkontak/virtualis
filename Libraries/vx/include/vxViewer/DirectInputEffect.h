#pragma once

#ifndef _WIN64


// define declarations
#define DIRECTINPUT_VERSION 0x0800

// include declarations
#include <dinput.h>
#include <atlbase.h>
#include "DirectInputDevice.h"

/**
 * Represents a force feedback effect composed of one or more IDirectInputEffect instances
 */
class VX_VIEWER_DLL DirectInputEffect
{
// member functions
public:
  
  /// constructor
  DirectInputEffect();
  
  /// destructor
  virtual ~DirectInputEffect();

  /// loads the direct input files from an ffe file
  bool LoadFromFile( CComPtr< IDirectInputDevice8 > & spDevice, const std::string & sEffectFile );

  /// release all the direct input effects
  void ClearEffects();

  void Play( uint1 uRepeatCount );


private:

  /// callback function for configuring the selected input device's objects
  static BOOL __stdcall EnumAndCreateEffects( LPCDIFILEEFFECT pDIFileEffect, void * pContext );

// member variables
private:

  /// direct input device
  CComPtr< IDirectInputDevice8 > m_spDevice;

  // direct input effects composing this effect
  std::vector< IDirectInputEffect * > m_diEffects;

}; // class DirectInputEffect


#endif // #ifndef _WIN64