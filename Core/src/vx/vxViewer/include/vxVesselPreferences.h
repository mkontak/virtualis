// $Id: vxVesselPreferences.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once
#include "vxElement.h"

class VX_VIEWER_DLL vxVesselPreferences : public vxElement
{
// Enumerations
public:

  // The different types of events.
  enum EventsEnum
  {
    EVENT_MODIFIED = 0x00000001,
  }; // enum EventsEnum

// Member Functions
public:

  /// constructor
  vxVesselPreferences();

  /// get display centerline
  const bool & GetDisplayCenterline() { return m_bDisplayCenterline; }
  
  /// set display centerline
  void SetDisplayCenterline( bool bDisplayCenterline ) { m_bDisplayCenterline = bDisplayCenterline; }

  /// get display outline
  const bool & GetDisplayOutline() { return m_bDisplayOutline; }

  /// set display outline
  void SetDisplayOutline( bool bDisplayOutline ) { m_bDisplayOutline = bDisplayOutline; }

  /// get display graphs
  const bool & GetDisplayGraphs() { return m_bDisplayGraphs; }

  /// set display graphs
  void SetDisplayGraphs( bool bDisplayGraphs ) { m_bDisplayGraphs = bDisplayGraphs; }

   /// get the arc length along which to measure tortuosity on the centerline
  static float4 GetToruosityArcLengthMM() { return m_iTortuosityMeasurementArcLengthMM; }

  /// get the ability to rotate the curved MPR view
  static bool GetRotatingCurvedMPR() { return m_bRotateCurvedMPR; }

  /// get the ability to rotate the curved MPR view
  static void SetRotatingCurvedMPR( bool bRotateCurvedMPR ) { m_bRotateCurvedMPR = bRotateCurvedMPR; }

  /// reset to defaults
  void Reset();

// Data Members
private:

  /// should we display the vessel outline
  bool m_bDisplayOutline;

  /// should we display the vessel graphs
  bool m_bDisplayGraphs;

  /// should we display the vessel centerline
  bool m_bDisplayCenterline;

  /// should the curved MPR be allowed to rotate
  static bool m_bRotateCurvedMPR;

  /// the arc length along which to measure tortuosity on the vessel centerline.
  static const int m_iTortuosityMeasurementArcLengthMM = 20; // only integral types are allowed (this is good enough)

}; // class vxVascularPreferences


// $Log: vxVesselPreferences.h,v $
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVesselPreferences.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
// $Id: vxVesselPreferences.h,v 1.1 2006/08/17 14:02:36 gdavidson Exp $
