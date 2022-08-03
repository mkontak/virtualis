// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: task.cs
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

namespace Viatronix.Enterprise.Entities
{

  #region Task class

  /// <summary>
  /// Job object used for the pending jobs list
  /// </summary>
  public class Task : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name            = "name";
      public const string Index           = "idx";
      public const string Executable      = "exe";
      public const string LogLevel        = "log";
      public const string LogFormat       = "fmt";
      public const string Parameters      = "params";
      public const string Type            = "type";
      public const string MaximumRunTime  = "max";              // Maximum run time in seconds
     } // class Attributes

    #endregion


    #region fields



    /// <summary>
    /// Maximum run time (Minutes)
    ///   -1 mean forever
    /// </summary>
    private int m_maximumRunTime = -1;

    /// <summary>
    /// Log Level (Default is 3)
    /// </summary>
    private int m_loglevel = 3;


    /// <summary>
    /// Log format (Default is XML)
    /// </summary>
    public Viatronix.Logging.MessageFormat m_logFormat = Viatronix.Logging.MessageFormat.Xml;

     /// <summary>
    /// Job Id
    /// </summary>
    private string m_jid = string.Empty;

    /// <summary>
    /// Name
    /// </summary>
    private string m_name= string.Empty;

    /// <summary>
    /// Stage
    /// </summary>
    private string m_stage = string.Empty;

    /// <summary>
    /// Index
    /// </summary>
    private int m_index = 0;

    /// <summary>
    /// Executable
    /// </summary>
    private string m_executable = string.Empty;

    /// <summary>
    /// Type of series
    /// </summary>
    private string m_type = string.Empty;

    /// <summary>
    /// Task list
    /// </summary>
    private List<Parameter> m_parameters = new List<Parameter>();

    /// <summary>
    /// Message
    /// </summary>
    private string m_message = string.Empty;

    #endregion



    #region properties


    /// <summary>
    /// Gets the parameters for the task
    /// </summary>
    public List<Parameter> Parameters
    {
      get { return m_parameters;  }
    }
    
 
    /// <summary>
    /// Gets/Sets the log level
    /// </summary>
    public int LogLevel
    {
      get { return m_loglevel;  }
      set { SetProperty<int>("LogLevel", value, ref m_loglevel ); }
    } // LogLevel


    /// <summary>
    /// Gets/Sets the task name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_name ); }
    } // Name


    /// <summary>
    /// Gets/Sets the series type associated with this task
    /// </summary>
    public string Type
    {
      get { return m_type; }
      set { SetProperty<string>("Type", value, ref m_type); }
    } // Application



    /// <summary>
    /// Gets/Sets the login associated with the job
    /// </summary>
    public string Executable
    {
      get { return m_executable; }
      set { SetProperty<string>("Executable", value, ref m_executable); }
    } // Executable


  
    /// <summary>
    /// Sets/Gets the index
    /// </summary>
    public int Index
    {
      get { return m_index;  }
      set { SetProperty<int>("Index", value, ref m_index );  }
    } // Index

    /// <summary>
    /// Returns the maximum run time in seconds
    /// </summary>
    public int MaximumRunTime
    {
      get { return m_maximumRunTime; }
    }

    #endregion


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


        writer.WriteStartElement(Tag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Type, m_type);
          writer.WriteAttributeString(Attributes.LogLevel, m_loglevel.ToString());
          writer.WriteAttributeString(Attributes.LogFormat, m_logFormat);
          writer.WriteAttributeString(Attributes.Executable, m_executable);
          writer.WriteAttributeString(Attributes.Index, m_index.ToString());
          writer.WriteAttributeString(Attributes.MaximumRunTime, m_maximumRunTime.ToString());


          WriteAttributeStrings(writer);
          WriteElements(writer);

          if (m_parameters.Count > 0)
          {
            writer.WriteStartElement("params");

            foreach (Parameter parameter in m_parameters)
            {
              writer.WriteRaw(parameter.ToXml());
            }

            writer.WriteEndElement();

          }

        }


        writer.WriteEndElement();
      }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml node
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {


      base.Load(node);

      /// Validates the schema
      //node.ValidateSchema(Schema.Settings);

      m_name            = node.Attributes[Attributes.Name].Value; ;
      m_loglevel        = Int32.Parse(node.Attributes[Attributes.LogLevel].Value);
      m_executable      = node.Attributes[Attributes.Executable].Value;
      m_type            = node.Attributes[Attributes.Type].Value;
      m_index           = Int32.Parse(node.Attributes[Attributes.Index].Value);


      node.TryGetAttribute<Viatronix.Logging.MessageFormat>(Attributes.LogFormat, ref m_logFormat); 
      node.TryGetAttribute<Int32>(Attributes.MaximumRunTime, ref m_maximumRunTime);


      XmlNode parameters = node.SelectSingleNode("params");

      if (parameters != null)
      {
        foreach (XmlNode parameter in parameters.SelectNodes("param"))
        {
          m_parameters.Add(Parameter.Create(parameter));
        }
      }
    
    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      Task task = new Task();
      CopyTo(task);

      task.Name = m_name;
      task.Type = m_type;
      task.LogLevel = m_loglevel;
      task.m_logFormat = m_logFormat;
      task.Executable = m_executable;
      task.Index = m_index;
      task.m_maximumRunTime = m_maximumRunTime;

      this.m_parameters.ForEach( parameter => task.m_parameters.Add((Parameter)parameter.Clone()) );


      return task;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Task other = obj as Task;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_jid, other.m_jid) && 
              string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion

    #region static methods


 
    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static Task Create(XmlNode node)
    {
      Task task = new Task();

      if (task == null)
        throw new OutOfMemoryException("Failed to allocate task");

      task.Load(node);

      return task;
    } // FromXml( node )

    #endregion

  } // class Task

  #endregion


} // namespace Viatronix.Enterprise.Entities

