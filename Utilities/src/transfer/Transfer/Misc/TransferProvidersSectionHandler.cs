using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Utilities.Transfer.ViewModels;

namespace Viatronix.Utilities.Transfer
{
  /// <summary>
  /// Handles the creation of the transfer providers section.
  /// </summary>
  public class TransferProvidersSectionHandler : IConfigurationSectionHandler
  {

    #region IConfigurationSectionHandler implementation

    /// <summary>
    /// Creates the section and interprets the data.
    /// </summary>
    /// <param name="parent">The parent node.</param>
    /// <param name="configContext">The configuration context.</param>
    /// <param name="section">The section to handle.</param>
    /// <returns></returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      List<TransferProviderViewModel> viewModels = new List<TransferProviderViewModel>();
      foreach(XmlNode providerNode in section.SelectNodes("/transferProviders/transferProvider"))
      {
        viewModels.Add(TransferProviderViewModel.Create(providerNode));
      }

      return viewModels.ToArray();
    } // Create( parent, configContext, section )

    #endregion

  } // class TransferProvidersSectionHandler
} // namespace Viatronix.Utilities.Transfer
