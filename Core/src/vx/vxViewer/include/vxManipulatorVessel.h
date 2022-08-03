// $Id: vxManipulatorVessel.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 *  define the manipulator for the vessel
 */


// pragmas
#pragma once


// includes
#include "vxManipulator.h"


class VX_VIEWER_DLL vxManipulatorVessel : public vxManipulator
{
// Inner classes
public:

  /**
   * A class to help maintain the selection state
   * of the vessel measurement endpoints.
   */
  class SelectedSection
  {
  // Enumerations
  public:

    enum SelectionEnum { None, Start, End };

  // Member Functions
  public:

    /// Constructor
    SelectedSection() : m_eSelection( None ) {}

    /// Set selection state
    void SetSelection( const enum SelectionEnum eSelection ) { m_eSelection = eSelection; }

    /// Get selection state
    enum SelectionEnum GetSelection() const { return m_eSelection; }

  // Data Members
  public:

    /// The current selection
    enum SelectionEnum m_eSelection;

  }; // class SelectedSection


// Member Functions
public:

  /// Constructor.
  vxManipulatorVessel( vxEnvironment & environment );

  /// Destructor.
  ~vxManipulatorVessel();

  /// Retrieve the current section index
  static float4 GetVesselSectionIndex( vxEnvironment & environment );

  /// Set the current section index
  static void SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex );

  /// Get the offset distance of the detail volume
  static Point3Df GetDetailVolumeOffset( vxEnvironment & environment );

  /// sculpt out the table
  static void SculptTable( vxEnvironment & environment );

}; // class vxManipulatorVessel


// $Log: vxManipulatorVessel.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.2  2005/06/08 14:59:51  vxconfig
// added easy select object
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/05/07 20:59:46  dongqing
// code walkthrough
//
// Revision 1.3  2003/03/24 14:29:33  frank
// Moved some common code to a base class
//
// Revision 1.2  2003/03/14 17:28:11  frank
// Added a GetDetailVolumeOffset()
//
// Revision 1.1  2003/03/14 15:57:00  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorVessel.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxManipulatorVessel.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
