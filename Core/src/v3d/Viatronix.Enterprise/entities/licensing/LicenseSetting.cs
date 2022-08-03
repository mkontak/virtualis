// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: LicenseSetting.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace Viatronix.Enterprise.Entities
{

  /// <summary>
  /// LicenseSetting object
  /// </summary>
  public class LicenseSetting : Setting
  {


 
  
    #region methods


    #region Create methods

    /// <summary>
    /// Creates the setting from the xml string
    /// </summary>
    /// <param name="xml">Xml string</param>
    /// <returns>LicenseSetting</returns>
    public static new LicenseSetting Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // Create(string xml)

    /// <summary>
    /// Creates a setting from the xml node
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>LicenseSetting</returns>
    public static new LicenseSetting Create(XmlNode node)
    {

      LicenseSetting setting = new LicenseSetting();

      if (setting == null)
        throw new OutOfMemoryException("Failed to allocate the setting");

      setting.Load(node);

      return setting;

    } // Create(XmlNode node)


    #endregion

    #endregion


    #region IEntity

    /// <summary>
    /// Clones the setting
    /// </summary>
    /// <param name="node">Xml node</param>
    /// <returns>LicenseSetting</returns>
    public override object Clone()
    {
      return base.Clone();
    } // Clone()



    #endregion
  }
}
