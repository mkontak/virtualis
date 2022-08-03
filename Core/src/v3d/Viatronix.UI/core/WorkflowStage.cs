// $Id: WorkflowStage.cs,v 1.3 2007/03/01 17:09:18 geconomos Exp $
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
  /// Represents a workflow stage in the explorer (i.e. tabs along the bottom)
  /// </summary>
	public class WorkflowStage
	{
    #region fields

    /// <summary>
    /// current xml version
    /// </summary>
    private const string TAB_WORKFLOW_XML_VERSION = "1.0";

 
    /// <summary>
    /// unique tab id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// associated text
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// inidcates if the tab is initially enabled
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// associated views
    /// </summary>
    private ArrayList m_viewTemplates = new ArrayList();

    /// <summary>
    /// the id for the default view
    /// </summary>
    private TemplateView m_defaultViewTemplate = null;

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
    /// Gets or ets the enabled flag
    /// </summary>
    public bool Enabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    } // Enabled

    /// <summary>
    /// Gets or sets the views
    /// </summary>
    public ArrayList ViewTemplates
    {
      get { return m_viewTemplates; }
      set { m_viewTemplates = value; }
    } // ViewTemplates


    /// <summary>
    /// Gets or sets the default view id
    /// </summary>
    public TemplateView DefaultViewTemplate
    {
      get { return m_defaultViewTemplate; }
      set { m_defaultViewTemplate = value; }
    } // DefaultViewTemplate

    
    #endregion

    #region initialization
		
    /// <summary>
    /// Constructor
    /// </summary>
    public WorkflowStage()
		{
    } // WorkflowStage()

    #endregion

    #region serialization

    /// <summary>
    /// Creates a new TemplateView instance from the specified xml node.
    /// </summary>
    /// <param name="parent">XmlNode containing initialization data</param>
    /// <returns>New TemplateView instance.</returns>
    public static WorkflowStage Create( XmlNode parent )
    {
      // version checking
      string version = parent.Attributes[ "version" ].Value;
      if( version != TAB_WORKFLOW_XML_VERSION )
        throw new ArgumentException( "Invalid xml version for class WorkflowStage." );

      WorkflowStage tab = new WorkflowStage();   

      if ( tab == null )
        throw new OutOfMemoryException("Failed to allocate WorkFlowStage");


      tab.Id            = parent.Attributes[ "id" ].Value;
      tab.Text          = parent.Attributes[ "text" ].Value;      

      if( parent.Attributes[ "enabled" ] != null )
        tab.Enabled = bool.Parse( parent.Attributes[ "enabled" ].Value );

      ArrayList menus = new ArrayList();
      foreach( XmlNode node in parent.SelectNodes( "menus/menu" ) ) 
        menus.Add( FrameMenu.Create(  node ) );

      ArrayList panes = new ArrayList();
      foreach( XmlNode node in parent.SelectNodes( "panes/pane" ) )
        panes.Add( TemplatePane.Create( node ) );

      string defaultTemplateId = string.Empty;
      if( parent.Attributes[ "default_view" ] != null )
        defaultTemplateId = parent.Attributes[ "default_view" ].Value;             

      foreach( XmlNode node in parent.SelectNodes( "views/view" ) )
      {
        TemplateView template = TemplateView.Create( node );
        if( template.Menus.Count == 0 )
          template.Menus = menus;
        if( template.Panes.Count == 0 )
          template.Panes = panes;

        if( template.Id.Equals( defaultTemplateId ) )
          tab.DefaultViewTemplate = template;

        tab.ViewTemplates.Add( template );
      }

      if( tab.DefaultViewTemplate == null )
        tab.DefaultViewTemplate = ( TemplateView )tab.ViewTemplates[ 0 ];

      return tab;
    } // Create( XmlNode parent )
    
    
    #endregion
  } // class WorkflowStage
} // namespace Viatronix.UI


#region revision history

// $Log: WorkflowStage.cs,v $
// Revision 1.3  2007/03/01 17:09:18  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/WorkflowStage.cs,v 1.3 2007/03/01 17:09:18 geconomos Exp $
// $Id: WorkflowStage.cs,v 1.3 2007/03/01 17:09:18 geconomos Exp $

#endregion