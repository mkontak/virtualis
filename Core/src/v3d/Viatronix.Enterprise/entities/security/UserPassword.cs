// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: UserPassword.cs
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
  /// UserPassword object
  /// </summary>
  public class UserPassword : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Login = "login";
      public const string Date = "date";
      public const string Password = "pwd";
      public const string ExpirationDate = "exp";
    } // class Attributes

    #endregion


    #region fields


    /// <summary>
    ///  UserPassword
    /// </summary>
    private string m_password = string.Empty;

    /// <summary>
    /// User login
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// Date tof the password
    /// </summary>
    private DateTimeOffset? m_date = null;


    /// <summary>
    /// Date the context expires
    /// </summary>
    private DateTimeOffset? m_expirationDate = null;

 
    #endregion



    #region properties



    /// <summary>
    /// Gets login
    /// </summary>
    public string Login
    {
      get { return m_login; }
    } // Login

    /// <summary>
    /// Password 
    /// </summary>
    public string Password
    {
      get { return m_password; }
    }

    /// <summary>
    /// Gets the date 
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value; } }

    /// <summary>
    /// Gets the expiration
    /// </summary>
    public DateTimeOffset ExpirationDate
    { get { return (m_expirationDate.HasValue ? m_expirationDate.Value : DateTimeOffset.MaxValue); } }

 
    #endregion


    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return string.Format("{0}-{1}", m_login, m_date.Value.ToString("o"));
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

          writer.WriteAttributeString(Attributes.Password, m_password);
          writer.WriteAttributeString(Attributes.Login, m_login);
          writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("o"));

          if ( m_expirationDate.HasValue )
            writer.WriteAttributeString(Attributes.ExpirationDate, m_expirationDate.Value.ToString("o"));

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

      m_password = node.Attributes[Attributes.Password].Value;
      m_login = node.Attributes[Attributes.Login].Value;
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);

      node.TryGetAttributeValue<DateTimeOffset?>(Attributes.ExpirationDate, ref m_expirationDate);

    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      UserPassword context = new UserPassword();
      CopyTo(context);

      context.m_password = m_password;
      context.m_login = m_login;
      context.m_date = m_date;
      context.m_expirationDate = m_expirationDate;


      return context;
    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      UserPassword other = obj as UserPassword;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_login, other.m_login) &&
              m_date == other.m_date;

    } // Equals( obj )


    #endregion

    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static UserPassword Create(XmlNode node)
    {
      UserPassword password = new UserPassword();

      if (password == null)
        throw new OutOfMemoryException("Failed to allocate UserPassword");

      password.Load(node);

      return password;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static UserPassword Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class UserPassword



} // namespace Viatronix.Enterprise.Entities

