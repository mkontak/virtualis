// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Context.cs
//
// Description: Object holding the authentication context/token used in the security system
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
  /// Authentication type object used for user objects
  /// </summary>
  public class Context : Entity
  {


    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string License = "lic";
      public const string Login = "login";
      public const string DateIssued = "iss";
      public const string DateRenewed = "mod";
      public const string TimeToLive = "ttl";
      public const string ReferenceCount = "ref";
      public const string ExpirationDate = "exp";
    } // class Attributes

    #endregion


    #region fields


    /// <summary>
    ///  Id
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// License Key
    /// </summary>
    private string m_license = string.Empty;

    /// <summary>
    /// User login
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// Date the context was issued
    /// </summary>
    private DateTimeOffset? m_dateIssued = null;


    /// <summary>
    /// Date the context expires
    /// </summary>
    private DateTimeOffset? m_expirationDate = null;

    /// <summary>
    /// Date the context was last reewed
    /// </summary>
    private DateTimeOffset? m_dateRenewed = null;

    /// <summary>
    /// Refernce count
    /// </summary>
    private UInt32 m_count = 0;

    /// <summary>
    /// Time to live
    /// </summary>
    private UInt32 m_ttl = 0;

 
    #endregion



    #region properties

    /// <summary>
    /// Context id
    /// </summary>
    public string Id
    {
      get { return m_id; }
    }


    /// <summary>
    /// Gets the license key
    /// </summary>
    public string License
    {
      get { return m_license; }
    } 

    /// <summary>
    /// Gets login
    /// </summary>
    public string Login
    {
      get { return m_login; }
    } // Login

    /// <summary>
    /// Gets the date the context was issued
    /// </summary>
    public DateTimeOffset DateIssued
    { get { return m_dateIssued.Value; } }

    /// <summary>
    /// Gets the date the context was renewed
    /// </summary>
    public DateTimeOffset DateRenewed
    { get { return m_dateRenewed.Value; } }

    /// <summary>
    /// Gets the expiration date
    /// </summary>
    public DateTimeOffset ExpirationDate
    { get { return m_expirationDate.Value; } }

    /// <summary>
    /// Reference Count
    /// </summary>
    public UInt32 ReferenceCount
    { get { return m_count; } }

    /// <summary>
    /// 
    /// </summary>
    public UInt32 TimeToLive
    { get { return m_ttl; } }
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

          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.License, m_license);
          writer.WriteAttributeString(Attributes.Login, m_login);
          writer.WriteAttributeString(Attributes.ReferenceCount, m_count.ToString());
          writer.WriteAttributeString(Attributes.TimeToLive, m_ttl.ToString());
          writer.WriteAttributeString(Attributes.DateIssued, m_dateIssued.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.DateRenewed, m_dateRenewed.Value.ToString("o"));
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

      m_id = node.Attributes[Attributes.Id].Value;
      m_license = node.Attributes[Attributes.License].Value;
      m_login = node.Attributes[Attributes.Login].Value;
      m_count = UInt32.Parse(node.Attributes[Attributes.ReferenceCount].Value);
      m_ttl = UInt32.Parse(node.Attributes[Attributes.TimeToLive].Value);
      m_dateRenewed = DateTimeOffset.Parse(node.Attributes[Attributes.DateRenewed].Value);
      m_dateIssued = DateTimeOffset.Parse(node.Attributes[Attributes.DateIssued].Value);
      m_expirationDate = DateTimeOffset.Parse(node.Attributes[Attributes.ExpirationDate].Value);

    }

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      Context context = new Context();
      CopyTo(context);

      context.m_id = m_id;
      context.m_license = m_license;
      context.m_login = m_login;
      context.m_ttl = m_ttl;
      context.m_count = m_count;
      context.m_dateIssued = m_dateIssued;
      context.m_dateRenewed = m_dateRenewed;
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
      Context other = obj as Context;

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
    public static Context Create(XmlNode node)
    {
      Context context = new Context();

      if (context == null)
        throw new OutOfMemoryException("Failed to allocate Context");

      context.Load(node);

      return context;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static Context Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Context



} // namespace Viatronix.Enterprise.Entities

