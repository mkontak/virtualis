// $Id: ColorList.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ColorList.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (dachille@viatronix.com)

#ifndef ColorList_h
#define ColorList_h


// includes
#include "Triple.h"


/**
 * A list of unique colors that are designed to be cycled through.
 */
class ColorList
{
// Member Functions
public:

  /// constructor
  inline ColorList();
  
  /// destructor
  virtual ~ColorList() {}

  /// retrieves the next available color
  inline Triple<uint1> GetColor();

protected:

  /// resets all default colors to the collection
  inline void Reset();

// Data Members
private:

  /// list of colors
  std::list<Triple<uint1> > m_colors;

}; // class ColorList

#include "ColorList.inl"


#endif // ColorList_h


// $Log: ColorList.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/08 20:06:36  frank
// code review
//
// Revision 1.8  2003/05/08 20:02:54  frank
// code review
//
// Revision 1.7  2003/05/08 17:33:32  michael
// code review
//
// Revision 1.6  2003/05/06 16:18:48  frank
// code review
//
// Revision 1.5  2003/04/29 21:55:37  michael
// some initial code reviews
//
// Revision 1.4  2002/03/08 17:15:18  michael
// further removed include dependencies
//
// Revision 1.3  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 1.2  2001/11/09 19:32:48  geconomos
// Updated to make color list non-static
//
// Revision 1.1  2001/10/30 16:21:58  dmitry
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/ColorList.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: ColorList.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
