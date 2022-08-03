// $Id: WindowLevel.h,v 1.1.12.1 2007/10/04 20:03:23 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: name (email)
//
// Complete History at bottom of file

#ifndef WindowLevel_h_
#define WindowLevel_h_

// LW: Don't wont to include "global.h"
// Temporily put the prototype here.
template <class T> 
extern inline const T& Bound(const T&min, const T& val, const T& max);// { return(min<val) ? ((val<max)?val:max) : min;} // assumes min<max  //desired: min<=val<=max

extern inline float4 Interpolate(const float4 & fX1, const float4 & fX2, const float4 & fX3,
                          const float4 & fY1, const float4 & fY3);

///Temporarily put the definition of WindowLevel class  here
class WindowLevel 
{
public:
  /// default constructor
  WindowLevel()
  {
    m_fWindowHU = 1000.0F;
    m_fLevelHU = 0.0F;
  } // default constructor
  /// constructor
  WindowLevel(const float4 fHUWin, const float4 fHULev)
  {
    AdjustWindow(fHUWin); AdjustLevel(fHULev);
  } // constructor

  /// Returns intensity after transformation in the range (0,1).
  float4 GetIntensity(const float4 fHU) const
  {
    const float4 fLowerRange(m_fLevelHU - m_fWindowHU/2.0F);
    const float4 fUpperRange(m_fLevelHU + m_fWindowHU/2.0F);
    const float4 fTransformed(Interpolate(fLowerRange, fHU, fUpperRange, 0.0F, 1.0F));
    return Bound(0.0F, fTransformed, 1.0F);
  } // GetIntensity()
  /// set window
  void AdjustWindow(const float fHU)
  {
    m_fWindowHU = Bound(0.0f, m_fWindowHU + fHU, 2000.0F);
  } // AdjustWindow()
  /// set level
  void AdjustLevel(const float fHU)
  {
    m_fLevelHU = Bound(-1000.0f, m_fLevelHU + fHU, 1000.0F);
  } // AdjustLevel()
  /// get window
  const float4 & GetWindow() const
  {
    return m_fWindowHU;
  } // GetWindow()
  /// get level
  const float4 & GetLevel() const
  {
    return m_fLevelHU;
  } // GetLevel()
private:
  /// the window
  float4 m_fWindowHU;
  /// the level
  float4 m_fLevelHU;
}; // class WindowLevel

#endif

// $Log: WindowLevel.h,v $
// Revision 1.1.12.1  2007/10/04 20:03:23  jmeade
// 'set' constructor; comments.
//
// Revision 1.1  2005/08/05 12:54:58  geconomos
// moved from vx repository
//
// Revision 3.1  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/WindowLevel.h,v 1.1.12.1 2007/10/04 20:03:23 jmeade Exp $
// $Id: WindowLevel.h,v 1.1.12.1 2007/10/04 20:03:23 jmeade Exp $
