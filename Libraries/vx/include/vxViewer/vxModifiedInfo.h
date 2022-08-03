// $Id: vxModifiedInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

/**
 *  This class is used to pass modified information of a client class to objects sinked to its'
 *  shareable encapsulation.
 */

#ifndef vxModifiedInfo_h
#define vxModifiedInfo_h


// includes
#include "vxIDs.h"


// defines
#define vxModInfo(PARENT, FLAG) (vxModifiedInfo(typeid(PARENT), "", FLAG))
#define vxModInfoTxt(PARENT, FLAG, TXT) (vxModifiedInfo(typeid(PARENT), "", FLAG, TXT))
#define vxModInfoID(PARENT, PARENT_ID, FLAG) (vxModifiedInfo(typeid(PARENT), PARENT_ID, FLAG))


// class definition
class VX_VIEWER_DLL vxModifiedInfo 
{
// functions
public:

  /// constructor
  vxModifiedInfo(const type_info & typeinfo, const vxID & id, uint4 uFlag = 0, std::string sText = "");

  /// copy constructor
  vxModifiedInfo(const vxModifiedInfo & other);

  /// destructor
  virtual ~vxModifiedInfo() {};

  /// creates a copy of this object on the heap
  virtual void Clone(vxModifiedInfo ** ppObject) const;

  /// returns the modified information flag
  uint4 GetFlag() const { return m_uFlag; }

  /// sets the modified information flag 
  void SetFlag(const uint4 uFlag) { m_uFlag = uFlag; }

  /// appends the modified information flag
  void AddFlag(const uint4 uFlag);

  /// clears the modified information flag
  void Clear() { m_uFlag = 0; }

  /// returns the text information
  const std::string & GetText() const { return m_sText; }

  /// sets the text information
  void SetText(const std::string & sText) { m_sText = sText; }

  /// returns the type information of the client class
  const type_info & GetTypeInfo() const { return m_typeinfo; }

  /// returns the ID
  const vxID & GetID() const { return m_id; }
  
  /// sets the sender of the modified event
  void SetSender(const void * pSender){ m_pSender = pSender; }

  /// returns the sender of the modified event
  const void * GetSender() const { return m_pSender; }

private:	

  // assignment operator
  vxModifiedInfo& operator=(const vxModifiedInfo &); // should only be public if implemented

// data
private:
  
  // object that was modified
  const void * m_pSender;
  
  /// contains the type information of the client class
  const type_info & m_typeinfo;

  /// contains the modified information
  uint4 m_uFlag;

  /// text message
  std::string m_sText;

  /// identification number
  vxID m_id;
}; // class ModifiedInfo


// defines
#define vxModIndInfo(PARENT, INDEX, FLAG) (vxModifiedIndexedInfo(typeid(PARENT), INDEX, "", FLAG))
#define vxModIndInfoID(PARENT, PARENT_ID, INDEX, FLAG) (vxModifiedIndexedInfo(typeid(PARENT), INDEX, PARENT_ID, FLAG))


/**
 * This class expands the functionality of the vxModifiedInfo to incude the index of the client
 * class within the collection.
 */
class VX_VIEWER_DLL vxModifiedIndexedInfo: public vxModifiedInfo
{
// functions
public:

  /// constructor
  vxModifiedIndexedInfo(const type_info & client, int4 iIndex, const vxID & id, uint4 uFlag = 0);

  /// copy constructor
  vxModifiedIndexedInfo(const vxModifiedIndexedInfo & other);

  /// destructor
  virtual ~vxModifiedIndexedInfo() {};

  /// creates a copy of this object on the heap
  virtual void Clone(vxModifiedInfo ** ppObject) const;

  /// returns the index of the client class within the collection
  uint4 GetIndex() const { return m_iIndex; }

  /// sets the index of the client class within the collection
  void SetIndex(const int4 iIndex) { m_iIndex = iIndex; }

// data
private:
  
  /// contains the index of the client class within the collection
  int4 m_iIndex;
}; // vxModifiedIndexedInfo


#endif // vxModifiedInfo_h


// Revision History:
// $Log: vxModifiedInfo.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/12/18 14:14:46  frank
// Renamed the element ID filename to match class name
//
// Revision 1.8  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.7  2003/06/18 15:19:52  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.6  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.5  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.4  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.3  2002/09/16 19:57:35  dmitry
// vxID.h included
//
// Revision 1.2  2002/08/05 16:20:07  dmitry
// Text information added to ModifiedInfo.
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.11  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.10  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.9  2002/07/12 21:08:15  manju
// Added another modified class.
//
// Revision 1.8  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.7  2002/06/07 20:15:21  dmitry
// vxModifiedIDInfo class added
//
// Revision 1.6  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.5  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.4  2002/04/17 19:24:13  dmitry
// Switched to the new event handling architecture.
//
// Revision 1.3  2002/04/03 14:47:05  geconomos
// Added virtual destructor (tsk... tsk... tsk... Dmitry)
//
// Revision 1.2  2002/03/27 20:19:46  michael
// planeMPR is now included in the modifed/update mechanism
//
// Revision 1.1  2002/03/25 00:51:53  michael
// added the renamed fil4es to 74_vxUtiles, avoiding forward dependencies
//
// Revision 1.3  2002/03/13 15:09:40  dmitry
// Cleaned up constructors, added ModIndInfo Macro.
//
// Revision 1.2  2002/03/13 14:50:04  dmitry
// Added new constructor to vxModifiedInfo and a ModInfo Macro.
//
// Revision 1.1  2002/03/07 22:46:42  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxModifiedInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxModifiedInfo.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $