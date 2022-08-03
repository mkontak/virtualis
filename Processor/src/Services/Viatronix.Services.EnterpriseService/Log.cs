// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix
{
  internal static class Log
  {

    #region fields

    /// <summary>
    /// source
    /// </summary>
    private const string Source = "v3dentsvc";

    #endregion

    #region methods

    /// <summary>
    /// Logs a critical message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Critical( string message, string className, string methodName )
    {
      Viatronix.Enterprise.Logging.Log.Critical( Source, message, className, methodName );
    } // Critical( message, className, methodName )


    /// <summary>
    /// Logs a error message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Error( string message, string className, string methodName )
    {
      Viatronix.Enterprise.Logging.Log.Error( Source, message, className, methodName );
    } // Error( message, className, methodName )


    /// <summary>
    /// Logs a warning message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Warning( string message, string className, string methodName )
    {
      Viatronix.Enterprise.Logging.Log.Warning( Source, message, className, methodName );
    } // Warning( message, className, methodName )


    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Debug( string message, string className, string methodName )
    {
      Viatronix.Enterprise.Logging.Log.Debug( Source, message, className, methodName );
    } // Debug( message, className, methodName )


    /// <summary>
    /// Logs a information message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Information( string message, string className, string methodName )
    {
      Viatronix.Enterprise.Logging.Log.Information( Source, message, className, methodName );
    } // Information( message, className, methodName )

    /// <summary>
    /// Logs a information message to a specific source
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Information(string source, string message, string className, string methodName)
    {
      Viatronix.Enterprise.Logging.Log.Information(source, message, className, methodName);
    } // Information( message, className, methodName )


    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Timing(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Timing(Source, message, className, methodName);
    } // Debug( source, message, className, methodName )

    #endregion

  } // class Log
} // namespace Viatronix
