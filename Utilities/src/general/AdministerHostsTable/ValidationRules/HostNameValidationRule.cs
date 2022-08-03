using System;
using System.Configuration;
using System.Windows.Controls;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
{
  /// <summary>
  /// Validates that an identifier is not an empty string, doesn't have a number at the starting position.
  /// </summary>
  public class HostNameValidationRule : FreeformValidationRule
  {

    #region methods

    /// <summary>
    /// Validates the input.
    /// </summary>
    /// <param name="value"></param>
    /// <param name="cultureInfo"></param>
    /// <returns></returns>
    public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
    {
      string identifier = (string)value;
      string extraChars = ConfigurationManager.AppSettings["extraValidCharacters"];
      for (int i = 0; i < identifier.Length; ++i)
      {
        bool found = false;
        if ((identifier[i] >= 'A' && identifier[i] <= 'Z') || (identifier[i] >= 'a' && identifier[i] <= 'z') || (identifier[i] >= '0' && identifier[i] <= '9') || identifier[i] == '-' || identifier[i] == '_' || identifier[i] == '.')
        {
          found = true;
        }

        // if not found in the regular list check the extended table.
        if (!found)
        {
          for (int j = 0; j < extraChars.Length; ++j)
          {
            if (identifier[i] == extraChars[j])
            {
              found = true;
              break;
            }
          }
        }

        if (!found)
        {
          return new ValidationResult(false, Messages.GetMessage("VX004"));
        }
      }

      // The validation for me worked, return the base validator.
      return base.Validate(value, cultureInfo);
    } // Validate( value, cultureInfo )

    #endregion

  } // class IdentifierValidationRule
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
