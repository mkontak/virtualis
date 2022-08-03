// $Id: ReportCollection.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ReportCollection.h
// Description:  
// Created:  
// Author: George geconomos@viatronix.net
//
// Complete History at bottom of file.

#ifndef ReportCollection_h_
#define ReportCollection_h_

#include <list>
#include <utility>

template<class P, class PI, class T, class U,  const IID* piid>
class ReportCollection  
{

protected:

  /// collection owner
  CComPtr<PI> m_spParent;
  
  /// table of contents item typedef
  typedef std::pair<CComBSTR, CComPtr<T> > TTocItem;
  
  /// table of contents typedef
  typedef std::list<TTocItem> TToc;
  
  /// table of contents
  TToc m_Toc;
  
  /// read table of contents from underlying datastore
  virtual HRESULT ReadTOC(CComBSTR bsName);
  
  /// write table of contents to underlying datastore
  virtual HRESULT WriteTOC(CComBSTR bsName);
  
  /// adds an entry to the table of contents
  virtual HRESULT AddEntry(CComBSTR bsEntryID, T** ppiEntry);
  
  /// get an entry from the table of contents
  TTocItem GetEntry(int4 iIndex);
  
  /// removes an entry from the table of contents
  void RemoveEntry(int4 iIndex);
  
  /// finds an item in the table of contents
  int4 FindItem(CComBSTR bsItemID);
  
  /// verify index into table of contents
  int4 VerifyIndex(VARIANT& vIndex);

  /// free all objects held in the table of contents
  void Free();

};

#include "ReportCollection.inl"

#endif 

// $Log: ReportCollection.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:34   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.2  2001/05/08 16:20:03  geconomos
// More coding standards
//
// Revision 1.1  2001/04/16 13:50:43  geconomos
// Coding standards(or there lack of)
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/ReportCollection.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: ReportCollection.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $