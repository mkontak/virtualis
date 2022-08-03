// $Id: ParsedStudy.cs,v 1.1.2.2 2009/06/04 19:05:06 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.Text;

using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// This class encapsulates the large amount of studies that could exist under a single patient.
  /// </summary>
  public class ParsedStudy
  {
    #region fields

    /// <summary>
    /// The study that this ParsedStudy represents.
    /// </summary>
    Study m_study = null;

    /// <summary>
    /// The list of ParsedSeries that belong to this ParsedStudy.
    /// </summary>
    List<ParsedSeries> m_seriesList = null;

    #endregion

    #region construction

    /// <summary>
    /// Locked out, user should use the provided create methods.
    /// </summary>
    private ParsedStudy()
    {
      m_seriesList = new List<ParsedSeries>();
    } // ParsedStudy()

    #endregion

    #region properties

    /// <summary>
    /// Accessor/Mutator for the List of ParsedSeries.
    /// </summary>
    public List<ParsedSeries> ParsedSeriesList
    {
      get { return m_seriesList;  }
      set { m_seriesList = value; }
    } // ParsedSeriesList

    /// <summary>
    /// Accessor/Mutator for the Study object.
    /// </summary>
    public Study Study
    {
      get { return m_study;  }
      set { m_study = value; }
    } // Study

    #endregion

    #region methods

    /// <summary>
    /// Informs whether or not the ParsedStudy has any Series associated with it.
    /// </summary>
    /// <returns>True if more than one ParsedSeries is in the list, otherwise false.</returns>
    public bool HasSeries()
    {
      return m_seriesList.Count == 0;
    } // HasSeries()


    /// <summary>
    /// Adds the Series to the list.
    /// </summary>
    /// <param name="seriesXmlFileName">Path to the Xml file containing series data.</param>
    /// <returns>The created parsed series object.</returns>
    public ParsedSeries AddSeries(string seriesXmlFileName)
    {
      ParsedSeries result = null;

      if(seriesXmlFileName != null && File.Exists(seriesXmlFileName))
      {
        result = ParsedSeries.Create(seriesXmlFileName, this);
        result.Series.StudyUid = m_study.Uid;
        m_study.Series.Add(result.Series);
        m_seriesList.Add(result);
      } // if(seriesXmlFileName != null && File.Exists(seriesXmlFileName))

      return result;
    } // AddSeries(string seriesXmlFileName)


    /// <summary>
    /// Adds the series to the study.
    /// </summary>
    /// <param name="series">The series to add.</param>
    public void AddSeries(ParsedSeries series)
    {
      m_seriesList.Add(series);
      m_study.Series.Add(series.Series);
      series.Series.StudyUid = m_study.Uid;
    } // AddSeries(ParsedSeries series)


    /// <summary>
    /// Removes the previously added ParsedSeries if it is defficient.
    /// </summary>
    /// <param name="series">The desired ParsedSeries to remove.</param>
    public void RemoveSeries(ParsedSeries series)
    {
      // Make sure the object exists in the list before trying to remove it.
      if(m_seriesList.Contains(series))
      {
        m_seriesList.Remove(series);
      } // if(m_seriesList.Contains(series))
    } // RemoveSeries()


    /// <summary>
    /// Finds the parsed series in the series list by uid.
    /// </summary>
    /// <param name="uid">The series uid.</param>
    /// <returns>The series or null if it couldn't be found.</returns>
    public ParsedSeries Find(string uid)
    {
      foreach (ParsedSeries parSeries in ParsedSeriesList)
      {
        if (parSeries.Series.Uid == uid)
        {
          return parSeries;
        }
      }

      return null;
    } // Find( uid )

    #endregion

    #region static methods

    /// <summary>
    /// Creates a ParsedStudy from a Study object.
    /// </summary>
    /// <param name="study">The Study that the ParsedStudy should be created from.</param>
    /// <returns>The created ParsedStudy object.</returns>
    public static ParsedStudy Create(Study study)
    {
      ParsedStudy result = new ParsedStudy();
      result.Study = study;

      return result;
    } // Create(Study study)


    /// <summary>
    /// Creates a ParsedStudy from a study Xml file.  
    /// </summary>
    /// <param name="studyXmlFileName">The study xml file that the ParsedStudy should be created from.</param>
    /// <returns>The created ParsedStudy object.</returns>
    public static ParsedStudy Create(string studyXmlFileName)
    {
      XmlDocument document = new XmlDocument();
      document.Load(studyXmlFileName);
      Study study = Study.Create(document.DocumentElement);

      ParsedStudy result = new ParsedStudy();
      result.Study = study;

      return result;
    } // Create(string studyXmlFileName)


    /// <summary>
    /// Creates a ParsedStudy from a study Xml file.  Also fills in the patient. 
    /// </summary>
    /// <param name="studyXmlFileName">The study xml file that the ParsedStudy should be created from.</param>
    /// <param name="patient">The Patient that this ParsedStudy belongs to.</param>
    /// <returns>The created ParsedStudy object.</returns>
    public static ParsedStudy Create(string studyXmlFileName, Patient patient)
    {
      ParsedStudy result = new ParsedStudy();

      XmlDocument document = new XmlDocument();
      document.Load(studyXmlFileName);

      result.Study = Study.Create(document.DocumentElement);
      result.Study.Name = patient.Name;
      result.Study.MedicalId = patient.MedicalId;
      result.Study.Dob = patient.Dob;
      result.Study.Gender = patient.Gender;
      result.Study.Weight = patient.Weight;

      patient.Studies.Add(result.Study);

      return result;
    } // Create(string studyXmlFileName, Patient patient)

    #endregion

  } // class ParsedStudy
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: ParsedStudy.cs,v $
// Revision 1.1.2.2  2009/06/04 19:05:06  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ParsedStudy.cs,v 1.1.2.2 2009/06/04 19:05:06 kchalupa Exp $
// $Id: ParsedStudy.cs,v 1.1.2.2 2009/06/04 19:05:06 kchalupa Exp $

#endregion
