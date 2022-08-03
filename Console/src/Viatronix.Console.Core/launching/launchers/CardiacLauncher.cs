// $Id: CardiacLauncher.cs,v 1.1.2.16 2009/09/14 13:04:15 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;
using System.Drawing;
using System.Windows.Forms;
using Viatronix.Logging;
using Viatronix.UI;
using Viatronix.Enterprise.Entities;


namespace Viatronix.Console
{


  /// <summary>
  /// Implements all functionality necessary to launch a Cardiac Viewer.
  /// </summary>
  public class CardiacLauncher : V3DLauncher
  {

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static CardiacLauncher()
    {
    } // CaridacLauncher()

    #endregion


    #region override methods

    /// <summary>
    /// Launches the viewer.
    /// </summary>
    public override void LaunchViewer()
    {
      //////////////////////////////////////////////////////////////////////
      // Look at all the datasets collections and label them.
      //
      // This is not needed due to fact that a series processed as XP 
      // cannot be grouped with a CAR and we no longer relabel in the DB
      /////////////////////////////////////////////////////////////////////
      //foreach (List<Series> datasets in this.Datasets)
      //{
      //
      //  Study.UpdatePhaseInformation(datasets, true);
      //
      //}

      Comparison<List<Series>> comparer = new Comparison<List<Series>>(DatasetSort);
      this.Datasets.Sort(comparer);

      List<List<Series>> ds = new List<List<Series>>();

      foreach( List<Series> col in this.Datasets )
      {
        ds.Add(col);
      }

      this.Datasets = ds;

      // Launch the viewer
      base.LaunchViewer();



    } // LaunchViewer()

    /// <summary>
    /// Provides the Phase number sorting for the Datasets property
    /// </summary>
    /// <param name="col1">Series collection 1</param>
    /// <param name="col2">Series collection 2</param>
    /// <returns></returns>
    private int DatasetSort(List<Series> col1, List<Series> col2)
    {
      return 1;
      //Series vol1 = col1.FindFirst("Volume");
      //Series vol2 = col2.FindFirst("Volume");

      //if (vol1.PhaseNumber > vol2.PhaseNumber)
      //  return -1;
      //else if (vol1.PhaseNumber < vol2.PhaseNumber)
      //  return 1;
      //else 
      //  return 0;
    } // DatasetSort(List<Series> col1, List<Series> col2)

    /// <summary>
    /// Extracts the phase percentage from the series supplied
    /// </summary>
    /// <param name="series">Series to extract the phase percentage from</param>
    /// <returns>Returns the phase percentage (default is o%)</returns>
    private float GetPhasePercentage(Series series)
    {

      float percentage = 0.0F;

      if (series.Type == SeriesTypes.Dicom )
      {
        percentage = GetPhasePercentage(series.Description);
      }
      else
      {
        Series dicom = null;


        if ( series.References.TryFindType(SeriesTypes.Dicom, ref dicom) )
          percentage = GetPhasePercentage(dicom);

      }

      return percentage;
    } // GetPhasePercentage(Series series)

    /// <summary>
    /// Extracts the phase percentage from the description 
    /// </summary>
    /// <param name="description">Description to extract the phase percentage from</param>
    /// <returns>Returns the phase percentage (default is o%)</returns>
    private float GetPhasePercentage(string description)
    {

      float percentage = 0.0F;

      char[] percentChar = { '%' };

      int pos = description.IndexOf('%');

      if (pos > -1)
      {
        char[] descArray = description.ToCharArray();

        int endPosition = 0;

        for (int i = pos; i >= 0; --i)
        {
          if (char.IsDigit(descArray[i]))
          {
            endPosition = i;
            break;
          }
        }

        int startPosition = 0;
        for (int i = endPosition; i >= 0; --i)
        {
          if (!char.IsDigit(description[i]) && descArray[i] != '.')
          {
            startPosition = i;
            break;
          }
        }

        percentage = float.Parse(description.Substring(startPosition, (endPosition - startPosition)));
      }

      return percentage;

    } // GetPhasePercentage(string description)

    /// <summary>
    /// Updates the datasets if the series exists in it
    /// </summary>
    /// <param name="series">Series to be used for updating</param>
    private void UpdateDatasets(Series series)
    {

      //////////////////////////////////
      //// Loop throgh all the datasets
      ///////////////////////////////////
      //for (int i = 0; i < this.Datasets.Count; ++i)
      //{

      //  Series updateSeries = this.Datasets[i].Find(series);

      //  /////////////////////////////////////////////
      //  // If the series is found update the phase
      //  /////////////////////////////////////////////
      //  if (updateSeries != null)
      //  {
      //    updateSeries.PhaseNumber = series.PhaseNumber;
      //    updateSeries.PhasePercent = series.PhasePercent;
      //    updateSeries.PhaseSize = series.PhaseSize;
      //    updateSeries.TotalPhases = series.TotalPhases;
      //  } // END ... If the series was found
      //} // END .. for each series collection in the datasets list

    } // UpdateDatasets(Series series)


    /// <summary>
    /// Updates the datasets if the series exists in it
    /// </summary>
    /// <param name="series">Series to be used for updating</param>
    private bool InDatasets(Series series)
    {

      bool found = false;

      ////////////////////////////////
      // Loop throgh all the datasets
      /////////////////////////////////
      for (int i = 0; i < this.Datasets.Count; ++i)
      {

        if ( this.Datasets[i].Contains(series) )
        {
          found = true;
          break;
        } // END ... If the series was found
      } // END .. for each series collection in the datasets list

      return found;

    } // InDatasets(Series series)

    #endregion


  } // class CardiacLauncher

} // namespace Viatronix.Console


#region revision history

// $Log: CardiacLauncher.cs,v $
// Revision 1.1.2.16  2009/09/14 13:04:15  mkontak
// Fix to make sure only CAR series are launched
//
// Revision 1.1.2.12  2009/08/25 12:44:15  mkontak
// Changed UpdatePhaseInformation method
//
// Revision 1.1.2.11  2009/06/23 13:25:12  mkontak
// Phase split
//
// Revision 1.1.2.10  2008/09/12 19:04:13  mkontak
// Reverse the order of the voumes for cardiac from last to first
//
// Revision 1.1.2.9  2007/12/14 19:04:02  mkontak
// Upon return from the viewer launch the Console will be refreshed
//
// Revision 1.1.2.8  2007/10/08 11:26:10  mkontak
// Moved cardiac code to study and series fand refreshes the screen
//
// Revision 1.1.2.7  2007/09/24 19:19:14  mkontak
// Fix
//
// Revision 1.1.2.6  2007/09/20 18:11:10  mkontak
// Added phase size
//
// Revision 1.1.2.5  2007/09/19 15:02:38  mkontak
// Fix to make sure the phase information is written to the series.xml
//
// Revision 1.1.2.4  2007/09/13 17:17:21  mkontak
// Cardiac
//
// Revision 1.1.2.3  2007/09/11 20:54:44  mkontak
// Changes for Cardiac Analysis
//
// Revision 1.1.2.2  2007/06/06 03:33:14  mkontak
// Modified the launcher to account for duplicate Uid and to label accordingly,
// Also ignore series that do not look like they are a phase.,
//
// Revision 1.1.2.1  2007/05/25 17:58:38  mkontak
// Cardiac launcher added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/launchers/CardiacLauncher.cs,v 1.1.2.16 2009/09/14 13:04:15 mkontak Exp $
// $Id: CardiacLauncher.cs,v 1.1.2.16 2009/09/14 13:04:15 mkontak Exp $

#endregion
