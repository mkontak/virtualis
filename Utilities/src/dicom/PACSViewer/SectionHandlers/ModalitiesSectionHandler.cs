// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using Viatronix.Utilities.Dicom.PACSViewer.Models;

namespace Viatronix.Utilities.Dicom.PACSViewer.SectionHandlers
{
  /// <summary>
  /// Section handler for modalities.
  /// </summary>
  public class ModalitiesSectionHandler : IConfigurationSectionHandler
  {

    #region methods

    /// <summary>
    /// Handles the modalities section.
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="configContext"></param>
    /// <param name="section"></param>
    /// <returns></returns>
    public object Create(object parent, object configContext, XmlNode section)
    {
      ObservableCollection<ModalityModel> modalities = new ObservableCollection<ModalityModel>();
      foreach(XmlNode node in section.SelectNodes("modality"))
      {
        ModalityModel modality = new ModalityModel();

        modality.Id = node.Attributes["id"].Value;
        modality.Description = node.Attributes["desc"].Value;

        modalities.Add(modality);
      }

      return modalities;
    } // Create( parent, configContext, section )

    #endregion

  } // class ModalitiesSectionHandler
} // namespace Viatronix.Utilities.Dicom.PACSViewer.SectionHandlers
