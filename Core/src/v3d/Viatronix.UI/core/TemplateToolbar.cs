// $Id: TemplateToolBar.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// Describes a single toolbar that can be added to the viewer application.
  /// </summary>
	public class TemplateToolBar
	{
    #region fields

    /// <summary>
    /// xml version
    /// </summary>
    private const string FRAME_TOOLBAR_XML_VERSION = "1.0";

    /// <summary>
    /// id
    /// </summary>
    private string m_id = string.Empty;
    
    /// <summary>
    /// text to display
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// associated toolbar control typename
    /// </summary>
    private string m_typename = string.Empty;

    /// <summary>
    /// extra xml initialization data for toolbar
    /// </summary>
    private string m_innerXml = string.Empty;

    /// <summary>
    /// horizontal alignment
    /// </summary>
    private HorizontalAlignment m_align = HorizontalAlignment.Left;

    /// <summary>
    ///  indicates if this toolbar 
    /// </summary>
    private bool m_newRow = false;

    #endregion
		
    #region properties
    
    /// <summary>
    /// Gets or sets the Id
    /// </summary>
    public string Id
    {
      get { return m_id; }
      set { m_id = value; }
    } // Id

    /// <summary>
    /// Gets or sets the text
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text

    /// <summary>
    /// Gets or sets the typename
    /// </summary>
    public string Typename
    {
      get { return m_typename; }
      set { m_typename = value; }
    } // Typename

    /// <summary>
    /// Gets or sets the inner xml
    /// </summary>
    public string InnerXml
    {
      get { return m_innerXml; }
      set { m_innerXml = value; }
    } // InnerXml

    /// <summary>
    /// Gets or sets if this toolbar start a new row
    /// </summary>
    public bool NewRow
    {
      get { return m_newRow; }
      set { m_newRow = value; }
    } // NewRow

    /// <summary>
    /// Gets or set the horizonal alignment for the control
    /// </summary>
    public HorizontalAlignment Align
    {
      get { return m_align; }
      set { m_align = value; }
    } // Align

    #endregion

    #region initialization

    /// <summary>
    /// Default Consturctor
    /// </summary>
    public TemplateToolBar()
    {
    } // TemplateToolBar()
    
    /// <summary>
    /// Consturctor from id, text, typename and xml
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="text">toolbar</param>
    /// <param name="typename">typename</param>
    /// <param name="innerXml">xml initialization data</param>
    public TemplateToolBar( string id, string text, string typename, string innerXml ) : this( id, text, typename, innerXml, HorizontalAlignment.Left )
		{
    } //TemplateToolBar( string id, string text, string typename, string innerXml ) : this( id, text, typename, innerXml, HorizontalAlignment.Left )


    /// <summary>
    /// Constructor from id, text, typename, innerxml and alignment
    /// </summary>
    /// <param name="id">id</param>
    /// <param name="text">toolbar</param>
    /// <param name="typename">typename</param>
    /// <param name="innerXml">xml initialization data</param>
    /// <param name="align">horizontal alignment</param>
    public TemplateToolBar( string id, string text, string typename, string innerXml, HorizontalAlignment align )
		{
      m_id = id;
      m_text = text;
      m_typename = typename;
      m_innerXml = innerXml;
      m_align = align;
    } // TemplateToolBar( string id, string text, string typename, string innerXml, HorizontalAlignment align )
    
    #endregion

    #region serialization

    /// <summary>
    /// Creates a ToolBarTemplate from xml
    /// </summary>
    /// <param name="parent">xml node</param>
    /// <returns>ToolBarTemplate instance</returns>
    public static TemplateToolBar Create( XmlNode parent )
    {
      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != FRAME_TOOLBAR_XML_VERSION )
        throw new ArgumentException( "Invalid xml version for class TemplateToolBar." );

      TemplateToolBar toolbar = new TemplateToolBar();
      toolbar.Id        = parent.Attributes[ "id" ].Value;
      toolbar.Text      = parent.Attributes[ "text" ].Value; 
      toolbar.Typename  = parent.Attributes[ "typename" ].Value;
      toolbar.InnerXml  = parent.InnerXml;

      if( parent.Attributes[ "align" ] != null )
      {
        toolbar.Align   = (HorizontalAlignment) Enum.Parse( typeof( HorizontalAlignment ), parent.Attributes[ "align" ].Value, true );
      }
      
      if( parent.Attributes[ "new_row" ] != null )
      {
        toolbar.NewRow  = Boolean.Parse( parent.Attributes[ "new_row" ].Value);
      }
      
      return toolbar;
    } // CReate( XmlNode parent )

    
    #endregion

	} // class TemplateToolBar
} // namespace Viatronix.Viewer.Plugins

#region revision history

// $Log: TemplateToolBar.cs,v $
// Revision 1.4.2.2  2009/02/06 19:19:13  kchalupa
// Region crashproofing.
//
// Revision 1.4.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.4  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.3  2006/08/11 21:10:14  gdavidson
// Added Align property
//
// Revision 1.2  2005/11/23 01:58:18  frank
// removed unused "show caption" option
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.1  2004/09/15 17:49:38  geconomos
// moved files
//
// Revision 1.2  2004/07/09 18:51:11  geconomos
// extended parameter list for FromXml( ... ) to take the parent Plugin instance
//
// Revision 1.1  2004/05/13 13:28:42  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/TemplateToolBar.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $
// $Id: TemplateToolBar.cs,v 1.4.2.2 2009/02/06 19:19:13 kchalupa Exp $

#endregion