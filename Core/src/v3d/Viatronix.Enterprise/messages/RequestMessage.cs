// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: RequestMessage.cs
//
// Description: Request message 
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using Viatronix.Encryption;
using Viatronix.Enterprise.Data;
using System.Collections.Specialized;
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Messaging
{

  /// <summary>
  /// Message type
  /// </summary>
  public enum OperationTypes
  {
    // ====================
    // Generial Resources
    // ====================
    Get,                    // GET data
    Update,                 // UPDATE data
    Insert,                 // INSERT/ADD data
    Delete,                 // DELETE data

    // =========================
    // Authentication/Security
    // ==========================
    Acquire,                // Acquire context/authentication token
    Release,                // Release context/authentication token
    Renew,                  // Renew context/authentication toekn

    // =========================
    // Internal services
    // =========================
    Start,                  // Start the service
    Stop,                   // Stop the service

    // ========================
    // Internal File Service
    // ========================
    Read,                   // Read 
    Write,                  // Write

    Process,                

    Custom
  }


  /// <summary>
  /// Encoding used
  /// </summary>
  public enum Encoding
  {
    ASCII = 0,          // ASCII Encoded data (Default)
    Base64 = 1          // BASE64 Encoded data
  };

  /// <summary>
  /// Implements the REST service Resquest Message
  /// </summary>
  public class RequestMessage
  {

    #region Attributes class

    /// <summary>
    /// These are the attributes when we receive POST, PUT and DELETE which is XML
    /// </summary>
    public class Attributes
    {
      public const string ContextId   = "cid";    // Context/Authentication token
      public const string Source      = "src";    // Source of the request (used for backend)
      public const string System      = "sys";    // Client system name makeing request
      public const string Encoding    = "enc";    // Encoding
      public const string Application = "app";    // Application
      public const string Operation   = "op";     // Operation
      public const string Version     = "ver";    // Version 
    };  // class Attributes

    /// <summary>
    /// These are the attributes used when a GET is performed (URL)
    /// </summary>
    public class UrlAttributes
    {
      public const string ContextId   = "rqcid";    // Context/Authentication token
      public const string Source      = "rqsrc";    // Source of the request (used for backend)
      public const string System      = "rqsys";    // Client system name makeing request
      public const string Encoding    = "rqenc";    // Encoding
      public const string Application = "rqapp";    // Application
      public const string Operation   = "rqop";     // Operation
      public const string Version     = "rqver";    // Version 
    };  // class Attributes

    #endregion

    #region fields

    /// <summary>
    /// Internal context id
    /// </summary>
    private readonly string m_internal = Viatronix.Encryption.Crypto.Decrypt("cbHmZgzHtMUP0s69dPhPei5bYZHW1k7QFyx0oEYkyE8Vqgeb4rY/lpbtIw23fbJJ");

    /// <summary>
    /// XML 
    /// </summary>
    private string m_xml = string.Empty;

    /// <summary>
    /// Context ID
    /// </summary>
    private string m_contextId = string.Empty;
    
    /// <summary>
    /// Operation Type
    /// </summary>
    private OperationTypes m_opType = OperationTypes.Custom;

    /// <summary>
    /// Source of request
    /// </summary>
    private string m_source = string.Empty;

    /// <summary>
    /// Operation (Used for custom)
    /// </summary>
    private string m_operation = string.Empty;

    /// <summary>
    /// User
    /// </summary>
    private string m_login = string.Empty;

    /// <summary>
    /// Resource name ( usually a table name )
    /// </summary>
    private string m_resource = string.Empty;

    /// <summary>
    /// Gets the client's system name
    /// </summary>
    private string m_client = Environment.MachineName;

    /// <summary>
    /// Parameters
    /// </summary>
    private Dictionary<string, string> m_parameters = new Dictionary<string, string>();

    /// <summary>
    /// Message
    /// </summary>
    private string m_payload;

    /// <summary>
    /// Version
    /// </summary>
    private string m_version = "1.0";

    /// <summary>
    /// Encoding
    /// </summary>
    private Encoding m_encoding = Encoding.ASCII;

    /// <summary>
    /// Application of request
    /// </summary>
    private string m_application = System.Reflection.Assembly.GetEntryAssembly().GetName().Name;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    private RequestMessage()
    {
      string arg0 =  Environment.GetCommandLineArgs()[0];
      m_source = (string.IsNullOrEmpty(arg0) ? string.Empty : Path.GetFileNameWithoutExtension(arg0));
    }


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="resource"></param>
    /// <param name="contextId"></param>
    private RequestMessage(string resource, string operation, string contextId) : base()
    {
      m_operation = operation;
      m_resource = resource;
      SetContextId(contextId);
    }


    #endregion

    #region methods

    /// <summary>
    /// Sets the context id. Validates the id against the database
    /// </summary>
    /// <param name="contextId">Context id</param>
    private void SetContextId(string contextId)
    {

      m_contextId = contextId;

      // =================================================
      // Only validate the context if is contains value
      // =================================================
      if (!string.IsNullOrWhiteSpace(m_contextId))
      {

        if (m_contextId == m_internal)
          m_login = "Internal";
        else
        {
          // Get the context xml ( If the context is invalid it will throw an exception )
          string contextXml = SecurityGateway.Get( m_contextId, "context", string.Format("<context id=\"{0}\"/>",m_contextId));

          if (!string.IsNullOrEmpty(contextXml))
          {
            // We need to parse out the login from the context xml
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(contextXml);

            XmlNode contextNode = doc.SelectSingleNode("collection/context");

            if ( contextNode == null )
              throw new System.IO.InvalidDataException("No context recevied"); 

            /// Sets the login
            if ( ! contextNode.TryGetAttribute<string>("login", ref m_login) )
              throw new System.IO.InvalidDataException("No login found in the context"); 

          }
          else
            throw new System.IO.InvalidDataException("Invalid context specified");
        }
      } // END ... If the context id is not NULL or blank

    } // SetContextId(string contextId)

    #endregion


    #region properties

    /// <summary>
    /// Application of request
    /// </summary>
    public string Application
    { get { return m_application; } }

    /// <summary>
    /// Source of request
    /// </summary>
    public string Source
    { 
      get { return m_source;  }
      set { m_source = value; }
    }

    /// <summary>
    /// Client of request
    /// </summary>
    public string Client
    { get { return m_client; } }

    /// <summary>
    /// Generate the Url 
    /// </summary>
    /// <returns></returns>
    public string Url
    {

      get 
      {
        StringBuilder url = new StringBuilder();

        url.Append(m_resource);
        url.Append("?");


        url.Append(string.Format("{0}={1}", UrlAttributes.ContextId, m_contextId));
        url.Append(string.Format("&{0}={1}", UrlAttributes.Source, m_source));
        url.Append(string.Format("&{0}={1}", UrlAttributes.Application, m_application));
        url.Append(string.Format("&{0}={1}", UrlAttributes.System, m_client));

        foreach (KeyValuePair<string, string> pair in m_parameters)
        {
          if ( pair.Key != Attributes.ContextId && pair.Key != Attributes.Version )
            url.Append(string.Format("&{0}={1}", pair.Key, pair.Value));
        }


        //url.Remove(url.Length - 1, 1);


        return url.ToString();
    
      }

    } // ToUrl()


    /// <summary>
    /// Gets the context Id
    /// </summary>
    public string ContextId
    { get { return m_contextId; } }
  
    /// <summary>
    /// Gets the operation
    /// </summary>
    public string Operation
    { get { return m_operation; } }

    /// <summary>
    /// Gets the resource the parameters are associated with
    /// </summary>
    public string Resource
    { get { return m_resource; } }

    /// <summary>
    /// Gets the users login is it is known
    /// </summary>
    public string Login
    { get { return m_login; } }

    /// <summary>
    /// Gets the payload
    /// </summary>
    public string Payload
    { get { return m_payload; } }

    /// <summary>
    /// Gets the parameters as a name/value pair
    /// </summary>
    public Dictionary<string,string> Parameters
    { get { return m_parameters; } }

    /// <summary>
    ///  Gets the encoding
    /// </summary>
    public Encoding Encoding
    { get { return m_encoding; } }

    #endregion


    #region methods

    #region Create methods


    /// <summary>
    /// Creates the request message using the payload specified
    /// </summary>
    /// <param name="contextId">Authentication token</param>
    /// <param name="op">Operation</param>
    /// <param name="resource">Resource</param>
    /// <param name="payload">Payload</param>
    /// <returns>RequestMessaqge</returns>
    static public RequestMessage Create(string contextId,  OperationTypes operationType, XmlBuilder builder)
    {
      // New request
      RequestMessage request = new RequestMessage();

      // Make sure no memory error
      if (request == null)
        throw new OutOfMemoryException("Failed to allocate Parameters object");

      request.m_contextId   = contextId;
      request.m_operation   = operationType.ToString();
      request.m_resource    = builder.Resource;
      request.m_parameters  = builder.Attributes;
      request.m_payload     = builder.ToXml();

       return request;

    } //  Create(string contextId, OperationTypes op, string resource, XmlBuilder builder)


    /// <summary>
    /// 
    /// </summary>
    /// <param name="stream">Stream</param>
    /// <param name="key"></param>
    /// <param name="encrypted"></param>
    /// <returns></returns>
    static public RequestMessage Create(Stream stream, byte[] key, bool encrypted)
    {


      using (StreamReader reader = new StreamReader(stream))
      {
        return Create(reader.ReadToEnd(), key, encrypted);
      }
      
    }



    /// <summary>
    /// 
    /// </summary>
    /// <param name="stream"></param>
    /// <returns></returns>
    static public RequestMessage Create(string requestMessage, byte[] key, bool encrypted)
    {

      RequestMessage request = null;
      string xml = string.Empty;

      try
      {

        // New request
        request = new RequestMessage();

        // Make sure no memory error
        if (request == null)
          throw new OutOfMemoryException("Failed to allocate Parameters object");

        // Make sure there was a message
        if (requestMessage.Length == 0)
          throw new System.IO.InvalidDataException("Missing request message");


        // Parse the xml
        XmlDocument doc = new XmlDocument();

        // Decrypt is we are flagged to be encrypted
        string decrypted = (encrypted ? Crypto.Decrypt(requestMessage, key) : requestMessage);

        // Decrypt if encrypted
        doc.LoadXml(decrypted);



#if EXE
        // log request message
        Log.Debug( "IN: " + doc.OuterXml, "RequestHandler", "Create" );
#endif

        // The request is wrapped with <rq />
        if (doc.DocumentElement.Name != "rq")
          throw new System.IO.InvalidDataException("Invalid request received");

        // Validate the schema for the request
        doc.DocumentElement.ValidateSchema(Schema.Settings);

        // Validates the requests payload
        doc.DocumentElement.FirstChild.ValidateSchema(Schema.Settings);

        // Look for operation
        if (! doc.DocumentElement.TryGetAttributeValue<string>(Attributes.Operation, ref request.m_operation)  )
          throw new MissingAttributeException("Operation attribute not specified in request");




        // Set the operation
        Enum.TryParse<OperationTypes>(request.m_operation, out request.m_opType);

        string contextId = string.Empty;

        // Look for context id
        if (! doc.DocumentElement.TryGetAttributeValue<string>(Attributes.ContextId, ref contextId)  )
          throw new MissingAttributeException("Context Id attribute not specified in request");

        request.SetContextId(contextId);

        // Look for source 
        if (! doc.DocumentElement.TryGetAttributeValue<string>(Attributes.Source, ref request.m_source)  )
          throw new MissingAttributeException("Source attribute not specified in request");

        // Look for application
        if (!doc.DocumentElement.TryGetAttributeValue<string>(Attributes.Application, ref request.m_application))
          throw new MissingAttributeException("Application attribute not specified in request");

        // Look for client system
        if (!doc.DocumentElement.TryGetAttributeValue<string>(Attributes.System, ref request.m_client))
          throw new MissingAttributeException("System attribute not specified in request");

        int encoding = 0;
        doc.DocumentElement.TryGetAttributeValue<int>(Attributes.Encoding, ref encoding);
        request.m_encoding = (Encoding)encoding;

        // The first child 
        request.m_resource = doc.DocumentElement.FirstChild.Name;

        /// Save the xml
        request.m_payload = doc.DocumentElement.InnerXml;

        // =============================================
        // Build the parameter list using the attributes
        // =============================================
        foreach (XmlAttribute att in doc.DocumentElement.FirstChild.Attributes)
        {
          request.m_parameters.Add(att.Name, att.Value);
        } // END ... For each attribute

        
        /// Validate the schema since POST and PUT will contain XML.
        request.ToXmlNode().ValidateSchema(Schema.Settings);

        return request;

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RequestMessage", "Create");

        throw;
      }


    } // Create(Stream stream)

 
    /// <summary>
    /// Creates the parameters object from the list supplied   
    ///       name=value,name=value,name=,name=
    /// </summary>
    /// <param name="resource">Resource name</param>
    /// <param name="operation">Operation being performed (either a "get" or "delete")</param>
    /// <param name="parameters">Parameter list</param>
    /// <param name="key">Certificate key</param>
    /// <param name="encrypted">Encryption flag</param>
    /// <returns>RequestMessage object</returns>
    static public RequestMessage Create(string resource, string operation, NameValueCollection parameters, byte[] key, bool encrypted)
    {

      // Null
      RequestMessage request = null;

      try
      {

        // List of ranges
        Dictionary<string, string> ranges = new Dictionary<string, string>();

 
        /// Create a new request message object
        request = new RequestMessage(resource, operation, string.Empty);

        // Make sure there is no memory issues
        if (request == null)
          throw new OutOfMemoryException("Failed to allocate Parameters object");

        StringBuilder payload = new StringBuilder();
        payload.Append(string.Format("<{0} ", resource));

        // =========================================================================================
        // Check if encryption is turned for the message level. If so then we need to decrypt the 
        // message prior to parsing.
        // ==========================================================================================
        if (encrypted)
        {

          // Decrypt message
          string message = Crypto.Decrypt(parameters.GetKey(0), key);


          // & separate the parameters
          char[] seperator = { '&' };
          string[] list = message.Split(seperator);


          char[] seperator2 = { '=' };


          // ===============================================
          // Loop through all the parameter pairs
          //  name=value
          // ==============================================
          foreach (string item in list)
          {

            string[] pair = item.Split(seperator2);

            if (pair[0] == UrlAttributes.ContextId)
              request.SetContextId(pair[1]);
            else if (pair[0] == UrlAttributes.Source)
              request.m_source = pair[1];
            else if (pair[0] == UrlAttributes.Application)
              request.m_application = pair[1];
            else if (pair[0] == UrlAttributes.System)
              request.m_client = pair[1];
            else
            {
              request.m_parameters.Add(pair[0], pair[1]);
              payload.Append(string.Format(" {0}=\"{1}\"", pair[0], pair[1]));

            }

          } // END ... For each parameter in the list



        }
        else
        {
          // =====================================
          // Loop through each of the parameters
          // ====================================
          foreach (string parameterKey in parameters.AllKeys)
          {

            // ========================================
            // Loop through all the parameter values
            // ========================================
            foreach (string parameterValue in parameters.GetValues(parameterKey))
            {

              // =================================================================================================
              // If context id set the id for the request which validates the id against the back-end resource
              // =================================================================================================
              if (parameterKey == UrlAttributes.ContextId)
                request.SetContextId(parameterValue);
              else if (parameterKey == UrlAttributes.Source)
                request.m_source = parameterValue;
              else if (parameterKey == UrlAttributes.Application)
                request.m_application = parameterValue;
              else if (parameterKey == UrlAttributes.System)
                request.m_client = parameterValue;
              else
              {
                if (parameterValue.StartsWith("RANGE:"))
                {
                  ranges.Add(parameterKey, parameterValue);

                }
                else
                {
                  request.m_parameters.Add(parameterKey, parameterValue);
                  payload.Append(string.Format(" {0}=\"{1}\"", parameterKey, parameterValue));
                }
              }
 
            } // END ... For each parameter value

          } // END ...For each parameter key

   


        }

        // ====================================
        // If there are ranges we add then now
        // ====================================
        if (ranges.Count > 0)
        {

          // End 
          payload.Append(">");
          Char[] del = { ',' };

          // ===========================================================
          // Loop through all the ranges and add the xml node elements
          // ===========================================================
          foreach (KeyValuePair<string, string> pair in ranges)
          {
            string[] fields = pair.Value.Substring(6).Split(del);
            payload.Append(string.Format("<{0}  from=\"{1}\" to=\"{2}\" />", pair.Key, fields[0], fields[1]));
          } // END ... For each range

          // 
          payload.Append(string.Format("</{0}>", resource));

        }
        else
          payload.Append("/>");

        request.m_payload = payload.ToString();
 
        // Validate the requests schema (Not done since this is either a GET or DELETE which I do not require a version field)
        // request.ToXmlNode().ValidateSchema(Schema.Settings);

        // return the request message
        return request;

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "RequestHandler", "Create");

        throw;
      }


    } // Create(string resource, NameValueCollection parameters, byte[] key, bool encrypted)

    #endregion

    /// <summary>
    /// Try and get the value specified
    /// </summary>
    /// <param name="name">Name of parameter</param>
    /// <param name="value">Value of the parameter</param>
    /// <returns>True if set, false otherwise</returns>
    public string GetValue(string name, string defaultValue)
    {
      string value;
      if ( m_parameters.TryGetValue(name, out value) )
        return value;
      else
        return defaultValue;
    } // TryGetValue(string name, out string  value)

      
    /// <summary>
    /// Try and get the value specified
    /// </summary>
    /// <param name="name">Name of parameter</param>
    /// <param name="value">Value of the parameter</param>
    /// <returns>True if set, false otherwise</returns>
    public bool TryGetValue(string name, out string  value)
    {
      return m_parameters.TryGetValue(name, out value);
    } // TryGetValue(string name, out string  value)

    /// <summary>
    /// Indexer
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public string this[string name]
    {
      get { return m_parameters[name]; }
    } // this[string name]

    /// <summary>
    /// Generate the XML version of the parameters
    /// </summary>
    /// <returns></returns>
    public string ToXml()
    {

      if (string.IsNullOrEmpty(m_xml))
      {
        StringBuilder xml = new StringBuilder();

        xml.Append("<rq ");
        xml.Append(string.Format("{0}=\"{1}\" ", Attributes.ContextId, m_contextId));
        xml.Append(string.Format("{0}=\"{1}\" ", Attributes.Source, m_source));
        xml.Append(string.Format("{0}=\"{1}\" ", Attributes.Application, m_application));
        xml.Append(string.Format("{0}=\"{1}\" ", Attributes.System, m_client));
        xml.Append(string.Format("{0}=\"{1}\" ", Attributes.Encoding, (Int32)m_encoding));
        xml.Append(m_operation.Length > 0 ? string.Format("{0}=\"{1}\" ", Attributes.Operation, m_operation) : "");
        xml.Append(string.Format("{0}=\"1.0\" >", Attributes.Version));

        xml.Append(this.Payload);

        xml.Append("</rq>");

        m_xml = xml.ToString();
      }

      return m_xml;
    } // ToXml()



    /// <summary>
    /// Converts a string into XmlNode
    /// </summary>
    /// <param name="xml">xml string</param>
    /// <returns>XmlNode</returns>
    private XmlNode ToXmlNode()
    {
      string xml = ToXml();

      XmlDocument doc = new XmlDocument();
      if (!string.IsNullOrWhiteSpace(xml))
        doc.LoadXml(xml);

      return doc.DocumentElement;
    } // ToXmlNode( )

    #endregion

  } // class Parameters

} // namespace Viatronix::Enterprise
