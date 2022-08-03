// Copyright © 2014, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File:  ILaunch.cs
//
// Description: 
//
// Owner: (mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Viatronix.Launching
{

  /// <summary>
  /// Interface definition for a launcher.
  /// </summary>
  public interface ILauncher
  {

    /// <summary>
    /// Launches the viewer specified in the application with the parameters
    /// </summary>
    /// <param name="request">Launch request</param>
    /// <param name="user">User to launch session on</param>
    /// <param name="section">Specifications section for launching</param>
    /// <returns>Viewer object with that was started</returns>
    Session Launch( ILaunchRequest request, Viatronix.Launching.Configuration.SpecificationsSection section,  WindowsSession windowsSession );

    /// <summary>
    /// Closes the viewer
    /// </summary>
    void CloseSession(Session session);

    /// <summary>
    /// Gets the launcher version
    /// </summary>
    string Version
    { get; }

 
  }
}
