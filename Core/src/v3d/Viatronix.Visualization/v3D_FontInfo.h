// $Id: v3D_FontInfo.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxFontInfo.h"

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public ref class FontInfo : public Viatronix::Visualization::IElement
{
// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pFontInfo ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// member functions
public:
  
  /// initializes a FontInfo object with default values.
  FontInfo();
  
  /// destructor
  virtual ~FontInfo();

  /// finalizer
  !FontInfo();
  
  /// gets the font size
  property int Size
  {
    int get() { return m_pFontInfo->GetSize(); }
    void set( int iSize );
  }
    
  /// gets the font family name
  property System::String ^ FontFamily
  {
    System::String ^ get() { return gcnew System::String( m_pFontInfo->GetFontFamily().c_str() ); }
    void set( System::String ^ mpFontFamily );
  }

// member variables
private:

  // underlying implemetation
  vxShareableObject< vxFontInfo > * m_pFontInfo;

}; // class Font

CLOSE_VISUALIZATION_NS

// $Log: v3D_FontInfo.h,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2004/10/04 15:10:34  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/10/02 10:26:11  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_FontInfo.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_FontInfo.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
