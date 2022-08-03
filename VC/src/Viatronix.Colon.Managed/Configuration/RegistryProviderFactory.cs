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
using Viatronix.Colon.Data;

using Viatronix.Colon.Configuration.Provider;

namespace Viatronix.Colon.Configuration
{
  public static class RegistryProviderFactory
  {

    #region fields

    /// <summary>
    /// singleton instance
    /// </summary>
    private static IRegistryProvider m_provider = null;

    #endregion

    /// <summary>
    /// Creates an instance of an IRegistryProvider
    /// </summary>
    /// <returns>IRegistryProvider</returns>
    public static IRegistryProvider Create()
    {  
      if( m_provider == null )
        m_provider = ( !DatabaseGateway.IsEnabled ) ? (IRegistryProvider) new FileRegistryProvider() : new DatabaseRegistryProvider();

      return m_provider;
    } // Create()

  } // class RegistryProviderFactory
} // namespace Viatronix.Colon.Configuration
