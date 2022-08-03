// 

// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: CorsSupportBehavior.cs
//
// Description: CORS Behavior implementation
//
// Cross-origin resource sharing (CORS) is a mechanism that allows JavaScript on a web page to make XMLHttpRequests to another domain, 
// not the domain the JavaScript originated from. Such "cross-domain" requests would otherwise be forbidden by web browsers, per the 
// same origin security policy. CORS defines a way in which the browser and the server can interact to determine whether or not to allow 
// the cross-origin request.It is more useful than only allowing same-origin requests, but it is more secure than simply allowing all 
// such cross-origin requests.
//
// Cross-Origin Resource Sharing documentation:  http://www.w3.org/TR/cors/
//
//
// Owner: Mark Kontak  (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Web;
using System.ServiceModel.Description;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.Net;
using System.ServiceModel.Configuration;

namespace Viatronix.WebHttpCors
{
  

#region class CustomOperationInvoker

  /// <summary>
  /// Implements an operation invoker for the CORS endpoint
  /// </summary>
  public class CustomOperationInvoker : IOperationInvoker
  {


    /// <summary>
    /// Inner operation invoker
    /// </summary>
    IOperationInvoker m_innerInvoker = null;


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="innerInvoker">Invoker</param>
    public CustomOperationInvoker(IOperationInvoker innerInvoker)
    {
      m_innerInvoker = innerInvoker;
    }

    /// <summary>
    /// Rerurns the array of allocate inputs
    /// </summary>
    /// <returns>Array of objects</returns>
    public object[] AllocateInputs()
    {
      return m_innerInvoker.AllocateInputs();
    } // AllocateInputs()


    /// <summary>
    /// Invokes the operation
    /// </summary>
    /// <param name="instance"></param>
    /// <param name="inputs"></param>
    /// <param name="outputs"></param>
    /// <returns></returns>
    public object Invoke(object instance, object[] inputs, out object[] outputs)
    {

      // ==============================================================================
      // Check if the unhandled request is due to preflight checks (OPTIONS header)
      // ==============================================================================
      if (OperationContext.Current.Extensions.Find<PreflightDetected>() != null)
      {
        // Override the standard error handling, so the request won't contain an error
        outputs = null;
        return null;
      }
      else
      {
        // No preflight - probably a missed call (wrong URI or method)
        return m_innerInvoker.Invoke(instance, inputs, out outputs);
      }

    } // Invoke(object instance, object[] inputs, out object[] outputs)

    /// <summary>
    /// Asynchronous begin invoke of operation
    /// 
    ///   Not supported - an exception will be thrown
    ///   
    /// </summary>
    /// <param name="instance"></param>
    /// <param name="inputs"></param>
    /// <param name="callback"></param>
    /// <param name="state"></param>
    /// <returns></returns>
    public IAsyncResult InvokeBegin(object instance, object[] inputs, AsyncCallback callback, object state)
    { return m_innerInvoker.InvokeBegin(instance, inputs, callback, state); }

    /// <summary>
    /// Asynchronous end invoke of opertaion 
    /// 
    ///   Not supported - an exception will be thrown
    ///   
    /// </summary>
    /// <param name="instance"></param>
    /// <param name="outputs"></param>
    /// <param name="result"></param>
    /// <returns></returns>
    public object InvokeEnd(object instance, out object[] outputs, IAsyncResult result)
    {  return m_innerInvoker.InvokeEnd(instance, out outputs, result); }

    /// <summary>
    /// Returns true of the operation is synchronous
    /// </summary>
    public bool IsSynchronous
    {
      get {  return m_innerInvoker.IsSynchronous; }
    }

  } // class CustomOperationInvoker

#endregion


#region class PreflightDetected

  /// <summary>
  /// Implements a operation context for handling the CORS response
  /// </summary>
  public class PreflightDetected : IExtension<OperationContext>
  {

    /// <summary>
    /// Request headers
    /// </summary>
    string m_requestedHeaders = null;

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="requestedHeaders">Request header</param>
    public PreflightDetected(string requestedHeaders)
    {
      RequestedHeaders = requestedHeaders;
    }

    /// <summary>
    /// Gets/Sets the request headers
    /// </summary>
    public string RequestedHeaders
    {
      get {   return m_requestedHeaders ?? string.Empty;  }
      set { m_requestedHeaders = value;  }
    }

    /// <summary>
    /// Attach to ownser
    /// </summary>
    /// <param name="owner"></param>
    public void Attach(OperationContext owner)
    {  /* Not required */ }

    /// <summary>
    /// Detach from owner
    /// </summary>
    /// <param name="owner"></param>
    public void Detach(OperationContext owner)
    { /* Not required */ }

  } // class PreflightDetected

#endregion


#region class CorsMessageInspector

  /// <summary>
  /// Implements a message inspector
  /// </summary>
  public class CorsMessageInspector : IDispatchMessageInspector
  {

    /// <summary>
    /// Handles the after receive request event
    /// </summary>
    /// <param name="request">Request message</param>
    /// <param name="channel">Client channel</param>
    /// <param name="instanceContext">Instance context</param>
    /// <returns></returns>
    public object AfterReceiveRequest(ref Message request, IClientChannel channel, InstanceContext instanceContext)
    {

      HttpRequestMessageProperty httpRequest = request.Properties["httpRequest"] as HttpRequestMessageProperty;

      // ==============================
      // Make sure there is a request
      // ==============================
      if (httpRequest != null)
      {

        // ===========================
        // Check for the OPTIONS verb
        // ===========================
        if (httpRequest.Method == "OPTIONS")
        {
          // Store the requested headers as a PreFlightDetected object
          OperationContext.Current.Extensions.Add(new PreflightDetected(httpRequest.Headers["Access-Control-Request-Headers"]));
        } // END ... If the verb is OPTIONS
          
      } // END ... If there is a request

      return null;

    } // AfterReceiveRequest(ref Message request, IClientChannel channel, InstanceContext instanceContext)

    /// <summary>
    /// Handles the before send reply event
    /// </summary>
    /// <param name="reply">Reply message</param>
    /// <param name="correlationState"></param>
    public void BeforeSendReply(ref Message reply, object correlationState)
    {

		  HttpResponseMessageProperty property = null;

      // ======================================================
      // If the reply is NULL we will set ip the reply message
      // ======================================================
		  if (reply == null)
		  {
        reply = Message.CreateMessage(MessageVersion.None, null);
			  property = new HttpResponseMessageProperty();
			  reply.Properties[HttpResponseMessageProperty.Name] = property;
			  property.StatusCode = HttpStatusCode.OK;
		  }
		  else
		  {
			  property = reply.Properties[HttpResponseMessageProperty.Name] as HttpResponseMessageProperty;
		  }

      // Create a prefilght detected request
		  PreflightDetected preflightRequest = OperationContext.Current.Extensions.Find<PreflightDetected>();
		  if (preflightRequest != null)
		  {
			  // Add allow HTTP headers to respond to the preflight request
			  if (preflightRequest.RequestedHeaders == string.Empty)
				  property.Headers.Add("Access-Control-Allow-Headers", "Accept");
			  else
				  property.Headers.Add("Access-Control-Allow-Headers", preflightRequest.RequestedHeaders + ", Accept");

			  property.Headers.Add("Access-Control-Allow-Methods", "*");

		  }

		  // Add allow-origin header to each response message, because client expects it
		  property.Headers.Add("Access-Control-Allow-Origin", "*");
    } // BeforeSendReply(ref Message reply, object correlationState)

  } // class CorsMessageInspector

#endregion


#region class CorsSupportBehavior

  /// <summary>
  /// CORS Support Behavior endpoint
  /// </summary>
  public class CorsSupportBehavior : IEndpointBehavior
  {

    /// <summary>
    /// Adds a binding parametert to the endpoint
    /// </summary>
    /// <param name="endpoint">Service endpoint</param>
    /// <param name="bindingParameters">Binding partameters collection</param>
    public void AddBindingParameters(ServiceEndpoint endpoint, System.ServiceModel.Channels.BindingParameterCollection bindingParameters)
    { }

    /// <summary>
    /// Added a client behavior implementation to the endpoint
    /// </summary>
    /// <param name="endpoint">Service endpoint</param>
    /// <param name="clientRuntime">Client runtime</param>
    public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
    { }

    /// <summary>
    /// Apply the dispath behavior
    /// </summary>
    /// <param name="endpoint">Service endpoint</param>
    /// <param name="endpointDispatcher">Endpoint dispatcher</param>
    public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
    {
      // Register a message inspector, and an operation invoker for undhandled operations
      endpointDispatcher.DispatchRuntime.MessageInspectors.Add(new CorsMessageInspector());

      IOperationInvoker invoker = endpointDispatcher.DispatchRuntime.UnhandledDispatchOperation.Invoker;

      endpointDispatcher.DispatchRuntime.UnhandledDispatchOperation.Invoker =  new CustomOperationInvoker(invoker);

    } // ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)

    /// <summary>
    /// Validates the endpoint
    /// </summary>
    /// <param name="endpoint">Service endpoint</param>
    public void Validate(ServiceEndpoint endpoint)
    {
      // Make sure that the behavior is applied to an endpoing with WebHttp binding
      if (!(endpoint.Binding is WebHttpBinding))
          throw new InvalidOperationException("The CorsSupport behavior can only be used in WebHttpBinding endpoints");
    } // Validate(ServiceEndpoint endpoint)

  } // class CorsSupportBehavior

#endregion


#region class CorsSupportBehaviorElement


  /// <summary>
  /// CORS behavior element 
  /// </summary>
  public class CorsSupportBehaviorElement : BehaviorExtensionElement
  {

    /// <summary>
    /// Gets the behaior type
    /// </summary>
    public override Type BehaviorType
    {
       get { return typeof(CorsSupportBehavior); }
    }

    /// <summary>
    /// Creates the behavior object
    /// </summary>
    /// <returns></returns>
    protected override object CreateBehavior()
    {
      return new CorsSupportBehavior();
    }
  } // class CorsSupportBehaviorElement

#endregion


} // namespace Viatronix.WebHttpCors