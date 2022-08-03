// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Job.cs
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
  /// Job object used for the pending jobs list
  /// </summary>
  public class Job : Entity, IProcessEntity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id            = "id";
      public const string Source        = "src";
      public const string Destination   = "dst";
      public const string Volume        = "vol";
      public const string DateCreated   = "crdte";
      public const string DateStarted   = "stdte";
      public const string Application   = "app";
      public const string Login         = "login";
      public const string State         = "state";
      public const string Task          = "task";
      public const string Progress      = "prog";
      public const string ImageCount    = "icnt";
      public const string Descrition    = "desc";
      public const string PatientName   = "name";
      public const string Modality      = "mod";
      public const string Orientation   = "ort";
      public const string Stage         = "stage";
      public const string TemplateId    = "tid";
      public const string Study         = "study";
      public const string System        = "sys";
      public const string Message       = "msg";              // Output only
      public const string ConversionId  = "cid";
    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Study UId
    /// </summary>
    private string m_study = string.Empty;

    /// <summary>
    /// System the job is running on
    /// </summary>
    private string m_system = string.Empty;

    /// <summary>
    /// Job Queue Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Login
    /// </summary>
    private string m_login = string.Empty;


    /// <summary>
    /// Application 
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// State of job
    /// </summary>
    private States m_state = States.None;

    /// <summary>
    /// Date created
    /// </summary>
    private DateTimeOffset? m_dateCreated = null;

    /// <summary>
    /// Date started
    /// </summary>
    private DateTimeOffset? m_dateStarted = null;

    /// <summary>
    /// Source Uid
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// Destination Uid
    /// </summary>
    private string m_destination = string.Empty;

    /// <summary>
    /// Volume name/id
    /// </summary>
    private string m_volume = string.Empty;

    /// <summary>
    /// Used in conjunction with the volume name to identify the volume
    /// </summary>
    private string m_conversionId = string.Empty;

    /// <summary>
    /// The template that this job should be processed as.
    /// </summary>
    private string m_templateId = string.Empty;

    /// <summary>
    /// Patient name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Series description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Image count
    /// </summary>
    private UInt32 m_count;

    /// <summary>
    /// Progress
    /// </summary>
    private float m_progress;

    /// <summary>
    /// Orientation
    /// </summary>
    private string m_orientation = string.Empty;

    /// <summary>
    /// Task list
    /// </summary>
    private Task m_task = new Task();

    /// <summary>
    /// Modality
    /// </summary>
    private string m_modality = string.Empty;

    /// <summary>
    /// Stage
    /// </summary>
    private string m_stage = string.Empty;

    /// <summary>
    /// Message
    /// </summary>
    private string m_message = string.Empty;

    #endregion
 
    #region properties

    /// <summary>
    /// Gets the flag indicating if the job can process
    /// </summary>
    public virtual bool CanProcess
    { get { return m_state == States.Receiving; } }

    /// <summary>
    /// System
    /// </summary>
    public string System
    { 
      get { return m_system;  }
      set { SetProperty<string>("System", value, ref m_system);  }
    }

    /// <summary>
    /// Stage
    /// </summary>
    public string Stage
    {
      get { return m_stage; }
      set { SetProperty<string>("Stage", value, ref m_stage); }
    }

    /// <summary>
    /// Gets the modality
    /// </summary>
    public string Modality
    {  
      get { return m_modality;  }
      set { SetProperty<string>("Modality", value, ref m_modality);  }
    }

    /// <summary>
    /// Gets the image count
    /// </summary>
    public UInt32 ImageCount
    { 
      get { return m_count;  }
      set { SetProperty<UInt32>("ImageCount", value, ref m_count); }
    }

    /// <summary>
    /// Gets the series description
    /// </summary>
    public string Description
    { 
      get { return m_description;  }
      set { SetProperty<string>("Description", value, ref m_description);  }
    }

    /// <summary>
    /// Gets the patient name
    /// </summary>
    public string PatientName
    { 
      get { return m_name;  } 
      set { SetProperty<string>("PatientName", value, ref m_name); }
    }

    /// <summary>
    /// Gets the patient orientation
    /// </summary>
    public string Orientation
    { 
      get { return m_orientation; } 
      set { SetProperty<string>("Orientation", value, ref m_orientation); }
    }


    /// <summary>
    /// Returns true if the ID is null or empty
    /// </summary>
    public bool IsEmpty
    {
      get { return string.IsNullOrEmpty(m_id);  }
    }

    /// <summary>
    /// Returns true if the job is completed
    /// </summary>
    public bool IsCompleted
    {
      get { return m_state == States.Completed; }
    }

    /// <summary>
    /// Returns true if the job is failed
    /// </summary>
    public bool IsFailed
    {
      get { return m_state == States.Failed; }
    }

    /// <summary>
    /// Returns true if the job is processing
    /// </summary>
    public bool IsProcessing
    {
      get { return m_state == States.Processing; }
    }

    /// <summary>
    /// Returns true if the job is processing
    /// </summary>
    public bool IsPending
    {
      get { return m_state == States.Pending; }
    }


    /// <summary>
    /// Gets or sets the template id.
    /// </summary>
    public string TemplateId
    {
      get { return m_templateId; }
      set { m_templateId = value; }
    } // TemplateId


    /// <summary>
    /// Gets/Sets the state
    /// </summary>
    public States State
    {
      get { return m_state; }
      set { SetProperty<States>("State", value, ref m_state); }
    }

    /// <summary>
    /// Task list
    /// </summary>
    public Task Task
    {
      get { return m_task;  }
    } // Tasks

    /// <summary>
    /// Gets/Sets the Id 
    /// </summary>
    public string Id
    {
      get { return m_id; }
      private set { SetProperty<string>("Id", value, ref m_id); }
    } // Id

    /// <summary>
    /// Gets/Sets the Message
    /// </summary>
    public string Message
    {
      get { return m_message; }
      set { SetProperty<string>("Message", value, ref m_message); }
    } // Message

    /// <summary>
    /// Gets/Sets the study uid
    /// </summary>
    public string Study
    {
      get { return m_study; }
      set { SetProperty<string>("Study", value, ref m_study); }
    } // Study

    /// <summary>
    /// Gets/Sets the source uid
    /// </summary>
    public string Source
    {
      get { return m_source; }
      set { SetProperty<string>("Source",value, ref m_source); }
    } // Uid

    /// <summary>
    /// Gets/Sets the destination uid
    /// </summary>
    public string Destination
    {
      get { return m_destination; }
      set { SetProperty<string>("Destination", value, ref m_destination); }
    } // DicomUid

    /// <summary>
    /// Gets/Sets the volume name
    /// </summary>
    public string VolumeName
    {
      get { return m_volume; }
      set { SetProperty<string>("Volume",value, ref m_volume); }
    } // VolumeName

    /// <summary>
    /// Gets the Conversion Id
    /// </summary>
    public string ConversionId
    { get { return m_conversionId; } } 



    /// <summary>
    /// Gets/Sets the volume series uid 
    /// </summary>
    public string Volume
    {
      get { return m_volume; }
      set { SetProperty<string>("Volume", value, ref m_volume); }
    } // VolumeUid


    /// <summary>
    /// Gets/Sets the jobs application
    /// </summary>
    public string Application
    {
      get { return m_application; }
      set { SetProperty<string>("Application",value, ref m_application); }
    } // Application


    /// <summary>
    /// Gets/Sets the login associated with the job
    /// </summary>
    public string Login
    {
      get { return m_login; }
      set { SetProperty<string>("Login", value, ref m_login); }
    } // Login


    /// <summary>
    /// Gets/Sets the Date the job was created
    /// </summary>
    public DateTimeOffset DateCreated
    {
      get { return m_dateCreated.Value; }
    } // DateCreated

    /// <summary>
    /// Gets date the job was started
    /// </summary>
    public DateTimeOffset DateStarted
    {
      get { return m_dateStarted.Value; }
    } // DateStarted

    /// <summary>
    /// Gets duration of time in seconds the job has been running
    /// </summary>
    public double Duration
    {
      get 
      {
        double duration = 0.0F;
        if (m_dateStarted != null)
        {
          duration = DateTimeOffset.Now.Subtract(m_dateStarted.Value).TotalSeconds;
        }
        return duration;
      }
    } // DateStarted


    /// <summary>
    /// Gets the progress of the job
    /// </summary>
    public float Progress
    {
      get { return m_progress;  }
      set { SetProperty<float>("Progress", value, ref m_progress);  }
    } // Progress


 
    #endregion

    #region methods

    #region static methods

 
    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Job object.</returns>
    public static Job Create(XmlNode node)
    {

 
      Job job = new Job();

      if (job == null)
        throw new OutOfMemoryException("Failed to allocate job");

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
      return m_id.GetHashCode();
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

          writer.WriteAttributeString(Attributes.Id,          m_id);
          writer.WriteAttributeString(Attributes.State,       ((int)m_state).ToString());
          writer.WriteAttributeString(Attributes.Application, m_application);
          writer.WriteAttributeString(Attributes.Login,       m_login);
          writer.WriteAttributeString(Attributes.PatientName, m_name);
          writer.WriteAttributeString(Attributes.Orientation, m_orientation);
          writer.WriteAttributeString(Attributes.Source,      m_source);
          writer.WriteAttributeString(Attributes.Destination, m_destination);
          writer.WriteAttributeString(Attributes.Stage,       m_stage);
          writer.WriteAttributeString(Attributes.Modality,    m_modality);
          writer.WriteAttributeString(Attributes.Descrition,  m_description);
          writer.WriteAttributeString(Attributes.System,      m_system);
          writer.WriteAttributeString(Attributes.ImageCount,  m_count.ToString());
          writer.WriteAttributeString(Attributes.Message,     m_message);
          writer.WriteAttributeString(Attributes.TemplateId, m_templateId);

          if (m_dateCreated.HasValue)
            writer.WriteAttributeString(Attributes.DateCreated, m_dateCreated.Value.ToString("O"));

          if (m_dateStarted.HasValue)
            writer.WriteAttributeString(Attributes.DateStarted, m_dateStarted.Value.ToString("O"));

          writer.WriteAttributeString(Attributes.Study, m_study);
          writer.WriteAttributeString(Attributes.Volume, m_volume);
          writer.WriteAttributeString(Attributes.ConversionId, m_conversionId);
          writer.WriteAttributeString(Attributes.Progress, m_progress.ToString());

          WriteAttributeStrings(writer);
          WriteElements(writer);

          writer.WriteStartElement("task");
          writer.WriteRaw(m_task.ToXml());
          writer.WriteEndElement();

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

      m_id            = node.Attributes[Attributes.Id].Value;
      m_state         = (States)Enum.Parse(typeof(States), node.Attributes[Attributes.State].Value);
      m_dateCreated   = DateTimeOffset.Parse(node.Attributes[Attributes.DateCreated].Value);

      node.TryGetAttribute<DateTimeOffset?>(Attributes.DateStarted, ref m_dateStarted);

      m_source        = node.Attributes[Attributes.Source].Value;
      m_destination   = node.Attributes[Attributes.Destination].Value;
      m_study         = node.Attributes[Attributes.Study].Value;
      m_volume        = node.Attributes[Attributes.Volume].Value;
      m_conversionId  = node.Attributes[Attributes.ConversionId].Value;
      m_application   = node.Attributes[Attributes.Application].Value;
      m_login         = node.Attributes[Attributes.Login].Value;
      m_name          = node.Attributes[Attributes.PatientName].Value;
      m_orientation   = node.Attributes[Attributes.Orientation].Value;
      m_modality      = node.Attributes[Attributes.Modality].Value;
      m_count         = UInt32.Parse(node.Attributes[Attributes.ImageCount].Value);
      m_description   = node.Attributes[Attributes.Descrition].Value;
      m_system        = node.Attributes[Attributes.System].Value;
      m_progress      = Single.Parse(node.Attributes[Attributes.Progress].Value);

      // Get the tasks node 
      XmlNode taskNode = node.SelectSingleNode("task");

      // ============================================================================================
      // If the there are tasks, which there should be then create the tasks collection/dictionary
      // =============================================================================================
      if (taskNode != null)
      {

        m_task = Task.Create(taskNode.FirstChild);

      } // END ... If there was a task node


    } // Load(XmlNode node)
 
    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      Job job = new Job();
      CopyTo(job);

      job.m_id = m_id;
      job.m_state = m_state;
      job.m_dateCreated = m_dateCreated;
      job.m_dateStarted = m_dateStarted;
      job.m_source = m_source;
      job.m_destination = m_destination;
      job.m_stage = m_stage;
      job.m_study = m_study;
      job.m_volume = m_volume;
      job.m_conversionId = m_conversionId;
      job.m_application = m_application;
      job.m_login = m_login;
      job.m_name = m_name;
      job.m_orientation = m_orientation;
      job.m_modality = m_modality;
      job.m_count = m_count;
      job.m_description = m_description;
      job.m_system = m_system;
      job.m_progress = m_progress;
      job.m_task = m_task;

      return job;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Job other = obj as Job;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_id, other.m_id);

    } // Equals( obj )

    #endregion


    #endregion

  } // class Job
} // namespace Viatronix.Enterprise.Entities
