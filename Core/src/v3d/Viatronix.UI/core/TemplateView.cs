// $Id: TemplateView.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Collections;
using System.Xml;

namespace Viatronix.UI
{
	/// <summary>
	/// Describes a single "tab" that can be plugged into the viewer's main application window.
	/// </summary>
  public class TemplateView
	{
    #region fields

 
    /// <summary>
    /// current xml version
    /// </summary>
    private const string TEMPLATE_VIEW_XML_VERSION = "1.0";

    /// <summary>
    /// unique tab id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// associated text
    /// </summary>
    private string m_text = string.Empty;
    
    /// <summary>
    /// indicates if the pane manager should be visible
    /// </summary>
    private bool m_displayPanes = true;

    /// <summary>
    /// additional menu items to merge with application menu
    /// </summary>
    private ArrayList m_menus = new ArrayList();

    /// <summary>
    /// Plugin Panes associated with tab
    /// </summary>
    private ArrayList m_panes = new ArrayList();

    /// <summary>
    /// typename of associated IView implementer
    /// </summary>
    private string m_typename = string.Empty;

    /// <summary>
    /// additional xml data to initialize the tab control with
    /// </summary>
    private string m_initializationData = string.Empty;

    #endregion

    #region properties

      
    /// <summary>
    /// Gets or sets the unique id.
    /// </summary>
    public string Id
    {
      get { return m_id; }
      set { m_id = value; }
    } // Id
    
   
    /// <summary>
    /// Gets or sets the text.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text

    
    /// <summary>
    /// Gets or sets the display panes flag
    /// </summary>
    public bool DisplayPanes
    {
      get { return m_displayPanes; }
      set { m_displayPanes = value; }
    } // DisplayPanes

   
    /// <summary>
    /// Gets or sets the typename of associated IView implementer.
    /// </summary>
    public string Typename
    {
      get { return m_typename; }
      set { m_typename = value; }
    } // Typename
    
    /// <summary>
    /// Gets or sets the Menus collections
    /// </summary>
    public ArrayList Menus
    {
      get { return m_menus; }
      set { m_menus = value; }
    } // Menus
   
  
    /// <summary>
    /// Gets or sets the Panes collections
    /// </summary>
    public ArrayList Panes
    {
      get { return m_panes; }
      set { m_panes = value; }
    } // Panes
   

    /// <summary>
    /// Gets or sets the additional xml initialization data.
    /// </summary>
    public string InitializationData
    {
      get { return m_initializationData; }
      set { m_initializationData = value; }
    } // InitializationData

    
    #endregion

    #region serialization

    /// <summary>
    /// Creates a new TemplateView instance from the specified xml node.
    /// </summary>
    /// <param name="parent">XmlNode containing initialization data</param>
    /// <returns>New TemplateView instance.</returns>
    public static TemplateView Create(  XmlNode parent )
    {

      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != TEMPLATE_VIEW_XML_VERSION )
        throw new ArgumentException( "Invalid xml version for class TemplateView." );

      TemplateView tab = new TemplateView();

      if (tab == null)
        throw new OutOfMemoryException("Failed to allocate TemplateView for tab");

      tab.Id        = parent.Attributes[ "id" ].Value;
      tab.Text      = parent.Attributes[ "text" ].Value;
      tab.Typename  = parent.Attributes[ "typename" ].Value; 

      if( parent.Attributes[ "displayPanes" ] != null )
      {
        tab.DisplayPanes  = Boolean.Parse( parent.Attributes[ "displayPanes" ].Value ); 
      }
     
      XmlNode additionalDataNode = parent.SelectSingleNode( "additional_data" );
      if( additionalDataNode != null )
      {
        tab.InitializationData  = additionalDataNode.InnerXml;
      }

      foreach( XmlNode node in parent.SelectNodes( "menus/menu" ) )
      {
        tab.Menus.Add( FrameMenu.Create(  node ) );
      }

      foreach( XmlNode node in parent.SelectNodes( "panes/pane" ) )
      {
        tab.Panes.Add(TemplatePane.Create(node));
      }
      
      return tab;
    } // FromXml( XmlNode parent )

    
    #endregion
	
  } // class TemplateView
} // namespace Viatronix.Viewer.Plugins

#region revision history

// $Log: TemplateView.cs,v $
// Revision 1.4.2.2  2009/02/06 19:19:13  kchalupa
// Region crashproofing.
//
// Revision 1.4.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.4  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.3  2006/04/26 15:34:15  gdavidson
// Added ability to hide panes on creation.
//
// Revision 1.2  2005/10/27 15:49:38  gdavidson
// reworked pane creation per view
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.3  2004/12/10 16:56:44  geconomos
// added default tool id
//
// Revision 1.2  2004/10/18 02:49:43  geconomos
// reworked plugin tab
//
// Revision 1.1  2004/10/12 00:03:42  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/TemplateView.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $
// $Id: TemplateView.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $

#endregion