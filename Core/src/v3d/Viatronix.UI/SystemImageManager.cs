// $Id: SystemImageManager.cs,v 1.1 2001/01/02 12:41:27 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;


namespace Viatronix.UI
{

  #region enumerations

  public enum ImageSize
  {
    Large,
    Small,
  }


  public enum FolderStyle
  {
    Open,
    Closed,
  }

  #endregion

	/// <summary>
	/// Summary description for SystemImageManager.
	/// </summary>
  public sealed class SystemImageManager
  {

    #region fields

    private static SystemImageManager m_instance = null;

    private Hashtable m_imageTable = new Hashtable();

    private ImageList m_smallImageList = new ImageList();

    private ImageList m_largeImageList = new ImageList();

    private const string FolderPath = "Default Folder Path";

    #endregion

    #region construction

    private SystemImageManager()
    {
      m_smallImageList.Images.Add( LoadFolder( FolderStyle.Closed, ImageSize.Small ));
      m_smallImageList.Images.Add( LoadFolder( FolderStyle.Open, ImageSize.Small ));

      m_largeImageList.Images.Add( LoadFolder( FolderStyle.Closed, ImageSize.Large ));
      m_largeImageList.Images.Add( LoadFolder( FolderStyle.Open, ImageSize.Large ));

      m_imageTable.Add( FolderPath, new SystemImageInfo( 0, 1 ));
    } // SystemImageManager()

    #endregion

    #region properties

    public static SystemImageManager Instance
    {
      get
      {
        if( m_instance == null )
          m_instance = new SystemImageManager();

        return m_instance;
      }
    }


    public ImageList SmallImageList
    {
      get { return m_smallImageList; }
    } // SmallImageList


    public ImageList LargeImageList
    {
      get { return m_largeImageList; }
    } // LargeImageList

    #endregion

    #region methods
    
    private void AddFileImage( string path )
    {      
      if( m_imageTable.ContainsKey( path ))
        return;

      m_smallImageList.Images.Add( LoadImage( path, ImageSize.Small ));
      m_largeImageList.Images.Add( LoadImage( path, ImageSize.Large ));

      m_imageTable.Add( path, new SystemImageInfo( m_smallImageList.Images.Count - 1 ));
    } // AddFileImage( path )


    private Image LoadImage( string path, ImageSize size )
    {
      return Load( path, FolderStyle.Closed, size );
    } // LoadImage( path )


    private Image LoadFolder( FolderStyle style, ImageSize size )
    {
      return Load( FolderPath, style, size );
    } // LoadFolder( style )


    private Image Load( string path, FolderStyle style, ImageSize size )
    {

      Win32.SHGFI flags = Win32.SHGFI.SHGFI_SYSICONINDEX | Win32.SHGFI.SHGFI_USEFILEATTRIBUTES | Win32.SHGFI.SHGFI_ICON;
      
      if( size == ImageSize.Small ) 
        flags |= Win32.SHGFI.SHGFI_SMALLICON; 
      else 
        flags |= Win32.SHGFI.SHGFI_LARGEICON;

      if( style == FolderStyle.Open )
        flags |= Win32.SHGFI.SHGFI_OPENICON;


      Win32.FILE fileAttribute = Win32.FILE.FILE_ATTRIBUTE_NORMAL;
      if( path == FolderPath )
      {
        fileAttribute = Win32.FILE.FILE_ATTRIBUTE_DIRECTORY;

        if( style == FolderStyle.Open )
          flags |= Win32.SHGFI.SHGFI_OPENICON;
      }

      Win32.SHFILEINFO shInfo = new Win32.SHFILEINFO(); 
      int infoSize = System.Runtime.InteropServices.Marshal.SizeOf( shInfo ); 
      Win32.Shell32.SHGetFileInfo( path, fileAttribute, ref shInfo, (uint)infoSize, flags ); 

      try
      {
        return Bitmap.FromHicon( shInfo.hIcon );
      }
      finally
      {
        Win32.User32.DestroyIcon( shInfo.hIcon );
      }
    } // Load( path, style )


   
    #endregion

    #region static methods

    public int GetImageIndex( string path )
    {
      AddFileImage( path );

      SystemImageInfo info = (SystemImageInfo) m_imageTable[ path ];
      if( info == null )
        return -1;

      return info.Index;
    } // GetImageIndex( path )
 

    public int GetFolderIndex( FolderStyle style )
    {
      SystemImageInfo info = (SystemImageInfo) m_imageTable[ FolderPath ];
      if( info == null )
        return -1;
      
      int index = ( style == FolderStyle.Open ) ? info.SelectedIndex : info.Index;
      return index;
    } // GetFolderIndex( style )

    #endregion

  } // class SystemImageManager

  #region SystemImageInfo class

  internal class SystemImageInfo
  {
    
    #region fields

    private int m_index = -1;

    private int m_selectedIndex = -1;

    #endregion

    #region construction

    public SystemImageInfo( int index ) : this( index, index )
    {
    }


    public SystemImageInfo( int index, int selectedIndex )
    {
      m_index = index;
      m_selectedIndex = selectedIndex;
    }

    #endregion

    #region properties

    public int Index
    {
      get { return m_index; }
    }


    public int SelectedIndex
    {
      get { return m_selectedIndex; }
    }
                        

    #endregion

  } // class SystemImageInfo

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: SystemImageManager.cs,v $
// Revision 1.1  2001/01/02 12:41:27  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/SystemImageManager.cs,v 1.1 2001/01/02 12:41:27 gdavidson Exp $
// $Id: SystemImageManager.cs,v 1.1 2001/01/02 12:41:27 gdavidson Exp $

#endregion