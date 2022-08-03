using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// Maps the database DicomServers table
  /// </summary>
  public class DicomServer : Entity
  {

    #region class Attributes

    public static class Attributes
    {
      public const string AETitle   = "aetitle";
      public const string Server    = "server";
      public const string Port      = "port";
      public const string Date      = "date";
      public const string Receivers = "receivers";
    } // class Attributes

    #endregion

    #region fields



    /// <summary>
    /// AE Title
    /// </summary>
    private String m_aetitle = string.Empty;

    /// <summary>
    /// Server
    /// </summary>
    private string m_server = string.Empty;

    /// <summary>
    /// Port
    /// </summary>
    private UInt32 m_port = 0;

    /// <summary>
    /// Date
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Receivers
    /// </summary>
    private List<Receiver> m_receivers = new List<Receiver>();

    #endregion


    #region properties

    /// <summary>
    /// Gets the aetitle
    /// </summary>
    public string AETitle
    { get { return m_aetitle;  } }

    /// <summary>
    /// Gets the server name
    /// </summary>
    public string Server
    { get { return m_server;  } }

    /// <summary>
    /// Gets the port
    /// </summary>
    public UInt32 Port
    { get { return m_port;  } }

    /// <summary>
    /// Gets the date
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value;  } }

    /// <summary>
    ///  Gets the receivers associated with this dicom server
    /// </summary>
    public List<Receiver> Receivers
    { get { return m_receivers;  } }

    #endregion

    #region methods

    /// <summary>
    /// Creates the DicomServer from the xml node
    /// </summary>
    /// <param name="node">Xml</param>
    /// <returns>DicomServer</returns>
    public static DicomServer Create(XmlNode node)
    {

      DicomServer server = new DicomServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate DicomServer");


      server.m_aetitle = node.Attributes[Attributes.AETitle].Value;
      server.m_server = node.Attributes[Attributes.Server].Value;
      server.m_port = UInt32.Parse(node.Attributes[Attributes.Port].Value);
      server.m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);

      XmlNode receivers = node.SelectSingleNode(Attributes.Receivers);

      if (receivers != null)
        EntityCollection.Load(server.m_receivers, receivers);

   


      return server;

    } // Create(XmlNode node)

 
    #endregion


    #region IEntity


    /// <summary>
    /// Creates the DicomServer from the xml node
    /// </summary>
    /// <param name="node">Xml</param>
    /// <returns>DicomServer</returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      m_aetitle = node.Attributes[Attributes.AETitle].Value;
      m_server = node.Attributes[Attributes.Server].Value;
      m_port = UInt32.Parse(node.Attributes[Attributes.Port].Value);
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);

      foreach (XmlNode receiver in node.SelectNodes(Attributes.Receivers + "/receiver"))
      {
        m_receivers.Add(Receiver.Create(receiver));
      }


    } // Load(XmlNode node)

    /// <summary>
    /// Clones the dicom server object
    /// </summary>
    /// <returns></returns>
    public override object Clone()
    {

      DicomServer server = new DicomServer();

      if (server == null)
        throw new OutOfMemoryException("Failed to allocate DicomServer");


      server.m_aetitle = m_aetitle;
      server.m_server = m_server;
      server.m_port = m_port;
      server.m_date = m_date;

      m_receivers.ForEach( receiver => server.m_receivers.Add(receiver) );


      return server;

    } // Clone()

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


          writer.WriteAttributeString(Attributes.AETitle, m_aetitle);
          writer.WriteAttributeString(Attributes.Server, m_server);
          writer.WriteAttributeString(Attributes.Port, m_port.ToString());
          writer.WriteAttributeString(Attributes.Date, m_date.ToString());


          WriteAttributeStrings(writer);
          WriteElements(writer);

          if (m_receivers.Count > 0)
          {
            // output profile xml
            writer.WriteStartElement(Attributes.Receivers);
            {
              foreach (Receiver receiever in m_receivers)
              { writer.WriteRaw(receiever.ToXml()); }

            }
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
      DicomServer other = obj as DicomServer;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_aetitle, other.m_aetitle) &&
                string.Equals(m_server, other.m_server) &&
                  m_port == other.m_port;

    } // Equals( obj )


    #endregion
  } // class DicomServer

}
