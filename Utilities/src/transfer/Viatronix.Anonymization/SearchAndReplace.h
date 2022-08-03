// $Id: SearchAndReplace.h,v 1.6 2007/03/12 14:43:20 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: [Mark Kontak] [mkontak@viatronix.com]
// Co-Author: [Romy Sreedharan] [ romy@viatronix.com]

// pragmas
#pragma once

using namespace System;

OPEN_ANMZN_NS 

/**
 * Implements a string pair for the searching and replacing
 */
public ref class SearchAndReplace
{
public:

  /// Constructor
  SearchAndReplace(String ^ sSearch, String ^ sReplace) : m_sSearch(sSearch), m_sReplace(sReplace) { }

  /// Returns a string representation of this object
  virtual  String ^ ToString() override
  { return String::Format("( {0} / {1} )", m_sSearch, m_sReplace);  }

public:

  /// Returns the search string
  property String ^ Search
  { 
    String ^ get() { return m_sSearch; }
  }

  /// Returns the replace string
  property String ^ Replace
  { 
    String ^ get() { return m_sReplace; }
  }

private:

  /// Search string
  String ^ m_sSearch;

  /// Replace string
  String ^ m_sReplace;
};  // class SearchAndReplace

CLOSE_ANMZN_NS


// $Log: SearchAndReplace.h,v $
// Revision 1.6  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/SearchAndReplace.h,v 1.6 2007/03/12 14:43:20 mkontak Exp $
// $Id: SearchAndReplace.h,v 1.6 2007/03/12 14:43:20 mkontak Exp $

