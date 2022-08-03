// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DicomHost.cs
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
  /// DicomHost object used for database runnable tasks
  /// </summary>
  public class DicomHost : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    private static class DicomHostXmlAttributes
    {

      public const string Name        = "name";
      public const string Server      = "server";
      public const string AETitle     = "aetitle";
      public const string Port        = "port";
      public const string Services    = "services";
      public const string Description = "desc";
      public const string HostAttributes = "attributes";

    } // class DicomHostXmlAttributes

    #endregion

    #region fields

    /// <summary>
    /// Name of the job
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Server name/ip
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// AE Title
    /// </summary>
    private string m_aetitle;

 
    /// <summary>
    /// Port Number
    /// </summary>
    private uint m_port = 0;

    /// <summary>
    /// Services
    /// </summary>
    private DicomServiceTypes m_services = DicomServiceTypes.None;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;


    /// <summary>
    /// Attributes
    /// </summary>
    private Dictionary<string,string> m_attributes = new Dictionary<string,string>();

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
    /// Server
    /// </summary>
    public string Server
    {
      get { return m_server; }
      set { m_server = value; }
    }

    /// <summary>
    /// Type of job
    /// </summary>
    public DicomServiceTypes Services
    {
      get { return m_services; }
      set { m_services = value; }
    }

    /// <summary>
    /// Port
    /// </summary>
    public uint Port
    {
      get { return m_port; }
      set { m_port = value; }
    }

    /// <summary>
    /// AE Title
    /// </summary>
    public string AETitle 
    {
      get { return m_aetitle; }
      set { m_aetitle = value; }
    }
    

    /// <summary>
    /// Description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    }


    /// <summary>
    /// Gets or sets the attributes.
    /// </summary>
    public Dictionary<string, string> Attributes
    {
      get { return m_attributes; }
      set { m_attributes = value; }
    } // Attributes

    #endregion

    #region methods




    #region static methods


    /// <summary>
    /// Creates a new instance of a DicomHost object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A DicomHost object.</returns>
    public static DicomHost Create(XmlNode node)
    {


      DicomHost host = new DicomHost();

      if (host == null)
        throw new OutOfMemoryException("Failed to allocate DicomHost");

      host.Load(node);

      return host;
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

    /// <summary>
    /// Returns just the name
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_name;
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

          writer.WriteAttributeString(DicomHostXmlAttributes.Name, m_name);
          writer.WriteAttributeString(DicomHostXmlAttributes.Server, m_server);
          writer.WriteAttributeString(DicomHostXmlAttributes.AETitle, m_aetitle);
          writer.WriteAttributeString(DicomHostXmlAttributes.Port, m_port.ToString());
          writer.WriteAttributeString(DicomHostXmlAttributes.Services, (int)m_services);
          writer.WriteAttributeString(DicomHostXmlAttributes.Description, m_description);


          if (m_attributes.Count > 0)
          {
            writer.WriteStartElement(DicomHostXmlAttributes.HostAttributes);

            foreach ( KeyValuePair<string,string> attribute in m_attributes)
            {
              writer.WriteStartElement(attribute.Key);
              writer.WriteRaw(attribute.Value);
              writer.WriteEndElement();

            }

            writer.WriteEndElement();
          }
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

      m_aetitle = node.Attributes[DicomHostXmlAttributes.AETitle].Value;
      m_server = node.Attributes[DicomHostXmlAttributes.Server].Value;
      m_services = (DicomServiceTypes)Enum.Parse(typeof(DicomServiceTypes), node.Attributes[DicomHostXmlAttributes.Services].Value.ToString());
      m_port = UInt32.Parse(node.Attributes[DicomHostXmlAttributes.Port].Value);
      m_description = node.Attributes[DicomHostXmlAttributes.Description].Value;
      m_name = node.Attributes[DicomHostXmlAttributes.Name].Value;

      XmlNode attributes = node.SelectSingleNode(DicomHostXmlAttributes.HostAttributes);

      if (attributes != null)
      {
        foreach (XmlNode attribute in attributes.ChildNodes)
        {
          m_attributes.Add(attribute.Name, attribute.InnerText);
        }
      }

    } // Load(XmlNode node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      DicomHost host = new DicomHost();
      CopyTo(host);

      host.m_name = m_name;
      host.m_server = m_server;
      host.m_aetitle = m_aetitle;
      host.m_port = m_port;
      host.m_services = m_services;
      host.m_attributes = m_attributes;

      return host;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      DicomHost other = obj as DicomHost;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion


    #endregion

  } // class DicomHost
} // namespace Viatronix.Enterprise.Entities
