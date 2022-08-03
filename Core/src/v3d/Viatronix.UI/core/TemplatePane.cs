// $Id: TemplatePane.cs,v 1.2.2.2 2009/02/06 19:19:13 kchalupa Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Xml;

namespace Viatronix.UI
{
  /// <summary>
  /// Describes a single "pane" that can be added to the viewer application.
  /// </summary>
	public class TemplatePane
	{
    #region fields

 
    /// <summary>
    /// current xml version
    /// </summary>
    private const string TEMPLATE_PANE_XML_VERSION = "1.0";

    /// <summary>
    /// unique id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// pane text
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// typename of class implementing ui
    /// </summary>
    private string m_typename = string.Empty;

    /// <summary>
    /// indicates if the pane is initially expanded
    /// </summary>
    private bool m_expanded = false;

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
    /// Gets or sets the typename of associated IPluginTab implementer.
    /// </summary>
    public string Typename
    {
      get { return m_typename; }
      set { m_typename = value; }
    } // Typename

    /// <summary>
    /// Gets or sets if the pane is initially expanded
    /// </summary>
    public bool Expanded 
    {
      get { return m_expanded; }
      set { m_expanded = value; }
    } // Expanded 
    
    #endregion

    #region initialization

		/// <summary>
		/// Initializes a TemplatePane with default values.
		/// </summary>
    public TemplatePane()
		{
		} // TemplatePane()

    
    #endregion

    #region serialization

    /// <summary>
    /// Creates a new TemplatePane instance from the specified xml node.
    /// </summary>
    /// <param name="parent">XmlNode containing initialization data</param>
    /// <returns>New TemplatePane instance.</returns>
    public static TemplatePane Create(  XmlNode parent )
    {
      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != TEMPLATE_PANE_XML_VERSION )
        throw new ArgumentException( "Invalid xml version for class TemplatePane." );

      TemplatePane pane = new TemplatePane();

      if (pane == null)
        throw new OutOfMemoryException("Failed to allocate Pane");


      pane.Id = parent.Attributes[ "id" ].Value;
      
      if( parent.Attributes[ "text" ] != null )
      {
        pane.Text      = parent.Attributes[ "text" ].Value;
      }
      
      if( parent.Attributes[ "typename" ] != null )
      {
        pane.Typename  = parent.Attributes[ "typename" ].Value; 
      }
      
      if( parent.Attributes[ "expanded" ] != null )
      {
        pane.Expanded  = Boolean.Parse( parent.Attributes[ "expanded" ].Value); 
      }

      return pane;
    } // Create( XmlNode parent )

    
    #endregion

	} // class TemplatePane
} // namespace Viatronix.Viewer.Plugins

#region revision history

// $Log: TemplatePane.cs,v $
// Revision 1.2.2.2  2009/02/06 19:19:13  kchalupa
// Region crashproofing.
//
// Revision 1.2.2.1  2009/01/14 15:01:57  kchalupa
// Issue 6217: Provides a set of methods and constants for data output that is region neutral.
//
// Revision 1.2  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.1  2005/01/26 19:07:05  geconomos
// initial revision
//
// Revision 1.1  2005/01/12 17:10:08  geconomos
// moved from viatronix.v3d.viewer.plugins
//
// Revision 1.1  2004/10/18 02:48:41  geconomos
// renamed class
//
// Revision 1.1  2004/09/15 17:49:38  geconomos
// moved files
//
// Revision 1.2  2004/07/09 18:50:05  geconomos
// corrected spelling mistakes
//
// Revision 1.1  2004/05/13 13:28:42  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/TemplatePane.cs,v 1.2.2.2 2009/02/06 19:19:13 kchalupa Exp $
// $Id: TemplatePane.cs,v 1.2.2.2 2009/02/06 19:19:13 kchalupa Exp $

#endregion