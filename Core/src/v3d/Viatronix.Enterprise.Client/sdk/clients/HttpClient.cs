using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using Viatronix.Enterprise.Logging;
using Viatronix.Enterprise.Messaging;

namespace Viatronix.Enterprise.SDK
{

  /// <summary>
  /// Implements a client to the HTTP Rest Service.
  /// 
  /// GET       -   Gets data 
  /// POST      -   Post (Update) data 
  /// PUT       -   Put (Insert) data
  /// DELETE    -   Delete data
  /// 
  /// Note:     Post() can be used to perform all operations. Also used to perform special oprations
  ///           such as AcquireContext(), Release() , ...
  /// </summary>
  public class HttpClient
  {

    #region fields

    /// <summary>
    /// Base URL
    /// </summary>
    private string m_baseUrl = string.Empty;

    /// <summary>
    /// Encrypt flag
    /// </summary>
    private bool? m_encrypt = null;

    /// <summary>
    /// Certificate
    /// </summary>
    private X509Certificate m_certificate = null;

    #endregion

    #region costructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="baseUrl"></param>
    public HttpClient(string baseUrl)
    {
      m_baseUrl = baseUrl;

      ServicePointManager.ServerCertificateValidationCallback += new RemoteCertificateValidationCallback(ValidateServerCertificate);

    } // Client(baseUrl)

    #endregion


    #region properties

    public X509Certificate Certificate
    {
      get { return m_certificate;  }
      set { m_certificate = value;  }
    }

    /// <summary>
    ///  Gets the URL
    /// </summary>
    public string Url
    { get { return m_baseUrl;  } }


    /// <summary>
    /// Gets the encrypt flag
    /// </summary>
    public bool Encrypt
    { 
      get 
      {
        if (m_encrypt == null)
        {
          string url = m_baseUrl + "/encrypt";

          WebRequest webRequest = HttpWebRequest.Create(url);
          webRequest.Method = "GET";
          webRequest.Proxy = null;
          webRequest.Credentials = CredentialCache.DefaultCredentials;

          WebResponse ws = webRequest.GetResponse();

          ResponseMessage response = ResponseMessage.Create(ws.GetResponseStream());

          m_encrypt = Boolean.Parse(response.Message);
        }

        return m_encrypt.Value;

      } 
    }

    /// <summary>
    /// Returns true if the REST service is available
    /// </summary>
    public bool IsAvailable
    {
      get
      {
        bool available = false;
        try
        {
          
          if ( Echo("ok") == "ok" )
            available = true;

        }
        catch (Exception)
        {
          available = false;
        }

        return available;

      }
    } // IsAvailable

    #endregion

    #region methods


    /// <summary>
    /// Echo's used to determine if the service is up
    /// </summary>
    /// <param name="message"></param>
    /// <returns></returns>
    public string Echo(string message)
    {
      string url = m_baseUrl + "/echo/" + message;

      WebRequest webRequest = HttpWebRequest.Create(url);
      webRequest.Method = "GET";
      webRequest.Proxy = null;
      webRequest.Credentials = CredentialCache.DefaultCredentials;

      WebResponse ws = webRequest.GetResponse();

      ResponseMessage response = ResponseMessage.Create(ws.GetResponseStream());

      return response.Message;
 
    } // Echo()

    /// <summary>
    /// Query for the resource defined in the request
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="builder">xml</param>
    /// <returns></returns>
    public ResponseMessage Get(string context, XmlBuilder builder)
    {
      ResponseMessage response = Get(RequestMessage.Create(context, OperationTypes.Get, builder));

      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);


      return response;
    } // Get(string context, XmlBuilder builder)

    /// <summary>
    /// Query for the resource defined in the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns></returns>
    public ResponseMessage Get(RequestMessage request)
    {

      DateTime start = DateTime.Now;

      try
      {
        string url = m_baseUrl + "/" + request.Url;

        WebRequest webRequest = HttpWebRequest.Create(url);
        webRequest.Method = "GET";
        webRequest.Proxy = null;
        webRequest.Credentials = CredentialCache.DefaultCredentials;

        WebResponse ws = webRequest.GetResponse();

        return ResponseMessage.Create(ws.GetResponseStream());
      }
      finally
      {
        Log.Timing(string.Format("Retrieval of ({0}) : {1}", request.Resource, DateTime.Now.Subtract(start)), "Client", "Get");
      }

    } // Query(RequestMessage request)

    /// <summary>
    /// Delete the specified resource
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="builder">xml</param>
    /// <returns></returns>
    public ResponseMessage Delete(string context, XmlBuilder builder)
    {
      ResponseMessage response = Delete(RequestMessage.Create(context, OperationTypes.Delete, builder));


      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);


      return response;

    } // Delete(string context, XmlBuilder builder)

    /// <summary>
    /// Delete the resource item defined in the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns></returns>
    public ResponseMessage Delete(RequestMessage request)
    {

      DateTime start = DateTime.Now;

      try
      {

        string url = m_baseUrl + "/" + request.Url;

        WebRequest webRequest = HttpWebRequest.Create(url);
        webRequest.Method = "DELETE";
        webRequest.Proxy = null;
        webRequest.Credentials = CredentialCache.DefaultCredentials;

        WebResponse ws = webRequest.GetResponse();

        return ResponseMessage.Create(ws.GetResponseStream());
      }
      finally
      {
        Log.Timing(string.Format("Delete of ({0}) : {1}", request.Resource, DateTime.Now.Subtract(start)), "Client", "Get");
      }

    } // Delete(RequestMessage request)


    /// <summary>
    /// Puts/Inserts the specified resource
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="builder">xml</param>
    /// <returns></returns>
    public ResponseMessage Insert(string context, XmlBuilder builder)
    {
      ResponseMessage response = Put(RequestMessage.Create(context, OperationTypes.Insert, builder));


      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);


      return response;

    } // Put(string context, XmlBuilder builder)

    /// <summary>
    /// Delete the resource item defined in the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns></returns>
    public ResponseMessage Put(RequestMessage request)
    {
      DateTime start = DateTime.Now;

      try
      {

        WebRequest webRequest = HttpWebRequest.Create(m_baseUrl);
        webRequest.Method = "PUT";
        webRequest.Proxy = null;
        webRequest.Credentials = CredentialCache.DefaultCredentials;

        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
        Byte[] bytes = encoding.GetBytes(request.ToXml());


        // Write the request
        webRequest.GetRequestStream().Write(bytes, 0, bytes.Length);

        WebResponse ws = webRequest.GetResponse();

        return ResponseMessage.Create(ws.GetResponseStream());
      }
      finally
      {
        Log.Timing(string.Format("Put of ({0}) : {1}", request.Resource, DateTime.Now.Subtract(start)), "Client", "Get");
      }

    } // Put(RequestMessage request)


    /// <summary>
    /// Posts/Updates the specified resource
    /// </summary>
    /// <param name="context">Context id</param>
    /// <param name="builder">xml</param>
    /// <returns></returns>
    public ResponseMessage Update(string context, XmlBuilder builder)
    {
      ResponseMessage response = Post(RequestMessage.Create(context, OperationTypes.Update, builder));


      if (response.Failed)
        throw ExceptionFactory.Create(response.Message, response.ErrorCode);


      return response;

    } // Post(string context, XmlBuilder builder)

    /// <summary>
    /// Delete the resource item defined in the request
    /// </summary>
    /// <param name="request">Request</param>
    /// <returns></returns>
    public ResponseMessage Post(RequestMessage request)
    {
      DateTime start = DateTime.Now;

      try
      {


        WebRequest webRequest = HttpWebRequest.Create(m_baseUrl);
        webRequest.Method = "POST";
        webRequest.Proxy = null;
        webRequest.Credentials = CredentialCache.DefaultCredentials;


        string xml = request.ToXml();

        System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
        Byte[] bytes = encoding.GetBytes(xml);

        // Write the request
        webRequest.GetRequestStream().Write(bytes, 0, bytes.Length);

        WebResponse ws = webRequest.GetResponse();

        return ResponseMessage.Create(ws.GetResponseStream());
      }
      finally
      {
        Log.Timing(string.Format("Put of ({0}) : {1}", request.Resource, DateTime.Now.Subtract(start)), "Client", "Get");
      }

    } // Update(RequestMessage request)

    /// <summary>
    /// Validates the servers certificate
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="certificate"></param>
    /// <param name="chain"></param>
    /// <param name="sslPolicyErrors"></param>
    /// <returns></returns>
    private static bool ValidateServerCertificate(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
    {
      return true;
    } // ValidateServerCertificate(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)

    #endregion


  } // class Client

} // namespace Viatronix.Enterprise.SDK
