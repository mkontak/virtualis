// $Id: ImagePreviewer.cs,v 1.2.2.1 2007/03/26 21:07:17 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;


namespace Viatronix.Console.ImageRedactor
{

  /// <summary>
  /// Summary description for ImagePreviewer.
  /// </summary>
  public class ImagePreviewer : System.Windows.Forms.ListView
  {
    #region fields

    /// <summary>
    /// Thumb nail Items
    /// </summary>
    private ImageList m_thumbNailItems = new ImageList();

    /// <summary>
    /// Icon space X
    /// </summary>
    private int iconSpaceX = 1;

    /// <summary>
    /// Icon space Y
    /// </summary>
    private int iconSpaceY = 1;

    /// <summary>
    /// LVM_FIRST
    /// </summary>
    private const int LVM_FIRST = 0x1000;

    /// <summary>
    /// LVM_SETICONSPACING
    /// </summary>
    private const int LVM_SETICONSPACING = ( LVM_FIRST + 53 );

    /// <summary>
    /// THUMBNAIL_SIZE
    /// </summary>
    private const int THUMBNAIL_SIZE = 72;

    #endregion

    #region delegates

    /// <summary>
    /// Send Message delegate
    /// </summary>
    [DllImport( "user32", CharSet = CharSet.Auto )]
    private extern static int SendMessage(IntPtr hWnd, int wMsg, int wParam, IntPtr lParam);

    #endregion

    #region Propertis

    /// <summary>
    /// IconSpaceX
    /// </summary>
    public int IconSpaceX
    {
      get { return iconSpaceX; }
      set
      {
        iconSpaceX = value;
        if ( this.Handle != IntPtr.Zero )
        {
          SetIconSpacing();
        }
      }
    }//IconSpaceX

    /// <summary>
    /// IconSpaceY
    /// </summary>
    public int IconSpaceY
    {
      get { return iconSpaceY; }
      set
      {
        iconSpaceY = value;
        if ( this.Handle != IntPtr.Zero )
        {
          SetIconSpacing();
        }
      }
    }//IconSpaceX


    /// <summary>
    /// SetIcon Spacing
    /// </summary>
    private void SetIconSpacing()
    {
      unchecked
      {
        int cX = 0, cY = 0;

        int lXYC = SendMessage( this.Handle, LVM_SETICONSPACING, 0, new System.IntPtr( -1 ) );

        if ( iconSpaceX > 0 )
          cX = iconSpaceX;//+ THUMBNAIL_SIZE;
        else
          cX = ( lXYC & 0xFFFF );//+ THUMBNAIL_SIZE;

        if ( iconSpaceY > 0 )
          cY = iconSpaceY;//+ THUMBNAIL_SIZE;
        else
          cY = ( lXYC / 0x10000 );// + THUMBNAIL_SIZE;

        Int32 i = ( cY << 2 ) | ( cX );

        SendMessage( this.Handle, LVM_SETICONSPACING, 0, ( IntPtr ) i );
      }
    }//SetIconSpacing() 

    #endregion

    #region construction

    /// <summary>
    /// ImagePreviewer Constructor
    /// </summary>
    public ImagePreviewer()
    {
      AutoArrange = false;
      LabelWrap = false;
      MultiSelect = false;
      Scrollable = true;
      BackColor = System.Drawing.SystemColors.HighlightText;
      BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
      RightToLeft = System.Windows.Forms.RightToLeft.No;
      Alignment = System.Windows.Forms.ListViewAlignment.Left;
      View = View.LargeIcon;

      LargeImageList = m_thumbNailItems;
      // SmallImageList = m_thumbNailItems;
      LargeImageList.ImageSize = new System.Drawing.Size( THUMBNAIL_SIZE, THUMBNAIL_SIZE );
      // SmallImageList.ImageSize = new System.Drawing.Size(  THUMBNAIL_SIZE  , THUMBNAIL_SIZE  );
    }//ImagePreviewer()

    #endregion

    #region methods

    /// <summary>
    /// Get the selected Indexes
    /// </summary>
    /// <returns>ListView.SelectedIndexCollection</returns>
    public ListView.SelectedIndexCollection GetSelectedIndexes()
    {
      return SelectedIndices;
    }//GetSelectedIndexes()

    /// <summary>
    /// Selected Index
    /// </summary>
    public int SelectedIndex
    {
      get { return ( SelectedIndices.Count <= 0 ) ? -1 : SelectedIndices [ 0 ]; }
    }//SelectedIndex

    /// <summary>
    /// SelectItem
    /// </summary>
    /// <param name="index">int</param>
    public void SelectItem(int index)
    {
      Items [ index ].Selected = true;
      Select();
    }//SelectItem(int index)

    /// <summary>
    /// Thumbnail callback
    /// </summary>
    /// <returns>bool</returns>
    public bool ThumbnailCallback()
    {
      return false;
    }//ThumbnailCallback()

    /// <summary>
    /// Adds the image to the previewer
    /// </summary>
    /// <param name="image">Bitmap</param>
    public void Add(System.Drawing.Bitmap image)
    {
      Image.GetThumbnailImageAbort callback = new Image.GetThumbnailImageAbort( ThumbnailCallback );
      Image thumbnail = image.GetThumbnailImage( THUMBNAIL_SIZE, THUMBNAIL_SIZE, callback, IntPtr.Zero );

      m_thumbNailItems.Images.Add( thumbnail );

      System.Windows.Forms.ListViewItem item = new ListViewItem( string.Empty, m_thumbNailItems.Images.Count - 1 );
      Items.Add( item );
    }//Add(System.Drawing.Bitmap image)


    #endregion

  }//class ImagePreviewer
}//namespace Viatronix.Console.ImageRedactor



// $Log: ImagePreviewer.cs,v $
// Revision 1.2.2.1  2007/03/26 21:07:17  romy
// fixed issue #5519
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/ImagePreviewer.cs,v 1.2.2.1 2007/03/26 21:07:17 romy Exp $
// $Id: ImagePreviewer.cs,v 1.2.2.1 2007/03/26 21:07:17 romy Exp $


