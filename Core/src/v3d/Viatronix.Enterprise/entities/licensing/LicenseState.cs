// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: LicenseState.cs
//
// Description: Implements a license state object 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace Viatronix.Enterprise.Entities
{


  /// <summary>
  /// Implements the license state which identifies the states that an idividual license can be in
  /// </summary>
  public class LicenseState : State
  {


 
    #region static methods


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static new LicenseState Create(XmlNode node)
    {
      LicenseState state = new LicenseState();

      if (state == null)
        throw new OutOfMemoryException("Failed to allocate LicenseState");

      state.Load(node);

      return state;
    } // FromXml( node )


    /// <summary>
    /// Creates a new instance of a Job object from xml.
    /// </summary>
    /// <param name="node">The xml used to reconstruct a Series object.</param>
    /// <returns>A Task object.</returns>
    public static new LicenseState Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // FromXml( node )

    #endregion

  } // class Task



} // namespace Viatronix.Enterprise.Entities

