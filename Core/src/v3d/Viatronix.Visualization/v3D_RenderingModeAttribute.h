// $Id: v3D_RenderingModeAttribute.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_Enums.h"


OPEN_VISUALIZATION_NS


[ System::AttributeUsageAttribute(System::AttributeTargets::Property, AllowMultiple=false) ]
public ref class RenderingModeAttribute : public System::Attribute
{
// member functions
public:
  
  /// initializes a RenderingModeAttribute with the specified display text
  RenderingModeAttribute( System::String ^ mpDisplayText, Viatronix::Visualization::RenderingOptions mode );
  
  /// gets the display text of the rendering mode
  property System::String ^ DisplayText 
  {
    System::String ^ get() { return m_mpDisplayText; }
    void set( System::String ^ mpDisplayText ) { m_mpDisplayText = mpDisplayText; }
  }
    
  /// gets the rendering options
  property Viatronix::Visualization::RenderingOptions Options
  {
    Viatronix::Visualization::RenderingOptions get() { return m_eOptions; }
    void set( Viatronix::Visualization::RenderingOptions eOptions ) { m_eOptions = eOptions; }
  }
  
// member variables
private:

  // name of rendering mode attribute
  System::String ^ m_mpDisplayText;
  
  // rendering mode bit
  Viatronix::Visualization::RenderingOptions m_eOptions;
  
}; // class RenderingModeAttribute

CLOSE_VISUALIZATION_NS

// $Log: v3D_RenderingModeAttribute.h,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.2  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.1  2003/10/15 01:26:52  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingModeAttribute.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_RenderingModeAttribute.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $