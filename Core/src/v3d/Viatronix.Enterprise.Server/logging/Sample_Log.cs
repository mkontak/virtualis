// copyright © 2010, viatronix inc., all rights reserved.
//
// this is proprietary source code of viatronix inc. the contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of viatronix inc. 
//
// author: greg davidson ( mailto:gdavidson@viatronix.com )


// ******************************************
//
// Copy file to project and modify the Source field
//
// *****************************************

//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Viatronix
//{
//  internal static class Log
//  {

//    #region fields

//    /// <summary>
//    /// source
//    /// </summary>
//    private const string Source = "<enter source name>";

//    #endregion

//    #region methods

//    /// <summary>
//    /// Logs a critical message
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void Critical( string message, string className, string methodName )
//    {
//      Viatronix.Enterprise.Logging.Log.Critical( Source, message, className, methodName );
//    } // Critical( message, className, methodName )


//    /// <summary>
//    /// Logs a error message
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void Error( string message, string className, string methodName )
//    {
//      Viatronix.Enterprise.Logging.Log.Error( Source, message, className, methodName );
//    } // Error( message, className, methodName )


//    /// <summary>
//    /// Logs a warning message
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void Warning( string message, string className, string methodName )
//    {
//      Viatronix.Enterprise.Logging.Log.Warning( Source, message, className, methodName );
//    } // Warning( message, className, methodName )


//    /// <summary>
//    /// Logs a debug message
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void Debug( string message, string className, string methodName )
//    {
//      Viatronix.Enterprise.Logging.Log.Debug( Source, message, className, methodName );
//    } // Debug( message, className, methodName )


//    /// <summary>
//    /// Logs a information message
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void Information( string message, string className, string methodName )
//    {
//      Viatronix.Enterprise.Logging.Log.Warning( Source, message, className, methodName );
//    } // Information( message, className, methodName )


//    /// <summary>
//    /// Logs a critical message and throws exception
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void ThrowCritical<T>( string message, string className, string methodName ) where T : Exception
//    {
//      Viatronix.Enterprise.Logging.Log.ThrowCritical<T>( Source, message, className, methodName );
//    } // ThrowCritical( message, className, methodName )


//    /// <summary>
//    /// Logs a error message and throws exception
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void ThrowError<T>( string message, string className, string methodName ) where T : Exception
//    {
//      Viatronix.Enterprise.Logging.Log.ThrowError<T>( Source, message, className, methodName );
//    } // ThrowError( message, className, methodName )


//    /// <summary>
//    /// Logs a warning message and throws exception
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void ThrowWarning<T>( string message, string className, string methodName ) where T : Exception
//    {
//      Viatronix.Enterprise.Logging.Log.ThrowWarning<T>( Source, message, className, methodName );
//    } // ThrowWarning( message, className, methodName )


//    /// <summary>
//    /// Logs a debug message and throws exception
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void ThrowDebug<T>( string message, string className, string methodName ) where T : Exception
//    {
//      Viatronix.Enterprise.Logging.Log.ThrowDebug<T>( Source, message, className, methodName );
//    } // ThrowDebug( message, className, methodName )


//    /// <summary>
//    /// Logs a information message and throws exception
//    /// </summary>
//    /// <param name="message">message</param>
//    /// <param name="className">class name</param>
//    /// <param name="methodName">method name</param>
//    public static void ThrowInformation<T>( string message, string className, string methodName ) where T : Exception
//    {
//      Viatronix.Enterprise.Logging.Log.ThrowInformation<T>( Source, message, className, methodName );
//    } // ThrowInformation( message, className, methodName )

 
//    #endregion

//  } // class Log
//} // namespace Viatronix
