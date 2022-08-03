using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Handles the shpericity section.
  /// </summary>
  public class ShpericitySectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler implementation

    /// <summary>
    /// Section handler for the sphericities.
    /// </summary>
    /// <param name="parent">The parent section.</param>
    /// <param name="settings">The configuration settings.</param>
    /// <param name="section">The section to handle.</param>
    /// <returns>The section contents as an array of ints.</returns>
    public object Create(object parent, object settings, XmlNode section)
    {
      List<int> sphericities = new List<int>();

      foreach (XmlNode node in section.SelectNodes("sphereicityValue"))
      {
        sphericities.Add(Convert.ToInt32(node.Attributes["value"].Value));
      }

      return sphericities.ToArray();
    } // Create( parent, settings, section )

    #endregion

  } // class ShpericitySectionHandler
} // namespace Viatronix.Colon.Medicsight.CAD.Validation
