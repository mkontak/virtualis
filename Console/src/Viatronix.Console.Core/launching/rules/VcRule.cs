// $Id: VcRule.cs,v 1.4.2.6 2008/03/28 20:43:43 mkontak Exp $
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
	/// Launch rule for VC
	/// </summary>
  public class VcRule : LaunchRule
  {

    #region fields

    /// <summary>
    /// session string
    /// </summary>
    public const string Session = "session";

    /// <summary>
    /// supine string
    /// </summary>
    public const string Supine = "supine";

    /// <summary>
    /// prone string
    /// </summary>
    public const string Prone = "prone";

    /// <summary>
    /// decubitus left string
    /// </summary>
    public const string DecubitusLeft = "decubitus-left";

    /// <summary>
    /// decubitus right string
    /// </summary>
    public const string DecubitusRight = "decubitus-right";

    #endregion

    #region properties

    /// <summary>
    /// Determines if the associated viewer supports CAD.
    /// </summary>
    public virtual bool SupportsCAD
    {
      get { return false; }
    } // SupportsCAD

    #endregion

    #region methods





    /// <summary>
    /// Returns a mapping between orientations and the corresponding series.
    /// </summary>
    /// <param name="seriesCollection">Series collection</param>
    /// <returns>A mapping between orientations and the corresponding series.</returns>
    private Dictionary<string, List<Series>> ProcessSeries(List<Series> seriesCollection)
    {
      Dictionary<string, List<Series>> map = new Dictionary<string, List<Series>>();
      map[Session] = new List<Series>();
      //map[Supine] = new List<Series>();
      //map[Prone] = new List<Series>();
      //map[DecubitusLeft] = new List<Series>();
      //map[DecubitusRight] = new List<Series>();

      List<Series> sessions = seriesCollection.FindAll(SeriesTypes.Session);
      foreach (Series series in sessions)
      {
        if ( series.IsApplication("VC") ) 
          map[Session].Add(series);
      }

      List<Series> preprocessed = seriesCollection.FindAll(SeriesTypes.Preprocessed);

      foreach (Series series in preprocessed)
      {
        if ( ! series.IsApplication("VC") )
          continue;

        string key = series.PatientOrientation.ToLower() + "_" + series.Uid.ToString();

        List<Series> list;

        if (map.ContainsKey(key))
        {
          list = map[key];
        }
        else
        {
          list = new List<Series>();
          map.Add(key, list);
        }


        list.Add(series);
        //if (this.SupportsCAD)
        //{
        //  foreach (Series cad in series.Dependents.FindAll("CAD"))
        //  {
        //    list.Add(cad);
        //  }
        //}
      }


      if (this.SupportsCAD )
      {

        List<Series> cad = seriesCollection.FindAll(SeriesTypes.Cad);

        foreach (Series series in cad)
        {
          if ( ! series.IsApplication("vc") )
            continue;

          string key = series.PatientOrientation.ToLower() + "_" + series.Uid.ToString();

          List<Series> list;

          if (map.ContainsKey(key))
          {
            list = map[key];
          }
          else
          {
            list = new List<Series>();
            map.Add(key, list);
          }


          list.Add(series);
        }
      }

      return map;
    } // ProcessSeries( seriesCollection )


    private List<Series> GetSeries(string orientation, Dictionary<string, List<Series>> map)
    {
      List<Series> list = new List<Series>();
      foreach (KeyValuePair<string, List<Series>> element in map)
      {
        if (element.Key.StartsWith(orientation))
        {
          list.AddRange(element.Value);
        }
      }

      return list;
    }

    /// <summary>
    /// Creates a list of IViewerControllers
    /// </summary>
    /// <param name="map">A mapping between orientations and the corresponding series.</param>
    /// <returns>A collection of viewer controllers</returns>
    private List<IViewerLauncher> GenerateControllers(Dictionary<string, List<Series>> map)
    {
      List<IViewerLauncher> controllers = new List<IViewerLauncher>();
      AddControllers(map[Session], true, controllers);


      List<Series> supine = GetSeries(Supine, map);
      supine.AddRange(GetSeries("hfs", map));
      supine.AddRange(GetSeries("ffs", map));

      List<Series> prone = GetSeries(Prone, map);
      prone.AddRange(GetSeries("hfp", map));
      prone.AddRange(GetSeries("ffp", map));
      
      List<Series> decubLeft = GetSeries(DecubitusLeft, map);
      decubLeft.AddRange(GetSeries("hfdl", map));
      decubLeft.AddRange(GetSeries("ffdl", map));

      List<Series> decubRight = GetSeries(DecubitusRight, map);
      decubRight.AddRange(GetSeries("hfdr", map));
      decubRight.AddRange(GetSeries("ffdr", map));

      // supine possibilities
      foreach (Series series in supine)
      {
        AddControllers(series, prone, controllers);
        AddControllers(series, decubLeft, controllers);
        AddControllers(series, decubRight, controllers);
        AddControllers(series, supine, controllers);
      }

      // prone possibilities
      foreach (Series series in prone)
      {
        AddControllers(series, decubLeft, controllers);
        AddControllers(series, decubRight, controllers);
        AddControllers(series, prone, controllers);
      }

      // "decubitus left" possibilities
      foreach (Series series in decubLeft)
      {
        AddControllers(series, decubRight, controllers);
        AddControllers(series, decubLeft, controllers);
      }

      // "decubitus right" possibilities
      foreach (Series series in decubRight)
        AddControllers(series, decubRight, controllers);

      AddControllers(supine, false, controllers);
      AddControllers(prone, false, controllers);
      AddControllers(decubLeft, false, controllers);
      AddControllers(decubRight, false, controllers);

      return controllers;
    } // GenerateControllers( map )
    

    /// <summary>
    /// Adds an IViewerController to the controllers collection.
    /// </summary>
    /// <param name="activeSeries">Active series</param>
    /// <param name="list">a list of possible series</param>
    /// <param name="controllers">collection of controllers</param>
    private void AddControllers( Series activeSeries, List< Series > list, List< IViewerLauncher > controllers )
    {
      List<Series> primaryCollection = new List<Series>();
      primaryCollection.Add( activeSeries );
      primaryCollection.AddRange( activeSeries.References );

      foreach( Series series in list )
      {
        if( series == activeSeries )
          continue;

        if( activeSeries.Type == series.Type )
        {
          List<Series> secondaryCollection = new List<Series>();
          secondaryCollection.Add( series );
          secondaryCollection.AddRange( series.References );

          List<List<Series>> datasets = new List<List<Series>>();
          datasets.Add( primaryCollection );
          datasets.Add( secondaryCollection );

          string description = activeSeries.Description + " - " + series.Description;
          controllers.Add( CreateLauncherInstance( description, datasets ));
        }
      }
    } // AddControllers( activeSeries, list, controllers )


    private Series FindSeries(List<Series> col, string orientation)
    {

      Series orientationSeries = null;

      foreach (Series series in col)
      {
        if (series.IsPatientOrientation(orientation))
        {
          orientationSeries = series;
          break;
        }
      }

      return orientationSeries;
    } // FindSeries(string orientation)

    /// <summary>
    /// Loads the dependent series from the series parameter into the supplied series collection
    /// </summary>
    /// <param name="series"></param>
    /// <param name="seriesCollection"></param>
    private void LoadDependents(Series series, List<Series> references, List<Series> seriesCollection)
    {

      Global.DataProvider.GetDependents( series);


      foreach (Series s in series.Dependents)
      {
        if ( s.Type != SeriesTypes.Session && references.Contains(s) ) 
          seriesCollection.Add(s);
      }

    } // LoadDependents( Series series, List<Series> seriesCollection )

    /// <summary>
    /// Adds an IViewerController for each series in the collection.
    /// 
    ///  MKK:   Changed volSeries to dicomSeries and now look for all dicom series associated with the session.
    ///         Since there are no longer any volume series we only need to check the dicom series. Volumes are
    ///         now artifacts of the dicom series.
    /// </summary>
    /// <param name="seriesCollection">collection of series</param>
    /// <param name="isSession">true if the series are sessions; otherwise false</param>
    /// <param name="controllers">collection of controllers</param>
    private void AddControllers( List< Series > seriesCollection, bool isSession, List< IViewerLauncher > controllers )
    {
      foreach( Series series in seriesCollection )
      {
        if( isSession )
        {
          //// split by study
          //List<List<Series>> studyDatasets = SplitByStudy( series.References );
          //if( studyDatasets.Count > 1 )
          //{
          //  if( studyDatasets.Count > 2 )
          //    studyDatasets.RemoveRange( 2, studyDatasets.Count );

          //  IViewerLauncher launcher = CreateLauncherInstance( series.Description, studyDatasets );
          //  launcher.IsSession = true;

          //  controllers.Add( launcher );
          //}
          //// split by orientationa
          //else
          {

            Global.DataProvider.GetReferences( series);

            List<Series> dicomSeries = series.References.FindAll(SeriesTypes.Dicom);

            List<Series> primaryCollection = new List<Series>();
            List<Series> secondaryCollection = new List<Series>();

            // find the primary series

            if (dicomSeries.Count > 0)
            {
              Series primarySeries = FindSeries(dicomSeries, Supine) ?? FindSeries(dicomSeries, "hfs") ?? FindSeries(dicomSeries, "ffs");
              
              if (primarySeries == null)
              {
                primarySeries = FindSeries(dicomSeries, Prone) ?? FindSeries(dicomSeries, "hfp") ?? FindSeries(dicomSeries, "ffp");
                if (primarySeries == null)
                {
                  primarySeries = FindSeries(dicomSeries, DecubitusLeft) ?? FindSeries(dicomSeries, "hfdl") ?? FindSeries(dicomSeries, "ffdl");
                  if (primarySeries == null)
                  {
                    primarySeries = FindSeries(dicomSeries, DecubitusRight) ?? FindSeries(dicomSeries, "hfdr") ?? FindSeries(dicomSeries, "ffdr");
                  }
                }
              }

              dicomSeries.Remove(primarySeries);
              primaryCollection.Add(primarySeries);
              LoadDependents(primarySeries, series.References, primaryCollection);

            }
            else
            {
              throw new ArgumentException("Unable to find any series associated with the session.");
            }


            if (dicomSeries.Count > 0)
            {
              secondaryCollection.Add(dicomSeries[0]);
              LoadDependents(dicomSeries[0], series.References, secondaryCollection);
            }

            List<List<Series>> datasets = new List<List<Series>>();
            datasets.Add(new List<Series>());
            datasets[0].Add( series );

            datasets.Add( primaryCollection );
            if ( secondaryCollection.Count > 0 ) datasets.Add( secondaryCollection );

            IViewerLauncher launcher = CreateLauncherInstance( series.Description, datasets );
            launcher.IsSession = true;

            controllers.Add( launcher );
          }
        }
        else
        {
          List<List<Series>> dataset = new List<List<Series>>();
          dataset.Add(new List<Series>());
          dataset[0].Add( series );
          dataset[0].AddRange( series.References );

          controllers.Add( CreateLauncherInstance( series.Description, dataset ) );
        }
      }
    } // AddControllers( seriesCollection, isSession, controllers )


    /// <summary>
    /// Returns a collection of series object matching the provided orientation.
    /// </summary>
    /// <param name="orientation">Orientation of the series.</param>
    /// <returns>a series collection</returns>
    private List<Series> FindCollectionByOrientation(string orientation, List<Series> collection)
    {
      List<Series> seriesCollection = new List<Series>();
      foreach( Series series in collection )
      {
        if( series.IsPatientOrientation( orientation ))
          seriesCollection.Add( series );
      }

      return seriesCollection;
    } // FindCollectionByOrientation( orientation )


    /// <summary>
    /// Returns a series collection matching one of the specified orientations.
    /// </summary>
    /// <returns>A series collection</returns>
    private List<Series> FindCollectionByOrientation(List<string> orientations, List<Series> collection)
    {
      foreach( string orientation in orientations )
      {
        List<Series> seriesCollection = FindCollectionByOrientation(orientation, collection);
        if( seriesCollection.Count > 0 )
          return seriesCollection;
      }

      return new List<Series>();
    } // FindAlternateCollection()


    /// <summary>
    /// Splits the series collections into datsets based on study
    /// </summary>
    /// <param name="collection">collection of series</param>
    /// <returns>datasets</returns>
    //private List<List<Series>> SplitByStudy(List<Series> collection)
    //{
    //  List<Study> studies = new List<Study>();
    //  List<List<Series>> datasets = new List<List<Series>>();
    //  foreach( Series series in collection )
    //  {
    //    int index = studies.Count;
    //    for( int i = 0; i < studies.Count; ++i )
    //    {
    //      if( studies[i] == series.Study )
    //      {
    //        index = i;
    //        break;
    //      }
    //    }

    //    if( index == studies.Count )
    //    {
    //      studies.Add( series.Study );
    //      datasets.Add(new List<Series>());
    //    }

    //    datasets[index].Add( series );
    //  }

    //  return datasets;
    //} // SplitByStudy( collection )


    #endregion

    #region override methods

    /// <summary>
    /// Creates a collection of controllers capable of launching the series collection.
    /// </summary>
    /// <param name="collection">List<Series></param>
    /// <returns>A collection of possible IViewerControllers.</returns>
    public override List<IViewerLauncher> CreateLaunchers(List<Series> collection)
    {
      return GenerateControllers( ProcessSeries( collection ));
    } // CreateViewers( collection )

    #endregion

  } // class VcRule
} // namespace Viatronix.Console

#region revision history

// $Log: VcRule.cs,v $
// Revision 1.4.2.6  2008/03/28 20:43:43  mkontak
// Issue 6054 - Non-CAD sessions were launching CAD
//
// Revision 1.4.2.5  2008/03/03 21:57:30  mkontak
// Issue 6038
//
// Revision 1.4.2.4  2008/02/29 02:29:20  mkontak
// Issue 6025
//
// Revision 1.4.2.3  2008/02/26 14:31:41  mkontak
// Fix single CAD series problem
//
// Revision 1.4.2.2  2008/02/15 21:05:04  mkontak
// Issue 6018: fix to make sure the order of the datasets are correct
//
// Revision 1.4.2.1  2008/02/11 14:17:45  mkontak
// FIx 2 same orientation opening
//
// Revision 1.4  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.3  2007/02/12 20:48:54  gdavidson
// Issue #5375: Allow prone to be loaded as the primary dataset when launching a session
//
// Revision 1.2  2007/02/08 15:48:02  gdavidson
// Added comments
//
// Revision 1.1  2006/10/30 15:40:29  gdavidson
// Moved from Viatronix.Console
//
// Revision 1.6  2006/08/10 19:04:55  gdavidson
// Issue #4922: Allow launching of alternate patient orientations.
//
// Revision 1.5  2006/02/13 15:19:11  gdavidson
// Fixed cast exception on the supine series collection.
//
// Revision 1.4  2006/02/10 21:27:54  gdavidson
// Created a new List<Series> for prone and supine.
//
// Revision 1.3  2006/01/30 19:35:58  gdavidson
// Fixed launching multiple datasets
//
// Revision 1.2  2006/01/09 19:48:13  gdavidson
// Added Logo property to IViewerRule interface
//
// Revision 1.1  2005/12/14 20:02:18  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/launching/rules/VcRule.cs,v 1.4.2.6 2008/03/28 20:43:43 mkontak Exp $
// $Id: VcRule.cs,v 1.4.2.6 2008/03/28 20:43:43 mkontak Exp $

#endregion
