// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Service.cs
//
// Description: Implements the HTTP Rest service.
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;
using System.ServiceModel;
using System.ServiceModel.Web;
using System.Text;
using System.Xml;
using System.IO;
using System.Reflection;
using System.Configuration;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise;
using Viatronix.Encryption;
using Viatronix.Enterprise.Providers;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Messaging;
using Viatronix.Enterprise.Configuration;


namespace Viatronix.Services
{

  /// <summary>
  /// Implements the HTTP service
  /// </summary>
  public class RestService : IRestService, IFileRestService
  {

    #region fields

    /// <summary>
    /// Encryption key
    /// </summary>
    private static byte[] m_key = new byte[256];

    /// <summary>
    /// Encrypted flag
    /// </summary>
    private static bool m_encrypt = (ConfigurationManager.AppSettings["encrypt"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["encrypt"]) : false);

    /// <summary>
    /// Authorize flag
    /// </summary>
    private static bool m_authorize = (ConfigurationManager.AppSettings["authorize"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["authorize"]) : true);

    /// <summary>
    /// Transaction logging flag
    /// </summary>
    private static bool m_transactionLogging = (ConfigurationManager.AppSettings["translog"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["translog"]) : true);


    /// <summary>
    /// The File Server Must be maintained
    /// </summary>
    private static IFileServerProvider m_fileServerProvider = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor
    /// </summary>
    static RestService()
    {
      /// Read the key from the manifest
      using (BinaryReader reader = new BinaryReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.Services.res.webservice.data")))
        reader.Read(m_key, 0, m_key.Length);
    } // Service()


     #endregion



    /// <summary>
    /// Processes a POST operation for the File Server integrated server
    /// </summary>
    /// <param name="resource"></param>
    /// <returns></returns>
    public Stream PostFileServer(Stream body)
    { return ProcessFileServerRequest(body); }

    /// <summary>
    /// Processes the HTTP GET operation
    /// </summary>
    /// <param name="resource">Resource to be retrieved</param>
    /// <returns>XML results</returns>
    public string Get(string resource)
    { return ProcessRequest(resource, "get"); }


    /// <summary>
    /// Process the HTTP DELETE operation
    /// </summary>
    /// <param name="resource">Resource to delete</param>
    /// <returns>Nothing or failure message</returns>
    public string Delete(string resource)
    { return ProcessRequest(resource, "delete"); }



    /// <summary>
    /// Processes the HTTP POST operation 
    /// </summary>
    /// <param name="body">Body contains the request and resource XML</param>
    /// <returns>Nothing or failure message</returns>
    public string Post(Stream body)
    { return ProcessRequest(body); }



    /// <summary>
    /// Processes the HTTP PUT operation
    /// </summary>
    /// <param name="body">Body contains the request and resource XML</param>
    /// <returns>Nothing or failure message</returns>
    public string Put(Stream body)
    { return ProcessRequest(body); }


    #region Processing Methods

    /// <summary>
    /// Handles ECCHO command which is like a PING. Will respond back to the calling entity with the message sent.
    /// </summary>
    /// <returns>Response XML</returns>
    public string Echo(string message)
    {
      return ResponseMessage.Create(MessageFormat.Text, message).ToXml();
    } // Echo(string message)

    /// <summary>
    /// Returns the flag indicating if the service is handling encrypted text
    /// </summary>
    /// <returns>Response XML</returns>
    public string Encrypt()
    {
      return ResponseMessage.Create(MessageFormat.Text, m_encrypt.ToString()).ToXml();
    } // Encrypt()


 


     /// <summary>
    /// Process the HTTP GET/DELETE REQUEST
    /// </summary>
    /// <param name="resource">Resource</param>
    /// <param name="operation">Operation to be performed</param>
    /// <returns>Returns the XML result data or error</returns>
    public string ProcessRequest(string resource, string operation)
    {

      ResponseMessage response = null;
      string requestXml = string.Empty;
      string responseXml = string.Empty;

      try
      {


        // Create the request
        RequestMessage request = RequestMessage.Create(resource, operation, WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters, m_key, m_encrypt);


        // Gets the request XML (Used for transactional logging)
        requestXml = request.ToXml();

        // Find the provider
        IResourceProvider provider = ResourceProviderFactory.Create(request);

        // Check authorization flag and make sure the login is specified
        if (m_authorize && string.IsNullOrWhiteSpace(request.Login))
          throw new ApplicationException("Not authorized.");

        // Processes the request
        response = provider.ProcessRequest(request);

      }
      catch (NotSupportedException e)
      {

        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        response = ResponseMessage.Create(MessageFormat.Text, string.Format("{0}({1}) is not supported", operation.ToUpper(), resource), (int)ErrorCodes.Unsupported);

      }
      catch (ViatronixException e)
      {
        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        response = ResponseMessage.Create(MessageFormat.Text, e.Message, (int)e.ErrorCode);

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        response = ResponseMessage.Create(MessageFormat.Text, e.Message, -1);
      }

      try
      {
        // Set the content type
        WebOperationContext.Current.OutgoingResponse.ContentType = "text/xml";

        // Convert response to XML
        responseXml = response.ToXml();


        // Return either clear text or encrypted text
        return (m_encrypt ? Crypto.Encrypt(responseXml, m_key) : responseXml);
      }
      finally
      {

        // ======================================================
        // Only log the transactions if the flag is set to true,
        // we log the entries together so they can be matched up
        // ======================================================
        if (m_transactionLogging)
        {
          Log.Information("v3dentsvc_trans", "IN: " + requestXml, "EnterpriseService", "ProcessRequest");
          Log.Information("v3dentsvc_trans", "OUT: " + response.ToLoggingXml(), "EnterpriseService", "ProcessRequest");
        }

      }


    } // ProcessRequest(string resource, string operation)



    /// <summary>
    /// Processes the HTTP PUT/POST requests
    /// </summary>
    /// <param name="stream">XML stream</param>
    /// <returns>XML Results or failure message</returns>
    private string ProcessRequest(Stream stream)
    {

      ResponseMessage response = null;
      string requestXml = string.Empty;
      string responseXml = string.Empty;

      try
      {

        // Creates the request from the stream
        RequestMessage request = RequestMessage.Create(stream, m_key, m_encrypt);

        /// Sets the request text
        requestXml = request.ToXml();

        // Create the resource provider for the resource specified
        IResourceProvider provider = ResourceProviderFactory.Create(request);

        // Determine of authorization is being used then make sure the login was specified
        if (m_authorize && string.IsNullOrWhiteSpace(request.Login))
          throw new ApplicationException("Not authorized.");

        // Process the request
        response = provider.ProcessRequest(request);

      }
      catch (ViatronixException e)
      {
        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        response = ResponseMessage.Create(MessageFormat.Text, e.Message, (int)e.ErrorCode);

      }
      catch (Exception e)
      {
        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        response = ResponseMessage.Create(MessageFormat.Text, e.Message, -1);
      }

      responseXml = response.ToXml();

      // ======================================================
      // Only log the transactions if the flag is set to true,
      // we log the entries together so they can be matched up
      // ======================================================
      if (m_transactionLogging)
      {
        Log.Information("v3dentsvc_trans", "IN: " + requestXml, "EnterpriseService", "ProcessRequest");
        Log.Information("v3dentsvc_trans", "OUT: " + responseXml, "EnterpriseService", "ProcessRequest");
      }

      // Encrypt the response if necessary
      return (m_encrypt ? Crypto.Encrypt(responseXml, m_key) : responseXml);
      
     } // ProcessFileServerRequest(Stream stream)

    /// <summary>
    /// Process the HTTP FILE SERVER request
    /// </summary>
    /// <param name="stream">XML request stream</param>
    /// <returns>Streamed response</returns>
    private Stream ProcessFileServerRequest(Stream stream)
    {

      DateTime start = DateTime.Now;

      try
      {

        long size = WebOperationContext.Current.IncomingRequest.ContentLength;
        byte[] buffer = new byte[size];
        stream.Read(buffer, 0, (int)size);

        // =====================================================================
        // Creates the file server provider if it does not already exist
        // =====================================================================
        if (m_fileServerProvider == null)
        {

          // Get the resource provider section from the config file 
          ResourceProviderSection section = ResourceProviderSection.GetSection();

          // Create the resource provider for the resource specified
          ResourceProviderElement element = section.Providers["fileserver"];

          if (element.Type == null)
            throw new InvalidDataException("fileserver provider type is invalid");

          // Create the file server provider instance
          m_fileServerProvider = (IFileServerProvider)Activator.CreateInstance(element.Type);

        } // END ... If the file server providcer instance does not already exists


        // Returns the memory stream holding the results of the file server operation
        return new MemoryStream(m_fileServerProvider.ProcessRequest(buffer));


      }
      catch (Exception e)
      {
        Log.Error(e.Message, "EnterpriseService", "ProcessRequest");
        return new MemoryStream(ASCIIEncoding.ASCII.GetBytes(ResponseMessage.Create(MessageFormat.Text, e.Message, -1).ToXml()));
      }
      finally
      {
        Log.Timing(String.Format("File server request took {0}ms", DateTime.Now.Subtract(start).TotalMilliseconds), "Service", "ProcessFileServerRequest");
      }


    } // ProcessFileServerRequest(Stream stream)


    #endregion

  } //

}


