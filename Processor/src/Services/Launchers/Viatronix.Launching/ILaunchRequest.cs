// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  ILaunchRequest.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Launching
{


  /// <summary>
  /// Interface definition for a launch request
  /// </summary>
  public interface ILaunchRequest
  {


    /// <summary>
    /// Application code
    /// </summary>
    String Application
    { get; }

    /// <summary>
    /// Uid list
    /// </summary>
    String Uids
    { get; }

    /// <summary>
    /// User
    /// </summary>
    String UserName
    { get; }

    /// <summary>
    /// Users alias
    /// </summary>
    String Alias
    { get; }

    /// <summary>
    /// Password 
    /// </summary>
    String Password
    { get; }

    /// <summary>
    /// Context Id
    /// </summary>
    String ContextId
    { get;  }

    /// <summary>
    /// Session description
    /// </summary>
    String Description
    { get; }

    /// <summary>
    /// Windows session user to launch on
    /// </summary>
    String WindowsSessionUser
    { get; }

    /// <summary>
    /// New session flag
    /// </summary>
    Boolean NewSession
    { get; }

    /// <summary>
    /// Actual parameter list
    /// </summary>
    Dictionary<string, string> Parameters
    { get; }

    /// <summary>
    /// Returns true if the username and password contains data.
    /// </summary>
    bool HasAuthenticationInformation
    { get; }
  }
}
