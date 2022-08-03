// $Id: vxFontInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


#ifndef vxFontInfo_h
#define vxFontInfo_h


class VX_VIEWER_DLL vxFontInfo
{
/// member functions
public:
  
  /// default constructor
  vxFontInfo();
  
  /// constructor from size and family name.
  vxFontInfo( uint4 uSize, const std::string & sFamilyName );
  
  /// virtual destuctor
  virtual ~vxFontInfo();
  
  /// gets the size of the font in pixels
  uint4 GetSize() const { return m_uSize; }
  
  /// sets the size of the font in pixels
  void SetSize( uint4 uSize ) { m_uSize = uSize; }
  
  /// gets the font family name
  const std::string & GetFontFamily() const { return m_sFontFamily; }
  
  /// sets the font family name
  void SetFontFamily( const std::string &  sFontFamily ) { m_sFontFamily = sFontFamily; }

/// member variables
private:

  /// size of font
  uint4 m_uSize;
  
  /// font family name
  std::string m_sFontFamily;

}; // class vxFontInfo

#endif // vxFontInfo_h

// $Log: vxFontInfo.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/10/02 10:22:44  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxFontInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxFontInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $