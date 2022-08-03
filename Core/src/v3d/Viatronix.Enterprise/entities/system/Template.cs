// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Template.cs
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
  /// Contains the processing templates
  /// </summary>
  public class Template : Entity
  {

    #region class Attributes


    /// <summary>
    /// Attributes matching up to the database
    /// </summary>
    public static class Attributes
    {

      public const string Id          = "id";
      public const string Application = "app";
      public const string Modality    = "mod";
      public const string Description = "desc";
      public const string Default     = "default";
      public const string Tasks       = "tasks";
      public const string Version     = "ver";
    };

    #endregion

    #region fields

 
    /// <summary>
    /// Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Application
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// Modality
    /// </summary>
    private string m_modality = string.Empty;

    /// <summary>
    /// Default
    /// </summary>
    private bool m_default = false;

    /// <summary>
    /// Tasks
    /// </summary>
    private string m_tasks = string.Empty;

    #endregion


    #region properties


    /// <summary>
    /// Sets/Gets the id
    /// </summary>
    public string Id
    { get { return m_id;  } }

    /// <summary>
    /// Sets/Gets the description
    /// </summary>
    public string Description
    { get { return m_description;  } }


    /// <summary>
    /// Sets/Gets the application this process is associated with
    /// </summary>
    public string Application
    { get { return m_application;  } }

    /// <summary>
    /// Sets/Gets the modality this process is associated with
    /// </summary>
    public string Modality
    { get { return m_modality;  } }

    /// <summary>
    /// Default flag
    /// </summary>
    public bool Default
    { 
      get { return m_default;  }
      set { m_default = value; }
    }

    /// <summary>
    /// Gets the Tasks xml
    /// </summary>
    public string Tasks
    { get { return m_tasks;  } }


    #endregion


    #region methdods

    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_id.GetHashCode();
    }

 
    /// <summary>
    /// Creates the processing template from the xml node
    /// </summary>
    /// <param name="node">Node contain processing template data</param>
    /// <returns>Template</returns>
    public static Template Create(XmlNode node)
    {

      Template template = new Template();


      if (template == null)
        throw new OutOfMemoryException("Failed to allocate Template");

      template.m_id = node.Attributes[Attributes.Id].Value;
      template.m_description = node.Attributes[Attributes.Description].Value;
      template.m_modality = node.Attributes[Attributes.Modality].Value;
      template.m_application = node.Attributes[Attributes.Application].Value;
      template.m_default = (node.Attributes[Attributes.Default].Value == "1" ? true : false);

      template.m_tasks = node.SelectSingleNode(Attributes.Tasks).OuterXml;

      return template;

    } // Create(XmlNode node)


    /// <summary>
    /// Generates the xml for this object
    /// </summary>
    /// <returns></returns>
    public override string ToXml()
    {

      using (MemoryStream stream = new MemoryStream())
      {
        XmlTextWriter writer = new XmlTextWriter(new StreamWriter(stream));

        writer.WriteStartElement(this.Tag);

        writer.WriteAttributeString(Attributes.Id, m_id);
        writer.WriteAttributeString(Attributes.Application, m_application);
        writer.WriteAttributeString(Attributes.Modality, m_modality);
        writer.WriteAttributeString(Attributes.Description, m_description);

        writer.WriteAttributeString(Attributes.Default, m_default.ToString());
        writer.WriteAttributeString(Attributes.Version, Version);

        if (! string.IsNullOrEmpty(m_tasks))
        {
          writer.WriteStartElement(Attributes.Tasks);
          writer.WriteRaw(m_tasks);
          writer.WriteEndElement();
        }

        writer.Flush();

        ASCIIEncoding encoding = new ASCIIEncoding();
        return encoding.GetString(stream.ToArray());
      }

    } // ToXml()


    #endregion


    #region iEntity


    /// <summary>
    /// Clones the series
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {

      Template template = new Template();

      if (template == null)
        throw new OutOfMemoryException("Failed to allocation template");


      XmlDocument doc = new XmlDocument();
      doc.LoadXml(this.ToXml());

      template.Load(doc.DocumentElement);



      return template;

    } // Clone()

    /// <summary>
    /// Creates a new instance of a Series object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Series object.</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      string version = string.Empty;
      node.TryGetAttributeValue<string>(EntityAttributes.Version, ref version);


      node.TryGetAttributeValue<string>(Attributes.Id, ref m_id);

      node.TryGetAttributeValue<string>(Attributes.Application, ref m_application);
      node.TryGetAttributeValue<string>(Attributes.Description, ref m_description);
      node.TryGetAttributeValue<string>(Attributes.Modality, ref m_modality);
      
      int defaultBit = 0;
      node.TryGetAttributeValue<int>(Attributes.Default, ref defaultBit);
      m_default = (defaultBit == 1);

      m_tasks = node.SelectSingleNode("tasks").OuterXml;

    } // Load( node )

    #endregion
  } // class Template

}
