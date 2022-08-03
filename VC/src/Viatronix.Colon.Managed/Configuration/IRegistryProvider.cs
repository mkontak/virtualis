// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Viatronix.Colon.Configuration
{
  public interface IRegistryProvider
  {
    /// <summary>
    /// Load a TextReader of registry information
    /// </summary>
    /// <param name="type">section type</param>
    /// <param name="reader">TextReader</param>
    /// <returns>true if successful; otherwise false</returns>
    bool TryLoad( string type, out TextReader reader );

    /// <summary>
    /// Saves the xml of a RegistrySection
    /// </summary>
    /// <param name="type">section type</param>
    /// <param name="xml">section xml</param>
    void Save( string type, string xml );

  } // interface IRegistryProvider
} // namespace Viatronix.Colon.Configuration
