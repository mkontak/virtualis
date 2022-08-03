using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;


namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Implements the ReceivingHistory object from the auditing database
  /// </summary>
  public class ReceivingHistory : Entity
  {


    #region class Attributes

    /// <summary>
    /// Mirrors the database
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Date = "date";
      public const string Name = "name";
      public const string Receiver = "rcvr";
      public const string SeriesUid = "uid";
      public const string ImageCount = "icnt";
      public const string Source = "src";
      public const string DicomServer = "svr";
     };

    #endregion

    #region fields

    /// <summary>
    /// Identifier
    /// </summary>
    private UInt64 m_id;

    /// <summary>
    /// receiver
    /// </summary>
    private string m_receiver = string.Empty;

    /// <summary>
    /// source (calling ae title)
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// Dicom server 
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// PatientName 
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Series Uid 
    /// </summary>
    private string m_seriesUid = string.Empty;

     /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Image count
    /// </summary>
    private UInt32 m_imageCount;

    #endregion

    #region properties

    /// <summary>
    /// Identifier
    /// </summary>
    public UInt64 Id
    { get { return m_id; } }

    /// <summary>
    /// Gets date the dicom was received
    /// </summary>
    public DateTimeOffset ReceivingDate
    { get { return m_date.Value; } }


    /// <summary>
    /// Gets series uid
    /// </summary>
    public string SeriesUid
    { get { return m_seriesUid; } }

 

    /// <summary>
    /// Gets patient name
    /// </summary>
    public string PatientName
    { get { return m_name; } }

    /// <summary>
    /// Gets the image count
    /// </summary>
    public UInt32 ImageCount
    { get { return m_imageCount; } }

    /// <summary>
    /// Gets the source
    /// </summary>
    public string Source
    { get { return m_source; } }

    /// <summary>
    /// Gets server that received the adata
    /// </summary>
    public string Server
    { get { return m_server; } }

    /// <summary>
    /// Gets receiver
    /// </summary>
    public string Receiver
    { get { return m_receiver; } }

 



    #endregion

    #region methods


    /// <summary>
    /// String representation of the object (uses the id)
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return m_id.ToString();
    } // ToString()


    /// <summary>
    /// Creates the Product from the xml
    /// </summary>
    /// <param name="xml">The xml to de-serialize.</param>
    /// <returns>Product object.</returns>
    static public ReceivingHistory Create(string xml)
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
    static public ReceivingHistory Create(XmlNode node)
    {
      ReceivingHistory history = new ReceivingHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create ReceivingHistory object");

      history.Load(node);

      return history;
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

          writer.WriteAttributeString(Attributes.Id, m_id.ToString());
          writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("o"));
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.SeriesUid, m_seriesUid);
          writer.WriteAttributeString(Attributes.DicomServer, m_server);
          writer.WriteAttributeString(Attributes.ImageCount, m_imageCount.ToString());
          writer.WriteAttributeString(Attributes.Receiver, m_receiver);
          writer.WriteAttributeString(Attributes.Source, m_source);
 
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

      m_id = UInt64.Parse(node.Attributes[Attributes.Id].Value.ToString());

      m_name = node.Attributes[Attributes.Name].Value.ToString();
      m_receiver = node.Attributes[Attributes.Receiver].Value.ToString();
      m_server = node.Attributes[Attributes.DicomServer].Value.ToString();
      m_source = node.Attributes[Attributes.Source].Value.ToString();
      m_imageCount = UInt32.Parse(node.Attributes[Attributes.ImageCount].Value.ToString());
      m_seriesUid = node.Attributes[Attributes.SeriesUid].Value.ToString();
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value.ToString());

    } // Load( node )

    /// <summary>
    /// Clones the directory object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {
      ReceivingHistory history = new ReceivingHistory();

      if (history == null)
        throw new OutOfMemoryException("Failed to create ReceivingHistory object");


      history.m_id = m_id;
      history.m_source = m_source;
      history.m_server = m_server;
      history.m_seriesUid = m_seriesUid;
      history.m_date = m_date;
      history.m_receiver = m_receiver;
      history.m_name = m_name;
      history.m_imageCount = m_imageCount;


      return history;

    } // Clone()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      ReceivingHistory other = obj as ReceivingHistory;

      return other != null &&
             base.Equals(other) &&
              (m_id == other.m_id);

    } // Equals( obj )


    #endregion

  } // class ReceivingHistory

} // namespace Viatronix.Enterprise.Entities
