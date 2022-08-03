// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DeletePath.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// DeletePath object is a management object used by the ServiceManager. It contains the path that the system had troubles deleting at the time.
  /// </summary>
  public class DeletePath : Entity
  {

    #region class Attributes

    /// <summary>
    /// user fields directly related to the jobs table in the database
    /// </summary>
    public static class Attributes
    {
      public const string TransactionId = "id";         // transaction id
      public const string Uid           = "uid";        // Uid associated with the path
      public const string Path          = "path";       // Path local to the server
     } // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Transaction id associated with the delete
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Uid associated with the path
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Path that failed to be deleted (Local to the server)
    /// </summary>
    private string m_path = string.Empty;

     #endregion

    #region properties

    /// <summary>
    /// Get the transaction id
    /// </summary>
    public string TransactionId
    { get { return m_id; } }

    /// <summary>
    /// Gets the series or study uid
    /// </summary>
    public string Uid
    { get { return m_uid; } }

    /// <summary>
    /// Gets the path
    /// </summary>
    public string Path
    {  get { return m_path; }  }



    #endregion

    #region methods

    #region static methods


    /// <summary>
    /// Creates a new instance of a DeletePath object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A DeletePath object.</returns>
    public static DeletePath Create(XmlNode node)
    {


      DeletePath path = new DeletePath();

      if (path == null)
        throw new OutOfMemoryException("Failed to allocate DeletePath");

      path.Load(node);

      return path;
    } // Create( node )

    #endregion


    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_id.GetHashCode();
    }



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

          writer.WriteAttributeString(Attributes.TransactionId, m_id);
          writer.WriteAttributeString(Attributes.Uid, m_uid);
          writer.WriteAttributeString(Attributes.Path, m_path);
   
        }

        writer.WriteEndElement();


      }


      return builder.ToString();

    } // ToXml()

    /// <summary>
    /// Loads the xml
    /// </summary>
    /// <param name="node"></param>
    public override void Load(XmlNode node)
    {

      base.Load(node);


      m_id = node.Attributes[Attributes.TransactionId].Value;
      m_uid   = node.Attributes[Attributes.Uid].Value;
      m_path = node.Attributes[Attributes.Path].Value;


    } // Load(XmlNode node)

    /// <summary>
    /// Clones the object
    /// </summary>
    /// <returns>clone</returns>
    public override object Clone()
    {
      DeletePath path = new DeletePath();
      CopyTo(path);

      path.m_id = m_id;
      path.m_uid = m_uid;
      path.m_path = m_path;

      return path;
    } // Clone()


    /// <summary>
    /// Checks for value equality of job objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      DeletePath other = obj as DeletePath;

      return other != null &&
             base.Equals(other) &&
             string.Equals(m_id, other.m_id) &&
              string.Equals(m_uid, other.m_uid);

    } // Equals( obj )

    #endregion


    #endregion

  } // class DeletePath
} // namespace Viatronix.Enterprise.Entities
