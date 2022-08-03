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

namespace Viatronix.Colon.Configuration
{
  internal sealed class RegistrySectionNames
  {

    #region fields

    /// <summary>
    /// local settings
    /// </summary>
    public const string LOCAL_SETTINGS = "localSettings";

    /// <summary>
    /// user settings
    /// </summary>
    public const string USER_SETTINGS = "userSettings";

    /// <summary>
    /// global settings
    /// </summary>
    public const string GLOBAL_SETTINGS = "globalSettings";

    #endregion

    #region construction

    /// <summary>
    /// private constructor
    /// </summary>
    private RegistrySectionNames()
    {
    }

    #endregion

    #region methods

    /// <summary>
    /// Calculates the registry section name
    /// </summary>
    /// <param name="definedName">name</param>
    /// <returns>section name</returns>
    public static string GetSectionName( string definedName )
    {
      return ( definedName == USER_SETTINGS ) ? USER_SETTINGS : 
                                                                (( string.IsNullOrEmpty( definedName )) ? LOCAL_SETTINGS : GLOBAL_SETTINGS );
    } // GetSectionName( definedName )

    #endregion

  } // RegistrySectionNames
} // namespace Viatronix.Colon.Configuration
