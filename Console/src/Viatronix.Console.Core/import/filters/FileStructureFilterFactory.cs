// $Id: FileStructureFilterFactory.cs,v 1.2 2007/03/12 19:38:35 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Configuration;
using System.Collections;
using System.Xml;

using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Provides access to the IFileStructureFilters loaded with Console.
	/// </summary>
	public sealed class FileStructureFilterFactory
	{

    #region fields

    /// <summary>
    /// A collection of IFileStructureFilter.
    /// </summary>
    private static ArrayList m_filters = new ArrayList();

    #endregion
		
    #region construction

    /// <summary>
    /// Loads all the IFileStructureFilters contained within its collection.
    /// </summary>
    static FileStructureFilterFactory()
    {
      try
      {
        XmlNode filterNode = (XmlNode) ConfigurationManager.GetSection( "fileStructureFilters" );
        foreach( XmlNode node in filterNode.SelectNodes( "filter" ))
        {
          IFileStructureFilter filter = (IFileStructureFilter) Activator.CreateInstance( Type.GetType( node.Attributes[ "type" ].Value ));
          filter.Name = node.Attributes[ "name" ].Value;
          m_filters.Add( filter );
        }
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( "Could not read filter from configuration file. " + e.Message );
      }
    } // FileStructureFilterFactory()

    
    /// <summary>
    /// Creates a new instance of a FileStructureFilterFactory.
    /// </summary>
    private FileStructureFilterFactory()
		{
    } // FileStructureFilterFactory()

    #endregion

    #region methods

    /// <summary>
    /// Returns the IFileStructureFilter associated with the specified path.
    /// </summary>
    /// <param name="path">The files system path.</param>
    /// <returns>The IFileStructureFilter associated with the specified path; otherwise null</returns>
    public static IFileStructureFilter CreateFilter( string path  )
    {
      foreach( IFileStructureFilter filter in m_filters )
      { 
        if( filter.CanProcess( path ))
          return filter;
      }

      return null;
    } // CreateFilter( path )

    #endregion

	} // class FileStructureFilterFactory
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: FileStructureFilterFactory.cs,v $
// Revision 1.2  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.10  2006/06/06 19:46:15  mkontak
// changed deprecated code
//
// Revision 1.9  2006/02/07 18:07:28  gdavidson
// Added a name property to the filters
//
// Revision 1.8  2005/11/23 11:52:57  mkontak
// Using new MessageBox implementation
//
// Revision 1.7  2005/11/18 19:21:58  gdavidson
// Added messagebox notification for static constructor failure
//
// Revision 1.6  2005/11/18 03:45:15  gdavidson
// Added exception handling to the static constructor.
//
// Revision 1.5  2005/11/07 16:43:31  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.4  2005/06/16 13:03:42  gdavidson
// Moved IFileStructureFilter to Viatronix.v3D.Core.
//
// Revision 1.3  2005/06/02 13:37:35  gdavidson
// Configured from the app.config file.
//
// Revision 1.2  2005/04/19 19:51:07  gdavidson
// Added VCFileStructureFilterCD filter.
//
// Revision 1.1  2001/01/02 12:47:16  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/FileStructureFilterFactory.cs,v 1.2 2007/03/12 19:38:35 mkontak Exp $
// $Id: FileStructureFilterFactory.cs,v 1.2 2007/03/12 19:38:35 mkontak Exp $

#endregion
