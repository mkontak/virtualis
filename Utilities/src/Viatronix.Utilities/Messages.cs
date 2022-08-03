using System;
using System.Windows;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities
{

  #region messages class

  /// <summary>
  /// Provides access to the messages in the app.config.
  /// </summary>
  public static class Messages
  {

    #region fields

    /// <summary>
    /// A dectionary containing a mapping from the message code to the message text.
    /// </summary>
    private static Dictionary<string, string> m_messageDictionary = null; 

    #endregion

    #region construction

    /// <summary>
    /// Static constructor.
    /// </summary>
    static Messages()
    {
      m_messageDictionary = (Dictionary<string, string>)ConfigurationManager.GetSection("messages");
    } // Messages()

    #endregion

    #region methods

    /// <summary>
    /// Retrieves the message from the specified code.
    /// </summary>
    /// <param name="code">The code.</param>
    /// <returns>The message that corresponds to the specified code.</returns>
    public static string GetMessage(string code)
    {
      string value = string.Empty;
      if (m_messageDictionary.TryGetValue(code, out value))
      {
        return value;
      }

      throw new ApplicationException("Specified message code not found in dictionary.");
    } // GetMessage( code )

    #endregion

  } // class Messages

  #endregion

  #region messages section handler class

  /// <summary>
  /// The handler for the messages section.
  /// </summary>
  public class MessagesSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler implementation

    /// <summary>
    /// Creates an object containing the information in the messages config section.
    /// </summary>
    /// <param name="parent">Parent object.</param>
    /// <param name="configContext">Configuration context object.</param>
    /// <param name="section">Section XML node.</param>
    /// <returns>An object containing the information in the messages config secion.</returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      Dictionary<string, string> messageDictionary = new Dictionary<string, string>();

      foreach (XmlNode message in section.SelectNodes("message"))
      {
        XmlAttribute code = message.Attributes["code"];
        XmlAttribute value = message.Attributes["value"];

        if (code == null || value == null)
        {
          MessageBox.Show("Messages section in app.config is ill formed.  Application will exit.");
          Application.Current.Shutdown(1);
        }
        else
        {
          string temp = string.Empty;
          if (messageDictionary.TryGetValue(code.Value, out temp))
          {
            MessageBox.Show("Messages section in app.config has duplicate error code.  Application will exit.");
            Application.Current.Shutdown(1);
          }
          else
          {
            messageDictionary.Add(code.Value, value.Value);
          }
        }
      }

      return messageDictionary;
    } // Create( parent, configContext, section )

    #endregion

  } // class MessagesSectionHandler

  #endregion

} // namespace Viatronix.Utilities
