// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: AgentJob.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// AgentJob object used for database runnable tasks
  /// </summary>
  public class AgentJob : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Procedure = "proc";
      public const string DateCreated = "date";
      public const string RequireContext = "ctx";
      public const string Frequency = "freq";
      public const string FrequencyNumber = "fn";
      public const string Description = "desc";
      public const string Enabled = "enabled";
      public const string Name = "name";
      public const string Type = "type";
      public const string Time = "time";
      public const string Days = "days";
      public const string Parameters = "params";
      public const string MaximumRunTime = "max";
      public const string User = "user";
      public const string Password = "pwd";
      public const string AdHoc = "adhoc";

    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Enabled flag
    /// </summary>
    private bool m_enabled = true;

    /// <summary>
    /// Name of the job
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Procedure
    /// </summary>
    private string m_procedure = string.Empty;

    /// <summary>
    /// Require context flag
    /// </summary>
    private bool m_requireContext = true;

    /// <summary>
    /// Date created
    /// </summary>
    private DateTimeOffset? m_dateCreated = null;

    /// <summary>
    /// Time of process
    /// </summary>
    private DateTime? m_time = null;

    /// <summary>
    /// Days mask used for weekly
    /// </summary>
    private Days m_days;

    /// <summary>
    /// Frequency 
    /// </summary>
    private JobFrequency m_frequency = JobFrequency.Daily;

    /// <summary>
    /// Frequence Number
    /// </summary>
    private uint m_number = 0;

    /// <summary>
    /// Agent job types
    /// </summary>
    private AgentJobTypes m_type = AgentJobTypes.SQL;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Parameters
    /// </summary>
    private string m_parameters = string.Empty;

    /// <summary>
    /// Maximum time in seconds for the process to run
    /// </summary>
    private int m_maximumRunTime = 0;

    /// <summary>
    /// User to run under
    /// </summary>
    private string m_user = string.Empty;

    /// <summary>
    /// Users password (encrypted)
    /// </summary>
    private string m_password = string.Empty;


    /// <summary>
    /// AdHoc SQL 
    /// </summary>
    private string m_adhoc = string.Empty;

    #endregion


    #region properties

   /// <summary>
    /// Gets the name
    /// </summary>
    public string Name
    { 
      get { return m_name; }
      set { m_name = value; }
    }

    /// <summary>
    /// Procedure
    /// </summary>
    public string Procedure
    {
      get { return m_procedure; }
      set { m_procedure = value; }
    }

    /// <summary>
    /// Type of job
    /// </summary>
    public AgentJobTypes Type
    { 
      get { return m_type; }
      set { m_type = value; }
    }

    /// <summary>
    /// Frequency
    /// </summary>
    public JobFrequency Frequency
    { 
      get { return m_frequency; }
      set { m_frequency = value; }
    }

    /// <summary>
    /// Frequency Number
    /// </summary>
    public uint FrequencyNumber
    { 
      get { return m_number; }
      set { m_number = value; }
    }

    /// <summary>
    /// Gets the time
    /// </summary>
    public string Time
    { get { return ( m_time.HasValue ? m_time.Value.ToString("HH:mm:ss") : "" ); } } 


    /// <summary>
    /// Description
    /// </summary>
    public string Description
    { 
      get { return m_description; }
      set { m_description = value; }
    }

 
    /// <summary>
    /// Gets/Sets the Date the job was created
    /// </summary>
    public DateTimeOffset DateCreated
    { get { return m_dateCreated.Value; } } 

    /// <summary>
    /// Gets the enabled flag
    /// </summary>
    public bool IsEnabled
    {
      get { return m_enabled; }
      set { m_enabled = value; }
    }

    /// <summary>
    /// Gets the enabled flag
    /// </summary>
    public bool RequiresContext
    { 
      get { return m_requireContext; }
      set { m_requireContext = value; }
    }

    /// <summary>
    /// Gets the user to run under
    /// </summary>
    public string User
    { 
      get { return m_user; }
      set { m_user = value; }
    }

    /// <summary>
    /// Gets the user password to run under
    /// </summary>
    public string Password
    { 
      get { return m_password; }
      set { m_password = value; }
    }

    /// <summary>
    /// Maximun runtime
    /// </summary>
    public int MaximumRunTime
    { 
      get { return m_maximumRunTime; }
      set { m_maximumRunTime = value; }
    }

    /// <summary>
    /// Parameters
    /// </summary>
    public string Parameters
    { 
      get { return m_parameters; }
      set { m_parameters = value; }
    }

    /// <summary>
    /// Sets/Gets the AdHic SQL statements
    /// </summary>
    public string AdHoc
    {
      get { return m_adhoc;  }
      set { m_adhoc = value; }
    }

    #endregion

    #region methods


   
    /// <summary>
    /// Gets the next run time from the specified date time
    /// </summary>
    /// <param name="date"></param>
    /// <returns></returns>
    public DateTime GetNextRunTime(DateTime date)
    {

      DateTime runOn = date;

      switch ( m_frequency )
      {
        case JobFrequency.Daily:
          {
            if (m_time.HasValue)
            {

              runOn = new DateTime(date.Year, date.Month, date.Hour, m_time.Value.Hour, m_time.Value.Minute, m_time.Value.Second);

              // =====================================
              // If the time has past then add a day
              // =====================================
              if (runOn < date)
                runOn.AddDays(1);

            }
            else if (m_number > 0)
            {
              runOn = date.AddSeconds(m_number);
            }
            else
              Log.Warning(string.Format("Agent job {0} has no settings for the {1} setup, must have a frequency number of every X seconds or a exact time", m_name, m_type), "AgentJob", "GetNextRunTime");
          }
          break;

        case JobFrequency.Weekly:
          {

            // ===============================
            // Weekly must have a time set
            // ===============================
            if (m_time.HasValue)
            {

              if (m_days != Days.None)
              {
                
                // Start with the date and time set 
                runOn = new DateTime(date.Year, date.Month, date.Day, date.Hour, m_time.Value.Hour, m_time.Value.Minute, m_time.Value.Second);

                // ===============================
                // Move only 7 days for Sun-Sat
                // ===============================
                for (int days = 0; days < 7; days++)
                {
                  runOn = runOn.AddDays(days);

                  Days currentDay = (Days)((int)Math.Pow(2, (int)runOn.DayOfWeek));

                  // ========================================
                  // If we are the same day check the time
                  // ========================================
                  if ((m_days & currentDay) == currentDay && runOn > date)
                    break;

                }
              }
              else
                Log.Warning(string.Format("Agent weekly job {0} has no day setting", m_name, m_type), "AgentJob", "GetNextRunTime");


            }
            else
              Log.Warning(string.Format("Agent weekly job {0} has no time setting ", m_name, m_type), "AgentJob", "GetNextRunTime");


          }
          break;
        case JobFrequency.Monthly:
          {

            // =============================
            // Monthly must have a time
            // =============================
            if (m_time.HasValue)
            {

              if (m_days != Days.None)
              {

                // Set the run on using the day as the month's day and the time as the time within that day. Assume that the day is correct
                runOn = new DateTime(date.Year, date.Month, (int)Math.Pow(2,(int)m_days), date.Hour, m_time.Value.Hour, m_time.Value.Minute, m_time.Value.Second);

              }
              else
                Log.Warning(string.Format("Agent weekly job {0} has no day setting", m_name, m_type), "AgentJob", "GetNextRunTime");


            }
            else
              Log.Warning(string.Format("Agent weekly job {0} has no time setting ", m_name, m_type), "AgentJob", "GetNextRunTime");


          }
          break;

      };

      return runOn;


    } // GetNextRunTime(DateTime date)


    #region static methods


    /// <summary>
    /// Creates a new instance of a AgentJob object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A AgentJob object.</returns>
    public static AgentJob Create(XmlNode node)
    {


      AgentJob job = new AgentJob();

      if (job == null)
        throw new OutOfMemoryException("Failed to allocate AgentJob");

      job.Load(node);

      return job;
    } // Create( node )

    #endregion


    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_name.GetHashCode();
    }



    #region IEntity


    /// <summary>
    /// Creates an xml representation of a Series object.
    /// </summary>
    /// <returns>Xml string representing the Series object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Days, ((int)m_days).ToString());
          writer.WriteAttributeString(Attributes.Frequency, m_frequency.ToString());
          writer.WriteAttributeString(Attributes.FrequencyNumber, m_number.ToString());
          writer.WriteAttributeString(Attributes.Procedure, m_procedure);
          writer.WriteAttributeString(Attributes.Enabled, (m_enabled ? "1" : "0" ));
          writer.WriteAttributeString(Attributes.RequireContext, (m_requireContext ? "1" : "0"));
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.DateCreated, m_dateCreated.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.Time, m_time.Value.ToString("HH:mm:ss"));
          writer.WriteAttributeString(Attributes.Type, m_type.ToString());
          writer.WriteAttributeString(Attributes.Parameters, m_parameters);
          writer.WriteAttributeString(Attributes.MaximumRunTime, m_maximumRunTime.ToString());
          writer.WriteAttributeString(Attributes.User, m_user);
          writer.WriteAttributeString(Attributes.Password, m_password);

        }

        writer.WriteEndElement();


      }


      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {

      base.Load(node);


      // validate job schema
      //node.ValidateSchema(Schema.Settings);

      m_dateCreated = DateTimeOffset.Parse(node.Attributes[Attributes.DateCreated].Value);
      m_procedure = node.Attributes[Attributes.Procedure].Value;
      m_frequency = (JobFrequency)Enum.Parse(typeof(JobFrequency), node.Attributes[Attributes.Frequency].Value.ToString());
      m_number = UInt32.Parse(node.Attributes[Attributes.FrequencyNumber].Value);
      m_enabled = (node.Attributes[Attributes.Enabled].Value.ToString() == "1" ? true : false);
      m_requireContext = (node.Attributes[Attributes.RequireContext].Value.ToString() == "1" ? true : false);
      m_description = node.Attributes[Attributes.Description].Value;
      m_name = node.Attributes[Attributes.Name].Value;
      m_days = (Days)Enum.Parse(typeof(Days), node.Attributes[Attributes.Days].Value.ToString());
      m_parameters = node.Attributes[Attributes.Parameters].Value;
      m_maximumRunTime = Int32.Parse(node.Attributes[Attributes.MaximumRunTime].Value);
      m_user = node.Attributes[Attributes.User].Value;
      m_password = node.Attributes[Attributes.Password].Value;

      node.TryGetAttributeValue<DateTime?>(Attributes.Time, ref m_time);

      m_type = (AgentJobTypes)Enum.Parse(typeof(AgentJobTypes), node.Attributes[Attributes.Type].Value.ToString());

 
    } // Load(XmlNode node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      AgentJob job = new AgentJob();
      CopyTo(job);

      job.m_procedure = m_procedure;
      job.m_dateCreated = m_dateCreated;
      job.m_frequency = m_frequency;
      job.m_number = m_number;
      job.m_enabled = m_enabled;
      job.m_requireContext = m_requireContext;
      job.m_description = m_description;
      job.m_name = m_name;
      job.m_days = m_days;
      job.m_time = m_time;
      job.m_type = m_type;
      job.m_parameters = m_parameters;
      job.m_maximumRunTime = m_maximumRunTime;
      job.m_user = m_user;
      job.m_password = m_password;

      return job;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      AgentJob other = obj as AgentJob;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion


    #endregion

  } // class AgentJob
} // namespace Viatronix.Enterprise.Entities
