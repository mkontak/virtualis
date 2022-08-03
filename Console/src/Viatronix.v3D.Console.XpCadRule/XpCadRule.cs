// $Id: XpCadRule.cs,v 1.1.2.1 2007/12/05 18:10:43 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]



using System;
using System.Collections.Generic;
using System.Xml;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{

  /// <summary>
  /// Implements the rule for launching explorer with CAD
  /// </summary>
  class XpCadRule : VolumeRule
  {
    #region properties

    /// <summary>
    /// Determines if the associated viewer supports CAD.
    /// </summary>
    public bool SupportsCAD
    {
      get { return true; }
    } // SupportsCAD

    #endregion



    /// <summary>
    /// Creates a collection of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">SeriesCollection</param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public override List<IViewerLauncher> CreateLaunchers(SeriesCollection collection)
    {

      List<IViewerLauncher> launchers = base.CreateLaunchers(collection);

      foreach (Series series in collection)
      {
        // skip series if the modality is not allowed
        if (this.Modalities.Count > 0 && this.Modalities.Contains(series.Modality.ToLower()) == false)
          continue;


        // find all volume series
        if (series.IsType("Cad"))
        {
          SeriesCollection seriesCollection = new SeriesCollection();

          Series volume = series.ReferencedSeries.FindFirst("Volume");
          if (volume != null)
          {
            seriesCollection.Add(volume);
            seriesCollection.Add(series);
            List<SeriesCollection> datasets = new List<SeriesCollection>();
            datasets.Add(seriesCollection);
            launchers.Add(CreateLauncherInstance(series.Description, datasets));
          }
        }
      }

      return launchers;

    } // CreateViewers( collection )

  } // class XpCadRule

} // namespace Viatronix.v3D.Console


#region revision history

// $Log: XpCadRule.cs,v $
// Revision 1.1.2.1  2007/12/05 18:10:43  mkontak
// initial check in
//
// Revision 1.1  2007/11/16 20:34:53  mkontak
// New rule for XP with CAD
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.XpCadRule/XpCadRule.cs,v 1.1.2.1 2007/12/05 18:10:43 mkontak Exp $
// $Id: XpCadRule.cs,v 1.1.2.1 2007/12/05 18:10:43 mkontak Exp $

#endregion

