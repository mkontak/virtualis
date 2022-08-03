// $Id: MenuHelp.cs,v 1.4.2.1 2007/04/19 14:17:31 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this Help
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.Reflection;

using Viatronix.UI;


namespace Viatronix.Console
{
  /// <summary>
  /// Provides the "Help" menu.
  /// </summary>
  public class MenuHelp : Viatronix.UI.FrameMenu
  {  

    #region fields

    /// <summary>
    /// "About" menu item image
    /// </summary>
    private static readonly System.Drawing.Bitmap MENU_IMAGE_ABOUT = null;

    /// <summary>
    /// "About" menu item image
    /// </summary>
    private static readonly System.Drawing.Bitmap MENU_IMAGE_HELP = null;

    /// <summary>
    /// "Help" menu text
    /// </summary>
    private static readonly string MENU_TEXT_HELP = "Help";

    /// <summary>
    /// "About" menu text 
    /// </summary>
    private static readonly string MENU_TEXT_ABOUT = "About";

    /// <summary>
    /// reference to the about dialog
    /// </summary>
    private static FormAbout m_aboutDialog = null;

    /// <summary>
    /// Context menu object
    /// </summary>
    private static Viatronix.UI.ContextMenu m_contextMenu = new Viatronix.UI.ContextMenu ( );

    /// <summary>
    /// Event Handler object for menu item click
    /// </summary>
    private static EventHandler m_handler = null;

    /// <summary>
    /// About Menu item
    /// </summary>
    private static MenuItemEx m_aboutMenuItem = null;

    /// <summary>
    /// Help menu item
    /// </summary>
    private static MenuItemEx m_helpMenuItem = null;
        
    #endregion

    #region initialization


    /// <summary>
    /// static constructor
    /// </summary>
    static MenuHelp()
    {
      MENU_IMAGE_ABOUT = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.Console.res.toolbar-standard-about.png" ) );
      MENU_IMAGE_HELP = new Bitmap( Assembly.GetExecutingAssembly().GetManifestResourceStream( "Viatronix.Console.res.toolbar-standard-help.png" ) );

      Global.Frame.VisibleChanged += new EventHandler( OnFormVisibleChangedHandler );

    } // MenuHelp()


    /// <summary>
    /// Initializes a MenuHelp instance with default values.
    /// </summary>
    public MenuHelp()
    {
      this.Text = "Help";

      m_handler = new EventHandler ( this.OnMenuClickHandler );

      m_helpMenuItem  = new MenuItemEx ( MENU_TEXT_HELP, m_handler, MENU_IMAGE_HELP );
      m_aboutMenuItem = new MenuItemEx ( MENU_TEXT_ABOUT, m_handler, MENU_IMAGE_ABOUT );  
 
    } // MenuHelp()

    #endregion

    #region methods
    
    /// <summary>
    /// gets the context menu
    /// </summary>
    /// <returns>the context menu</returns>
    public override Viatronix.UI.ContextMenu GetContextMenu()
    {
      m_contextMenu.MenuItems.Clear ( );
      m_contextMenu.MenuItems.Add ( m_helpMenuItem );
      m_contextMenu.MenuItems.Add ( m_aboutMenuItem );

      return m_contextMenu;
    } // GetContextMenu()

    
    /// <summary>
    /// Handler for the menu click event
    /// </summary>
    /// <param name="sender">sender of this event</param>
    /// <param name="args">EventArgs containing data related to this event</param>
    private void OnMenuClickHandler( object sender, EventArgs args )
    {
      MenuItemEx item = ( MenuItemEx ) sender;
      
      if( item.Text == MENU_TEXT_HELP )
        ShowHelp();

      else if( item.Text == MENU_TEXT_ABOUT )
        ShowAbout();
    } // OnMenuClickHandler( object sender, EventArgs args )
    
        
    /// <summary>
    /// Shows the help file for the current plugin
    /// </summary>
    public static void ShowHelp()
    {
      try
      {
        string helpfile = System.IO.Path.Combine(
        System.IO.Path.GetDirectoryName( Assembly.GetEntryAssembly().Location ),
        System.Configuration.ConfigurationManager.AppSettings [ "HelpFile" ] );

        if ( System.IO.File.Exists( helpfile ) )
        {
          System.Windows.Forms.Help.ShowHelp( Global.Frame /*ApplicationForm*/, helpfile );
        }
        else
        {
          Viatronix.UI.MessageBox.ShowError( string.Format( "Unable to locate help file at {0}. Please contact service.", helpfile ) );
        }
      } // try
      catch ( Exception e )
      {
        Viatronix.UI.MessageBox.ShowError( e );
      }
    } // ShowHelp()

    
    /// <summary>
    /// Shows the about box.
    /// </summary>
    public static void ShowAbout()
    {
      m_aboutDialog = new FormAbout();
      m_aboutDialog.Version = Assembly.GetEntryAssembly().GetName().Version.ToString();
      m_aboutDialog.ProductID = "V3D-Console";
      m_aboutDialog.ComputerName = Environment.MachineName;
      m_aboutDialog.SiteID = Global.Site.Id.ToString();

      m_aboutDialog.AllowClose = false;
      m_aboutDialog.Click += new EventHandler( OnAboutClickHandler );

      m_aboutDialog.ShowDialog( Global.Frame );

      m_aboutDialog = null;
    } // ShowAbout()
    
    #endregion

    #region event handlers

    /// <summary>
    /// Handles the visible changed event for the main form.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private static void OnFormVisibleChangedHandler( object sender, EventArgs args )
    {
      if( m_aboutDialog != null )
        m_aboutDialog.Visible = Global.Frame.Visible;
    } // OnFormVisibleChangedHandler( sender, args )


    /// <summary>
    /// Handles the click event for the about dialog.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private static void OnAboutClickHandler( object sender, EventArgs args )
    {
      m_aboutDialog.Close();
    } // OnFormVisibleChangedHandler( sender, args )

    #endregion

  } // class MenuHelp
} // namespace Viatronix.Console

#region revision history

// $Log: MenuHelp.cs,v $
// Revision 1.4.2.1  2007/04/19 14:17:31  romy
// code added to show Help file
//
// Revision 1.4  2007/03/07 22:12:17  gdavidson
// Changed ownership
//
// Revision 1.3  2006/08/17 20:41:51  romy
// Fixed crash among menu and timer
//
// Revision 1.2  2006/05/09 20:17:35  gdavidson
// Added site and product information
//
// Revision 1.1  2006/02/10 21:39:15  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/menus/MenuHelp.cs,v 1.4.2.1 2007/04/19 14:17:31 romy Exp $
// $Id: MenuHelp.cs,v 1.4.2.1 2007/04/19 14:17:31 romy Exp $

#endregion