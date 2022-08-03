// $Id: ShellTree.C,v 1.3 2007/03/09 15:53:30 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "ShellTree.h"
#include "resource.h"

/**
 * Constructor
 */
ShellTree::ShellTree() : 
  m_pidlRoot( NULL ),
  m_bIncludeFiles( false ),
  m_hItemToPopulate( NULL ),
  m_uContextMenuId( 0 )
{
  /// get the shell allocator
  SHGetMalloc( &m_spMalloc );
  
  /// get the desktop shell folder
  SHGetDesktopFolder( &m_spDesktopFolder );
} // ShellTree()


/**
 * Destructor
 */
ShellTree::~ShellTree()
{
  if( m_pidlRoot != NULL )
    m_spMalloc->Free( m_pidlRoot );
} // ~ShellTree()


/**
 * message map
 */
BEGIN_MESSAGE_MAP(ShellTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
  ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
END_MESSAGE_MAP()


/**
 * Allows for initialization prior to subclassing
 */
void ShellTree::PreSubclassWindow()
{
  // get the pidl for the desktop
	LPITEMIDLIST pidl( NULL );
	SHGetSpecialFolderLocation( GetSafeHwnd(), CSIDL_DESKTOP, &pidl );

  // get the image list for pidl
  SHFILEINFO sfi;
	ZeroMemory( &sfi, sizeof( SHFILEINFO ) );
  HIMAGELIST hSysImageList = (HIMAGELIST) SHGetFileInfo( ( LPCTSTR )pidl, 0, &sfi, sizeof( SHFILEINFO ), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

  // no longer need the dekstop pidl
	m_spMalloc->Free( pidl );
	
  // set the imaglist on the tree
	TreeView_SetImageList( GetSafeHwnd(), hSysImageList, TVSIL_NORMAL );

  CTreeCtrl::PreSubclassWindow();

} // PreSubclassWindow()


/**
 * Initializes the tree and sets the root pidl
 *
 * @param    pidlRoot         root pidl
 * @param    bIncludeFiles    indicates if files should be included in the tree
 */
void ShellTree::Initialize( LPCITEMIDLIST pidlRoot, bool bIncludeFiles )
{
	m_bIncludeFiles = bIncludeFiles;

  m_pidlRoot = ILCombine( NULL, pidlRoot );

	RefreshSubItems( TVI_ROOT );
} // Initialize( LPCITEMIDLIST pidlRoot, bool bIncludeFiles )


/**
 * Gets the currently selected path
 *
 * @return    currently selected path
 */
std::string ShellTree::GetSelectedPath() const
{
  std::string sPath;
  
  HTREEITEM hItem = GetSelectedItem();
  if( hItem != NULL )
  {
    LPITEMIDLIST pidl( reinterpret_cast< LPITEMIDLIST >( GetItemData( hItem ) ) );

    char sTmp[ MAX_PATH ];
	  if( SHGetPathFromIDList( pidl, sTmp ) )
	    sPath = sTmp;
  }
  return sPath;
} // GetSelectedPath()


/**
 * Refreshes the subitems for the specified item
 *
 * @param   hItem   item whose subitems are to be refreshed
 */
void ShellTree::RefreshSubItems( HTREEITEM hItem )
{
  // make sure that the item is not the root item and that wit has children
	if( hItem != TVI_ROOT && !ItemHasChildren( hItem ) )
		return;

  // disable painting
	SetRedraw( FALSE );
	
  // remove all existing children
  DeleteChildren( hItem );
	
  // is this the root item?
  if( hItem == TVI_ROOT )
  {
		PopulateRoot();
  }
	else
	{
    PreExpandItem( hItem );
		ExpandItem( hItem );
	}

  // renable painting
	SetRedraw( TRUE );
} // RefreshSubItems( HTREEITEM hItem )


/**
 * Deletes all children for the specified item
 *
 * @param   hItem   item whose children are to be deleted
 */
void ShellTree::DeleteChildren( HTREEITEM hItem )
{
	HTREEITEM hChild = GetChildItem( hItem );
	while( hChild != NULL )
	{
		HTREEITEM hNext( GetNextSiblingItem( hChild ) );
		DeleteItem( hChild );
		hChild = hNext;
	}
} // DeleteChildren( HTREEITEM hItem )


/**
 * Populates the root of the tree
 */
void ShellTree::PopulateRoot()
{
  // expand the root
	PreExpandItem( TVI_ROOT );
	ExpandItem( TVI_ROOT );

  SetRedraw( FALSE );

  // force the update and don't scroll
  SCROLLINFO si;
	GetScrollInfo( SB_HORZ, &si );
	EnsureVisible( GetChildItem( TVI_ROOT ) );
  SetScrollInfo( SB_HORZ, &si, FALSE );
  
  SetRedraw();
} // PopulateRoot()


/**
 * Populates the specified item
 *
 * @param   hItem     item to populate
 */
void ShellTree::PreExpandItem( HTREEITEM hItem )
{
	m_hItemToPopulate = hItem;

	// fix redraw when expanded programatically
	UpdateWindow();
	
  // hide changes until it's expanded
	SetRedraw( FALSE );
	
  // add dummy item
	InsertItem("", m_hItemToPopulate);
} // PreExpandItem( HTREEITEM hItem )


/**
 * Expands the specified item
 *
 * @param   hItem   item to expand
 */
void ShellTree::ExpandItem( HTREEITEM hItem )
{
  if( m_hItemToPopulate == NULL )
		return;

  // delay redraw after populating
	SetRedraw( FALSE );

  // remove all children
  DeleteChildren( m_hItemToPopulate );

  // populate the item
  bool bCheckChildren = PopulateItem( hItem );

  // change parent to reflect current children number
	if( hItem != TVI_ROOT )
	{
		TVITEM item;
		item.hItem = hItem;
		item.mask = TVIF_HANDLE | TVIF_CHILDREN;
		item.cChildren = GetChildItem(hItem) == NULL? 0 : 1;
		if( bCheckChildren )
    {
			SetItem( &item );
    }
		else if (item.cChildren == 0)
    {
			// restore item's plus button if no children inserted
			SetItemState( hItem, 0, TVIS_EXPANDED );
    }
	}
	// redraw now
	SetRedraw();

} // ExpandItem( HTREEITEM hItem )


/**
 * Populates the specified item
 * 
 * @param   hItem   item to populate
 */
bool ShellTree::PopulateItem( HTREEITEM hItem )
{
  bool bReturn( false );

  // get parent pidl
  LPITEMIDLIST pidlParent( NULL );
	if( hItem == TVI_ROOT )
		pidlParent = m_pidlRoot;
	else
		pidlParent = reinterpret_cast< LPITEMIDLIST >( GetItemData( hItem ) );

	// get parent shell folder
	CComPtr< IShellFolder > spParentFolder;
	m_spDesktopFolder->BindToObject( pidlParent, NULL, IID_IShellFolder, reinterpret_cast< void ** >( &spParentFolder ) );

	// no folder object
	if( !spParentFolder )
  {
		if( hItem == TVI_ROOT )	
		{
			spParentFolder = m_spDesktopFolder;
		}
		else
    {
			return true;
    }
  }

	// enum child pidls
	CComPtr< IEnumIDList > spEnumIDList;
	if( spParentFolder->EnumObjects( GetSafeHwnd(), SHCONTF_FOLDERS | ( ( m_bIncludeFiles ) ? SHCONTF_NONFOLDERS : 0), &spEnumIDList ) == NOERROR )
	{
		LPITEMIDLIST pidl( NULL);
		while( spEnumIDList->Next( 1, &pidl, NULL ) == NOERROR )
		{
			// get an absolute pidl
			LPITEMIDLIST pidlAbs( ILCombine( pidlParent, pidl ) );
			
      // add item
			TVINSERTSTRUCT tvis;
			ZeroMemory( &tvis, sizeof( TVINSERTSTRUCT ) );
			
      tvis.hParent = hItem;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
			tvis.item.lParam = reinterpret_cast< LPARAM >( pidlAbs );

      // fill with text, icons and children
			bReturn = FillItem(tvis.item, pidlAbs, spParentFolder, pidl);
			
			InsertItem( &tvis );

			// free enumerated object
			m_spMalloc->Free( pidl );
		}
	}

  if( !bReturn  )
		return false;

    // sort items
	TVSORTCB tvscb;
	tvscb.hParent = hItem;
	tvscb.lpfnCompare = CompareFunc;
	tvscb.lParam = reinterpret_cast< LPARAM>( spParentFolder.p );
	SortChildrenCB( &tvscb );


  // check if item is removable (ignore these)
	char sPath[ MAX_PATH ];
	if( SHGetPathFromIDList( pidlParent, sPath ) )
	{			
		UINT type = GetDriveType( sPath );
		if ( type != DRIVE_FIXED )
			return false;
	}

	return true;
} // PopulateItem( HTREEITEM hItem )


/**
 * Fills in the TVITEM structure with data
 *
 * @param     item        item to populate
 * @param     pidl        absolute pidl
 * @param     spFolder    parent IShellFolder
 * @param     pidlRel     relative pidl
 * @return                true if children should be checked
 */
bool ShellTree::FillItem( TVITEM & item, LPCITEMIDLIST pidl, CComPtr< IShellFolder > spParentFolder, LPCITEMIDLIST pidlRel )
{
	static CString sName; // must survive to this function
	SHFILEINFO sfi;
	DWORD dwAttributes;

	if (item.mask & TVIF_TEXT)
	{
		// get display name
		STRRET str;
		str.uType = STRRET_WSTR;
    spParentFolder->GetDisplayNameOf(pidlRel, SHGDN_INFOLDER, &str);

		switch (str.uType)
		{
		case STRRET_WSTR:
			sName = str.pOleStr;
			m_spMalloc->Free(str.pOleStr);
			break;
		case STRRET_CSTR:
			sName = str.cStr;
			break;
		case STRRET_OFFSET:
			sName = (char*)((LPBYTE)pidlRel+str.uOffset);
		}
		item.pszText = (LPTSTR)(LPCTSTR)sName;
	}

	if (item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SFGAO_LINK;
		spParentFolder->GetAttributesOf(1, &pidlRel, &dwAttributes);

		// get correct icon
		UINT uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;
		if (dwAttributes & SFGAO_LINK)
			uFlags |= SHGFI_LINKOVERLAY;

		if (item.mask & TVIF_IMAGE)
		{
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);
			item.iImage = sfi.iIcon;
		}
		if (item.mask & TVIF_SELECTEDIMAGE)
		{
			if (dwAttributes & SFGAO_FOLDER)
				uFlags |= SHGFI_OPENICON;

			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);
			item.iSelectedImage = sfi.iIcon;
		}
	}

	if (item.mask & TVIF_CHILDREN)
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SFGAO_REMOVABLE;
		spParentFolder->GetAttributesOf(1, &pidlRel, &dwAttributes);

		// get children
		item.cChildren = 0;
		if (dwAttributes & SFGAO_FOLDER)
		{
			if (m_bIncludeFiles)
				item.cChildren = 1;
			else if (dwAttributes & SFGAO_REMOVABLE)
				item.cChildren = 1;
			else
			{
				dwAttributes = SFGAO_HASSUBFOLDER;
				spParentFolder->GetAttributesOf(1, &pidlRel, &dwAttributes);

				item.cChildren = (dwAttributes & SFGAO_HASSUBFOLDER) ? 1 : 0;
			}
		}
	}

	// check if removable or remote media
	SHDESCRIPTIONID sdi;
	ZeroMemory(&sdi, sizeof(SHDESCRIPTIONID));
	if (SUCCEEDED(SHGetDataFromIDList(spParentFolder, pidlRel,SHGDFIL_DESCRIPTIONID, &sdi, sizeof(SHDESCRIPTIONID))))
	{
		switch (sdi.dwDescriptionId)
		{
		case SHDID_COMPUTER_REMOVABLE:
		case SHDID_COMPUTER_DRIVE35:
		case SHDID_COMPUTER_DRIVE525:
		case SHDID_COMPUTER_NETDRIVE:
		case SHDID_COMPUTER_CDROM:
		case SHDID_NET_DOMAIN:
		case SHDID_NET_SERVER:
		case SHDID_NET_SHARE:
		case SHDID_NET_RESTOFNET:
		case SHDID_NET_OTHER:
			return FALSE;
		}
	}

	return TRUE;
} // fills in the TVITEM structure with data


/**
 * TVN_ITEMEXPANDING handler
 *
 * @param   pNMHDR      NMHDR contains information related to this message
 * @param   pResult     resultant code
 */
void ShellTree::OnItemExpanding( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	NM_TREEVIEW * pNMTreeView = reinterpret_cast< NM_TREEVIEW * >( pNMHDR );

	if( pNMTreeView->action & TVE_EXPAND )
	  PreExpandItem( pNMTreeView->itemNew.hItem );

	*pResult = 0;
} // OnItemExpanding( NMHDR * pNMHDR, LRESULT * pResult ) 


/**
 * TVN_ITEMEXPANDED handler
 *
 * @param   pNMHDR      NMHDR contains information related to this message
 * @param   pResult     resultant code
 */
void ShellTree::OnItemExpanded( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	NM_TREEVIEW * pNMTreeView = reinterpret_cast< NM_TREEVIEW * >( pNMHDR );

	if( pNMTreeView->action & TVE_EXPAND )
		ExpandItem( pNMTreeView->itemNew.hItem );

	*pResult = 0;
} // OnItemExpanded( NMHDR * pNMHDR, LRESULT * pResult ) 


/**
 * NM_RCLICK handler
 *
 * @param   pNMHDR      NMHDR contains information related to this message
 * @param   pResult     resultant code
 */
void ShellTree::OnRClick( NMHDR * pNMHDR, LRESULT * pResult )
{
	// get the mouse cursor position 
	CPoint pt( GetMessagePos() );
	ScreenToClient( &pt );

  // hit test
	UINT uTest( 0 );
	HTREEITEM hItem( HitTest( pt, &uTest ) );

	// did the click occur on an item?
	if( ( hItem != NULL ) && ( uTest & TVHT_ONITEM ) )
	{
		HTREEITEM hCurrentItem( GetSelectedItem() );

    // is the item diffrent from what is selected?
		if( hItem != hCurrentItem )
      Select( hItem, TVGN_CARET );

		// display context menu
    ClientToScreen( &pt );
		ShowContextMenu( pt );
	}

	*pResult = 0;
} // OnRClick( NMHDR * pNMHDR, LRESULT * pResult)


/**
 * Displays a context menu at the specified position
 *
 * @param   point   position to display context menu (screen coordinates)
 */
void ShellTree::ShowContextMenu( CPoint & point )
{
  // has a context menu been specified
  if( m_uContextMenuId == 0 )
    return;

  // load the context menu
  CMenu menu;
  menu.LoadMenu( m_uContextMenuId );
	CMenu * pPopup( menu.GetSubMenu( 0 ) );

	// ensure that commands and menu update handling is done  properly, make the parent of the menu the main frame window 
	CWnd * pMenuParent = GetParent();

	UINT uCmd = pPopup->TrackPopupMenu( TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pMenuParent, NULL );

	// was a menu item chosen?
	if( uCmd != 0 )
	  pMenuParent->SendMessage( WM_COMMAND, uCmd, 0 );


	menu.DestroyMenu();
} // ShowContextMenu( CPoint & point )


/**
 * Compare function for sorting
 *
 * @param     lParam1     lparam value of item1
 * @param     lParam2     lparam value of item2
 * @param     lParamSort  lparam value of of the sort structure
 * @return                return a negative value if the first item should precede the second,
 *                        a positive value if the first item should follow the second, or zero 
 *                        if the two items are equivalent
 */
int CALLBACK ShellTree::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	HRESULT hr = ((IShellFolder*)lParamSort)->CompareIDs( 0, ILGetLast((LPCITEMIDLIST)lParam1), ILGetLast((LPCITEMIDLIST)lParam2) );
	
  if( FAILED( hr ) )
		return 0;
	
	short result( static_cast< short >( HRESULT_CODE( hr ) ) );
  if( result < 0 )
  {
		return -1;
  }
	if( result > 0 )
  {
		return 1;
  }
	return 0;
} // CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )


/**
 * Gets the last item in the specified list
 *
 * @param   pidl    pidl
 * @retun   last item in the specified pidl
 */
LPCITEMIDLIST ShellTree::ILGetLast( LPCITEMIDLIST pidl )
{
	LPCITEMIDLIST ret = pidl, tmp = ILGetNext(pidl);
	while (tmp != NULL)
	{
		ret = tmp;
		tmp = ILGetNext( tmp );
	}
	return ret;
} // ILGetLast( LPCITEMIDLIST pidl )


// $Log: ShellTree.C,v $
// Revision 1.3  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.2  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.1  2005/11/15 16:15:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ShellTree.C,v 1.3 2007/03/09 15:53:30 jmeade Exp $
// $Id: ShellTree.C,v 1.3 2007/03/09 15:53:30 jmeade Exp $
