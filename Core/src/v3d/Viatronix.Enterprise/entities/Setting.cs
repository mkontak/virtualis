// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Setting.cs
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
  /// Setting object
  /// </summary>
  public class Setting : Entity
  {

    #region class Attributes

    /// <summary>
    /// settings fields directly related to the settings table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name          = "name";
      public const string Description   = "desc";
      public const string Value         = "value";
      public const string Subsystem     = "subsystem";
      public const string Changable     = "chg";
      public const string Expand        = "expand";
     } // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// Flag to indicate that the value is to be expanded
    /// </summary>
    private bool m_expand = false;

    /// <summary>
    /// Customer changable flag
    /// </summary>
    private bool m_changable = false;

    /// <summary>
    /// Setting name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Setting description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Setting value
    /// </summary>
    private string m_value = string.Empty;

    /// <summary>
    /// Setting subsystem
    /// </summary>
    private string m_subsystem = string.Empty;

 
    #endregion


    #region properties

    /// <summary>
    /// Gets the customer changable flag
    /// </summary>
    public bool Changable
    { get { return m_changable; } }

    /// <summary>
    /// Gets the name
    /// </summary>
    public string Name
    { 
      get { return m_name; }
      private set { SetProperty<string>("Name", value, ref m_name);  }
    }

    /// <summary>
    /// Gets the subsystem
    /// </summary>
    public string Subsystem
    {
      get { return m_subsystem; }
      private set { SetProperty<string>("Subsystem", value, ref m_subsystem); }
    }


    /// <summary>
    /// Gets the description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      private set { SetProperty<string>("Description", value, ref m_description); }
    }

    /// <summary>
    /// Gets the value
    /// </summary>
    public string Value
    {
      get { return m_value; }
      private set { SetProperty<string>("Value", value, ref m_value); }
    }


    #endregion

    #region methods


    #region Create methods

    /// <summary>
    /// Creates the setting from the xml string
    /// </summary>
    /// <param name="xml">Xml string</param>
    /// <returns>Setting</returns>
    public static Setting Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // Create(string xml)

    /// <summary>
    /// Creates a setting from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Setting</returns>
    public static Setting Create(XmlNode node)
    {

      Setting setting = new Setting();

      if (setting == null)
        throw new OutOfMemoryException("Failed to allocate the setting");

      setting.Load(node);

      return setting;

    } // Create(XmlNode node)


    #endregion

    #endregion


    #region IEntity


    /// <summary>
    /// Creates a setting from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Setting</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      //node.ValidateSchema(Schema.Settings);


      m_name = node.Attributes[Attributes.Name].Value;
      m_subsystem = node.Attributes[Attributes.Subsystem].Value;
      m_value = node.Attributes[Attributes.Value].Value;
      m_description = node.Attributes[Attributes.Description].Value;

      string expand = "";
      node.TryGetAttribute<string>(Attributes.Expand, ref expand);

      m_expand = (expand == "1" || string.Compare("true", expand, true) == 0 ? true : false);

      int changable = Int32.Parse(node.Attributes[Attributes.Changable].Value);
      m_changable = ( changable == 0 ? false : true );

    } // Load(XmlNode node)

    /// <summary>
    /// Creates the xml from the object
    /// </summary>
    /// <returns></returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Subsystem, m_subsystem);
          writer.WriteAttributeString(Attributes.Value, m_value);
          writer.WriteAttributeString(Attributes.Changable, (m_changable ? "1" : "0" ));
          writer.WriteAttributeString(Attributes.Expand, (m_expand ? "1" : "0"));

          WriteAttributeStrings(writer);
          WriteElements(writer);

        }

        writer.WriteEndElement();

      }

      return builder.ToString();
    }

    /// <summary>
    /// Clones the setting
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Setting</returns>
    public override object Clone()
    {

      Setting setting = new Setting();

      if (setting == null)
        throw new OutOfMemoryException("Failed to allocate the setting");


      setting.m_name        = m_name;
      setting.m_subsystem   = m_subsystem;
      setting.m_value       = m_value;
      setting.m_description = m_description;
      setting.m_changable   = m_changable;
      setting.m_expand = m_expand;
 
      return setting;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Setting other = obj as Setting;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion
  }
}
