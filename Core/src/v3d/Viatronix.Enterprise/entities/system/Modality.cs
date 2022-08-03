// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Modality.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Text;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// Summary description for Modality.
  /// </summary>
  public class Modality : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Code        = "code";
      public const string Description = "desc";
    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Name of the Right object
    /// </summary>
    private string m_code;

    /// <summary>
    /// Right Description object.
    /// </summary>
    private string m_description;

    #endregion fields

    #region properties

    /// <summary>
    /// Property returns the modalitry code
    /// </summary>
    public string Code
    {
      get { return m_code; }
      set { m_code = value; }
    } // Code

    /// <summary>
    /// Property returns the Right Description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    } // Description

    #endregion Properties

    #region methods

    /// <summary>
    /// Creates the right from the xml
    /// </summary>
    /// <param name="node"></param>
    static public Modality Create(XmlNode node)
    {
      Modality mod = new Modality();

      if (mod == null)
        throw new OutOfMemoryException("Failed to allocate modality");

      mod.Load(node);

      return mod;
    }

    /// <summary>
    /// Returns string representation of the right (Description)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return Code + " - " + Description;
    } // ToString()


    /// <summary>
    /// Get hash code
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()

    
    /// <summary>
    /// Modality constructor
    /// </summary>
    public Modality()
    {
    }//Modality


    /// <summary>
    /// Modality constructor
    /// </summary>
    /// <param name="code">code</param>
    /// <param name="description">description</param>
    public Modality(string code, string description)
    {
      m_code = code;
      m_description = description;
    }//Modality(string code, string description)


    #endregion Methods

    #region IEntity

    /// <summary>
    /// Loads the xml into the object
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {
      base.Load(node);

      m_code = node.Attributes[Attributes.Code].Value;
      m_description = node.Attributes[Attributes.Description].Value;

    } // Load(node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Modality mod = new Modality();

      if (mod == null)
        throw new OutOfMemoryException("Failed to allocate modality");

      mod.m_code = m_code;
      mod.m_description = m_description;

      return mod;

    } // Clone()

    /// <summary>
    /// Convert to XML
    /// </summary>
    /// <returns></returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {

        WriteAttributeStrings(writer);
        WriteElements(writer);
      }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// ToXml 
    /// </summary>
    /// <param name="writer">XmlTextWriter</param>
    public virtual void WriteAttributeStrings(XmlTextWriter writer)
    {

      writer.WriteStartElement(this.Tag);
      {

        writer.WriteAttributeString(Attributes.Code, m_code);
        writer.WriteAttributeString(Attributes.Description, m_description);

        base.WriteAttributeStrings(writer);

      }
      writer.WriteEndElement();

    } // WriteAttributeStrings( writer )

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Modality other = obj as Modality;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_code, other.m_code);

    } // Equals( obj )


    #endregion

  } //public class Modality
}//namespace Viatronix.Enterprise.Entities

