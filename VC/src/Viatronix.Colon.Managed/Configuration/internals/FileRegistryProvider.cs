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
using System.Xml;
using System.Configuration;

namespace Viatronix.Colon.Configuration.Provider
{
  internal sealed class FileRegistryProvider : IRegistryProvider
  {
    
    #region IRegistryProvider Members

    /// <summary>
    /// Load a TextReader of registry information
    /// </summary>
    /// <param name="type">section type</param>
    /// <param name="reader">TextReader</param>
    /// <returns>true if successful; otherwise false</returns>
    public bool TryLoad( string type, out TextReader reader )
    {
      string file = Path.Combine( VCSettingsSection.CommonApplicationData, string.Format( "colon_{0}.config", type ));

      reader = ( File.Exists( file )) ? new StreamReader( file ) : null;

      return ( reader != null );
    } // Load( type, out reader )


    /// <summary>
    /// Save RegistrySection xml to a file
    /// </summary>
    /// <param name="type">section type</param>
    /// <param name="xml">xml</param>
    public void Save( string type, string xml )
    {
      string file = Path.Combine( VCSettingsSection.CommonApplicationData, string.Format( "colon_{0}.config", type ));

      using( StreamWriter writer = new StreamWriter( file ))
        writer.Write( xml );
    } // Save( type, xml )

    #endregion

  } // class FileRegistryProvider
} // namespace Viatronix.Colon.Configuration.Provider
