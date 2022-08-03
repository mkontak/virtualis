// $Id: LaunchException.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;


namespace Viatronix.v3D.Console
{

  #region enum LaunchStage

  /// <summary>
  /// Enumeration of all launch stages
  /// </summary>
  public enum LaunchStage
  {
    None,
    InitializeLaunch,
    StartProcess,
    InitializeCommunication,
    DisplayViewer,
    StartViewer  
  } // enum LaunchStage

  #endregion

  /// <summary>
  /// The exception that is thrown when a launch error occurs.
  /// </summary>
  public class LaunchException : System.ApplicationException
  {

    #region fields

    /// <summary>
    /// The launch stage that generated the exception.
    /// </summary>
    LaunchStage m_stage = LaunchStage.None;

    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a LaunchException.
    /// </summary>
    /// <param name="stage">The stage that generated the exception.</param>
    public LaunchException( LaunchStage stage )
    {
      m_stage = stage;
    } // LaunchException( stage )


    /// <summary>
    /// Creates a new instance of a LaunchException.
    /// </summary>
    /// <param name="stage">The stage that generated the exception.</param>
    /// <param name="message">Additional explanation.</param>
    public LaunchException( LaunchStage stage, string message ) : base( message )
    {
      m_stage = stage;
    } // LaunchException( stage, message )


    /// <summary>
    /// Creates a new instance of a LaunchException.
    /// </summary>
    /// <param name="stage">The stage that generated the exception.</param>
    /// <param name="message">Additional explanation.</param>
    /// <param name="innerException">An inner exception</param>
    public LaunchException( LaunchStage stage, string message, System.Exception innerException ) : base( message, innerException )
    {
      m_stage = stage;
    } // LaunchException( stage, message, innerException )

    #endregion

    #region properties

    /// <summary>
    /// Returns the launch stage.
    /// </summary>
    public LaunchStage LaunchStage
    {
      get { return m_stage; }
    } // LaunchStage

    #endregion

  } // class LaunchException
} // namespace Viatronix.v3D.Core

#region revision history

// $Log: LaunchException.cs,v $
// Revision 1.2  2007/03/06 19:15:00  gdavidson
// Added class comments
//
// Revision 1.1  2006/10/30 15:36:08  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/launching/LaunchException.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $
// $Id: LaunchException.cs,v 1.2 2007/03/06 19:15:00 gdavidson Exp $

#endregion
