using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace Viatronix.Enterprise.Entities
{
  /// <summary>
  /// Message 
  /// </summary>
  public class Message : Entity
  {

    #region class Attributes


    /// <summary>
    /// Attribute
    /// </summary>
    public static class Attributes
    {
      public const string Id = "id";
      public const string Type = "type";
      public const string Date = "date";
      public const string Application = "app";
      public const string Name = "name";
      public const string Source = "src";
      public const string Message = "msg";
      public const string Orientation = "ort";
    } // class Attributes

    #endregion

 
    #region fields


    /// <summary>
    /// Identifier
    /// </summary>
    private Int32 m_id = 0;


    /// <summary>
    /// Patient name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Type of message
    /// </summary>
    private MessageTypes m_type = MessageTypes.Information;

    /// <summary>
    /// Date of the message
    /// </summary>
    private DateTimeOffset? m_date = null;

    /// <summary>
    /// Application id
    /// </summary>
    private string m_application = string.Empty;

    /// <summary>
    /// Orientation
    /// </summary>
    private string m_orientation = string.Empty;

    /// <summary>
    /// Source of message
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// Message text
    /// </summary>
    private string m_text = string.Empty;

    #endregion


    #region properties

 
    public MessageTypes Type
    { get { return m_type;  } }

    public string Source
    { get { return m_source;} }

    /// <summary>
    /// Gets the application id
    /// </summary>
    public string Application
    { get { return m_application;  } }

    /// <summary>
    /// Gets the patient name
    /// </summary>
    public string Name
    { get { return m_name;  } }

    /// <summary>
    /// Gets the message id
    /// </summary>
    public Int32 Id
    { get { return m_id; } }

    /// <summary>
    /// Gets the date the message was logged
    /// </summary>
    public DateTimeOffset Date
    { get { return m_date.Value; } }

    /// <summary>
    /// Gets the patient orientation
    /// </summary>
    public string Orientation
    { get { return m_orientation; } }

 
    /// <summary>
    /// Message
    /// </summary>
    public string Text
    { get { return m_text;  } }

    #endregion


    #region methods

    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_id.GetHashCode();
    }



    /// <summary>
    /// Creates the Message object from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Message</returns>
    public static Message Create(XmlNode node)
    {

      Message message = new Message();

      if (message == null)
        throw new OutOfMemoryException("Failed to allocate Message");

      message.Load(node);
 

      return message;
    } // Create(XmlNode node)


    #region IEntity
    /// <summary>
    /// Creates the Message object from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>Message</returns>
    public override void Load(XmlNode node)
    {
      base.Load(node);

      m_id = Int32.Parse(node.Attributes[Attributes.Id].Value);
      m_application = node.Attributes[Attributes.Application].Value;
      m_orientation = node.Attributes[Attributes.Orientation].Value;
      m_type = (MessageTypes)UInt16.Parse(node.Attributes[Attributes.Type].Value);
      m_date = DateTimeOffset.Parse(node.Attributes[Attributes.Date].Value);
      m_source = node.Attributes[Attributes.Source].Value;
      m_text = node.Attributes[Attributes.Message].Value;
      m_name = node.Attributes[Attributes.Name].Value;
      
    } // Create(XmlNode node)



    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override object Clone()
    {

      Message message = new Message();
      CopyTo(message);

      // node.ValidateSchema(Schema.Settings);

      // Initialize
      message.m_source = m_source;
      message.m_application = m_application;
      message.m_date = m_date;
      message.m_id = m_id;
      message.m_orientation = m_orientation;
      message.m_text = m_text;
      message.m_type = m_type;

      return message;

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

          writer.WriteAttributeString(Attributes.Id, m_id.ToString());
          writer.WriteAttributeString(Attributes.Type, ((UInt16)m_type).ToString());
          writer.WriteAttributeString(Attributes.Application, m_application);
          writer.WriteAttributeString(Attributes.Source, m_source);
          writer.WriteAttributeString(Attributes.Orientation, m_orientation);
          writer.WriteAttributeString(Attributes.Message, m_text);
          writer.WriteAttributeString(Attributes.Name, m_name);

          if (m_date.HasValue)
            writer.WriteAttributeString(Attributes.Date, m_date.Value.ToString("O"));

          WriteAttributeStrings(writer);
          WriteElements(writer);

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
      Message other = obj as Message;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_id, other.m_id) ;

    } // Equals( obj )

    #endregion

    #endregion

  } // class Message

}
