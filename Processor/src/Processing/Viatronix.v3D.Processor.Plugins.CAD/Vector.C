// $Id: Vector.C,v 1.5 2007/03/15 13:11:43 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "Vector.h"
#include "vxCadVector.h"

using namespace vxCAD;

/**
 * Default Constructor
 */
Vector::Vector() : m_pVector ( NULL )
{
} // Vector()


/**
 * Gets the X component
 *
 * @param   pfx       contains the x component upon return
 * @return            S_OK for success
 */
STDMETHODIMP Vector::get_X( float * pfx )
{
	if( !pfx)
    return E_INVALIDARG;
  *pfx = m_pVector->GetVector().m_x;

  return S_OK;
} // get_X()


/**
 * Sets the X component
 *
 * @param   fx        x component
 * @return            S_OK for success
 */
STDMETHODIMP Vector::put_X( float fx )
{
  Vector3D<float4> v = m_pVector->GetVector();
  v.m_x = fx;
  m_pVector->SetVector( v );
  return S_OK;
} // put_X()


/**
 * Gets the Y component
 *
 * @param   pfy       contains the y component upon return
 * @return            S_OK for success
 */
STDMETHODIMP Vector::get_Y( float * pfy )
{
	if( !pfy )
    return E_INVALIDARG;
  *pfy = m_pVector->GetVector().m_y;

  return S_OK;
} // get_Y()


/**
 * Sets the Y component
 *
 * @param   fy        y component
 * @return            S_OK for success
 */
STDMETHODIMP Vector::put_Y( float fy )
{
  Vector3D<float4> v = m_pVector->GetVector();
  v.m_y = fy;
  m_pVector->SetVector( v );
  return S_OK;
} // put_Y()


/**
 * Gets the Z component
 *
 * @param   pfz       contains the z component upon return
 * @return            S_OK for success
 */
STDMETHODIMP Vector::get_Z( float * pfz )
{
	if( !pfz )
    return E_INVALIDARG;
  *pfz = m_pVector->GetVector().m_z;

  return S_OK;
} // get_Z()


/**
 * Sets the Z component
 *
 * @param   fz        z component
 * @return            S_OK for success
 */
STDMETHODIMP Vector::put_Z( float fz )
{
  Vector3D<float4> v = m_pVector->GetVector();
  v.m_z = fz;
  m_pVector->SetVector( v );
  return S_OK;
} // put_Z()


/**
 * Gets the units
 *
 * @param   pUnits    contains the units upon return
 * @return            S_OK for success
 */
STDMETHODIMP Vector::get_Units( VectorUnits * pUnits )
{
	if( !pUnits )
    return E_INVALIDARG;
  *pUnits = static_cast< VectorUnits >( m_pVector->GetUnits() );
  return S_OK;
} // get_Units()


/**
 * Sets the units
 *
 * @param   eUnits    vector units
 * @return            S_OK for success
 */
STDMETHODIMP Vector::put_Units( VectorUnits eUnits )
{
  m_pVector->SetUnits( static_cast< vxCadVector::Units >( eUnits ) );
  return S_OK;
} // put_Units( VectorUnits eUnits )


/**
 * Sets the x, y and Z component
 *
 * @param   fx        x component
 * @param   fy        y component
 * @param   fz        z component
 * @return            S_OK for success
 */
STDMETHODIMP Vector::Set( float fx, float fy, float fz )
{
  Vector3D<float4> v( fx, fy, fz );
  m_pVector->SetVector( v );
  return S_OK;
} // Set( float fx, float fy, float fz )


// $Log: Vector.C,v $
// Revision 1.5  2007/03/15 13:11:43  geconomos
// corrected compilation errors due  new vxCAD naemspace in vxCAD library
//
// Revision 1.4  2007/03/01 20:40:36  geconomos
// code review preparation
//
// Revision 1.3  2005/09/30 19:54:02  geconomos
// added VectorUnits
//
// Revision 1.2  2005/09/27 00:19:14  geconomos
// updated for changes made to vxCadVector
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Vector.C,v 1.5 2007/03/15 13:11:43 geconomos Exp $
// $Id: Vector.C,v 1.5 2007/03/15 13:11:43 geconomos Exp $
