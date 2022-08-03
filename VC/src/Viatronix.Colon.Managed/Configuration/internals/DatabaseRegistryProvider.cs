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
using Viatronix.Colon.Data;

namespace Viatronix.Colon.Configuration.Provider
{
  internal sealed class DatabaseRegistryProvider : IRegistryProvider
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
      bool success = false;
      string xml = string.Empty;
      if( type == RegistrySectionNames.USER_SETTINGS ) 
        success = DatabaseGateway.TryGetProfile( out xml );
      else if( type == RegistrySectionNames.LOCAL_SETTINGS ) 
        success = DatabaseGateway.TryGetConfig( Environment.MachineName, out xml );
      else
        success = DatabaseGateway.TryGetConfig( type, out xml );

      reader = (success ) ? new StringReader( xml ) : null;
      return success;
    } // Load( type, out reader )


    /// <summary>
    /// Save RegistrySection xml to a database
    /// </summary>
    /// <param name="type">section type</param>
    /// <param name="xml">section xml</param>
    public void Save( string type, string xml )
    {
      if( type == RegistrySectionNames.USER_SETTINGS ) 
        DatabaseGateway.SetProfile( xml ); 
      else if( type == RegistrySectionNames.LOCAL_SETTINGS )
        DatabaseGateway.SetConfig( Environment.MachineName, xml ); 
      else
        DatabaseGateway.SetConfig( type, xml );
    } // Save( type, xml )

    #endregion

  } // class DatabaseRegistryProvider
} // namespace Viatronix.Colon.Configuration.Provider
