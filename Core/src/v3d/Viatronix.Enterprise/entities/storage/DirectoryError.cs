using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements a directory resource that holds an entry containing the information of a directory tha could not be deletred by the resource provider.
  /// </summary>
  public class DirectoryError : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Uid = "uid";
      public const string Path = "path";
      public const string Message = "msg";
      public const string Date = "date";
      public const string Type = "type";
    };

    #endregion

    #region fields

    /// <summary>
    /// id of directory
    /// </summary>
    private UInt64 m_id = 0;

    /// <summary>
    /// Uid
    /// </summary>
    private string m_uid = string.Empty;

 
    /// <summary>
    /// path
    /// </summary>
    private string m_path = string.Empty;


    /// <summary>
    /// Error message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset m_date = DateTimeOffset.Now;


    /// <summary>
    /// Uid type (study or series)
    /// </summary>
    private string m_type = string.Empty;

    #endregion

    #region properties


    /// <summary>
    /// Gets the directory id ( VC, VAS, CAR, ... )
    /// </summary>
    public UInt64 Id
    {
      get { return m_id; }
    }

    /// <summary>
    /// Gets the uid
    /// </summary>
    public string Uid
    {
      get { return m_uid; }
    }

    /// <summary>
    /// Gets the Type
    /// </summary>
    public string Type
    {
      get { return m_type; }
    }

    /// <summary>
    /// Gets the directory path
    /// </summary>
    public string Path
    {
      get { return m_path; }
    }


    /// <summary>
    /// Gets the directory error message
    /// </summary>
    public string ErrorMessage
    {
      get { return m_message; }
    }

    /// <summary>
    /// Gets the directory date 
    /// </summary>
    public DateTimeOffset Date
    {
      get { return m_date; }
    }

 
    #endregion

    #region methods

    /// <summary>
    /// Provides a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    { return m_path; } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public DirectoryError Create(string xml)
    {
      DirectoryError directory = new DirectoryError();

      if (directory == null)
      {
        throw new OutOfMemoryException("Failed to create DirectoryError object");
      }

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(xml);

      return Create(doc.DocumentElement);

    } // Create( xml )


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public DirectoryError Create(XmlNode node)
    {
      DirectoryError directory = new DirectoryError();

      if (directory == null)
        throw new OutOfMemoryException("Failed to create DirectoryError object");

      directory.Load(node);

      return directory;
    } // Create( node )

    #endregion

    #region IEntity

    /// <summary>
    /// Creates an xml representation of a directory object.
    /// </summary>
    /// <returns>Xlm string representing the directory object.</returns>
    public override string ToXml()
    {
      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {


          writer.WriteAttributeString(Attributes.Id, m_id);
          writer.WriteAttributeString(Attributes.Message, m_message);
          writer.WriteAttributeString(Attributes.Path, m_path);
          writer.WriteAttributeString(Attributes.Uid, m_uid);
          writer.WriteAttributeString(Attributes.Type, m_type);
          writer.WriteAttributeString(Attributes.Date, m_date.ToString("0"));
 

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

      m_id = UInt64.Parse(node.Attributes[Attributes.Id].Value);
      m_path = node.Attributes[Attributes.Path].Value.ToString();
      m_uid = node.Attributes[Attributes.Uid].Value.ToString();
      m_message = node.Attributes[Attributes.Message].Value.ToString();
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());
      m_type = node.Attributes[Attributes.Type].Value.ToString();



    } // Load( node )

    /// <summary>
    /// Clones the directory object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      DirectoryError directory = new DirectoryError();

      if (directory == null)
        throw new OutOfMemoryException("Failed to create DirectoryError object");


      directory.m_id = m_id;
      directory.m_path = m_path;
      directory.m_message = m_message;
      directory.m_uid = m_uid;
      directory.m_date = m_date;
      directory.m_type = m_type;


      return directory;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      DirectoryError other = obj as DirectoryError;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_id, other.m_id);

    } // Equals( obj )


    #endregion

  } // class DirectoryError

} // namespace Viatronix.Enterprise.Entities
