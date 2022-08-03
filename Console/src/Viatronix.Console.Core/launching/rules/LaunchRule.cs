// $Id: LaunchRule.cs,v 1.5 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Drawing;
using System.Collections.Generic;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{
  /// <summary>
	/// Base class for all launch rules.
	/// </summary>
	public abstract class LaunchRule : ILaunchRule
	{

    #region fields

    /// <summary>
    /// the id of the viewer
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// the name of the viewer
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// the typename for the launcher
    /// </summary>
    private string m_laucherTypename = string.Empty;

    /// <summary>
    /// the path to the viewers executable
    /// </summary>
    private string m_executable = string.Empty;

    /// <summary>
    /// command line arguments
    /// </summary>
    private string m_commandLine = string.Empty;

    /// <summary>
    /// The viewer's logo.
    /// </summary>
    private Image m_logo = null;

    /// <summary>
    /// initialization information in xml form
    /// </summary>
    private string m_xml = string.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Creates a Session launcher
    /// </summary>
    /// <param name="series">Series object</param>
    /// <returns>an IViewerLauncher</returns>
    protected IViewerLauncher CreateSessionLauncher( Series series )
    {
      if( series.Type  ==  SeriesTypes.Session )
        throw new Exception( "The provided series is not a session series." );

      List<List<Series>> datasets = new List<List<Series>>();
      datasets.Add(new List<Series>());
      datasets[0].Add( series );
      datasets[0].AddRange( series.References );

      IViewerLauncher launcher = CreateLauncherInstance( series.Description, datasets );
      launcher.IsSession = true;

      return launcher;
    } // CreateSessionLauncher( series )


    /// <summary>
    /// Creates an IViewerController using the provided data.
    /// </summary>
    /// <param name="description">Description of the IViewerController.</param>
    /// <param name="hasSession">Determines if the IViewerController contains a session series.</param>
    /// <param name="collection">The series collection.</param>
    /// <returns>An IViewerController</returns>
    protected IViewerLauncher CreateLauncherInstance(string description, List<List<Series>> datasets)
    {
      IViewerLauncher launcher = (IViewerLauncher) Activator.CreateInstance( Type.GetType( this.LauncherTypename ));

      // provide information from launch rule
      launcher.ID = this.ID;
      launcher.Text = this.Text;
      launcher.Executable = this.Executable;
      launcher.Logo = this.Logo;
      launcher.CommandLine = this.CommandLine;
      launcher.AdditionalInfo = this.Xml;

      // add launch specific information
      launcher.Datasets = datasets;
      launcher.Description = description;
      launcher.IsSession = false;

      return launcher;
    } // CreateController( description, hasSession, collection )

    #endregion

    #region IViewerRule Members

    /// <summary>
    /// Gets or sets the id of the viewer.
    /// </summary>
    public string ID
    {
      get { return m_id; }
      set { m_id = value; }
    } // ID
   

    /// <summary>
    /// Gets or sets the text used to describe the viewer.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set { m_text = value; }
    } // Text


    /// <summary>
    /// Gets or sets the launcher typename.
    /// </summary>
    public string LauncherTypename
    {
      get { return m_laucherTypename; }
      set { m_laucherTypename = value; }
    } // LauncherTypename


    /// <summary>
    /// Gets or sets the full path to the viewer's executable.
    /// </summary>
    public string Executable
    {
      get { return m_executable; }
      set { m_executable = value; }
    } // Executable


    /// <summary>
    /// Gets or sets the command line arguments.
    /// </summary>
    public string CommandLine
    {
      get { return m_commandLine; }
      set { m_commandLine = value; }
    } // CommandLine


    /// <summary>
    /// Gets or sets the logo.
    /// </summary>
    public Image Logo
    {
      get { return m_logo; }
      set { m_logo = value; }
    } // Logo


    /// <summary>
    /// Additional information stored in xml. 
    /// </summary>
    public string Xml
    {
      get { return m_xml; }
      set { m_xml = value; }
    } // Xml


    /// <summary>
    /// Initializes any additional information specified by xml.
    /// </summary>
    /// <param name="xml">Additional information</param>
    public virtual void Initialize( string xml ) { }


    /// <summary>
    /// Creates a colelction of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">List<Series></param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public abstract List<IViewerLauncher> CreateLaunchers(List<Series> collection);

    #endregion

  } // class ViewerRule
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: LaunchRule.cs,v $
// Revision 1.5  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.4  2007/02/21 21:00:13  gdavidson
// Commented code
//
// Revision 1.3  2006/12/18 16:10:44  gdavidson
// Added functionality to intialize with additional xml data
//
// Revision 1.2  2006/11/02 22:41:09  gdavidson
// Added configurable command line arguments
//
// Revision 1.1  2006/10/30 15:40:29  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.2  2006/01/09 19:48:13  gdavidson
// Added Logo property to IViewerRule interface
//
// Revision 1.1  2005/12/14 20:02:18  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/LaunchRule.cs,v 1.5 2007/03/06 19:14:59 gdavidson Exp $
// $Id: LaunchRule.cs,v 1.5 2007/03/06 19:14:59 gdavidson Exp $

#endregion
