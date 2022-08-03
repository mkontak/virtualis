using System;
using System.IO;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  /// <summary>
  /// A logger for this program.
  /// </summary>
  internal static class MiniLogger
  {

    #region fields

    /// <summary>
    /// The log level.
    /// </summary>
    private static int m_logLevel = 5;

    /// <summary>
    /// The text writer.
    /// </summary>
    private static TextWriter m_writer = null;

    #endregion

    #region methods

    /// <summary>
    /// Initialize the logger.
    /// </summary>
    internal static void Initialize()
    {
      if (ConfigurationManager.AppSettings["LogLevel"] != null)
      {
        Convert.ToInt32(ConfigurationManager.AppSettings["LogLevel"]);
      }
      else
      {
        throw new ApplicationException("No value is specified for LogLevel."); 
      }

      string filename = ConfigurationManager.AppSettings["LogFile"];
      if (filename != null)
      {
        if (!Directory.Exists(Path.GetDirectoryName(filename)))
        {
          Directory.CreateDirectory(Path.GetDirectoryName(filename));
        }

        m_writer = new StreamWriter(filename, true);
      }
      else
      {
        throw new ApplicationException("No value is specified for LogFile.");
      }
    } // Initialize()


    /// <summary>
    /// Write a message to the log.
    /// </summary>
    /// <param name="messageType">The message type.</param>
    /// <param name="date">The datetime.</param>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Write(string messageType, DateTime date, string message, string className, string methodName)
    {
      m_writer.WriteLine("<{0} message=\"{1}\" date=\"{2}\" class=\"{3}\" method=\"{4}\" />", messageType, message, date, className, methodName);
      m_writer.Flush();
    } // WriteMessage(DateTime date, string message, string className, string methodName)


    /// <summary>
    /// Information message.
    /// </summary>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Debug(string message, string className, string methodName)
    {
      if (m_logLevel > 4)
      {
        Write("debug", DateTime.Now, message, className, methodName);
      }
    } // Debug( message, className, methodName )


    /// <summary>
    /// Information message.
    /// </summary>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Information(string message, string className, string methodName)
    {
      if (m_logLevel > 3)
      {
        Write("information", DateTime.Now, message, className, methodName);
      }
    } // Information( message, className, methodName )


    /// <summary>
    /// Warning message.
    /// </summary>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Warning(string message, string className, string methodName)
    {
      if (m_logLevel > 2)
      {
        Write("warning", DateTime.Now, message, className, methodName);
      }
    } // Warning( message, className, methodName )


    /// <summary>
    /// Information message.
    /// </summary>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Error(string message, string className, string methodName)
    {
      if (m_logLevel > 1)
      {
        Write("error", DateTime.Now, message, className, methodName);
      }
    } // Error( message, className, methodName )


    /// <summary>
    /// Critical message.
    /// </summary>
    /// <param name="message">The message to write out.</param>
    /// <param name="className">The class name.</param>
    /// <param name="methodName">The method name.</param>
    internal static void Critical(string message, string className, string methodName)
    {
      if (m_logLevel > 0)
      {
        Write("critical", DateTime.Now, message, className, methodName);
      }
    } // Critical( message, className, methodName )

    #endregion

  } // class MiniLogger
} // namespace Viatronix.Utilities.SelectiveManifestGenerator
