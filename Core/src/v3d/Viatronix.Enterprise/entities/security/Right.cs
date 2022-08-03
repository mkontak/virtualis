// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Right.cs
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

  #region Right class

  /// <summary>
  /// Summary description for Right.
  /// </summary>
  public class Right : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name        = "name";
      public const string BitMask     = "mask";
      public const string Description = "desc";
    } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Name of the Right object
    /// </summary>
    private string m_name;

    /// <summary>
    /// The int34 object which indicates the Right
    /// </summary>
    private Int64 m_bitmask;

    /// <summary>
    /// Right Description object.
    /// </summary>
    private string m_description;

    #endregion fields

    #region Properties

 
    /// <summary>
    /// Property returns the Right Name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    }//Name

    /// <summary>
    /// Property returns the Right value
    /// </summary>
    public Int64 BitMask
    {
      get { return m_bitmask; }
      set { m_bitmask = value; }
    }//Right

    /// <summary>
    /// Property returns the Right Description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    }//Description

    #endregion Properties

    #region Methods

    /// <summary>
    /// Creates the right from the xml
    /// </summary>
    /// <param name="node"></param>
    static public Right Create(XmlNode node)
    {
      Right right = new Right();

      if (right == null)
        throw new OutOfMemoryException("Failed to allocate right");

      right.Load(node);

      return right;
    }

    /// <summary>
    /// Returns string representation of the right (Description)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_description;
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
    /// Rights constructor
    /// </summary>
    public Right()
    {
    }//Rights


    /// <summary>
    /// Rights constructor
    /// </summary>
    /// <param name="name">string</param>
    /// <param name="pos">int64</param>
    /// <param name="description">string</param>
    public Right(string name, Int64 bitmask, string description)
    {
      m_name = name;
      m_bitmask = bitmask;
      m_description = description;
    }//Rights(string name,Int64 pos,string description)

 
    #endregion Methods


    #region IEntity

    /// <summary>
    /// Loads the xml into the object
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {
      base.Load(node);

      m_name = node.Attributes[Attributes.Name].Value;
      m_description = node.Attributes[Attributes.Description].Value;
      m_bitmask = Int64.Parse(node.Attributes[Attributes.BitMask].Value);
  
    } // Load(node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Right right = new Right();

      if (right == null)
        throw new OutOfMemoryException("Failed to allocate right");

      right.Name = m_name;
      right.Description = m_description;
      right.BitMask = m_bitmask;

      return right;
    
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

        writer.WriteAttributeString(Attributes.Name, m_name);
        writer.WriteAttributeString(Attributes.BitMask, m_bitmask.ToString());
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
      Right other = obj as Right;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

  } //public class Rights

  #endregion Right class


}//namespace Viatronix.Enterprise.Entities

