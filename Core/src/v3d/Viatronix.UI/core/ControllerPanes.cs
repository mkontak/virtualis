// $Id: ControllerPanes.cs,v 1.2 2007/03/01 17:09:18 geconomos Exp $
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

namespace Viatronix.UI
{
  /// <summary>
  /// Responsible for maintaining the frame's panes
  /// </summary>
  public class ControllerPanes : Viatronix.UI.Rollout, IEnumerable
	{
    #region fields

    /// <summary>
    /// contains a subset of all panes( represents visible panes for the current view )
    /// </summary>
    private ArrayList m_visiblePanes = new ArrayList();

    #endregion
  
    #region methods

    /// <summary>
    /// Removes any existing panes from the controller.
    /// </summary>
    public void Clear()
    {
      this.Controls.Clear();
      this.Panes.Clear();
    } // Clear()

    
    /// <summary>
    /// Gets the pane width the specified id.
    /// </summary>
    public Viatronix.UI.RolloutPane this[ string id ]
    {
      get { return this.Panes[ id ]; }
    } // this[ string id ]

    
    /// <summary>
    /// Gets the pane width the specified index.
    /// </summary>
    public Viatronix.UI.RolloutPane this[ int index ]
    {
      get { return this.Panes[ index ]; }
    } // this[ int index ]

    
    /// <summary>
    /// Gets if a pane with the specified id is contined within the collection.
    /// </summary>
    /// <param name="id">Id of pane.</param>
    /// <returns>True if pane is found; false otherwise</returns>
    public bool Contains( string id )
    {
      return this.Panes.Contains( id );
    } // Contains( string id )


    /// <summary>
    /// Gets the enumerator for the pnaes collection.
    /// </summary>
    /// <returns></returns>
    public IEnumerator GetEnumerator()
    {
      return m_visiblePanes.GetEnumerator();
    } // GetEnumerator()
  
    
    /// <summary>
    /// Called when one of the rollout panes is about to be expanded. This methods will
    /// create the associated plugin pane the first time it is expanded.
    /// </summary>
    /// <param name="pane"></param>
    protected override void OnPaneExpanding( Viatronix.UI.RolloutPane pane )
    {
      try
      {
        base.OnPaneExpanding( pane );

        // has the plugin pane been added yet?
        if( pane.Controls.Count == 0 )
        {
          // show wait cursor
          this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
        
          // create an instance of the IPane implementer, we are assuming that
          // it derives from System.Windows.Forms.Control also
          System.Windows.Forms.Control control = ( System.Windows.Forms.Control )
            System.Activator.CreateInstance( Type.GetType( (( TemplatePane ) pane.Tag ).Typename ) );

          // set the child of the rollout pane to the IPane implementer
          pane.SetChild( control );

        }
      } // try
      finally
      {
        // default cursor
        this.Cursor = System.Windows.Forms.Cursors.Default;
      }
    } // OnPaneExpanding( Viatronix.UI.RolloutPane pane )


    /// <summary>
    /// Initializes the panes based on the active view template.
    /// </summary>
    public void Reset( TemplateView template )
    {
        try
        {
          if( this.Visible )
            Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 0, 0 );
          
          SuspendLayout();        
  
          BeginInit();
  
          SetPaneCount( template.Panes.Count );
  
          // update the visibility for all the panes
          foreach( Viatronix.UI.RolloutPane pane in this.Panes )
          {
            // see if this pane has been specified in the view template
            TemplatePane templatePane = FindPane( template, ( ( TemplatePane ) pane.Tag ).Id );
              
            // has the template been found?
            if( templatePane != null )
            {
              pane.Visible = true;
              pane.SetExpanded( templatePane.Expanded );
            }
            else
            {
              pane.Visible = false;
              pane.SetExpanded( false );
            }
          } // foreach( Viatronix.UI.RolloutPane pane in this.Panes )
  
          // if the pane controller is visible force a redraw
          if( this.Visible )
            this.Refresh();
        } // try
        finally
        {
          // enable redraws
          EndInit();
  
          if( this.Visible )
            Win32.User32.SendMessage( this.Handle,(int) Win32.Msgs.WM_SETREDRAW, 1, 0 );
          
          ResumeLayout( true );        
        } // finally
    } // Reset( ViewTemplate template )
  
    
    /// <summary>
    /// Finds the pane in the view that matches the specified id.
    /// </summary>
    /// <param name="view">View instance containing panes.</param>
    /// <param name="id">Id of pane to find</param>
    /// <returns>TemplatePane instance if found; null otherwise.</returns>
    private TemplatePane FindPane( TemplateView viewTemplate, string id )
    {
      foreach( TemplatePane template in viewTemplate.Panes )
      {
        if( template.Id.Equals( id ) )
          return template;
      }
      return null;
    } // FindPane( TemplateView view, string id )
   
   
    #endregion

  } // class ControllerPanes
} // namespace Viatronix.Viewer


#region revision history

// $Log: ControllerPanes.cs,v $
// Revision 1.2  2007/03/01 17:09:18  geconomos
// code review preparation
//
// Revision 1.17  2004/12/10 17:34:00  gdavidson
// Update panes when not visible.
//
// Revision 1.16  2004/11/19 17:54:47  gdavidson
// Added Expand and Collapse methods to a Rollout pane.
//
// Revision 1.15  2004/11/18 21:28:42  gdavidson
// Preset the number of visible panes contained in the rollout.
//
// Revision 1.14  2004/11/18 18:39:11  gdavidson
// Modified Rolllout control.
//
// Revision 1.13  2004/11/17 19:08:10  gdavidson
// Added transparent background to RollOuts.
//
// Revision 1.12  2004/11/12 19:07:10  gdavidson
// Removed UpdatePanes and added an InitializePanes method.
//
// Revision 1.11  2004/11/02 15:56:16  geconomos
// added id property to rollout pane
//
// Revision 1.10  2004/11/01 15:19:24  geconomos
// updated to reflect new Global events
//
// Revision 1.9  2004/10/28 19:49:11  geconomos
// added RefreshPanes method
//
// Revision 1.8  2004/10/28 14:51:44  geconomos
// changed to let the view controller dicate when the panes are added.
//
// Revision 1.7  2004/10/22 19:58:00  geconomos
// added the ability to specify default panes
//
// Revision 1.6  2004/10/20 16:37:12  geconomos
// spacing
//
// Revision 1.5  2004/10/18 14:50:37  geconomos
// modified the rollout pane to use the back color from the theme manager
//
// Revision 1.4  2004/10/18 02:42:30  geconomos
// removed "preset pane" hack
//
// Revision 1.3  2004/10/15 17:09:56  geconomos
// Fixed bug while switching views when the palette is hidden.
//
// Revision 1.2  2004/10/12 00:18:17  geconomos
// Initial update for new graphics.
//
// Revision 1.1  2004/09/15 17:44:41  geconomos
// moved files
//
// Revision 1.4  2004/08/12 14:35:12  geconomos
// Updated to use new PluginPaneCollection class.
//
// Revision 1.3  2004/07/14 19:20:10  geconomos
// Calling IPluginPane2 interface when a tab is expanded.
//
// Revision 1.2  2004/07/14 13:01:48  geconomos
// Updated to use MainForm's new "SelectedPluginTab" property
//
// Revision 1.1  2004/07/09 19:01:22  geconomos
// Renamed from "ControlPalette"
//
// Revision 1.2  2004/06/09 19:18:50  geconomos
// Added handling for the changing of the selected viewer.
//
// Revision 1.1  2004/05/20 12:52:20  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Core/ui/ControllerPanes.cs,v 1.2 2007/03/01 17:09:18 geconomos Exp $
// $Id: ControllerPanes.cs,v 1.2 2007/03/01 17:09:18 geconomos Exp $

#endregion