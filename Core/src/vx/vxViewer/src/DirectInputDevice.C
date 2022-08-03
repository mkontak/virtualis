// $Id: DirectInputDevice.C,v 1.2 2007/03/02 17:19:14 geconomos Exp $
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
#include "DirectInputDevice.h"


/**
 * Constructor
 */
DirectInputDevice::DirectInputDevice() : 
  m_id( CLSID_NULL ),
  m_axes( false, false, false ),
  m_rotationalAxes( false, false, false ),
  m_iNumberOfSliders( 0 ),
  m_iNumberOfPOV( 0 )

{
} // DirectInputDevice()


/** 
 * Copy constructor
 * 
 * @param   other     other input device
 */
DirectInputDevice::DirectInputDevice( const DirectInputDevice & other ) : 
  m_id( other.m_id ),
  m_sInstanceName( other.m_sInstanceName ),
  m_sProductName( other.m_sProductName ),
  m_axes( other.m_axes ),
  m_rotationalAxes( other.m_rotationalAxes ),
  m_iNumberOfSliders( other.m_iNumberOfSliders ),
  m_iNumberOfPOV( other.m_iNumberOfPOV )
{
} // DirectInputDevice( const DirectInputDevice & other ) 


/**
 * Destructor
 */
DirectInputDevice::~DirectInputDevice()
{
} // ~DirectInputDevice()


/**
 * Assigment operator
 *
 * @param     rhs   other DirectInputDevice
 * @return    reference to self
 */
DirectInputDevice & DirectInputDevice::operator=( const DirectInputDevice & rhs )
{
  if( &rhs != this )
  {
    m_id = rhs.m_id;
    m_sInstanceName = rhs.m_sInstanceName;
    m_sProductName = rhs.m_sProductName;
    m_axes = rhs.m_axes;
    m_rotationalAxes = rhs.m_rotationalAxes;
    m_iNumberOfSliders = rhs.m_iNumberOfSliders;
    m_iNumberOfPOV = rhs.m_iNumberOfPOV;
  }
  return * this;
} // DirectInputDevice 


// $Log: DirectInputDevice.C,v $
// Revision 1.2  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.1  2005/10/21 12:54:43  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/DirectInputDevice.C,v 1.2 2007/03/02 17:19:14 geconomos Exp $
// $Id: DirectInputDevice.C,v 1.2 2007/03/02 17:19:14 geconomos Exp $
