// $Id: ImporterArgs.cs,v 1.1.2.5 2011/05/18 17:59:57 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;

using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// A humble class to hold the initial location and destination.
  /// </summary>
  public class ImporterArgs
  {

    #region fields

    /// <summary>
    /// The data provider used for importing studies.
    /// </summary>
    private IDataProvider m_provider = null;

    /// <summary>
    /// The initial location of the study.
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// The destination the studies should be copied to.
    /// </summary>
    private string m_destination = Registry.SystemSettings.CurrentSystemSettings.CacheBasePath;

    /// <summary>
    /// Should the patient, study collection and series collection be copied to a new location on the disk?
    /// </summary>
    private bool m_performCopy = false;

    /// <summary>
    /// Should the patient, study collection and series collection be recorded to the database?
    /// </summary>
    private bool m_performDatabaseRecord = false;

    /// <summary>
    /// Should the session be included in the copy and database record?
    /// </summary>
    private bool m_importSessions = false;

    /// <summary>
    /// Is debug enabled or not.
    /// </summary>
    private bool m_isDebugEnabled = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructs an IOArgs subject with the fields it will hold at it's inception.
    /// </summary>
    /// <param name="provider">The data provider.</param>
    /// <param name="source">The source to investigate.</param>
    /// <param name="destination">The destination to copy to.</param>
    /// <param name="performCopy">Should the files be copied?</param>
    /// <param name="performDatabaseRecord">Should the series be recorded into the database?</param>
    /// <param name="includeSession">Should the session be included in copying and recording?</param>
    public ImporterArgs(IDataProvider provider, string source, string destination, bool performCopy, bool performDatabaseRecord, bool includeSession)
    {
      m_provider = provider;
      m_source = source;
      m_destination = destination;
      m_performCopy = performCopy;
      m_performDatabaseRecord = performDatabaseRecord;
      m_importSessions = includeSession;
    } // public IOArgs(user, string source, string destination, bool performCopy ...

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the data provider used for importing studies.
    /// </summary>
    public IDataProvider Provider
    {
      get { return m_provider; }
      set { m_provider = value; }
    } // SecurityProvider


    /// <summary>
    /// Gets or sets the source of the import.
    /// </summary>
    public string Source
    {
      get { return m_source; }
      set { m_source = value; }
    } // Source


    /// <summary>
    /// Gets or sets the destination.
    /// </summary>
    public string Destination
    {
      get { return m_destination; }
      set { m_destination = value; }
    } // Destination


    /// <summary>
    /// Gets or sets if the file copy should be done.
    /// </summary>
    public bool PerformCopy
    {
      get { return m_performCopy; }
      set { m_performCopy = value; }
    } // PerformCopy


    /// <summary>
    /// Gets or sets if the database record should be done.
    /// </summary>
    public bool PerformDatabaseRecord
    {
      get { return m_performDatabaseRecord; }
      set { m_performDatabaseRecord = value; }
    } // PerformDatabaseRecord Property


    /// <summary>
    /// Gets or sets if debug is enabled.
    /// </summary>
    public bool IsDebugEnabled
    {
      get { return m_isDebugEnabled; }
      set { m_isDebugEnabled = value; }
    } // IsDebugEnabled


    /// <summary>
    /// Gets or sets if the sessions should be imported.
    /// </summary>
    public bool ImportSessions
    {
      get { return m_importSessions;  }
      set { m_importSessions = value; }
    } // PerformCopy Property

    #endregion

  } // ImporterArgs
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: ImporterArgs.cs,v $
// Revision 1.1.2.5  2011/05/18 17:59:57  kchalupa
// CONS-1640
// Change original base path reference to new data structure.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.4  2009/10/26 19:41:59  kchalupa
// Added CD Import commandline importer arg value.
//
// Revision 1.1.2.3  2009/06/04 18:57:18  kchalupa
// Coding standards.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ImporterArgs.cs,v 1.1.2.5 2011/05/18 17:59:57 kchalupa Exp $
// $Date: 2011/05/18 17:59:57 $23 July 2008
// $Id: ImporterArgs.cs,v 1.1.2.5 2011/05/18 17:59:57 kchalupa Exp $000000004

#endregion