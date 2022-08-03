// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: User.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using Viatronix.Enterprise;


namespace Viatronix.Enterprise.Entities
{

  #region UserProfile class

  /// <summary>
  /// Represents a user within the system
  /// </summary>
  public class UserProfile : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Login = "login";
      public const string Name = "name";
      public const string Profile = "profile";
    } // class Attributes

    #endregion


    #region fields


    /// <summary>
    /// login name
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// Type
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Config xml
    /// </summary>
    private string m_profile = string.Empty;


    #endregion


    #region properties


    /// <summary>
    /// Gets/Sets the login name
    /// </summary>
    public string Login
    {
      get { return m_login; }
      set { SetProperty<string>("Login", value, ref m_login); }
    } // Login


    /// <summary>
    /// Gets/Sets the name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set {SetProperty<string>("Name", value, ref m_name ); }
    } // Name



    /// <summary>
    /// Gets the context Id
    /// </summary>
    public string Profile
    {
      get { return m_profile; }
      set { SetProperty<string>("Profile", value, ref m_profile); }
    }



 
    #endregion constructors

    #region methods


    #region Create methods

    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static UserProfile Create(XmlNode node)
    {
      // Make sure the node is not NULL
      if (node == null)
        throw new InvalidDataException("UserProfile node is null");

      // Create the new user
      UserProfile user = new UserProfile();

      // Make sure allocation was successful
      if (user == null)
        throw new OutOfMemoryException("Failed to allocate UserProfile object");

      // Initialize
      user.m_login = node.Attributes[Attributes.Login].Value;
      user.m_name = node.Attributes[Attributes.Name].Value;
      user.m_profile = node.Attributes[Attributes.Profile].Value;

      return user;

    } // Create(XmlNode node)


    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static UserProfile Create(string xml)
    {

      XmlDocument doc = new XmlDocument();

      doc.LoadXml(xml);


      return Create(doc.DocumentElement);
    } // Create(string xml)

    #endregion


    #region IEntity


    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      // Initialize
      m_login = node.Attributes[Attributes.Login].Value;
      m_name = node.Attributes[Attributes.Name].Value;
      m_profile = node.Attributes[Attributes.Profile].Value;

      

    } // Load(XmlNode node)


    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override object Clone()
    {

      UserProfile profile = new UserProfile();
      CopyTo(profile);

      // node.ValidateSchema(Schema.Settings);

      // Initialize
      profile.m_login = m_login;
      profile.m_name = m_name;
      profile.m_profile = m_profile;

      return profile;

    } // Load(XmlNode node)


    /// <summary>
    /// Creates an xml representation of a User object.
    /// </summary>
    /// <returns>Xml string representing the User object.</returns>
    public override string ToXml()
    {

      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {




          writer.WriteAttributeString(Attributes.Login, m_login);
          writer.WriteAttributeString(Attributes.Name, m_name);
          WriteAttributeStrings(writer);
          WriteElements(writer);

          if (m_profile.Length > 0)
          {
            // output profile xml
            writer.WriteStartElement(Attributes.Profile);
            writer.WriteRaw(m_profile);
            writer.WriteEndElement();
          }



 
        }

        writer.WriteEndElement();

      }

      return builder.ToString();

    } // ToXml()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      UserProfile other = obj as UserProfile;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_login, other.m_login) &&
                string.Equals(m_name, other.m_name);

    } // Equals( obj )


    #endregion

    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_login.GetHashCode();
    }

 
    #endregion


  } // class UserProfile

  #endregion UserProfile class


} // namespace Viatronix.Enterprise.Entities

