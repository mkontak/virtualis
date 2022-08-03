// $Id: ParsedSeries.cs,v 1.1.2.6 2011/05/18 20:01:00 kchalupa Exp $
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
using System.Text;
using System.Collections.Generic;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Encryption;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities;
using Viatronix.Utilities.Registry;
using Viatronix.Enterprise.Data;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// Contains a list of files, as well as a series object.
  /// One of these will be created after each new seriesType is found.
  /// </summary>
  public class ParsedSeries
  {

    #region fields

    /// <summary>
    /// A list of files that are a part of this "series."
    /// </summary>
    private List<IFile> m_files;

    /// <summary>
    /// The series object that will be created while parsing is occuring.
    /// </summary>
    private Series m_series = null;

    /// <summary>
    /// The Parsed Study that this ParsedSeries belongs to.  Used in 3.0 Importing only at this time.
    /// </summary>
    private ParsedStudy m_parsedStudy = null;

    #endregion

    #region construction

    /// <summary>
    /// Initializes the series object from the series parameter.  Also ensures that if it's a dicom
    /// series the UID isn't generated.
    /// </summary>
    /// <param name="series">The series that this object will hold on to.</param>
    public ParsedSeries(Series series)
    {
      m_files = new List<IFile>();
      m_series = series;
    } // ParsedSeries(Series series)

    #endregion

    #region properties

    /// <summary>
    /// The list of files that belong to this "series".
    /// </summary>
    public List<IFile> Files
    {
      get { return m_files; }
    } // Type Property


    /// <summary>
    /// Accessor for the embedded series object.
    /// </summary>
    public Series Series
    {
      get { return m_series; }
    } // Series Property


    /// <summary>
    /// Accessor/Mutator for the ParsedStudy embedded variable.
    /// </summary>
    public ParsedStudy ParsedStudy
    {
      get { return m_parsedStudy;  }
      set { m_parsedStudy = value; }
    } // ParsedStudy Property

    #endregion

    #region methods

    /// <summary>
    /// Adds a file to the list for later manipulation.
    /// </summary>
    /// <param name="dst">The relative ioArgs destination of the file.</param>
    /// <param name="src">The absolute ioArgs source of the file.</param>
    /// <param name="macroReplace">The text that should replace the placeholder character in dst.</param>
    public void AddFile(string src, string dst, string macroReplace)
    {
      ImportedFile file = new ImportedFile(this, src, dst, macroReplace);

      // Check to make sure that the file was not added already.
      if(!m_files.Contains(file))
      {
        m_files.Add(file);
      } // if(!m_files.Contains(file))

      // Added for VC Series that only have an ECV file.
      if(m_series.Type == SeriesTypes.Dicom && src.ToLower().EndsWith(".vol"))
      {
        m_series.Application = string.Empty;
      } // if( m_series.Type == "volume" && src.ToLower().EndsWith(".vol"))
    } // AddFile(string src, string dst, string macroReplace)


    /// <summary>
    /// Adds a virtual file to the list for later manipulation.
    /// </summary>
    /// <param name="contents">The contents the file should have as a string.</param>
    /// <param name="relativeDestination">The relative ioArgs destination of the file.</param>
    /// <param name="macroReplace">The text that should replace the placeholder character in dst.</param>
    public void AddVirtualFile(string contents, string relativeDestination, string macroReplace)
    {
      VirtualFile file = new VirtualFile(this, contents, relativeDestination, macroReplace);

      // Check to make sure that the file was not added already.
      if( !m_files.Contains(file) )
      {
        m_files.Add(file);
      } // if(!m_files.Contains(file))
    } // AddVirtualFile(string contents, string relativeDestination, string macroReplace)


    /// <summary>
    /// Initiates a copy on all files contained within the current parsed series.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    /// <param name="totalFiles">Tht total file count that needs to be copied.</param>
    /// <param name="remainingFiles">The remaining files to send, including the files in this series.</param>
    /// <exception cref="System.IO.IOException">If there was an error in copying the series data.</exception>
    public void Copy(ImporterArgs args, int totalFiles, int remainingFiles)
    {
      try
      {
        string outPath = Path.Combine(args.Destination, m_parsedStudy.Study.Directory, m_series.Directory);

        // Create the series directory if it doesn't exist.
        if (!Directory.Exists(outPath))
        {
          Directory.CreateDirectory(outPath);
        } // if( !Directory.Exists(Path.Combine(baseDirectory, m_series.Location) ))

        // Serialize the pertinent data to the proper directory.
        //m_series.Serialize(outPath);

        int filesCopied = totalFiles - remainingFiles;

        // Initiate the copy procedure inside each ImportedFile.
        for(int i=0;i<m_files.Count; ++i)
        {
          Global.CheckForCancel();

          IFile file = m_files[i];

          int completed = 5 + (int)((((double)filesCopied++ / (double)totalFiles)) * 90.0);
          Global.NotifyProgress(string.Format("Copying File: {0}", Path.GetFileNameWithoutExtension(file.GetSourceFileName())), completed);

          file.Copy(args.Destination);
        } // foreach( ImportedFile file in m_files )
      } // try
      catch (Exception e)
      {
        Logging.Log.Error("Unknown exception occurred in copying series; [ERROR=" + e.Message + "]", "ParsedSeries", "Copy");

        throw;
      }
    } // Copy( args )


    /// <summary>
    /// Writes the patient, study and series to the database (in that order.)
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    /// <exception cref="System.ApplicationException">
    /// In the event that the series, study or patient objects fail to be recorded to the Viatronix DB.
    /// </exception>
    public void PerformDatabaseInsert(ImporterArgs args)
    {
      Global.CheckForCancel();

      try
      {
        args.Provider.Insert(m_parsedStudy.Study);
        args.Provider.Insert(m_series);

        foreach (Series referencedSeries in m_series.References)
        {
          //throw new NotImplementedException("Series reference insertion needs to be implemented.");

          // Add the series references.
          //args.Provider.Insert(m_series.Uid, referencedSeries.Uid);
        }
      }
      catch (Exception e)
      {
        Log.Error(m_parsedStudy.Study.Name.LastFirst + "; " +
          "; Import Failed; Failed to add Patient Record to Viatronix DB;  Rolling back to state of the database before this operation occurred;  Exception " + e.Message, "ParsedSeries", "PerformDatabaseInsert");

        Global.LogPatientImportFailed(args.Source);

        throw new ApplicationException("Failed to Record Patient to Viatronix DB");
      }
    } // PerformDatabaseInsert(ImporterArgs args)

    #endregion

    #region static methods

    /// <summary>
    /// Creates a ParsedSeries object from the given Xml file.
    /// </summary>
    /// <param name="seriesXmlFileName">Contains a serialized series.</param>
    /// <returns>A parsed series.</returns>
    public static ParsedSeries Create(string seriesXmlFileName)
    {
      XmlDocument document = new XmlDocument();
      document.Load(seriesXmlFileName);

      Series series = Series.Create(document.DocumentElement);

      return new ParsedSeries(series);
    } // Create(string seriesXmlFileName)


    /// <summary>
    /// Creates a ParsedSeries object from the given Xml file.
    /// </summary>
    /// <param name="seriesXmlFileName">Serialized series.</param>
    /// <param name="parsedStudy">The study to add to.</param>
    /// <returns>Parsed series from the xml.</returns>
    public static ParsedSeries Create(string seriesXmlFileName, ParsedStudy parsedStudy)
    {
      XmlDocument document = new XmlDocument();
      document.Load(seriesXmlFileName);

      Series series = Series.Create(document.DocumentElement);
      series.StudyUid = parsedStudy.Study.Uid;

      ParsedSeries result = new ParsedSeries(series);
      result.m_parsedStudy = parsedStudy;

      return result;
    } // Create()

    #endregion

  } // class ParsedSeries
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: ParsedSeries.cs,v $
// Revision 1.1.2.6  2011/05/18 20:01:00  kchalupa
// CONS-1640
// Updated the references to the SystemSettings to the new data structures.
//
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.5  2009/10/30 14:01:51  kchalupa
// Clean up the directories if copy fails.
//
// Revision 1.1.2.4  2009/06/04 19:17:09  kchalupa
// New status system logging; coding Standards.
//
// Revision 1.1.2.3  2009/05/19 15:15:12  kchalupa
// Get database connection string from the registry.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// 2008/09/10 15:25:32  kchalupa
// Changed the AddFile method to mark the SubType to "" if a VOL file is added to a Volume Series.
//
// 2008/06/30  kchalupa
// Initial Implementation.
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ParsedSeries.cs,v 1.1.2.6 2011/05/18 20:01:00 kchalupa Exp $
// $Date: 2011/05/18 20:01:00 $24 July 2008
// $Id: ParsedSeries.cs,v 1.1.2.6 2011/05/18 20:01:00 kchalupa Exp $000000004

#endregion
