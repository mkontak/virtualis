// Copyright© 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  LaunchRequest.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Specialized;
using Viatronix.Launching;

namespace Viatronix.Services.Messages
{

  class LaunchRequest : ILaunchRequest
  {

    #region Attributes
    /// <summary>
    /// These are the attributes used when a GET is performed (URL). The Alais is used for the user coming in without
    /// a password if that occurs then then the use/password combo is extractec from the server
    /// </summary>
    public class Attributes
    {
      public const string Application         = "app";    // Viewer to be launched ( VC, CAR, XP, ... )
      public const string Uids                = "uids";   // List of UIDS to launch
      public const string User                = "user";   // User|Pwd|Context
      public const string ContextId           = "cid";    // Context Id
      public const string Password            = "pwd";    // User password
      public const string Description         = "desc";   // New session description
      public const string NewSession          = "new";   // flag indicating a new session is to be launched
      public const string Alias               = "alias"; // Not part of URL   
      public const string WindowsSessionUser  = "wsu";   // Windows Session User
    };  // class Attributes

    #endregion


    #region fields

    /// <summary>
    /// Parameters
    /// </summary>
    Dictionary<string, string> m_parameters = new Dictionary<string, string>();

    #endregion


    #region constructors

    protected LaunchRequest() { }
 
    #endregion


    #region methods


    /// <summary>
    /// Creates the parameters object from the list supplied
    ///   name=value,name=value,name=,name=
    /// </summary>
    /// <param name="resource">Resource name</param>
    /// <param name="parameterList">Parameter list</param>
    /// <returns>Parameters object</returns>
    static public LaunchRequest Create(NameValueCollection parameters)
    {

      // Null
      LaunchRequest request = null;

      try
      {

        Viatronix.Logging.Log.Debug("Creating LaunchRequest", "LaunchRequest", "Create");

        if (parameters == null )
          throw new LaunchException("No parameters received form request!");

        /// Create a new request message object
        request = new LaunchRequest();

        // Make sure there is no memory issues
        if (request == null)
          throw new OutOfMemoryException("Failed to allocate launch request");

        // =====================================
        // Loop through each of the parameters
        // ====================================
        foreach (string parameterKey in parameters.AllKeys)
        {

          string [] values = parameters.GetValues(parameterKey);

          if ( values == null )
            throw new Viatronix.Launching.InvalidDataException(string.Format("Parameter {0} has no values, invalid url!", parameterKey));

          // ========================================
          // Loop through all the parameter values
          // ========================================
          foreach (string parameterValue in parameters.GetValues(parameterKey))
          {


            if (request.m_parameters.ContainsKey(parameterKey))
              throw new Viatronix.Launching.InvalidDataException(string.Format("Parameter {0} specified more then once!", parameterKey));

            request.m_parameters.Add(parameterKey, parameterValue);
          } // END ... For each parameter value

        } // END ...For each parameter key



        if (!request.m_parameters.ContainsKey(Attributes.Application))
          throw new MissingFieldException("No Viewer application specified!");

        if (!request.m_parameters.ContainsKey(Attributes.Uids))
          throw new MissingFieldException("No UID(s) specified");

        //if (!request.m_parameters.ContainsKey(Attributes.User))
        //  throw new MissingFieldException("No User specified!");

        // If the user was specified add as an alias
        if ( request.m_parameters.ContainsKey(Attributes.User))
          request.m_parameters.Add(Attributes.Alias, request.m_parameters[Attributes.User]);


        // return the request message
        return request;

      }
      catch (Exception e)
      {
        Logging.Log.Error(e.Message, "LaunchRequest", "Create");

        throw;
      }


    } // Create(string resource, NameValueCollection parameters, byte[] key, bool encrypted)

    #endregion


    #region properties

    /// <summary>
    /// Gets the Application id of the viewer to be launched
    /// </summary>
    public String Application
    { get { return ( m_parameters.ContainsKey(Attributes.Application) ? m_parameters[Attributes.Application].ToUpper() : string.Empty );  } }

    /// <summary>
    /// Gets the Windows Session User for the application to be launched on
    /// </summary>
    public String WindowsSessionUser
    { get { return (m_parameters.ContainsKey(Attributes.WindowsSessionUser) ? m_parameters[Attributes.WindowsSessionUser] : string.Empty); } }


    /// <summary>
    /// Gets the ;ist of uids to be launched
    /// </summary>
    public String Uids
    { get { return (m_parameters.ContainsKey(Attributes.Uids) ? m_parameters[Attributes.Uids] : string.Empty); } }

    /// <summary>
    /// Gets the user alias
    /// </summary>
    public String Alias
    { get { return (m_parameters.ContainsKey(Attributes.Alias) ? m_parameters[Attributes.Alias] : string.Empty); } }


    /// <summary>
    /// Gets the user 
    /// </summary>
    public String UserName
    {
      get { return (m_parameters.ContainsKey(Attributes.User) ? m_parameters[Attributes.User] : string.Empty); }
      set 
      {  
        if (m_parameters.ContainsKey(Attributes.User) ) 
           m_parameters[Attributes.User] = value;
        else
          m_parameters.Add(Attributes.User, value);
      } 

    }

    /// <summary>
    /// Gets the Password 
    /// </summary>
    public String Password
    { 
      get { return (m_parameters.ContainsKey(Attributes.Password) ? m_parameters[Attributes.Password] : string.Empty); }
      set
      {
        if (m_parameters.ContainsKey(Attributes.Password))
          m_parameters[Attributes.Password] = value;
        else
          m_parameters.Add(Attributes.Password, value);
      } 


    }

    /// <summary>
    /// Gets/Sets the Context id 
    /// </summary>
    public String ContextId
    { 
      get { return (m_parameters.ContainsKey(Attributes.ContextId) ? m_parameters[Attributes.ContextId] : string.Empty); }
      set { m_parameters[Attributes.ContextId] = value; }
    }

    /// <summary>
    /// Gets the Description
    /// </summary>
    public String Description
    { get { return (m_parameters.ContainsKey(Attributes.Description) ? m_parameters[Attributes.Description] : string.Empty); } }

    public Boolean NewSession
    { 
      get 
      {
        bool newSession = false;

        if ( m_parameters.ContainsKey(Attributes.NewSession) )
        {
          if ( m_parameters[Attributes.NewSession] == "1" )
            newSession = true;
          else if ( string.Compare(m_parameters[Attributes.NewSession],"true", false) == 0)
            newSession = true;
        }

        return newSession;
      }
    }

    public Dictionary<string,string>  Parameters
    {  get { return m_parameters;  } }


    /// <summary>
    /// If true indicates that there is a  username/password or context id
    /// </summary>
    public bool HasAuthenticationInformation
    {
      get
      {
        return (  ( ! string.IsNullOrEmpty(UserName) && ! string.IsNullOrEmpty(Password)  ) || ! string.IsNullOrEmpty(ContextId) );
      }
    } // HasAuthenticationInformation

    #endregion


  }
}
