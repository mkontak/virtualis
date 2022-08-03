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
using Viatronix.Enterprise.Logging;

namespace Viatronix.Enterprise.Logging
{
  public static class Log
  {


    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static Log()
    {
    } // Log

    #endregion

    #region methods



    /// <summary>
    /// Logs a critical message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Critical( string source, string message, string className, string methodName )
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Critical(message, className, methodName);
      else
        Viatronix.Logging.Log.Critical(source, message, className, methodName);
    } // Critical( source, message, className, methodName )

    /// <summary>
    /// Logs a critical message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Critical(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Critical(message, className, methodName);
    } // Critical( source, message, className, methodName )


    /// <summary>
    /// Logs a error message
    /// </summary>
    /// <param name="source"></param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Error( string source, string message, string className, string methodName )
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Error(message, className, methodName);
      else
        Viatronix.Logging.Log.Error(source, message, className, methodName);
    } // Error( source, message, className, methodName )

    /// <summary>
    /// Logs a error message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Error(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Error(message, className, methodName);
    } // Error( source, message, className, methodName )


    /// <summary>
    /// Logs a warning message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Warning( string source, string message, string className, string methodName )
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Warning(message, className, methodName);
      else
        Viatronix.Logging.Log.Warning(source, message, className, methodName);
    } // Warning( source, message, className, methodName )

    /// <summary>
    /// Logs a warning message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Warning(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Warning(message, className, methodName);
    } // Warning( source, message, className, methodName )

    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Debug( string source, string message, string className, string methodName )
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Debug(message, className, methodName);
      else
        Viatronix.Logging.Log.Debug(source, message, className, methodName);
    } // Debug( source, message, className, methodName )

    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Debug(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Debug( message, className, methodName);
    } // Debug( source, message, className, methodName )

    /// <summary>
    /// Logs a debug message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Timing(string source, string message, string className, string methodName)
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Timing(message, className, methodName);
      else
        Viatronix.Logging.Log.Timing(source, message, className, methodName);
    } // Debug( source, message, className, methodName )

    /// <summary>
    /// Logs a timing message
    /// </summary>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Timing(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Timing(message, className, methodName);
    } // Debug( source, message, className, methodName )

     /// <summary>
    /// Logs a information message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Information( string source, string message, string className, string methodName )
    {
      if (string.IsNullOrEmpty(source))
        Viatronix.Logging.Log.Information(message, className, methodName);
      else
        Viatronix.Logging.Log.Information(source, message, className, methodName);
    } // Information( source, message, className, methodName )

    /// <summary>
    /// Logs a information message
    /// </summary>
    /// <param name="source">source</param>
    /// <param name="message">message</param>
    /// <param name="className">class name</param>
    /// <param name="methodName">method name</param>
    public static void Information(string message, string className, string methodName)
    {
      Viatronix.Logging.Log.Information(message, className, methodName);
    } // Information( source, message, className, methodName )


    #endregion

  } // class Log
} // namespace Viatronix
