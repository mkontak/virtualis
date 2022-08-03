// $Id: VtxViewModes.inl,v 1.1 2005/08/05 12:54:58 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VtxViewModes.inl
// Description: these inline functions of class VtxViewModes are moved from ViatronStudy.inl. Bin Li
// VtxViewModes.inl: inline functions of Class VtxViewModes.
//    
// Created: April 10, 2001
// Author: Jeff Meade (jmeade@viatronix.net)
//
// Complete History at bottom of file.
//

VtxViewModes::VtxViewModes(const ViewModesValue &uModes, const ViewModesValue &uValid)
{
  m_uMode = uModes;
  SetValid(uValid);
}


VtxViewModes::VtxViewModes(const ViewModes::VIEWMODES &uModes, const ViewModes::VIEWMODES &uValid)
{
  m_uMode = uModes;
  SetValid(uValid);
}


VtxViewModes::VtxViewModes(const VtxViewModes &vtx)
{
  *this = vtx;
}

/// Set modes using VIEWMODES enum
inline void VtxViewModes::SetModes(const ViewModes::VIEWMODES &uModes, const ViewModes::VIEWMODES &uValid)
{
  m_uMode = uModes;
  SetValid(uValid);
}


VtxViewModes &VtxViewModes::operator=(const VtxViewModes &vtx)
{
  m_uMode = vtx.m_uMode;
  m_uValidModes = vtx.m_uValidModes;
  return *this;
}


bool VtxViewModes::Toggle(const ViewModes::VIEWMODES &uValue)
{
  if (*this == uValue)
  {
    *this -= uValue;
    return false;
  }
  else
  {
    *this += uValue;
    return true;
  }
}


void VtxViewModes::ClearMutexModes()
{
  m_uMode = (m_uMode & ViewModes::LOWMODES);
}


VtxViewModes::operator ViewModesValue() const
{
  return m_uMode;
}


bool VtxViewModes::operator !=(const ViewModes::VIEWMODES &uValue) const
{
  return !(*this == uValue);
}


bool VtxViewModes::operator &(const ViewModes::VIEWMODES &uValue) const
{
  if (IsIndependent(uValue))
  {
    // Simple bitwise-OR for independents
    return ((m_uMode & uValue)!=0);
  }
  else
  {
    // '==' on high-order bits for mutex
    return ((m_uMode & ViewModes::HIGHMODES) == ((uint8) uValue));
  }
}


bool VtxViewModes::operator ==(const ViewModes::VIEWMODES &uValue) const
{
  if (IsIndependent(uValue))
  {
    // Simple bitwise-OR for independents
    return ((m_uMode & uValue) == ((uint8) uValue));
  }
  else
  {
    // '==' on high-order bits for mutex
    return ((m_uMode & ViewModes::HIGHMODES) == ((uint8) uValue));
  }
}


bool VtxViewModes::operator !=(const VtxViewModes &modes) const
{
  return !(*this == modes);
}


bool VtxViewModes::operator ==(const VtxViewModes &modes) const
{
  return (this->m_uMode == modes.m_uMode);
}


uint8 VtxViewModes::operator &=(const ViewModes::VIEWMODES &uValue)
{
  return (*this += uValue);
}


/**
 * Mode set operator
 * NOTE: This is the basic set operator, i.e. any others (e.g. +=) should call this to keep code consistency.
 *
 * @param const ViewModes::VIEWMODES &uValue   Mode value
 * @return uint8                               New value
 */
uint8 VtxViewModes::operator +=(const ViewModes::VIEWMODES &uValue)
{
  if (IsIndependent(uValue))
  {
    return (m_uMode |= uValue);
  }
  else
  {
    return (m_uMode = (m_uMode & ViewModes::LOWMODES) + uValue);
  }
}


/**
 * Mode unset operator
 * NOTE: This is the basic unset operator, i.e. any others should call this to keep code consistency.
 *
 * @param const ViewModes::VIEWMODES &uValue   Mode value
 * @return uint8                               New value
 */
uint8 VtxViewModes::operator -=(const ViewModes::VIEWMODES &uValue)
{
  if (IsIndependent(uValue))
  {
    return (m_uMode &= ~uValue);
  }
  else
  {
    return (m_uMode &= ViewModes::LOWMODES);
  }
}


ViewModesValue VtxViewModes::GetIndependentModes() const
{
  return (m_uMode & ViewModes::LOWMODES);
}


ViewModesValue VtxViewModes::GetMutexModes() const
{
  return (m_uMode & ViewModes::HIGHMODES);
}


void VtxViewModes::SetValid(const ViewModesValue &uValue)
{
  m_uValidModes = (uValue & ViewModes::LOWMODES);
}


ViewModesValue VtxViewModes::GetValid() const
{
  return m_uValidModes;
}


inline bool VtxViewModes::IsIndependent(const ViewModes::VIEWMODES &uValue) const
{
  return (uValue < ViewModes::LOWHIGHBREAK);
}

// $Log: VtxViewModes.inl,v $
// Revision 1.1  2005/08/05 12:54:58  geconomos
// moved from vx repository
//
// Revision 3.2  2002/12/23 19:49:17  jmeade
// Cleanup, extending use of view modes class.
//
// Revision 3.1  2001/10/17 21:33:07  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:08   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.3  2001/06/06 19:41:11  jmeade
// Finally!!!!  Figured out this wierd and intermitent uint8 undefined type compiler
// error; tabs to spaces
//
// Revision 1.2  2001/04/11 11:23:32  frank
// Removed ulong, uint, ushort, uchar datatypes.
//
// Revision 1.1  2001/04/10 18:15:53  binli
// moved inline functions of class VtxViewModes from ViatronStudy to VtxViewModes.inl.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/VtxViewModes.inl,v 1.1 2005/08/05 12:54:58 geconomos Exp $
// $Id: VtxViewModes.inl,v 1.1 2005/08/05 12:54:58 geconomos Exp $
