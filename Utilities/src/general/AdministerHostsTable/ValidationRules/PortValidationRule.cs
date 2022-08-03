using System;
using System.Windows.Controls;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
{
  /// <summary>
  /// Validates the port.
  /// </summary>
  public class PortValidationRule : ValidationRule
  {

    #region methods

    /// <summary>
    /// Performs validation on the input.
    /// </summary>
    /// <param name="value">The value tp perform validation on.</param>
    /// <param name="cultureInfo">The culture info.</param>
    /// <returns>The result of the validation.</returns>
    public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
    {
      try
      {
        Int32 port = Convert.ToInt32(value);
        if (port > 0 && port < 65536)
        {
          return new ValidationResult(true, null);
        }
        else
        {
          return new ValidationResult(false, Messages.GetMessage("VX005"));
        }
      }
      catch (Exception)
      {
      }

      return new ValidationResult(false, Messages.GetMessage("VX006"));
    }

    #endregion

  } // class PortValidationRule
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ValidationRules
