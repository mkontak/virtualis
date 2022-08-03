// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PacsJob.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a PACS job that relates to the table in the database
  /// </summary>
  public class PacsJob : Entity
  {

    #region class Attributes

    /// <summary>
    /// Field attributes
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string DateQueued = "qdate";
      public const string DateStarted = "sdate";
      public const string DateCompleted = "cdate";
      public const string Operation = "op";
      public const string Progress = "prog";
      public const string Message = "msg";
      public const string State = "state";
      public const string Host = "host";
      public const string SeriesUid = "uid";
      public const string StudyUid = "suid";
      public const string DestinationAETitle = "dest";
      public const string Version = "ver";
    } // class Attributes

    #endregion


    #region enums

    public enum Operations
    {
      None,
      Store,
      Query,
      Retreive
    }

    public enum States
    {
      Pending,
      Processing,
      Completed,
      Failed,
    }

    #endregion

    #region fields

 

    /// <summary>
    /// Pacs host name
    /// </summary>
    private string m_host = string.Empty;


    /// <summary>
    /// Job id
    /// </summary>
    private Int32 m_id = 0;

    /// <summary>
    /// Message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// State
    /// </summary>
    private States m_state = States.Pending;

    /// <summary>
    /// Operation
    /// </summary>
    private Operations m_operation = Operations.None;

    /// <summary>
    /// Date queued
    /// </summary>
    private DateTimeOffset? m_dateQueued = null;

    /// <summary>
    /// Date started
    /// </summary>
    private DateTimeOffset? m_dateStarted = null;

    /// <summary>
    /// Date completed
    /// </summary>
    private DateTimeOffset? m_dateCompleted = null;


    /// <summary>
    /// Series Uid
    /// </summary>
    private string m_seriesUid = string.Empty;

    /// <summary>
    /// Study Uid
    /// </summary>
    private string  m_studyUid = string.Empty;

    /// <summary>
    /// Destination AEItle
    /// </summary>
    private string m_destination = string.Empty;

    /// <summary>
    /// Progress (% done)
    /// </summary>
    private Int32 m_progress = 0;

    #endregion

    #region properties

    /// <summary>
    ///  Gets the id
    /// </summary>
    public Int32 Id
    { get { return m_id; } }

    public string DestinationAETitle
    { get { return m_destination; }}

    public DateTimeOffset DateQueued
    { get { return m_dateQueued.Value;  } }

    public DateTimeOffset DateStarted
    { get { return m_dateStarted.Value; } }

    public DateTimeOffset DateCompleted
    { get { return m_dateCompleted.Value; } }

    public States State
    { 
      get { return m_state;  }
      set { m_state = value;  }
    }

    public Operations Operation
    {
      get { return m_operation;  }
      set { m_operation = value;  }
    }

    public string Host
    {
    get { return m_host;}
    set { m_host = value; }
    }

    public string SeriesUid
    { get { return m_seriesUid; }}

    public string StudyUid
    { get { return m_studyUid; }}

    public Int32 Progress
    { get { return m_progress; } }

    /// <summary>
    /// Gets the message
    /// </summary>
    public string Message
    { 
      get { return m_message;  }
      set { m_message = value;  }
    }


    #endregion


    #region methods


    /// <summary>
    /// Creates a push job
    /// </summary>
    /// <param name="host">PACS host the data originate from</param>
    /// <param name="aetitle">Destination AE title</param>
    /// <param name="studyUid">Study Uid</param>
    /// <param name="seriesUid">Series Uid</param>
    /// <returns></returns>
    public static PacsJob CreatePushJob(string host, string aetitle, string studyUid, string seriesUid)
    {
      PacsJob job = new PacsJob();

      if (job == null)
        throw new OutOfMemoryException("Failed to allocate PacsJob");

      job.m_host      = host;
      job.m_studyUid  = studyUid;
      job.m_seriesUid = seriesUid;
      job.m_operation = Operations.Store;
     

      return job;
    } //CreatePushJob(string host, string aetitle, string studyUid, string seriesUid)

    /// <summary>
    /// Creates a push job
    /// </summary>
    /// <param name="host">PACS host the data originate from</param>
    /// <param name="aetitle">Destination AE title</param>
    /// <param name="studyUid">Study Uid</param>
    /// <param name="seriesUid">Series Uid</param>
    /// <returns></returns>
    public static PacsJob CreateQueryJob(string host, XmlBuilder query = null)
    {
      PacsJob job = new PacsJob();

      if (job == null)
        throw new OutOfMemoryException("Failed to allocate PacsJob");

      job.m_host = host;


      return job;
    } //CreatePushJob(string host, query)

 
    /// <summary>
    /// Creates the PacsJo from the xml node specified
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static PacsJob Create(XmlNode node)
    {
      PacsJob job = new PacsJob();

      if (job == null)
        throw new OutOfMemoryException("Failed to allocate PacsJob");


      job.m_id = Int32.Parse(node.Attributes[Attributes.Id].Value);
      job.m_dateQueued = DateTimeOffset.Parse(node.Attributes[Attributes.DateQueued].Value);
      job.m_dateStarted = DateTimeOffset.Parse(node.Attributes[Attributes.DateStarted].Value);
      job.m_dateCompleted = DateTimeOffset.Parse(node.Attributes[Attributes.DateCompleted].Value);
      job.m_message = node.Attributes[Attributes.Message].Value;
      job.m_operation = (Operations)Enum.Parse(typeof(Operations), node.Attributes[Attributes.Operation].Value);
      job.m_state = (States)Enum.Parse(typeof(States),node.Attributes[Attributes.State].Value);
      job.m_host = node.Attributes[Attributes.Host].Value;
      job.m_seriesUid = node.Attributes[Attributes.SeriesUid].Value;
      job.m_studyUid = node.Attributes[Attributes.StudyUid].Value;
      job.m_destination = node.Attributes[Attributes.DestinationAETitle].Value;
      job.m_progress = Int32.Parse(node.Attributes[Attributes.Progress].Value);

      return job;
    } //  Create(XmlNode node)

    /// <summary>
    /// Creates an xml representation of a Series object.
    /// </summary>
    /// <returns>Xlm string representing the Series object.</returns>
    public override  string ToXml()
    {
      using (MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter(new StreamWriter(stream));
        writer.Formatting = Formatting.Indented;

        writer.WriteStartElement(this.Tag);

        writer.WriteAttributeString(Attributes.Id, m_id.ToString());
        writer.WriteAttributeString(Attributes.StudyUid, m_studyUid);

        if ( m_dateQueued.HasValue )
          writer.WriteAttributeString(Attributes.DateQueued, m_dateQueued.Value.ToString("O"));

        if ( m_dateStarted.HasValue )
          writer.WriteAttributeString(Attributes.DateStarted, m_dateStarted.Value.ToString("O"));

        if (m_dateCompleted.HasValue)
          writer.WriteAttributeString(Attributes.DateCompleted, m_dateCompleted.Value.ToString("O"));

        writer.WriteAttributeString(Attributes.SeriesUid, m_seriesUid);
        writer.WriteAttributeString(Attributes.DestinationAETitle, m_destination);
        writer.WriteAttributeString(Attributes.Progress, m_progress.ToString() );
        writer.WriteAttributeString(Attributes.Operation, m_operation.ToString());
        writer.WriteAttributeString(Attributes.State, m_state.ToString() );
        writer.WriteAttributeString(Attributes.Message, m_message);
        writer.WriteAttributeString(Attributes.Host, m_host);
        writer.WriteAttributeString(Attributes.Version, Version);

        writer.WriteEndElement();
        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString(stream.ToArray());
      }
    } // ToXml()





    #endregion

  } // class PacsJob
}
