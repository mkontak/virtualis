// $Id: FusionRule.cs,v 1.2.2.1 2007/07/05 14:27:41 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
		/// <summary>
	/// Determines if a Fusion viewer can be launched.
	/// </summary>
	public class FusionRule : LaunchRule
	{

    #region methods

 

    /// <summary>
    /// Finds the first PET volume in the collection staring at the specified index.
    /// </summary>
    /// <param name="collection">Series collection</param>
    /// <param name="startIndex">Searches start index</param>
    /// <param name="loop">Determines if the search should loop back.</param>
    /// <returns>The PET volume series or null</returns>
    private Series FindPetVolume(List<Series> collection, int startIndex, bool loop)
    {
      // iterate over the collection starting 1 after the start location
      for( int i = startIndex + 1; i < collection.Count; ++i )
      {
        Series series = collection[i];

        // TODO: Volume series no longer exists but are now sub-elements in the DICOM series
        if( series.Type == SeriesTypes.Dicom && series.Modality.Trim().ToLower() == "pt" )
          return series;
      }

      // search from the beginning 
      if( loop )
      {
        for( int i = 0; i < startIndex; ++i )
        {
          Series series = collection[i];
          // TODO: Volume series no longer exists but are now sub-elements in the DICOM series
          if (series.Type == SeriesTypes.Dicom && series.Modality.Trim().ToLower() == "pt")
            return series;
        }
      }

      return null;
    } // FindPetVolume( collection, startIndex, loop )

    #endregion

    #region override methods

    /// <summary>
    /// Creates a colelction of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">List<Series></param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public override List<IViewerLauncher> CreateLaunchers(List<Series> collection)
    {
      List<IViewerLauncher> launchers = new List<IViewerLauncher>();
      for( int i = 0; i < collection.Count; ++i )
      {
        Series series = collection[i];
        // create a session viewer controller
        if( series.Type == SeriesTypes.Session && series.Application.ToLower() == this.ID.ToLower() )
        {
          List<List<Series>> dataset = new List<List<Series>>();
          dataset.Add(new List<Series>());
          dataset[0].Add( series );

          // locate the ct volume in the session's referenced series
          // TODO: THere are no more volume files but only DICOM series. VOlume are now sub-elements in the DICOM 
          Series ctVolume =  null;
          
          if ( ! series.References.TryFindType(SeriesTypes.Dicom, ref ctVolume) )
            continue;

          // locate the pet volume in the session's referenced series
          Series petVolume = FindPetVolume( series.References, series.References.IndexOf( ctVolume ), true );
          if( petVolume == null )
            continue;

          dataset[0].Add( ctVolume );
          dataset[0].Add( petVolume );

          IViewerLauncher launcher = CreateLauncherInstance( series.Description, dataset );
          launcher.IsSession = true;

          launchers.Add( launcher );
        }

        // create a viewer controller for a new series launch
        // TODO: Volume series no longer exists but are now sub-elements in the DICOM series
        else if( series.Type == SeriesTypes.Dicom && series.Modality.Trim().ToLower() == "ct" )
        {
          List<List<Series>> dataset = new List<List<Series>>();
          dataset.Add(new List<Series>());
          dataset[0].Add( series );
          
          // locate the pet volume
          Series petVolume = FindPetVolume( collection, i, true );
          if( petVolume == null )
            continue;

          dataset[0].Add( petVolume );
          
          // TODO: New (Do we need the clone?)
          //series.References.Add((Series)petVolume.Clone(false, false));
          series.References.Add(petVolume);

          launchers.Add( CreateLauncherInstance( series.Description + " - " + petVolume.Description, dataset )); 
        }
      }
      

      return launchers;
    } // CreateViewers( collection )

    #endregion

	} // class FnRule
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: FusionRule.cs,v $
// Revision 1.2.2.1  2007/07/05 14:27:41  mkontak
// Fixed import issue by cloning new object for the references and dependents
// instead of reuing the surrent object. Using the current objects caused a cyclic
// dependency during a series clone.
//
// Revision 1.2  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:40:29  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.4  2006/01/16 14:24:14  gdavidson
// Set the pt series as a reference of the ct series
//
// Revision 1.3  2006/01/03 14:51:08  mkontak
// no message
//
// Revision 1.2  2005/12/15 17:58:36  gdavidson
// Added abstract property to V3DRule
//
// Revision 1.1  2005/12/14 20:02:18  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/FusionRule.cs,v 1.2.2.1 2007/07/05 14:27:41 mkontak Exp $
// $Id: FusionRule.cs,v 1.2.2.1 2007/07/05 14:27:41 mkontak Exp $

#endregion
