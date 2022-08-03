using System;
using System.Windows.Controls;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
{
  /// <summary>
  /// Validates that the input is not an empty string.
  /// </summary>
  public class FreeformValidationRule : ValidationRule
  {

    #region methods

    /// <summary>
    /// Validates the input.
    /// </summary>
    /// <param name="value">The value to validate.</param>
    /// <param name="cultureInfo">The culture to take into account.</param>
    /// <returns>Whether the value is valid or not.</returns>
    public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
    {
      string str = Convert.ToString(value);

      if (str == string.Empty)
      {
        return new ValidationResult(false, Messages.GetMessage("VX001"));
      }

      if (str.IndexOf('<') > -1)
      {
        return new ValidationResult(false, Messages.GetMessage("VX002"));
      }

      if (str.IndexOf('>') > -1)
      {
        return new ValidationResult(false, Messages.GetMessage("VX003"));
      }

      return new ValidationResult(true, null);
    } // Validate( value, cultureInfo )

    #endregion

  } // class NotEmptyValidationRule
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
