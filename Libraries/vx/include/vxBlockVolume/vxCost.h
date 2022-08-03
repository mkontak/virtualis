// $Id: vxCost.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Ingmar ingmar@viatronix.com


#ifndef vxCost_h
#define vxCost_h

// includes
#include "31_vxVolumeIterators.h"
#include "Global.h"
#include "Typedef.h"
#include "vxComplex.h"
#include  "vxBlockVolumeIteratorNeighbors.h"

   
class vxCost
{
public:
  vxCost(int4 k=0) { m_cost=k; }
  vxCost(float4 k) { m_cost=k; }
  vxCost(const vxComplex & c) { m_cost=c.real(); }
  virtual vxCost & operator = (const vxCost & rhs)       { if (&rhs != this) m_cost=rhs.m_cost; return *this; }
  virtual operator float() const { return m_cost; }
  virtual operator vxComplex() const { return vxComplex(m_cost, 0); }
  virtual bool operator> (vxCost rhs) const { return (m_cost > rhs.m_cost) ; }
  virtual bool operator==(vxCost rhs) const { return (m_cost ==rhs.m_cost) ; }
  virtual bool operator!=(vxCost rhs) const { return (!(*this != rhs)) ; }
  virtual vxCost operator -(vxCost rhs) const { return vxCost(m_cost - rhs.m_cost) ; }
  virtual vxCost operator +(vxCost rhs) const { return vxCost(m_cost + rhs.m_cost) ; }
  virtual vxCost operator *(float4 fScalar) const { return vxCost(m_cost * fScalar) ; }
public:
  float4 m_cost;
}; // class vxCost
   

#endif // vxCost_h

// $Log: vxCost.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:48:13  michael
// code review
//
// Revision 1.1  2002/08/28 15:06:53  ingmar
// initial version
//
// $Id: vxCost.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
