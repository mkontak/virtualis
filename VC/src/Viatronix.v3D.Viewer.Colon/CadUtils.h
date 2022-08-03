// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CadUtils.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net


#pragma once


#include "ColonSession.h"


// forward declarations
namespace ReaderLib
{
  class Dataset;
}

namespace vxCAD
{
  class vxCadFinding;
}


OPEN_UTILITIES_NS

namespace CAD
{

  /// Jump the viewpoint to a finding
  void JumpTo( ReaderLib::Dataset & dataset, const std::string & sID );

  /// Set display setting on a CAD finding
  void SetDisplayDecision( ReaderLib::Dataset & dataset, const std::string & sID, ColonSession::FindingInfo::DecisionEnum eDecision );

  /// selects next finding
  void NextFinding( ReaderLib::Dataset & dataset );

  /// selects previous finding
  void PreviousFinding( ReaderLib::Dataset & dataset );

  /// toggles the visibility of all findings
  void ToggleVisibility();

  /// refreshes the findings
  void Refresh();

  namespace Helper
  {
    /// helper function for jumpin to a finding
    void JumpTo( ReaderLib::Dataset & dataset, const vxCAD::vxCadFinding & finding );

    /// helper function for jumping to the next visible finding
    bool JumpToVisibleFinding( ReaderLib::Dataset & dataset, int4 index );
  }

} // namespace CAD


CLOSE_UTILITIES_NS