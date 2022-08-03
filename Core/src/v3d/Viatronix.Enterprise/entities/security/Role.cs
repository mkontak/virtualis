// $Id: Role.cs,v 1.15 2007/03/02 15:49:00 romy Exp $
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


  #region Role class

  /// <summary>
  /// Represents a role within the system that has assignable permissions
  /// </summary>
  public class Role : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Name                = "name";
      public const string Description         = "desc";
      public const string Permissions         = "prms";
      public const string Setting             = "set";
    } // class Attributes

    #endregion

    #region constants
    
    /// <summary>
    /// Role types
    /// </summary>
    public static class Types
    {
      public const string Service         = "Service";
      public const string Administrator   = "Administrator";
      public const string Operator        = "Operator";
      public const string Physician       = "Physician";
      public const string PhysicianAdmin  = "Phy-Admin";
      public const string OperatorAdmin   = "Op-Admin";
    } // class Types

    #endregion constants

    #region fields

 
    /// <summary>
    /// Password expiration
    /// </summary>
    private int m_passwordExpiration = 0;

    /// <summary>
    /// name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// description
    /// </summary>
    private string m_description = string.Empty;

    /// <summary>
    /// assigned permissions
    /// </summary>
    private Int64 m_permissions = 0;

    /// <summary>
    /// System flag
    /// </summary>
    private bool m_system = true;

    /// <summary>
    /// Settings flags
    /// </summary>
    private Settings m_settings = Settings.None;


    #endregion

    #region properties


    /// <summary>
    /// Gets/Sets the system the user is logged in on
    /// </summary>
    public Settings Settings
    {
      get { return m_settings; }
      set { SetProperty<Settings>("Setings", value, ref m_settings); }
    } // SystemAccount

    /// <summary>
    /// Gets the Service flag
    /// </summary>
    public bool IsService
    {
      get { return m_name == Types.Service; }
    } // IsService

    /// <summary>
    /// Gets the Administrator flag
    /// </summary>
    public bool IsAdministrator
    {
      get { return m_name == Types.Administrator || m_name == Types.PhysicianAdmin || m_name == Types.OperatorAdmin ; }
    } // IsAdministrator

    /// <summary>
    /// Gets the Operator flag
    /// </summary>
    public bool IsOperator
    {
      get { return m_name == Types.Operator || m_name == Types.OperatorAdmin; }
    } // IsOperator

    /// <summary>
    /// Gets the IsPhysicain flag
    /// </summary>
    public bool IsPhysician
    {
      get { return m_name == Types.Physician || m_name == Types.PhysicianAdmin; }
    } // IsPhysicain



    /// <summary>
    /// Gets or sets the name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty<string>("Name", value, ref m_name ); }
    }

    /// <summary>
    /// Gets or sets the description
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { SetProperty<string>("Description", value, ref m_description); }
    }

    /// <summary>
    /// Gets or sets the permissions
    /// </summary>
    public Int64 Permissions
    {
      get { return m_permissions; }
      set { SetProperty<Int64>("Permissions", value, ref m_permissions ); }
    }

    #endregion

    #region methods


    /// <summary>
    /// ToString() override returns the name
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {  return m_name;  } 


    /// <summary>
    /// Determine equality for a role

    /// <summary>
    /// Gets hash code for object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()




    /// <summary>
    /// Creates the role from the node
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Role Create(string xml)
    {
      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);

      return Create(doc.DocumentElement);
    } // Create(XmlNode node)

    /// <summary>
    /// Creates the role from the node
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static Role Create(XmlNode node)
    {
      Role role = new Role();

      if (role == null)
        throw new OutOfMemoryException("Failed to allocated role");

      role.Load(node);

      return role;
    } // Create(XmlNode node)

 

    #endregion


    #region IEntity
    /// <summary>
    /// Creates the role from the node
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_description = node.Attributes[Attributes.Description].Value;
      m_name = node.Attributes[Attributes.Name].Value;
      m_permissions = Int64.Parse(node.Attributes[Attributes.Permissions].Value);


    } // Create(XmlNode node)

    /// <summary>
    /// Creates an xml representation of a Role object.
    /// </summary>
    /// <returns>Xlm string representing the Role object.</returns>
    public override string ToXml()
    {
     
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {

          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.Description, m_description);
          writer.WriteAttributeString(Attributes.Permissions, m_permissions.ToString());

          WriteAttributeStrings(writer);
          WriteElements(writer);
        }

        writer.WriteEndElement();

      }

      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Clones the role
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      Role role = new Role();

      if (role == null)
        throw new OutOfMemoryException("Failed to allocated role");

      role.m_description = m_description;
      role.m_name = m_name;
      role.m_permissions = m_permissions;
      role.m_system = m_system;
      role.m_passwordExpiration = m_passwordExpiration;

      return role;
     

    } // Clone()

    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      Role other = obj as Role;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_name, other.m_name);

    } // Equals( obj )

    #endregion

  } // class Role

  #endregion Role class


} // namespace Viatronix.Enterprise.Entities
