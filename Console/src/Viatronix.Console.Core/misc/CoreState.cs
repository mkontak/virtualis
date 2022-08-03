// $Id: CoreState.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.Console
{

  public sealed class CoreState
  {

    #region constants

    /// <summary>
    /// connection string identifier string
    /// </summary>
    public const string ConnectionString = "ConnectionString";

    /// <summary>
    /// user identifier string
    /// </summary>
    public const string User = "User";

    /// <summary>
    /// dataprovider identifier string
    /// </summary>
    public const string DataProvider = "DataProvider";

    /// <summary>
    /// user rights identifier string
    /// </summary>
    public const string UserRights = "UserRights";

    /// <summary>
    /// site identifier string
    /// </summary>
    public const string Site = "Site";

    /// <summary>
    /// idle interval identifier string
    /// </summary>
    public const string IdleInterval = "IdleInterval";

    #endregion

    #region construction

    /// <summary>
    /// private constructor
    /// </summary>
    private CoreState()
    {
    } // CoreState()

    #endregion

  } // class CoreState
} // namespace Viatronix.v3D.Core


#region revision history

// $Log: CoreState.cs,v $
// Revision 1.3  2007/03/12 17:46:31  mkontak
// coding standards
//
// Revision 1.2  2007/03/01 19:00:43  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 15:57:37  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Core/data/CoreState.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $
// $Id: CoreState.cs,v 1.3 2007/03/12 17:46:31 mkontak Exp $

#endregion
