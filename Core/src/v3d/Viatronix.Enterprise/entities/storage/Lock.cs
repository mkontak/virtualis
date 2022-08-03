// $Id: Lock.cs,v 1.7.2.3 2009/02/06 19:19:12 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak (mailto:mkontak@viatronix.com)

using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Xml;
using System.Runtime.Serialization;

namespace Viatronix.Enterprise.Entities
{


  /// <summary>
  /// Summary description object locks
  /// </summary>
  public class Lock : Entity
  {

    #region Attributes class

    public class Attributes
    {
      public const string SeriesUid       = "uid";
      public const string StudyUid        = "suid";
      public const string Login           = "login";
      public const string TransactionId   = "tid";
      public const string Context         = "ctx";
      public const string Description     = "desc";
      public const string ExpirationDate  = "exp";
      public const string Date            = "date";
      public const string Version         = "ver";
    };  // class Attributes

    #endregion

    #region constants

    /// <summary>
    /// Lock file name
    /// </summary>
    private static string m_xmltag = "lock";

    #endregion

    #region fields

    /// <summary>
    /// Version
    /// </summary>
    private readonly string m_version = "3.0";

    /// <summary>
    ///  Series Uid
    /// </summary>
    private string m_series = string.Empty;

    /// <summary>
    ///  Study Uid
    /// </summary>
    private string m_study = string.Empty;

    /// <summary>
    /// Date/Time the series was locked
    /// </summary>
    private DateTimeOffset? m_dateLocked = null;

    /// <summary>
    /// The user who locked the series
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// The context this lock is under 
    /// </summary>
    private string m_context = string.Empty;

    /// <summary>
    /// Lock description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Time span
    /// </summary>
    private TimeSpan m_timeSpan = TimeSpan.MinValue;

    /// <summary>
    /// Expiration Date
    /// </summary>
    private DateTimeOffset? m_expiration = null;

 
    #endregion

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public Lock()
    {
    } // Lock()


 
    #endregion

    #region properties

    /// <summary>
    /// Gets the objcet version
    /// </summary>
    public override string Version
    {
      get { return m_version; }
    }


    /// <summary>
    /// Series Uid
    /// </summary>
    public string SeriesUid
    {
      get { return m_series; }
    } // Uid

    /// <summary>
    /// Study Uid
    /// </summary>
    public string StudyUid
    {
      get { return m_study; }
    } // Uid

    /// <summary>
    /// Type
    /// </summary>
    public string Description
    {
      get { return m_description; }
    } // Type

    /// <summary>
    /// Date/Time the series was locked
    /// </summary>
    public DateTimeOffset DateLocked
    {
      get { return m_dateLocked.Value; }
     } // DateLocked

    /// <summary>
    /// User who locked the series
    /// </summary>
    public string Login
    {
      get { return m_login; }
    } // Login

    /// <summary>
    /// Context the controls the lock
    /// </summary>
    public string Context
    {
      get { return m_context; }
    } // Context

    /// <summary>
    /// Date/Time the lock will expire
    /// </summary>
    public DateTimeOffset ExpirationDate
    {
      get { return m_expiration.Value; }
    } // ExpirationDate

    /// <summary>
    /// Returns true of the lock object is locked
    /// </summary>
    public bool IsLocked
    {
      get { return ( m_expiration != null ? m_expiration > DateTimeOffset.Now : false ); }
    }

    #endregion

    #region methods

    /// <summary>
    /// Check if the lock has expired
    /// </summary>
    /// <param name="dateTime">Date to check against</param>
    /// <returns>True if expired</returns>
    public bool IsLockExpired(DateTimeOffset dateTime)
    {
      return (m_expiration != null ? ( DateTimeOffset.Compare(dateTime, m_expiration.Value) < 0 ? true : false) : false );
    } // IsLockExpired(DateTime dateTime)


    #endregion

    #region static methods

    /// <summary>
    /// Creates the Lock from the xml string
    /// </summary>
    /// <param name="xml">Xml string</param>
    /// <returns>Lock object</returns>
    public static Lock Create(string xml)
    {

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);
    } // Create(string xml)
  

    /// <summary>
    /// Creates the lock from the xml node
    /// </summary>
    /// <param name="node">The xml node.</param>
    /// <returns>Lock object</returns>
    public static Lock Create(XmlNode node)
    {
      Lock lck = new Lock();


      if (lck == null)
        throw new OutOfMemoryException("Failed to create Lock");

      node.TryGetAttributeValue<string>(Attributes.SeriesUid, ref lck.m_series );
      node.TryGetAttributeValue<string>(Attributes.StudyUid, ref lck.m_study);
      node.TryGetAttributeValue<DateTimeOffset>(Attributes.Date, ref lck.m_dateLocked);
      node.TryGetAttributeValue<DateTimeOffset>(Attributes.ExpirationDate, ref lck.m_expiration);
      node.TryGetAttributeValue<string>(Attributes.Description, ref lck.m_description);
      node.TryGetAttributeValue<string>(Attributes.Login, ref lck.m_login);
      node.TryGetAttributeValue<string>(Attributes.Context, ref lck.m_context);
 
      return lck;
    } // Create( node )

    #endregion

    #region IEntity


    /// <summary>
    /// Creates an xml representation of a Study object.
    /// </summary>
    /// <returns>Xml string representing the Study object.</returns>
    public override string ToXml()
    {
      using (MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter(new StreamWriter(stream));

        writer.WriteStartElement(m_xmltag);

        writer.WriteAttributeString(Attributes.SeriesUid, m_series);
        writer.WriteAttributeString(Attributes.StudyUid, m_study);

        if ( m_dateLocked.HasValue )
          writer.WriteAttributeString(Attributes.Date, m_dateLocked.Value.ToString("O"));

        writer.WriteAttributeString(Attributes.Login, m_login);
        writer.WriteAttributeString(Attributes.Context, m_context);
        writer.WriteAttributeString(Attributes.Description, m_description  );
        writer.WriteAttributeString(Attributes.ExpirationDate, m_expiration.Value.ToString("O"));
        writer.WriteAttributeString(Attributes.Version, m_version);

        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString(stream.ToArray());
      }
    } // ToXml()

    #endregion

  }
}
