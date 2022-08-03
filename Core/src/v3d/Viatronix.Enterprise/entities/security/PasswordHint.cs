// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: PasswordHint.cs
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
  /// PasswordHint object
  /// </summary>
  public class PasswordHint : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Hint = "hint";
    } // class Attributes

    #endregion


    #region fields


    /// <summary>
    ///  Hint
    /// </summary>
    private string m_hint = string.Empty;

    /// <summary>
    /// Hint name
    /// </summary>
    private string m_name = string.Empty;

 
    #endregion



    #region properties



    /// <summary>
    /// Gets Hint
    /// </summary>
    public string Hint
    {
      get { return m_hint; }
    } // Hintt

    /// <summary>
    /// Name 
    /// </summary>
    public string Name
    {
      get { return m_name; }
    }


    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_name;
    } // ToString()

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
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Hint, m_hint);

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

      m_name = node.Attributes[Attributes.Name].Value;
      m_hint = node.Attributes[Attributes.Hint].Value;

    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      PasswordHint hint = new PasswordHint();
      CopyTo(hint);

      hint.m_name = m_name;
      hint.m_hint = m_hint;


      return hint;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      PasswordHint other = obj as PasswordHint;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static PasswordHint Create(XmlNode node)
    {
      PasswordHint password = new PasswordHint();

      if (password == null)
        throw new OutOfMemoryException("Failed to allocate PasswordHint");

      password.Load(node);

      return password;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static PasswordHint Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class PasswordHint



} // namespace Viatronix.Enterprise.Entities

