// $Id: vxInputFeedback.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

/**
 * This class is used to pass modified information of a client class to objects sinked to its'
 * shareable encapsulation.
 */

#ifndef vxInputFeedback_h
#define vxInputFeedback_h


// includes
#include <list>
#include <typeinfo>


// class definition
class vxFeedback
{
// enums
public:

  /// type of action
  enum ActionType
  {
    NONE = 0,
    BUSY = 1,
    DONE = 2
  };

// functions
public:

  /// constructor
  vxFeedback(const type_info & info, ActionType eAction)
    : m_info(info),
      m_eAction(eAction)
  {};
  
  /// returns the id
  const type_info & GetId() { return m_info; }

  /// returns the action type
  ActionType GetActionType() { return m_eAction; }

// data
private:

  /// info
  const type_info & m_info;

  /// action
  ActionType m_eAction;
}; // vxFeedback



// class definition
class VX_VIEWER_DLL vxInputFeedback
{
// functions
public:

  /// default constructor
  vxInputFeedback() {}
  
  /// destructor
  virtual ~vxInputFeedback();

  /// add feedback into list
  void Add(vxFeedback & feedback);

  /// returns the list of feedback information
  std::list<vxFeedback> & GetList();

  /// clear list of feedback information
  void Clear();

private:	
  
  /// copy constructor
  vxInputFeedback(vxInputFeedback & other);             // should only be public if really implemented!

  /// assignment operator
  vxInputFeedback & operator=(vxInputFeedback & other); // should only be public if really implemented!

// data
private:
  
  // contains the modified information
  std::list<vxFeedback> m_feedbackList;
}; // class vxInputFeedback


#endif // vxInputFeedback_h


// Revision History:
// $Log: vxInputFeedback.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/25 12:48:28  michael
// added "mailto:"
//
// Revision 1.5  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.4  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.3  2003/04/18 19:56:30  michael
// initial code review
//
// Revision 1.2  2002/10/17 22:16:46  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.1  2002/06/13 23:24:24  michael
// added vxInputFeedback class so that manipulators (input receivers) can send
// a message to a global queue which VB can read to remove them from the list
// of receivers upon completion of operation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxInputFeedback.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxInputFeedback.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
