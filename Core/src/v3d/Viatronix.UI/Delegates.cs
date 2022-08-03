// $Id: Delegates.cs,v 1.1 2006/07/19 13:51:50 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.UI
{
  
  /// <summary>
  /// Generic delegate which returns a single type.
  /// </summary>
  public delegate T GetterDelegate< T >();

  /// <summary>
  /// Generic delegate which takes a single parameter.
  /// </summary>
  public delegate void SetterDelegate< T >( T val ); 

  /// <summary>
  /// Generic delegate which takes two parameters.
  /// </summary>
  public delegate void SetterDelegate< T1, T2 >( T1 val1, T2 val2 ); 

  /// <summary>
  /// Generic delegate which takes three parameters.
  /// </summary>
  public delegate void SetterDelegate< T1, T2, T3 >( T1 val1, T2 val2, T3 val3 ); 

} // namespace Viatronix.UI


#region revision history

// $Log: Delegates.cs,v $
// Revision 1.1  2006/07/19 13:51:50  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Delegates.cs,v 1.1 2006/07/19 13:51:50 gdavidson Exp $
// $Id: Delegates.cs,v 1.1 2006/07/19 13:51:50 gdavidson Exp $

#endregion
