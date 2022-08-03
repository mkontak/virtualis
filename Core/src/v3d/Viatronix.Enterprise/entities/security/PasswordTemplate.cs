// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PasswordTemplate.cs
//
// Description: Implements a user password object
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
  /// <summary>
  /// PasswordTemplate object
  /// </summary>
  public class PasswordTemplate : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Description = "desc";
      public const string Default = "default";
      public const string Template = "template";
    } // class Attributes

    #endregion
    
    #region fields

    /// <summary>
    ///  Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// Default flag
    /// </summary>
    private Boolean m_default = false;

    /// <summary>
    /// Template xml
    /// </summary>
    private string m_template = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets the Id.
    /// </summary>
    public string Id
    {
      get { return m_id; }
    } // Id


    /// <summary>
    /// Gets the description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
    } // Description


    /// <summary>
    /// Gets if the this is the default template.
    /// </summary>
    public bool IsDefault
    { 
      get { return m_default; }
      set { m_default = value; }
    } // IsDefault


    /// <summary>
    /// Gets the minimum number of characters.
    /// </summary>
    public int MinimumChars
    {
      get
      {
        string minimum = null;
        if (TryGetValue("min", out minimum))
        {
          return Convert.ToInt32(minimum);
        }

        return 0;
      }
    } // MinimumChars


    /// <summary>
    /// Gets the maximum number of characters.
    /// </summary>
    public int MaximumChars
    {
      get
      {
        string maximum = null;
        if (TryGetValue("max", out maximum))
        {
          return Convert.ToInt32(maximum);
        }

        return 0;
      }
    } // MaximumChars


    /// <summary>
    /// Gets the minimum number of lowercase characters.
    /// </summary>
    public int MinimumLowercaseChars
    {
      get
      {
        string lcase = null;
        if (TryGetValue("lower", out lcase))
        {
          return Convert.ToInt32(lcase);
        }

        return 0;
      }
    } // MinimumLowercaseChars


    /// <summary>
    /// Gets the minimum number of uppercase characters.
    /// </summary>
    public int MinimumUppercaseChars
    {
      get
      {
        string ucase = null;
        if (TryGetValue("upper", out ucase))
        {
          return Convert.ToInt32(ucase);
        }

        return 0;
      }
    } // MinimumUppercaseChars


    /// <summary>
    /// Gets the minimum number of numeric characters.
    /// </summary>
    public int MinimumNumericChars
    {
      get
      {
        string numeric = null;
        if (TryGetValue("numeric", out numeric))
        {
          return Convert.ToInt32(numeric);
        }

        return 0;
      }
    } // MinimumNumericChars


    /// <summary>
    /// Gets the minimum number of special characters.
    /// </summary>
    public int MinimumSpecialChars
    {
      get
      {
        string special = null;
        if (TryGetValue("special", out special))
        {
          return Convert.ToInt32(special);
        }

        return 0;
      }
    } // MinimumSpecialChars


    /// <summary>
    /// Gets if spaces are allowed.
    /// </summary>
    public bool AllowSpaces
    {
      get
      {
        string spaces = null;
        if (TryGetValue("spaces", out spaces))
        {
          return Convert.ToBoolean(spaces);
        }

        return false;
      }
    } // AllowSpaces


    /// <summary>
    /// Gets if username can appear in the password.
    /// </summary>
    public bool AllowUsernameInPassword
    {
      get
      {
        string spaces = null;
        if (TryGetValue("spaces", out spaces))
        {
          return Convert.ToBoolean(spaces);
        }

        return false;
      }
    } // AllowUsernameInPassword


    /// <summary>
    /// The maximum number of repititions in the password.
    /// </summary>
    public int MaximumNumberOfRepititions
    {
      get
      {
        string repitition = null;
        if (TryGetValue("repitition", out repitition))
        {
          return Convert.ToInt32(repitition);
        }

        return 0;
      }
    } // MaximumNumberOfRepititions


    /// <summary>
    /// The time to live for the password.
    /// </summary>
    public int TimeToLive
    {
      get
      {
        string ttl = null;
        if (TryGetValue("ttl", out ttl))
        {
          return Convert.ToInt32(ttl);
        }

        return 0;
      }
    } // TimeToLive


    /// <summary>
    /// Gets the password frequency.
    /// </summary>
    public int Frequency
    {
      get
      {
        string freq = null;
        if (TryGetValue("frequency", out freq))
        {
          return Convert.ToInt32(freq);
        }

        return 0;
      }
    } // Frequency


    /// <summary>
    /// Gets the number of minimum differences between password changes.
    /// </summary>
    public int MinimumDifferentCharacters
    {
      get
      {
        string diff = null;
        if (TryGetValue("differ", out diff))
        {
          return Convert.ToInt32(diff);
        }

        return 0;
      }
    } // MinimumDifferentCharacters


    /// <summary>
    /// Gets the maximum number of times a character can be reused in the password.
    /// </summary>
    public int MaximumReuseCharacter
    {
      get
      {
        string reuse = null;
        if (TryGetValue("reuse", out reuse))
        {
          return Convert.ToInt32(reuse);
        }

        return 0;
      }
    } // MaximumReuseCharacter

    #endregion

    #region methods
    
    /// <summary>
    /// String representation of the object (uses the id).
    /// </summary>
    /// <returns>A string representation of the object (uses the id).</returns>
    public override string ToString()
    {
      return m_id;
    } // ToString()


    /// <summary>
    /// Attempts to get the value from the name.
    /// </summary>
    /// <param name="name">The name of the setting.</param>
    /// <param name="value">The corresponding value.</param>
    /// <returns>True if the name could be found and a value exists for it, false otherwise.</returns>
    public bool TryGetValue(string name, out string value)
    {
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(m_template);

      XmlNode node = null;
      if((node = doc.SelectSingleNode(name)) != null)
      {
        value = node.InnerText;
        return true;
      }

      value = null;
      return false;
    } // TryGetValue( name, value )

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
        writer.WriteStartElement(this.Tag);

        writer.WriteAttributeString(Attributes.Id, m_id);
        writer.WriteAttributeString(Attributes.Description, m_description);
        writer.WriteAttributeString(Attributes.Default, m_default.ToString());
        writer.WriteAttributeString(Attributes.Template, m_template);

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

      m_id = node.Attributes[Attributes.Id].Value;
      m_description = node.Attributes[Attributes.Description].Value;

      int defaultValue = 0;
      node.TryGetAttribute<int>(Attributes.Default,ref defaultValue);

      m_default = (defaultValue == 1 ? true : false);

      m_template = node.Attributes[Attributes.Template].Value;
    } // Load( node )


    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      PasswordTemplate template = new PasswordTemplate();
      CopyTo(template);

      template.m_id = m_id;
      template.m_description = m_description;
      template.m_default = m_default;
      template.m_template = m_template;

      return template;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      PasswordTemplate other = obj as PasswordTemplate;

      return other != null && base.Equals(other) && string.Equals(m_id, other.m_id);
    } // Equals( obj )

    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static PasswordTemplate Create(XmlNode node)
    {
      PasswordTemplate template = new PasswordTemplate();

      if (template == null)
        throw new OutOfMemoryException("Failed to allocate PasswordTemplate");

      template.Load(node);

      return template;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static PasswordTemplate Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class PasswordTemplate
} // namespace Viatronix.Enterprise.Entities
