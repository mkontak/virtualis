#include "StdAfx.h"

#ifndef _WIN64

#include "DirectInputEffect.h"

#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
DirectInputEffect::DirectInputEffect()  
{
}  // DirectInputEffect()


/**
 * Destructor
 */
DirectInputEffect::~DirectInputEffect()
{
  ClearEffects();
} // ~DirectInputEffect()


/**
 * Releases all the direct input effects
 */
void DirectInputEffect::ClearEffects()
{
  for( uint4 i = 0; i < m_diEffects.size(); ++i )
    m_diEffects[ i  ]->Release();
  m_diEffects.clear();
} // ClearEffects()



/**
 * Loads the effect from an .ffe file
 *
 * @param   sEffectFilename   full file name to .ffe file
 * @return  true upon success
 */
bool DirectInputEffect::LoadFromFile( CComPtr< IDirectInputDevice8 > & spDevice, const std::string & sEffectFile )
{
  HRESULT hr = S_OK;

  m_spDevice = spDevice;
  
  // enumerate the effects in the file selected, and create them in the callback
  if( FAILED( hr = m_spDevice->EnumEffectsInFile( sEffectFile.c_str(), EnumAndCreateEffects,  this, DIFEF_MODIFYIFNEEDED ) ) )
    return false;

  return true;
} // LoadFromFile( const std::string & sEffectFile )


/**
 * Callback function for configuring the selected input device's objects
 */
BOOL __stdcall DirectInputEffect::EnumAndCreateEffects( LPCDIFILEEFFECT piFileEffect, void * pContext )
{
  HRESULT hr( S_OK );

  DirectInputEffect * pThis = reinterpret_cast< DirectInputEffect * >( pContext );

  // create the file effect
  CComPtr< IDirectInputEffect > spEffect;
  if( FAILED( hr = pThis->m_spDevice->CreateEffect( piFileEffect->GuidEffect,  piFileEffect->lpDiEffect, &(spEffect), NULL ) ) )
  {
    LogErr( "Could not create force feedback effect on this device.", "DirectInputEffect", "EnumAndCreateEffects" );
    return DIENUM_CONTINUE;
  }

  pThis->m_diEffects.push_back( spEffect.p );

  spEffect.Detach();
  
  return DIENUM_CONTINUE;
} // EnumAndCreateEffects( const DIFILEEFFECT piFileEffect, void * pContext )


void DirectInputEffect::Play( uint1 uRepeatCount )
{
  for( uint4 i = 0; i < m_diEffects.size(); ++i )
    m_diEffects[ i ]->Start( uRepeatCount,DIES_SOLO );
}

#endif