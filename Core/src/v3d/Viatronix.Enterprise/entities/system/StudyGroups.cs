// $Id: StudyGroups.cs,v 1.1.2.1 2008/01/31 19:24:37 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Collections;
using System.Collections.Generic;

namespace Viatronix.Enterprise.Entities
{

 
  /// <summary>
  /// Implements the StudyGroup object within the system
  /// </summary>
  public class StudyGroup : Entity
  {

    #region Attributes class

    /// <summary>
    /// Attributes fr the study groups table
    /// </summary>
    public static class Attributes
    {
      public const string Name = "name";
      public const string Description = "desc";
    }; // class Attributes

    #endregion


    #region fields


    /// <summary>
    /// Service StudyGroup name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Administrator StudyGroup name
    /// </summary>
    private string m_description = string.Empty;


    #endregion

    #region properties

    /// <summary>
    /// Gets/Sets the name 
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_description); }
    } // Name

    /// <summary>
    /// Gets/Sets the description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { SetProperty<string>("Description",value, ref m_description); }
    } // Description

    #endregion

    #region methods

 
 
    /// <summary>
    /// ToString() override returns the name
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_name;
    } // ToString()


    
    /// <summary>
    /// Gets hash code for object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()

    #endregion


    #region static methods

    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public StudyGroup Create(string xml)
    {
 
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public StudyGroup Create(XmlNode node)
    {
      StudyGroup group = new StudyGroup();

      if (group == null)
        throw new OutOfMemoryException("Failed to create StudyGroup object");

      group.Load(node);

      return group;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a install object.
    /// </summary>
    /// <returns>Xlm string representing the install object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Description, m_description);

          WriteAttributeStrings(writer);
          WriteElements(writer);

        }

        writer.WriteEndElement();
      }

      return builder.ToString();
    } // ToXml()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_name = node.Attributes[Attributes.Name].Value.ToString();
      m_description = node.Attributes[Attributes.Description].Value.ToString();

    } // Load( node )

    /// <summary>
    /// Clones the install object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      StudyGroup group = new StudyGroup();

      if (group == null)
        throw new OutOfMemoryException("Failed to create StudyGroup object");


      group.m_name = m_name;
      group.m_description = m_description;


      return group;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      StudyGroup other = obj as StudyGroup;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

 
 
  } // class StudyGroup


} // namespace Viatronix.Enterprise.Entities

