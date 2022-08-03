// $Id: ILaunchRule.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $
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
	/// Defines methods that a launch rule must implemtent. 
	/// </summary>
	public interface ILaunchRule
	{

    /// <summary>
    /// Gets or sets the id of the viewer.
    /// </summary>
    string ID
    {
      get;
      set;
    } // ID
    
    /// <summary>
    /// Gets or sets the text used to describe the viewer.
    /// </summary>
    string Text
    {
      get;
      set;
    } // Text


    /// <summary>
    /// Gets or sets the launcher typename.
    /// </summary>
    string LauncherTypename
    {
      get;
      set;
    } // LauncherTypename


    /// <summary>
    /// Gets or sets the full path to the viewer's executable.
    /// </summary>
    string Executable
    {
      get;
      set;
    } // Executable


    /// <summary>
    /// Gets or sets any command line arguements
    /// </summary>
    string CommandLine
    {
      get;
      set;
    } // CommandLine


    /// <summary>
    /// Gets or sets the logo.
    /// </summary>
    Image Logo
    {
      get;
      set;
    } // Logo


    /// <summary>
    /// Additional information stored in xml. 
    /// </summary>
    string Xml
    {
      get;
      set;
    } // AdditionalInfo


    /// <summary>
    /// Initializes any additional information specified by xml.
    /// </summary>
    /// <param name="xml">Additional information</param>
    void Initialize( string xml );


    /// <summary>
    /// Creates a colelction of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">List<Series></param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    List<IViewerLauncher> CreateLaunchers( List<Series> collection );

	} // interface IViewerRule
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: ILaunchRule.cs,v $
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2006/12/18 16:10:30  gdavidson
// Added functionality to intialize with additional xml data
//
// Revision 1.2  2006/11/02 22:41:09  gdavidson
// Added configurable command line arguments
//
// Revision 1.1  2006/10/30 15:35:22  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.2  2006/01/09 19:48:13  gdavidson
// Added Logo property to IViewerRule interface
//
// Revision 1.1  2005/12/14 20:02:18  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/interfaces/ILaunchRule.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $
// $Id: ILaunchRule.cs,v 1.4 2007/03/06 19:14:59 gdavidson Exp $

#endregion
