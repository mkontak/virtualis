// $Id: vxRendererOverlayPhaseVolume.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)

#ifndef vxRendererOverlayPhaseVolume_h
#define vxRendererOverlayPhaseVolume_h


// include declarations
#include "vxRendererOverlayMPROrtho.h"


// forward declarations
class vxEnvironment;


/*
 * Base class for the rendering of overlay text.  Holds functionality
 * common to 2D and 3D overlays.
 */
class VX_VIEWER_DLL vxRendererOverlayPhaseVolume : public vxRendererOverlayMPROrtho
{
// Member Functions
public:

  /// Constructor
  vxRendererOverlayPhaseVolume( vxEnvironment & environment );

  /// Destructor
  virtual ~vxRendererOverlayPhaseVolume() {}
  
  /// Render the overlay
  virtual void Render();

protected:
private:

  /// Copy constructor
  vxRendererOverlayPhaseVolume( const vxRendererOverlayPhaseVolume & );                   // should only be public if really implemented!

  /// Assignment operator
  const vxRendererOverlayPhaseVolume & operator =( const vxRendererOverlayPhaseVolume & ); // should only be public if really implemented!

  /// Get the phase information as a string.
  std::string GetPhaseInfoString();

}; // class vxRendererOverlayPhaseVolume


#endif // vxRendererOverlayPhaseVolume_h


// $Log: vxRendererOverlayPhaseVolume.h,v $
// Revision 1.2  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.1.1.18.1  2006/05/05 13:05:41  geconomos
// added virtual destructors
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/11/04 15:47:40  michael
// accounting for the changes in the overlay renderers (inheritance and naming)
//
// Revision 1.7  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.6  2003/05/19 14:40:22  frank
// Removed unused return parameter
//
// Revision 1.5  2003/01/21 22:16:03  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.4  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.3  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.2  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.1  2002/08/02 22:37:57  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRendererOverlayPhaseVolume.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
// $Id: vxRendererOverlayPhaseVolume.h,v 1.2 2007/02/15 00:08:24 jmeade Exp $
