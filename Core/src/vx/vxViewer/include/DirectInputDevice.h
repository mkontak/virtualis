// $Id: DirectInputDevice.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once

// include declarations
#include "Triple.h"

// class declaration
class VX_VIEWER_DLL DirectInputDevice
{
// member functions
public:
  
  /// constructor
  DirectInputDevice();
  
  /// copy constructor
  DirectInputDevice( const DirectInputDevice & other );
  
  /// destructor
  virtual ~DirectInputDevice();
  
  /// gets the device id
  const GUID & GetId() const { return m_id; };
  
  /// sets the device id
  void SetId( const GUID & id ) { m_id = id; }
  
  /// gets the instance name
  const std::string & GetInstanceName() const { return m_sInstanceName; }
  
  /// sets the instance name
  void SetInstanceName( const std::string & sInstanceName ) { m_sInstanceName = sInstanceName; }

  /// gets the product name
  const std::string & GetProductName() const { return m_sProductName; }
  
  /// sets the product name
  void SetProductName( const std::string & sProductName ) { m_sProductName = sProductName; }
  
  /// gets the axes
  const Triple< bool > & GetAxes() const { return m_axes; }
  
  /// sets the axes
  void SetAxes( const Triple< bool > & axes ) { m_axes = axes; }
  
  /// gets the rotational axes
  const Triple< bool > & GetRotationalAxes() const { return m_rotationalAxes; }
  
  /// sets the rotational axes
  void SetRotationalAxes( const Triple< bool > & rotationalAxes ) { m_rotationalAxes = rotationalAxes; }
  
  /// gets the number of sliders
  int4 GetNumberOfSliders() const { return m_iNumberOfSliders; }

  /// sets the number of sliders
  void SetNumberOfSliders( int4 iNumberOfSliders ) { m_iNumberOfSliders = iNumberOfSliders; }

  /// gets the number of POV
  int4 GetNumberOfPOV() const { return m_iNumberOfPOV; }

  /// sets the number of POV
  void SetNumberOfPOV( int4 iNumberOfPOV ) { m_iNumberOfPOV = iNumberOfPOV; }

  /// assignment operator
  DirectInputDevice & operator=( const DirectInputDevice & rhs );

// member variables
private:

  // device id
  GUID m_id;
  
  // friendly name ( i.e. "Joystick1" )
  std::string m_sInstanceName;

  // product name
  std::string m_sProductName;
  
  // supported axes
  Triple< bool > m_axes;
  
  // supported rotational axes
  Triple< bool > m_rotationalAxes;
  
  // number of sliders
  int4 m_iNumberOfSliders;
  
  // number of POV caps
  int4 m_iNumberOfPOV;
  
}; // class DirectInputDevice


// $Log: DirectInputDevice.h,v $
// Revision 1.2  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.1  2005/10/21 12:54:29  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/DirectInputDevice.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
// $Id: DirectInputDevice.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
