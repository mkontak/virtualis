// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: State.cs
//
// Description: Implements a base State class
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
  /// Implements a base State class
  /// </summary>
  public class State : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Value = "value";
      public const string Description = "desc";
    } // class Attributes

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
    ///  Value
    /// </summary>
    private Int32 m_value = 0;


    #endregion



    #region properties

    /// <summary>
    /// Gets the state id
    /// </summary>
    public string Id
    {
      get { return m_id; }
    }


    /// <summary>
    /// Gets the bit value
    /// </summary>
    public Int32 Value
    {
      get { return m_value; }
    }


    /// <summary>
    /// Gets description
    /// </summary>
    public string Description
    {
      get { return m_description; }
    }



    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_id;
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

          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Value, m_value.ToString());
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

      m_description = node.Attributes[Attributes.Description].Value;
      m_id = node.Attributes[Attributes.Id].Value;
      m_value = Int32.Parse(node.Attributes[Attributes.Value].Value.ToString());


    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      State type = new State();
      CopyTo(type);

      type.m_id = m_id;
      type.m_description = m_description;
      type.m_value = m_value;

      return type;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      State other = obj as State;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_id, other.m_id);

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static State Create(XmlNode node)
    {
      State state = new State();

      if (state == null)
        throw new OutOfMemoryException("Failed to allocate State");

      state.Load(node);

      return state;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static State Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Task



} // namespace Viatronix.Enterprise.Entities

