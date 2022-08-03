// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.Dicom.PACSViewer.Models
{
  /// <summary>
  /// Object to hold the modality.
  /// </summary>
  public class ModalityModel
  {

    #region fields

    /// <summary>
    /// The identifier for the modality.
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// The description.
    /// </summary>
    private string m_description = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ModalityModel()
    {
    } // ModalityModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the identifier.
    /// </summary>
    public string Id
    {
      get { return m_id; }
      set { m_id = value; }
    } // Id


    /// <summary>
    /// Gets or sets the description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    } // Description

    #endregion

    #region methods

    /// <summary>
    /// Returns a string representation of the object.
    /// </summary>
    public override string ToString()
    {
      return Description;
    } // ToString()

    #endregion

  } // class ModalityModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.Models
