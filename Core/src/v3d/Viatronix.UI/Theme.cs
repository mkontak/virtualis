// $Id: Theme.cs,v 1.18 2005/11/16 18:10:13 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Drawing;
using System.Reflection;

namespace Viatronix.UI
{
  #region ThemeColor

  /// <summary>
  /// the available theme colors
  /// </summary>
  public enum ThemeColor
  {
    Purple,
    Red,
    Yellow,
    Default = Purple,
  } // enum ThemeColor

  #endregion

  /// <summary>
  /// Encapsulates all the colors, brushes and fonts that make up a theme.
  /// </summary>
	public class Theme
	{

    #region fields

    ///
    /// background
    ///

    /// <summary>
    /// the background color
    /// </summary>
    private static System.Drawing.Color m_bkgndColor;

    /// <summary>
    /// the background side left image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndSideLeftImage;

    /// <summary>
    /// the background side bottom image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndSideBottomImage;

    /// <summary>
    /// the background side right image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndSideRightImage;

    /// <summary>
    /// the background side top left image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndCornerTopLeftImage;

    /// <summary>
    /// the background corner bottom left image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndCornerBottomLeftImage;

    /// <summary>
    /// the background corner bottom right image
    /// </summary>
    private static System.Drawing.Bitmap m_bkgndCornerBottomRightImage;

    /// <summary>
    /// the background side bottom brush
    /// </summary>
    private static System.Drawing.TextureBrush m_bkgndSideBottomBrush;

    /// <summary>
    /// the background side left brush
    /// </summary>
    private static System.Drawing.TextureBrush m_bkgndSideLeftBrush;

    /// <summary>
    /// the background side right brush
    /// </summary>
    private static System.Drawing.TextureBrush m_bkgndSideRightBrush;


    ///
    /// toolbar
    ///

    /// <summary>
    /// the toolbar left image
    /// </summary>
    private static Bitmap m_toolbarLeftImage;

    /// <summary>
    /// the toolbar middle image
    /// </summary>
    private static Bitmap m_toolbarMiddleImage;

    /// <summary>
    /// the toolbar right image
    /// </summary>
    private static Bitmap m_toolbarRightImage;

    /// <summary>
    /// the toolbar middle brush
    /// </summary>
    private static System.Drawing.TextureBrush m_toolbarMiddleBrush;


    ///
    /// scrollable control 
    ///

    /// <summary>
    /// the fill color
    /// </summary>
    private static System.Drawing.Color m_fillColor;


    ///
    /// pane
    ///

    /// <summary>
    /// the pane expanded brush
    /// </summary>
    private static System.Drawing.SolidBrush m_paneExpandedBrush;

    /// <summary>
    /// the pane collapsed brush
    /// </summary>
    private static System.Drawing.SolidBrush m_paneCollapsedBrush;

    /// <summary>
    /// the pane font
    /// </summary>
    private static System.Drawing.Font m_paneFont;

    /// <summary>
    /// the pane background color
    /// </summary>
    private static System.Drawing.Color m_paneBackColor;


    ///
    /// menu
    ///

    /// <summary>
    /// the menu left image
    /// </summary>
    private static System.Drawing.Bitmap m_menuLeftImage;

    /// <summary>
    /// the menu middle image
    /// </summary>
    private static System.Drawing.Bitmap m_menuMiddleImage;

    /// <summary>
    /// the menu right image
    /// </summary>
    private static System.Drawing.Bitmap m_menuRightImage;

    /// <summary>
    /// the menu middle brush
    /// </summary>
    private static System.Drawing.TextureBrush m_menuMiddleBrush = null; 

    /// <summary>
    /// the menu text brush
    /// </summary>
    private static System.Drawing.SolidBrush m_menuTextBrush;

    /// <summary>
    /// the menu font
    /// </summary>
    private static System.Drawing.Font m_menuFont;


    ///
    /// context menu
    ///

    /// <summary>
    /// the context menu text brush
    /// </summary>
    private static System.Drawing.SolidBrush  m_contextMenuTextBrush;

    /// <summary>
    /// the context menu text disabled brush
    /// </summary>
    private static System.Drawing.SolidBrush  m_contextMenuTextDisabledBrush;

    /// <summary>
    /// the context menu font
    /// </summary>
    private static System.Drawing.Font m_contextMenuFont;


    ///
    /// tab control 
    ///

    /// <summary>
    /// the tab font
    /// </summary>
    private static System.Drawing.Font m_tabFont;

    /// <summary>
    /// the tab brush
    /// </summary>
    private static System.Drawing.SolidBrush m_tabBrush;

    /// <summary>
    /// the tab selected font
    /// </summary>
    private static System.Drawing.Font m_tabSelectedFont;

    /// <summary>
    /// the tab selected brush
    /// </summary>
    private static System.Drawing.SolidBrush m_tabSelectedBrush;
    

    ///
    /// selected state colors
    ///

    /// <summary>
    /// selected brush
    /// </summary>
    private static System.Drawing.SolidBrush m_selectedBrush = new SolidBrush( Color.FromArgb( 102,153,204 ) );

    /// <summary>
    /// selected pen
    /// </summary>
    private static System.Drawing.Pen m_selectedPen = new Pen( Color.FromArgb( 0,102,153 ) );

    /// <summary>
    /// selected viewer border color
    /// </summary>
    private static System.Drawing.Color m_selectedViewerBorderColor = Color.FromArgb( 0,102,153 );

    /// <summary>
    /// down brush
    /// </summary>
    private static System.Drawing.Brush m_downBrush = new SolidBrush( Color.FromArgb( 206,209,218 ));


    ///
    /// progress bar
    ///

    /// <summary>
    /// progress bar horizontal fill image
    /// </summary>
    private static System.Drawing.Bitmap m_progessbarHorzFillImage;

    /// <summary>
    /// progress bar horizontal track image
    /// </summary>
    private static System.Drawing.Bitmap m_progessbarHorzTrackImage;

    /// <summary>
    ///  progress bar horizontal fill brush
    /// </summary>
    private static System.Drawing.TextureBrush m_progessbarHorzFillBrush;

    /// <summary>
    /// progress bar track brush
    /// </summary>
    private static System.Drawing.TextureBrush m_progessbarTrackBrush;

    /// <summary>
    /// progress bar fill text brush
    /// </summary>
    private static System.Drawing.Brush m_progessbarFillTextBrush;

    /// <summary>
    /// progress bar track text brush
    /// </summary>
    private static System.Drawing.Brush m_progessbarTrackTextBrush;


    ///
    ///  system menu
    ///

    /// <summary>
    /// system menu button images
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuButtonImages;

    /// <summary>
    /// system menu close button images
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuCloseButtonImages;

    /// <summary>
    /// system menu maximize glyphs
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuMaximizeGlyphs;

    /// <summary>
    /// system menu minimize glyphs
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuMinimizeGlyphs;

    /// <summary>
    /// system menu restore glyphs
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuRestoreGlyphs;

    /// <summary>
    /// system menu close glyphs
    /// </summary>
    private static System.Drawing.Bitmap m_sysmenuCloseGlyphs;

    ///
    ///  form
    ///

    /// <summary>
    /// form side brush
    /// </summary>
    private static System.Drawing.TextureBrush m_formSideBrush;

    /// <summary>
    /// form bottom brush
    /// </summary>
    private static System.Drawing.TextureBrush m_formBottomBrush;
    
    /// <summary>
    /// form top brush
    /// </summary>
    private static System.Drawing.TextureBrush m_formTopBrush;

    /// <summary>
    /// form corner bottom left image
    /// </summary>
    private static System.Drawing.Bitmap m_formCornerBottomLeftImage;

    /// <summary>
    /// form corner bottom right image
    /// </summary>
    private static System.Drawing.Bitmap m_formCornerBottomRightImage;

    /// <summary>
    /// form corner top left image
    /// </summary>
    private static System.Drawing.Bitmap m_formCornerTopLeftImage;

    /// <summary>
    /// form corner top right image
    /// </summary>
    private static System.Drawing.Bitmap m_formCornerTopRightImage;

    /// <summary>
    /// form font
    /// </summary>
    private static System.Drawing.Font m_formFont;

    ///
    ///  general
    ///
    
    /// <summary>
    /// header font
    /// </summary>
    private static System.Drawing.Font m_headerFont;

    #endregion

    #region properties

    #region general properties
    
    /// <summary>
    /// gets and sets the fill color
    /// </summary>
    public static System.Drawing.Color FillColor
    {
      get { return m_fillColor; }
      set { m_fillColor = value; }
    }


    /// <summary>
    /// gets and sets the background color for the application
    /// </summary>
    public static Color BackColor
    {
      get { return m_bkgndColor; }
      set { m_bkgndColor = value; }
    } // BackColor


    /// <summary>
    /// Gets or sets the header font
    /// </summary>
    public static Font HeaderFont
    {
      get { return m_headerFont; }
      set { m_headerFont = value; }
    } // HeaderFont


    #endregion

    #region pane properties
    
    /// <summary>
    /// gets and sets the pane expanded brush
    /// </summary>
    public static System.Drawing.SolidBrush PaneExpandedBrush
    {
      get { return m_paneExpandedBrush; }
      set { m_paneExpandedBrush = value; }
    }

    /// <summary>
    /// gets and sets the pane collapsed brush
    /// </summary>
    public static System.Drawing.SolidBrush PaneCollapsedBrush
    {
      get { return m_paneCollapsedBrush; }
      set { m_paneCollapsedBrush = value; }
    }

    /// <summary>
    /// gets and sets the pane font
    /// </summary>
    public static System.Drawing.Font PaneFont
    {
      get { return m_paneFont; }
      set { m_paneFont = value; }
    }

    /// <summary>
    /// gets and sets the pane background color
    /// </summary>
    public static System.Drawing.Color PaneBackColor
    {
      get { return m_paneBackColor; }
      set { m_paneBackColor = value; }
    }

    #endregion
    
    #region menu properties

    /// <summary>
    /// gets and sets the menu left image
    /// </summary>
    public static System.Drawing.Bitmap MenuLeftImage
    {
      get { return m_menuLeftImage; }
      set { m_menuLeftImage = value; }
    }
    
    /// <summary>
    /// gets and sets the menu middle image
    /// </summary>
    public static System.Drawing.Bitmap MenuMiddleImage
    {
      get { return m_menuMiddleImage; }
      set { m_menuMiddleImage = value; }
    }

    /// <summary>
    /// gets and sets the menu right image
    /// </summary>
    public static System.Drawing.Bitmap MenuRightImage
    {
      get { return m_menuRightImage; }
      set { m_menuRightImage = value; }
    }
    
    /// <summary>
    /// gets and sets the menu middle brush
    /// </summary>
    public static System.Drawing.TextureBrush MenuMiddleBrush
    {
      get { return m_menuMiddleBrush; }
      set { m_menuMiddleBrush = value; }
    }
    
    /// <summary>
    /// gets and sets the menu text brush
    /// </summary>
    public static System.Drawing.SolidBrush MenuTextBrush
    {
      get { return m_menuTextBrush; }
      set { m_menuTextBrush = value; }
    }

    /// <summary>
    /// gets and sets the menu font
    /// </summary>
    public static System.Drawing.Font MenuFont
    {
      get { return m_menuFont; }
      set { m_menuFont = value; }
    }

    #endregion

    #region context menu properties
    
    /// <summary>
    /// gets and sets the context menu text brush
    /// </summary>
    public static System.Drawing.SolidBrush ContextMenuTextBrush
    {
      get { return m_contextMenuTextBrush; }
      set {  m_contextMenuTextBrush = value; }
    }

    /// <summary>
    /// gets and sets the context menu text disabled brush
    /// </summary>
    public static System.Drawing.SolidBrush ContextMenuTextDisabledBrush
    {
      get { return m_contextMenuTextDisabledBrush; }
      set {  m_contextMenuTextDisabledBrush = value; }
    }

    /// <summary>
    /// gets and sets the context menu font
    /// </summary>
    public static System.Drawing.Font ContextMenuFont
    {
      get { return m_contextMenuFont; }
      set { m_contextMenuFont = value; }
    }
    
    #endregion

    #region tabstrip properties
 
    /// <summary>
    /// gets and sets the tab brush
    /// </summary>
    public static System.Drawing.SolidBrush TabBrush
    {
      get { return m_tabBrush; }
      set { m_tabBrush = value; }
    }

    /// <summary>
    /// gets and sets the tab font
    /// </summary>
    public static System.Drawing.Font TabFont
    {
      get { return m_tabFont; }
      set { m_tabFont = value; }
    }
    
    /// <summary>
    /// gets and sets the tab selected brush
    /// </summary>
    public static System.Drawing.SolidBrush TabSelectedBrush
    {
      get { return m_tabSelectedBrush; }
      set { m_tabSelectedBrush = value; }
    }

    /// <summary>
    /// gets and sets the tab selected font
    /// </summary>
    public static System.Drawing.Font TabSelectedFont
    {
      get { return m_tabSelectedFont; }
      set { m_tabSelectedFont = value; }
    }

    #endregion

    #region selection properties
    
    /// <summary>
    /// gets and sets the selected brush
    /// </summary>
    public static System.Drawing.SolidBrush SelectedBrush
    {
      get { return m_selectedBrush; }
      set { m_selectedBrush = value; }
    }

    /// <summary>
    /// gets and sets the selected pen
    /// </summary>
    public static System.Drawing.Pen SelectedPen
    {
      get { return m_selectedPen; }
      set { m_selectedPen = value; }
    }

    /// <summary>
    /// gets and sets the down brush
    /// </summary>
    public static System.Drawing.Brush DownBrush
    {
      get { return m_downBrush; }
      set { m_downBrush = value; }
    }
    
    /// <summary>
    /// gets and sets the selected viewer border color
    /// </summary>
    public static System.Drawing.Color SelectedViewerBorderColor
    {
      get { return m_selectedViewerBorderColor; }
      set { m_selectedViewerBorderColor = value; }
    }
    

    #endregion

    #region toolbar properties

    /// <summary>
    /// gets and sets the toolbar left image
    /// </summary>
    public static System.Drawing.Bitmap ToolbarLeftImage
    {
      get { return m_toolbarLeftImage; }
      set { m_toolbarLeftImage = value; }
    }

    /// <summary>
    /// gets and sets the toolbar middle image
    /// </summary>
    public static System.Drawing.Bitmap ToolbarMiddleImage
    {
      get { return m_toolbarMiddleImage; }
      set { m_toolbarMiddleImage = value; }
    }
    
    /// <summary>
    /// gets and sets the toolbar middle brush
    /// </summary>
    public static System.Drawing.TextureBrush ToolbarMiddleBrush
    {
      get { return m_toolbarMiddleBrush; }
      set { m_toolbarMiddleBrush = value; }
    }
    
    /// <summary>
    /// gets and sets the toolbar right image
    /// </summary>
    public static System.Drawing.Bitmap ToolbarRightImage
    {
      get { return m_toolbarRightImage; }
      set { m_toolbarRightImage = value; }
    }

    #endregion
    
    #region progress bar properties

    /// <summary>
    /// gets the progress bar horizontal fill image
    /// </summary>
    public static System.Drawing.Bitmap ProgressBarHorzFillImage
    {
      get { return m_progessbarHorzFillImage; }
    } // ProgressBarHorzFillImage

    /// <summary>
    /// gets the progress bar horizontal track image
    /// </summary>
    public static System.Drawing.Bitmap ProgressBarHorzTrackImage
    {
      get { return m_progessbarHorzTrackImage; }
    } // ProgressBarHorzTrackImage

    /// <summary>
    /// gets the progress bar fill brush
    /// </summary>
    public static System.Drawing.TextureBrush ProgressBarFillBrush
    {
      get { return m_progessbarHorzFillBrush; }
    } // ProgressBarFillBrush

    /// <summary>
    /// gets the progress bar track brush
    /// </summary>
    public static System.Drawing.TextureBrush ProgressBarTrackBrush
    {
      get { return m_progessbarTrackBrush; }
    } // ProgressBarTrackBrush

    /// <summary>
    /// gets the progress bar fill text brush
    /// </summary>
    public static System.Drawing.Brush ProgressBarFillTextBrush
    {
      get { return m_progessbarFillTextBrush; }
    } // ProgressBarFillTextBrush

    /// <summary>
    /// gets the progress bar track text brush
    /// </summary>
    public static System.Drawing.Brush ProgressBarTrackTextBrush
    {
      get { return m_progessbarTrackTextBrush; }
    } // ProgressBarTrackTextBrush

    #endregion

    #region sysmenu properties

    /// <summary>
    /// gets and sets the system menu button images
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonImages
    {
      get { return m_sysmenuButtonImages; }
      set { m_sysmenuButtonImages = value; }
    }

    /// <summary>
    /// gets and sets the system menu button close images
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonCloseImages
    {
      get { return m_sysmenuCloseButtonImages; }
      set { m_sysmenuCloseButtonImages = value; }
    }

    /// <summary>
    /// gets and sets the system menu button minimize glyphs
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonMinimizeGlyphs
    {
      get { return m_sysmenuMinimizeGlyphs; }
      set { m_sysmenuMinimizeGlyphs = value; }
    }

    /// <summary>
    /// gets and sets the system menu button maximize glyphs
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonMaximizeGlyphs
    {
      get { return m_sysmenuMaximizeGlyphs; }
      set { m_sysmenuMaximizeGlyphs = value; }
    }
    
    /// <summary>
    /// gets and sets the system menu button restore glyphs
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonRestoreGlyphs
    {
      get { return m_sysmenuRestoreGlyphs; }
      set { m_sysmenuRestoreGlyphs = value; }
    }

    /// <summary>
    /// gets and sets the system menu button close glyphs
    /// </summary>
    public static System.Drawing.Bitmap SysmenuButtonCloseGlyphs
    {
      get { return m_sysmenuCloseGlyphs; }
      set { m_sysmenuCloseGlyphs = value; }
    }

    #endregion

    #region form properties
    
    /// <summary>
    /// gets and sets the form bottom brush
    /// </summary>
    public static TextureBrush FormBottomBrush
    {
      get { return m_formBottomBrush; }
      set { m_formBottomBrush = value; }
    } // FormBottomBrush

    /// <summary>
    /// gets and sets the form top brush
    /// </summary>
    public static TextureBrush FormTopBrush
    {
      get { return m_formTopBrush; }
      set { m_formTopBrush = value; }
    } // FormTopBrush

    /// <summary>
    /// gets and sets the form side brush
    /// </summary>
    public static TextureBrush FormSideBrush
    {
      get { return m_formSideBrush; }
      set { m_formSideBrush = value; }
    } // FormSideBrush

    /// <summary>
    /// gets and sets the form corner bottom left image
    /// </summary>
    public static Bitmap FormCornerBottomLeftImage
    {
      get { return m_formCornerBottomLeftImage; }
      set { m_formCornerBottomLeftImage = value; }
    } // FormCornerBottomLeftImage

    /// <summary>
    /// gets and sets the form corner bottom right image
    /// </summary>
    public static Bitmap FormCornerBottomRightImage
    {
      get { return m_formCornerBottomRightImage; }
      set { m_formCornerBottomRightImage = value; }
    } // FormCornerBottomRightImage

    /// <summary>
    /// gets and sets the form corner top left image
    /// </summary>
    public static Bitmap FormCornerTopLeftImage
    {
      get { return m_formCornerTopLeftImage; }
      set { m_formCornerTopLeftImage = value; }
    } // FormCornerTopLeftImage

    /// <summary>
    /// gets and sets the form corner top right image
    /// </summary>
    public static Bitmap FormCornerTopRightImage
    {
      get { return m_formCornerTopRightImage; }
      set { m_formCornerTopRightImage = value; }
    } // FormCornerTopRightImage


    /// <summary>
    /// gets and sets the form font
    /// </summary>
    public static System.Drawing.Font FormFont
    {
      get { return m_formFont; }
      set { m_formFont = value; }
    } // FormFont

    #endregion

    #endregion
	
    #region methods

    /// <summary>
    /// initialize an instance of the Theme
    /// </summary>
    static Theme()
    {
      // background ( common )
      m_bkgndColor = Color.FromArgb( 148, 166, 198 );
      m_bkgndSideRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-right.png" ));
      m_bkgndSideRightBrush = new TextureBrush( m_bkgndSideRightImage );
      m_bkgndCornerTopLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-corner-top-left.png" ));
      m_bkgndCornerBottomLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-corner-bottom-left.png" ));
      m_bkgndCornerBottomRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-corner-bottom-right.png" ));


      // general
      m_headerFont = new Font( "Arial", 12, FontStyle.Bold );
      m_formFont = new Font( "Arial", 8, FontStyle.Regular );


      // menu ( common )
      m_menuTextBrush = new SolidBrush( Color.FromArgb( 250, 250, 250 ) );
      m_menuFont = new Font( "Arial", 10, FontStyle.Bold );

      
      // context menu
      m_contextMenuTextBrush = new SolidBrush( Color.Black );
      m_contextMenuTextDisabledBrush = new SolidBrush( SystemColors.GrayText );
      m_contextMenuFont = new Font( "Tahoma", 8 );


      // pane
      m_paneExpandedBrush = new SolidBrush( Color.FromArgb( 250, 250, 250 ) );
      m_paneCollapsedBrush = new SolidBrush( Color.FromArgb( 250, 250, 250 ) );
      m_paneFont = new Font( "Arial", 10, FontStyle.Bold );
      m_paneBackColor = System.Drawing.Color.FromArgb( 123, 134, 165 );

      
      // toolbar
      m_toolbarLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.toolbar-left.png") );
      m_toolbarMiddleImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.toolbar-middle.png") );
      m_toolbarMiddleBrush = new TextureBrush( m_toolbarMiddleImage  );
      m_toolbarRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.UI.res.toolbar-right.png") );

     
      // tab control 
      m_tabFont = new Font( "Arial", 10 );
      m_tabBrush = new SolidBrush( Color.Black );
      m_tabSelectedFont = new Font( "Arial", 10 );
      m_tabSelectedBrush = new SolidBrush( Color.Black );
    
      
      // scrollable control
      m_fillColor = Color.FromArgb( 124, 129, 148 );

      
      // progressbar
      m_progessbarHorzFillImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.progressbar-horz-fill.bmp" ));
      m_progessbarHorzFillImage.MakeTransparent( Color.Fuchsia );
      m_progessbarHorzTrackImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.progressbar-horz-track.bmp" ));
      m_progessbarHorzTrackImage.MakeTransparent( Color.Fuchsia );
      m_progessbarHorzFillBrush = new TextureBrush( m_progessbarHorzFillImage );
      m_progessbarTrackBrush = new TextureBrush( m_progessbarHorzTrackImage, new Rectangle( 3, 0, 3, m_progessbarHorzTrackImage.Height ));
      m_progessbarFillTextBrush = Brushes.Blue;
      m_progessbarTrackTextBrush = Brushes.Black;

      //  system menu ( common )
      m_sysmenuCloseButtonImages = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-buttons-close.png" ));
      m_sysmenuMaximizeGlyphs = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-glyphs-maximize.png" ));
      m_sysmenuMinimizeGlyphs = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-glyphs-minimize.png" ));
      m_sysmenuRestoreGlyphs = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-glyphs-restore.png" ));
      m_sysmenuCloseGlyphs = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-glyphs-close.png" ));


      SetThemeColor( ThemeColor.Default );
    } // Theme()
    

    /// <summary>
    /// Load the theme from XML node.
    /// </summary>
    /// <param name="node"></param>
    public static void LoadThemeFromXml( System.Xml.XmlNode node )
    {
      //get the Theme Color
      System.Xml.XmlNode colorNode = node[ "color" ];
      if ( colorNode != null )
      {
        string [] rgb    = colorNode.InnerText.Split(',');
        Color themeColor = System.Drawing.Color.FromArgb ( System.Convert.ToInt32( rgb[0], 10), System.Convert.ToInt32( rgb[1], 10), System.Convert.ToInt32( rgb[2], 10) );
        SetThemeColor( themeColor );
      }
      
      //get the Back ground color
      System.Xml.XmlNode backColorNode = node[ "backColor" ];
      if ( backColorNode != null )
      {
        string [] backColor = backColorNode.InnerText.Split(',');
        m_bkgndColor        = System.Drawing.Color.FromArgb ( System.Convert.ToInt32( backColor[0], 10), System.Convert.ToInt32( backColor[1], 10), System.Convert.ToInt32( backColor[2], 10) );
      }

      //Get the Font and Its size.
      //default font size is 8.
      int size = 8;
      System.Xml.XmlNode fontNode = node[ "font" ];
      System.Xml.XmlNode fontSizeNode = node[ "fontSize" ];

      if ( fontSizeNode != null )
        size = System.Convert.ToInt32( fontSizeNode.InnerText );

      if ( fontNode != null )
        FormFont = new System.Drawing.Font( fontNode.InnerText, size );

    }//LoadThemeFromXml( System.Xml.XmlNode node )

    
    /// <summary>
    /// draw the background
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="bounds">the bounds within which to draw</param>
    public static  void DrawBackground( Graphics gfx, Rectangle bounds )
    {
      // fill color
      gfx.Clear( m_bkgndColor );

      // draw the left edge
      gfx.FillRectangle( m_bkgndSideLeftBrush,  bounds.X, bounds.Y, m_bkgndSideLeftImage.Width, bounds.Height );
      
      // draw the botom edge
      m_bkgndSideBottomBrush.TranslateTransform(0, ( bounds.Height - m_bkgndSideBottomBrush.Image.Height ) % m_bkgndSideBottomBrush.Image.Height );
      gfx.FillRectangle( m_bkgndSideBottomBrush,  bounds.X, bounds.Height - m_bkgndSideBottomBrush.Image.Height, bounds.Width, m_bkgndSideBottomBrush.Image.Height  );
      m_bkgndSideBottomBrush.ResetTransform();
      
      // draw the right edge
      m_bkgndSideRightBrush.TranslateTransform(( bounds.Width - m_bkgndSideRightImage.Width ) % m_bkgndSideRightImage.Width, 0 );
      gfx.FillRectangle( m_bkgndSideRightBrush,  bounds.Width - m_bkgndSideRightImage.Width, bounds.Y, m_bkgndSideRightImage.Width, bounds.Height );
      m_bkgndSideRightBrush.ResetTransform();
      
      // draw the top left corner
      gfx.DrawImage( m_bkgndCornerTopLeftImage, bounds.X, bounds.Y );

      // draw the bottom left corner
      gfx.DrawImage( m_bkgndCornerBottomLeftImage, bounds.X, bounds.Height - m_bkgndCornerBottomLeftImage.Height );

      // draw the bottom right corner
      gfx.DrawImage( m_bkgndCornerBottomRightImage, bounds.Width - m_bkgndCornerBottomRightImage.Width, bounds.Height - m_bkgndCornerBottomRightImage.Height );
    } // DrawBackground( Graphics gfx, Rectangle bounds )

    
    /// <summary>
    /// set the theme color
    /// </summary>
    /// <param name="color">the desired theme color</param>
    public static void SetThemeColor( ThemeColor color )
    {
      switch( color )
      {
        case ThemeColor.Purple:
          SetPurpleTheme();
          break;
        case ThemeColor.Red:
          SetRedTheme();
          break;
        case ThemeColor.Yellow:
          SetYellowTheme();
          break;
        default:
          break;
      } // switch( color )
    } // SetThemeColor( ThemeColor color )


    /// <summary>
    /// Sets the Theme Color
    /// </summary>
    /// <param name="red">int</param>
    /// <param name="green">int</param>
    /// <param name="blue">int</param>
    public static void SetThemeColor ( int red, int green, int blue )
    {
      SetThemeColor( Color.FromArgb( red, green, blue) );
    }// SetThemeColor ( int red, int green, int blue )

    
    /// <summary>
    /// set the theme color
    /// </summary>
    /// <param name="color">the desired theme color</param>
    public static void SetThemeColor( Color desiredColor )
    {

      SetRedTheme();
      Bitmap testImage = m_menuMiddleImage;
      Color initialPixel = testImage.GetPixel( testImage.Width / 2, testImage.Height / 2 );

      // try some variations of HSL
      float stepSize = 0.1f;
      float goodH = 0, goodS = 0, goodL = 0;
      float closest = float.MaxValue;
      for ( float h = 0; h <= 360; h += 1 )
      {
        for ( float s = 0; s <= 1; s += stepSize )
        {
          for ( float l = 0; l <= 1; l += stepSize )
          {
            
            // RGB -> HSL
            Utilities.HSL hsl = Utilities.HSL.FromRGB( initialPixel );
            
            // adjust HSL a bit
            float hue = hsl.Hue + h; if ( hue > 360 ) hue -= 360; hue = Math.Min( 360, Math.Max( 0, hue ) );
            //saturation = saturation + s + 1; if ( saturation > 1 ) saturation -= 1; saturation = Math.Min( 1, Math.Max( 0, saturation ) );
            //lightness  = lightness  + l + 1; if ( lightness  > 1 ) lightness  -= 1; lightness  = Math.Min( 1, Math.Max( 0, lightness  ) );

            // HSL -> RGB
            Utilities.HSL adjustedHsl = new Utilities.HSL( hue, hsl.Saturation, hsl.Luminance );
            Color adjustedRGB = adjustedHsl.RGB;

            // check for nearness to desired theme color
            float close = (float) Math.Sqrt( Math.Pow( adjustedRGB.R - desiredColor.R, 2 ) + Math.Pow( adjustedRGB.G - desiredColor.G, 2 ) + Math.Pow( adjustedRGB.B - desiredColor.B, 2 ) );
            if ( close < closest )
            {
              closest = close;
              goodH = h;
              //goodS = s;
              //goodL = l;
            }

          }
        }
      }

      // now adjust all the pixels in all the images
      Bitmap image1 = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-side-left-red.png" ));
      Bitmap image2 = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-bottom-middle-red.png" ));
      Bitmap image3 = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-top-middle-red.png" ));
      Bitmap [] workImages = { m_bkgndSideLeftImage, m_bkgndSideBottomImage, m_menuLeftImage, m_menuMiddleImage,
                               m_menuRightImage, m_sysmenuButtonImages, m_formCornerBottomLeftImage,
                               m_formCornerBottomRightImage, m_formCornerTopLeftImage, m_formCornerTopRightImage,
                               image1, image2, image3 };
      for ( int i=0; i<workImages.Length; i++ )
      {
        Bitmap workImage = workImages[i];
        for ( int y=0; y<workImage.Height; y++ )
        {
          for ( int x=0; x<workImage.Width; x++ )
          {

            // RGB -> HSL
            Color origPixel = workImage.GetPixel( x, y );
            Utilities.HSL hsl = Utilities.HSL.FromRGB( origPixel );
              
            // adjust HSL a bit
            float hue = hsl.Hue + goodH; if ( hue > 360 ) hue -= 360; hue = Math.Min( 360, Math.Max( 0, hue ) );

            // HSL -> RGB
            Utilities.HSL adjustedHsl = new Utilities.HSL( hue, hsl.Saturation, hsl.Luminance );
            Color adjustedRGB = Color.FromArgb( origPixel.A, adjustedHsl.RGB );
            workImage.SetPixel( x, y, adjustedRGB );

          }
        }
      }

      // make the brushes
      m_bkgndSideLeftBrush = new TextureBrush( m_bkgndSideLeftImage );
      m_bkgndSideBottomBrush = new TextureBrush( m_bkgndSideBottomImage  );
      m_menuMiddleBrush = new TextureBrush( m_menuMiddleImage );
      m_formSideBrush = new TextureBrush( image1 );
      m_formBottomBrush = new TextureBrush( image2 );
      m_formTopBrush = new TextureBrush( image3 );

    } // SetThemeColor( Color color )


    /// <summary>
    /// sets the purple theme
    /// </summary>
    private static void SetPurpleTheme()
    {
      // background ( color specific )
      m_bkgndSideLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-left-purple.png" ));
      m_bkgndSideLeftBrush = new TextureBrush( m_bkgndSideLeftImage );
      m_bkgndSideBottomImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-bottom-purple.png" ));
      m_bkgndSideBottomBrush = new TextureBrush( m_bkgndSideBottomImage  );

      // menu ( color specific )
      m_menuLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-purple-left.png" ));
      m_menuMiddleImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-purple-middle.png" ));
      m_menuRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-purple-right.png" ));
      m_menuMiddleBrush = new TextureBrush( m_menuMiddleImage );

      //  system menu ( color specific )
      m_sysmenuButtonImages = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-buttons-purple.png" ));

      // form ( color specific )
      m_formSideBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-side-left-purple.png" )));
      m_formBottomBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-bottom-middle-purple.png" )));
      m_formTopBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-top-middle-purple.png" )));
      m_formCornerBottomLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-left-purple.png" ));
      m_formCornerBottomRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-right-purple.png" ));
      m_formCornerTopLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-left-purple.png" ));
      m_formCornerTopRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-right-purple.png" ));
        
    } // SetPurpleTheme()
    
    
    /// <summary>
    /// sets the red theme
    /// </summary>
    private static void SetRedTheme()
    {
      // background ( color specific )
      m_bkgndSideLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-left-red.png" ));
      m_bkgndSideLeftBrush = new TextureBrush( m_bkgndSideLeftImage );
      m_bkgndSideBottomImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-bottom-red.png" ));
      m_bkgndSideBottomBrush = new TextureBrush( m_bkgndSideBottomImage  );

      // menu ( color specific )
      m_menuLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-red-left.png" ));
      m_menuMiddleImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-red-middle.png" ));
      m_menuRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-red-right.png" ));
      m_menuMiddleBrush = new TextureBrush( m_menuMiddleImage );

      //  system menu ( color specific )
      m_sysmenuButtonImages = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-buttons-red.png" ));

      // form ( color specific )
      m_formSideBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-side-left-red.png" )));
      m_formBottomBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-bottom-middle-red.png" )));
      m_formTopBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-top-middle-red.png" )));
      m_formCornerBottomLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-left-red.png" ));
      m_formCornerBottomRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-right-red.png" ));
      m_formCornerTopLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-left-red.png" ));
      m_formCornerTopRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-right-red.png" ));

    } // SetRedTheme()

    
    /// <summary>
    /// sets the yellow theme
    /// </summary>
    private static void SetYellowTheme()
    {
      // background ( color specific )
      m_bkgndSideLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-left-yellow.png" ));
      m_bkgndSideLeftBrush = new TextureBrush( m_bkgndSideLeftImage );
      m_bkgndSideBottomImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.bkgnd-side-bottom-yellow.png" ));
      m_bkgndSideBottomBrush = new TextureBrush( m_bkgndSideBottomImage  );

      // menu ( color specific )
      m_menuLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-yellow-left.png" ));
      m_menuMiddleImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-yellow-middle.png" ));
      m_menuRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.menu-yellow-right.png" ));
      m_menuMiddleBrush = new TextureBrush( m_menuMiddleImage );

      //  system menu ( color specific )
      m_sysmenuButtonImages = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.sysmenu-buttons-yellow.png" ));

      // form ( color specific )
      m_formSideBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-side-left-purple.png" )));
      m_formBottomBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-bottom-middle-purple.png" )));
      m_formTopBrush = new TextureBrush( new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-top-middle-purple.png" )));
      m_formCornerBottomLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-left-purple.png" ));
      m_formCornerBottomRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-bottom-right-purple.png" ));
      m_formCornerTopLeftImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-left-purple.png" ));
      m_formCornerTopRightImage = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.UI.res.form-corner-top-right-purple.png" ));

    } // SetYellowTheme()
    
    #endregion
  
  } // class Theme
} // namespace Viatronix.UI

#region revision history

// $Log: Theme.cs,v $
// Revision 1.18  2005/11/16 18:10:13  romy
// added LoadThemeFromXMl Method.
//
// Revision 1.17  2005/11/16 18:08:30  romy
// added LoadThemeFromXMl Method.
//
// Revision 1.16  2005/11/04 21:18:23  frank
// added ability to change the theme color to any nearly any system color.
//
// Revision 1.15  2005/10/05 13:05:36  gdavidson
// Added font property for the form.
//
// Revision 1.14  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.13  2005/02/24 19:11:51  gdavidson
// Added BackColor property.
//
// Revision 1.12.2.3  2005/02/28 21:10:53  frank
// code review
//
// Revision 1.12.2.2  2005/02/11 20:18:13  geconomos
// Changed font for context menus from "Arial" to "Tohama"
//
// Revision 1.12.2.1  2005/02/10 21:22:57  geconomos
// Issue #3932: Ability to disable load session if data does not exist
//
// Revision 1.12  2004/11/19 23:31:48  geconomos
// added form borders
//
// Revision 1.11  2004/11/19 22:30:43  gdavidson
// Added themes for the dialog.
//
// Revision 1.10  2004/11/17 22:38:44  geconomos
// added system menu graphics
//
// Revision 1.9  2004/11/17 13:23:58  geconomos
// added new theme elements
//
// Revision 1.8  2004/11/16 21:28:04  gdavidson
// Added new toolbar.
//
// Revision 1.7  2004/11/16 13:48:09  geconomos
// added support for side tabs
//
// Revision 1.6  2004/11/11 20:22:28  gdavidson
// Added themes for the progress bar.
//
// Revision 1.5  2004/10/20 13:10:21  geconomos
// changed the text color for the panes
//
// Revision 1.4  2004/10/18 14:50:17  geconomos
// added PaneBackColor property
//
// Revision 1.3  2004/10/18 02:17:26  geconomos
// added new items
//
// Revision 1.2  2004/10/13 16:13:38  geconomos
// new graphics
//
// Revision 1.1  2004/10/11 23:59:15  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Theme.cs,v 1.18 2005/11/16 18:10:13 romy Exp $
// $Id: Theme.cs,v 1.18 2005/11/16 18:10:13 romy Exp $

#endregion
