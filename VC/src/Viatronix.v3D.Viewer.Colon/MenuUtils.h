// $Id: MenuUtils.h,v 1.2 2006/01/31 14:26:19 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// constants
const size_t iSizeOfMENUITEMINFO = sizeof(MENUITEMINFO);

inline void ResetItem(MENUITEMINFO & item, const UINT & uMask);
inline bool IsMenuItemSelected(HMENU hMenu, const UINT & uPos, const bool & bByPos = false);
inline BOOL CheckRadioItem(HMENU hMenu, WORD iLowHighPos, UINT uPos, const bool & bCheck, const bool & bByPos = false);
inline int GetMenuItemPosition(HMENU hMenu, const UINT & uId);

inline void ResetItem(MENUITEMINFO & item, const UINT & uMask)
{
  memset(&item, 0, iSizeOfMENUITEMINFO);
  item.cbSize = iSizeOfMENUITEMINFO;
  item.fMask = uMask;
}

inline bool IsMenuItemSelected(HMENU hMenu, const UINT & uPos, const bool & bByPos)
{
  UINT uState = GetMenuState(hMenu, uPos, (bByPos ? MF_BYPOSITION : MF_BYCOMMAND));
  return (uState & MFS_CHECKED) == MFS_CHECKED;
}

inline BOOL CheckRadioItem(HMENU hMenu, WORD iLowHighPos, UINT uPos, const bool & bCheck, const bool & bByPos)
{
  // We will do everything by position
  if (!bByPos)
    uPos = GetMenuItemPosition(hMenu, uPos);

  if (bCheck)
  {
    return CheckMenuRadioItem(hMenu, LOBYTE(iLowHighPos)-1, HIBYTE(iLowHighPos)-1,
                              uPos, MF_BYPOSITION);
  }
  else
  {
    MENUITEMINFO menuItem;    ResetItem(menuItem, MIIM_TYPE);
    GetMenuItemInfo(hMenu, uPos, TRUE, &menuItem);

    CheckMenuItem(hMenu, uPos, MF_BYPOSITION | MF_UNCHECKED);

    return FALSE;
  }
}

inline int GetMenuItemPosition(HMENU hMenu, const UINT & uId)
{
  for (int i = 0, iCount = GetMenuItemCount(hMenu); i < iCount; i++)
  {
    if (GetMenuItemID(hMenu, i) == uId)
      return i;
  }

  return -1;
}


// $Log: MenuUtils.h,v $
// Revision 1.2  2006/01/31 14:26:19  frank
// code review
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:39:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:42   ingmar
// Initial revision.
// Revision 1.3  2001/05/11 20:29:28  jmeade
// Coding conventions
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MenuUtils.h,v 1.2 2006/01/31 14:26:19 frank Exp $
// $Id: MenuUtils.h,v 1.2 2006/01/31 14:26:19 frank Exp $
