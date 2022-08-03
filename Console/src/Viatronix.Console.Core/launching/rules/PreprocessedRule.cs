// $Id: PreprocessedRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Xml;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
  /// <summary>
	/// Creates launchers for preprocessed data.
	/// </summary>
	public class PreprocessedRule : LaunchRule
	{
    
    #region fields

    /// <summary>
    /// collection of allowed modalities.
    /// </summary>
    private List<string> m_modalities = new List<string>();

    /// <summary>
    /// determines if multiple datasets can be launched
    /// </summary>
    private bool m_allowMultipleDatasets = false;

    #endregion

    #region properties

    /// <summary>
    /// Returns if the rule creates viewer's capable of launching multiple datasets.
    /// </summary>
    public bool AllowMultipleDatasets
    {
      get { return m_allowMultipleDatasets; }
    } // MultipleDatasets

    #endregion

    #region override methods

    /// <summary>
    /// Initializes any additional information specified by xml.
    /// </summary>
    /// <param name="xml">Additional information</param>
    public override void Initialize( string xml )
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml( xml );

      // determine if multiple datasets are allowed
      if( doc.DocumentElement.Attributes[ "multiple" ] != null )
        m_allowMultipleDatasets = bool.Parse( doc.DocumentElement.Attributes[ "multiple" ].Value );
    }


    /// <summary>
    /// Creates a collection of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">List<Series></param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public override List<IViewerLauncher> CreateLaunchers(List<Series> collection)
    {
      List<List<Series>> multipleDatasets = new List<List<Series>>();    

      List<IViewerLauncher> launchers = new List<IViewerLauncher>();
      foreach( Series series in collection )
      {
        // only check preprocessed and cad mathcing the provided id
        if( series.Application.ToLower() == this.ID.ToLower() )
        {
          if( series.Type == SeriesTypes.Session )
          {
            launchers.Add( CreateSessionLauncher( series ));
            continue;
          }

          List<Series> seriesCollection = new List<Series>();
          seriesCollection.Add( series );
          seriesCollection.AddRange( series.References );

          if( this.AllowMultipleDatasets )
            multipleDatasets.Add( seriesCollection );

          List<List<Series>> dataset = new List<List<Series>>();
          dataset.Add( seriesCollection );
          launchers.Add( CreateLauncherInstance( series.Description, dataset )); 
        }
      }

      // create launcher for multiple datasets
      if( multipleDatasets.Count > 1 )
        launchers.Add( CreateLauncherInstance( "Multiple Datasets for " + this.Text, multipleDatasets )); 

      return launchers;
    } // CreateViewers( collection )

    #endregion

	} // class V3DRule
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: PreprocessedRule.cs,v $
// Revision 1.2  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:40:14  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/PreprocessedRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $
// $Id: PreprocessedRule.cs,v 1.2 2007/03/06 19:14:59 gdavidson Exp $

#endregion
