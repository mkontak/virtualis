using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace Viatronix.Enterprise.Messaging
{

  /// <summary>
  /// Message type
  /// </summary>
  public enum MessageFormat
  {
    Xml,                // Indicates that the message is in XML format
    Text,               // Indicates that the message is just a text string ( used for exception handling )
    Binary,             // Indicates that the message is binary ( Used for images/ Files )
    Custom              // Not Used
  }

  /// <summary>
  /// Implements the REST Service Response Message
  /// </summary>
  public class ResponseMessage
  {
    #region Attributes class

    public class Attributes
    {
      public const string ErrorCode = "code";
      public const string MessageFormat = "fmt";
      public const string Encoding = "enc";
      public const string Version = "ver";
    };  // class Attributes

    #endregion
    
    #region fields

    /// <summary>
    /// Tag
    /// </summary>
    private readonly string m_tag = "rsp";

    /// <summary>
    /// Version
    /// </summary>
    private string m_version = "1.0";

    /// <summary>
    /// Error code (0 - Success, != 0 Error )
    /// </summary>
    private int m_errorCode = 0;

    /// <summary>
    ///  format or message
    /// </summary>
    private MessageFormat m_format = MessageFormat.Xml;

    /// <summary>
    /// Actual message
    /// </summary>
    private string m_message = string.Empty;

    /// <summary>
    /// Encoding
    /// </summary>
    private Encoding m_encoding = Encoding.ASCII;

    #endregion

    #region constructors

    protected ResponseMessage(MessageFormat format, string message, int errorCode)
    {
      m_format = format;

      if (m_format == MessageFormat.Binary)
        m_encoding = Messaging.Encoding.Base64;

      m_message = message;
      m_errorCode = errorCode;
    }

    protected ResponseMessage()
    { }

    #endregion

    #region methods

    #region Create methods


    /// <summary>
    /// 
    /// </summary>
    /// <param name="format"></param>
    /// <param name="message"></param>
    /// <returns></returns>
    public static ResponseMessage Create(MessageFormat format, string message)
    {
      return Create(format, message, 0); 
    }
 
    /// <summary>
    /// 
    /// </summary>
    /// <param name="format"></param>
    /// <param name="message"></param>
    /// <param name="statusCode"></param>
    /// <returns></returns>
    public static ResponseMessage Create(MessageFormat format, string message, int statusCode)
    {
      ResponseMessage response = new ResponseMessage(format, message, statusCode);

      if (response == null)
        throw new OutOfMemoryException("Failed to allocate response message");

      return response;
    } // Create(MessageType type, string message)

    /// <summary>
    /// Creates the response message
    /// </summary>
    /// <param name="type">Type of message</param>
    /// <param name="stream">stream</param>
    /// <param name="statusCode">status code</param>
    /// <returns></returns>
    public static ResponseMessage Create(MessageFormat format, System.IO.Stream stream, int statusCode)
    {

      string message = string.Empty;

      if (format == MessageFormat.Binary)
      {

        
        long size = stream.Length;

        byte[] buffer = new byte[size];


        stream.Read(buffer, 0, (int)size);

        // Make sure the binary is base 64 encoded
        message = ASCIIEncoding.ASCII.GetString(buffer).ToBase64();

      }

      return Create(format, message, statusCode);

    } // Create(MessageType type, string message)

    /// <summary>
    /// Creates the response message
    /// </summary>
    /// <param name="type">Type of message</param>
    /// <param name="stream">stream</param>
    /// <param name="statusCode">status code</param>
    /// <returns></returns>
    public static ResponseMessage Create(Stream stream)
    {

      ResponseMessage response = new ResponseMessage();

      if (response == null)
        throw new OutOfMemoryException("Failed to allocate ResonseMessage");


      StreamReader reader = new StreamReader(stream);
      string responseXml = reader.ReadToEnd();

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(responseXml);

      if ( doc.DocumentElement.Name == "string" )
        doc.LoadXml(doc.DocumentElement.InnerText);

      XmlNode responseNode = (doc.DocumentElement.Name == response.m_tag ? doc.DocumentElement : doc.DocumentElement.SelectSingleNode(response.m_tag));

      if (responseNode == null)
        throw new InvalidDataException("Invalid response received");

      responseNode.TryGetAttribute<int>(Attributes.ErrorCode, ref response.m_errorCode);
      responseNode.TryGetAttribute<MessageFormat>(Attributes.MessageFormat, ref response.m_format);
      responseNode.TryGetAttribute<string>(Attributes.Version, ref response.m_version);

      int encoding = 0;
      responseNode.TryGetAttribute<int>(Attributes.Encoding, ref encoding);
      response.m_encoding = (Encoding)encoding;

      response.m_message = responseNode.InnerXml;


      return response;

    } // Create(Stream stream)

    #endregion

    /// <summary>
    /// Generate the XML to return
    /// </summary>
    /// <returns></returns>
    public string ToXml()
    {

      StringBuilder xml = new StringBuilder();

      xml.Append(string.Format("<{0} ", m_tag));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.ErrorCode, m_errorCode));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.MessageFormat, m_format.ToString().ToLower()));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.Encoding, (Int32)m_encoding));
      xml.Append(string.Format("{0}=\"1.0\" >", Attributes.Version));

      xml.Append(m_message);

      xml.Append(String.Format("</{0}>", m_tag));

      return xml.ToString();

    } // ToXml()

    /// <summary>
    /// Generate the XML to return
    /// </summary>
    /// <returns></returns>
    public string ToLoggingXml()
    {

      StringBuilder xml = new StringBuilder();

      xml.Append(string.Format("<{0} ", m_tag));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.ErrorCode, m_errorCode));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.MessageFormat, m_format.ToString().ToLower()));
      xml.Append(string.Format("{0}=\"{1}\" ", Attributes.Encoding, m_encoding.ToString().ToLower()));
      xml.Append(string.Format("{0}=\"1.0\" >", Attributes.Version));

      if (m_format == MessageFormat.Binary)
        xml.Append("BINARY DATA");
      else
        xml.Append(m_message);

      xml.Append(String.Format("</{0}>", m_tag));

      return xml.ToString();

    } // ToXml()


    #endregion

    #region properties

    /// <summary>
    /// Gets the message
    /// </summary>
    public string Message
    { get { return m_message;  } }

    /// <summary>
    /// Format of message
    /// </summary>
    public MessageFormat Format
    { get { return m_format; } }

    /// <summary>
    /// Error code generated from the server 
    /// </summary>
    public int ErrorCode
    { get { return m_errorCode; } }

    /// <summary>
    /// True if the response indicates failure
    /// </summary>
    public bool Failed
    { get { return (m_errorCode != 0 ? true : false); } }


    /// <summary>
    /// Gets the encoding
    /// </summary>
    public Encoding Encoding
    { get { return m_encoding; } }

    #endregion

  } // class ResponseMessage

} // Viatronix.Enterprise.Messaging
