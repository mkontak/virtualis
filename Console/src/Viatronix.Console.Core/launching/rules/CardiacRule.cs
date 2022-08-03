// $Id: CardiacRule.cs,v 1.4.2.3 2009/09/14 13:03:48 mkontak Exp $
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
	/// Determines if a Cardiac viewer can be launched.
	/// </summary>
	public class CardiacRule : LaunchRule, IComparer< Series >
	{

    #region override methods

    /// <summary>
    /// Creates a collection of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">SeriesCollection</param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public override List<IViewerLauncher> CreateLaunchers(List<Series> collection)
    {
      List<Series> cardiacList = new List<Series>();

      List<IViewerLauncher> launchers = new List<IViewerLauncher>();
      foreach( Series series in collection )
      {
        if (series.Type == SeriesTypes.Session && series.Application.ToLower() == this.ID.ToLower())
        {
          launchers.Add( CreateSessionLauncher( series ) );
          continue;
        }

        // find cardiac series ( ct volumes that have a phase value )
        // TODO: Volume series no longer exists they are now sub-elements off the DICOM series
        if( series.Type == SeriesTypes.Dicom && series.Modality.ToLower() == "ct" )
        {

          /// =====================================================================
          /// TODO: REST 
          /// was   series.IsPreprocessAs("CAR")
          /// now   series.IsApplication("CAR")
          /// The Launch rules are not done yet which will alleviate this 
          /// ====================================================================
          if (series.IsApplication("CAR"))
            cardiacList.Add( series );
          else
          {
            List<Series> singleSeriesCollection = new List<Series>();
            singleSeriesCollection.Add( series );

            List<List<Series>> datasets = new List<List<Series>>();
            datasets.Add( singleSeriesCollection );
            launchers.Add( CreateLauncherInstance( series.Description, datasets ) );
          }
        }
      }

      // create a collection for each series
      if( cardiacList.Count > 0 )
      {
        cardiacList.Sort( this );

        List<List<Series>> datasets = new List<List<Series>>();
        foreach( Series series in cardiacList )
        {
          List<Series> seriesCollection = new List<Series>();
          seriesCollection.Add( series );

          datasets.Add( seriesCollection );
        }

        string description = ( datasets.Count == 1 ) ? "Single Phase" : datasets.Count + " Phases";
        launchers.Add( CreateLauncherInstance( description, datasets ));
      }

      return launchers;
    } // CreateViewers( collection )

    #endregion

    #region IComparer<Series> Members

    /// <summary>
    /// Compares one series with another
    /// </summary>
    /// <param name="x">First series</param>
    /// <param name="y">Second series</param>
    /// <returns>ordering of the series</returns>
    public int Compare( Series x, Series y )
    {
      return 1;
      // TDOD NEw
      //return x.PhaseNumber.CompareTo( y.PhaseNumber );
    } // Compare( x, y )

    #endregion

  } // class CarRule
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: CardiacRule.cs,v $
// Revision 1.4.2.3  2009/09/14 13:03:48  mkontak
// Fix to make sure only CAR series are launched
//
// Revision 1.4.2.2  2009/07/24 16:37:39  mkontak
// Fix datasets building by not checking the phase percent
//
// Revision 1.4.2.1  2009/06/23 13:25:12  mkontak
// Phase split
//
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/21 20:59:12  gdavidson
// Commented code
//
// Revision 1.2  2006/11/16 22:35:52  gdavidson
// Changed rule to allow single volume launch
//
// Revision 1.1  2006/10/30 15:40:29  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.1  2006/08/05 19:03:47  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/CardiacRule.cs,v 1.4.2.3 2009/09/14 13:03:48 mkontak Exp $
// $Id: CardiacRule.cs,v 1.4.2.3 2009/09/14 13:03:48 mkontak Exp $

#endregion
