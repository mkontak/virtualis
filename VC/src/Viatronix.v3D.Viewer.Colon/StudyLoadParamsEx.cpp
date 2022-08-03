// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: StudyLoadParamEx.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "StudyLoadParamsEx.h"


/**
 * constructor
 */
StudyLoadParamsEx::StudyLoadParamsEx() :
  m_eLoaderStyle( LOADER_STYLE_LEGACY )
{
} // StudyLoadParamsEx()


/**
 * destructor
 */
StudyLoadParamsEx::~StudyLoadParamsEx()
{
} // ~StudyLoadParamsEx()


/**
 * copy constructor
 *
 *  @param other
 */
StudyLoadParamsEx::StudyLoadParamsEx( const StudyLoadParamsEx & other ) : StudyLoadParams( other ),
  m_sUIDs( other.m_sUIDs ),
  m_eLoaderStyle( other.m_eLoaderStyle )
{
} // StudyLoadParamsEx


/**
 * assignment operator
 *
 *  @param rhs
 */
StudyLoadParamsEx & StudyLoadParamsEx::operator=( const StudyLoadParamsEx & rhs )
{
  if( this != &rhs )
  {
    StudyLoadParams::operator=( rhs );
    m_sUIDs = rhs.m_sUIDs;
    m_eLoaderStyle = rhs.m_eLoaderStyle;
  }

  return *this;
} // operator=( rhs )
