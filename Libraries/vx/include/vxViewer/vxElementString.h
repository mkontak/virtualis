// $Id: vxElementString.h,v 1.1.2.1 2009/07/28 14:41:45 kchalupa Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Cheng Qian  (mailto:cqian@viatronix.com)

#pragma once

// includes
#include "vxElement.h"


/**
 * A string for elements
 */
class VX_VIEWER_DLL vxElementString : public vxElement
{
// construction
public:

  /// constructor
  vxElementString() {;}
  
// member functions
public:

  /// returns value
  const std::string  & GetValue() const {  return m_str ; }

  /// sets value
  void SetValue( const std::string str ) { m_str.clear();  m_str.assign( str ); }

// member variables
private:

  /// value
  std::string  m_str;

}; // class vxElementString


// $Log: vxElementString.h,v $
// Revision 1.1.2.1  2009/07/28 14:41:45  kchalupa
// Code Walkthrough
//
// Revision 1.1  2008/03/20 16:49:53  cqian
// first time check in
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxElementString.h,v 1.1.2.1 2009/07/28 14:41:45 kchalupa Exp $
// $Id: vxElementString.h,v 1.1.2.1 2009/07/28 14:41:45 kchalupa Exp $
