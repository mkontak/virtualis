// Copyright © 2010, Viatronix Inc., All Rights Reserved.
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
using Viatronix.UI.Services;

namespace Viatronix.UI
{
  public static class ServiceInjector
  {

    /// <summary>
    /// Loads services into ServiceContainer
    /// </summary>
    public static void InjectServices()
    {
      ServiceContainer.Instance.AddService< IMessageBoxService >( new MessageBoxService() );
    } // InjectServices()

  } // class ServiceInjector
} // namespace Viatronix.UI
