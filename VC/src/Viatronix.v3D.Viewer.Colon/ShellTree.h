// $Id: ShellTree.h,v 1.3 2007/03/07 19:38:55 romy Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (mailto:george@viatronix.com)

#pragma once

// class declaration
class ShellTree : public CTreeCtrl
{
// member functions
public:
  
  /// constructor  
  ShellTree();
  
  /// destructor
  virtual ~ShellTree();

  /// initializes the tree and sets the root pidl
  void Initialize( LPCITEMIDLIST pidlRoot, bool bIncludeFiles );

  /// gets the context menu id
  const uint4 GetContextMenuId() const { return m_uContextMenuId; }

  /// sets the context menu id
  void SetContextMenuId( const uint4 uContextMenuId ) { m_uContextMenuId = uContextMenuId; }

  /// gets currently selected path
  std::string GetSelectedPath() const;

protected:

  /// allows for initialization prior to subclassing
  virtual void PreSubclassWindow();
  
private:

  /// populates the root of the tree
  void PopulateRoot();

  /// populates the specified item
  bool PopulateItem( HTREEITEM hItem );

  /// performs
  void PreExpandItem( HTREEITEM hItem );

  /// expands the specified item
  void ExpandItem( HTREEITEM hItem );

  /// fills in the TVITEM structure with data
  bool FillItem( TVITEM & item, LPCITEMIDLIST pidl, CComPtr< IShellFolder > spParentFolder, LPCITEMIDLIST pidlRel );
  
  /// refreshes the subitems for the specified item
  void RefreshSubItems( HTREEITEM hParent );

  /// deletes all children for the specified item
  void DeleteChildren( HTREEITEM hItem );

  /// Shows the context menu
  void ShowContextMenu( CPoint & point );

  /// mfc message map
  DECLARE_MESSAGE_MAP()

  /// TVN_ITEMEXPANDING handler
  void OnItemExpanding( NMHDR * pNMHDR, LRESULT * pResult );

  /// TVN_ITEMEXPANDED handler
  void OnItemExpanded( NMHDR * pNMHDR, LRESULT * pResult );

  /// NM_RCLICK handler
  void OnRClick( NMHDR * pNMHDR, LRESULT * pResult );

  /// compare function for sorting
  static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort );
  
  /// get the last item in the list
  static LPCITEMIDLIST ILGetLast( LPCITEMIDLIST pidl );

// member variables
private:

  // root pidl
  LPITEMIDLIST m_pidlRoot;

  /// shell allocator
  CComPtr< IMalloc >  m_spMalloc;

  /// shell folder for the desktop
  CComPtr< IShellFolder >  m_spDesktopFolder;

  /// indicates if files are to be included in the tree
  bool m_bIncludeFiles;

  /// current item that is populating
  HTREEITEM m_hItemToPopulate;

  /// resource id of context menu (menu is assumed to be in the zero position)
  uint4 m_uContextMenuId;

}; // class ShellTree


// $Log: ShellTree.h,v $
// Revision 1.3  2007/03/07 19:38:55  romy
// added comments for method ShowContextMenu
//
// Revision 1.2  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.1  2005/11/15 16:15:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ShellTree.h,v 1.3 2007/03/07 19:38:55 romy Exp $
// $Id: ShellTree.h,v 1.3 2007/03/07 19:38:55 romy Exp $
