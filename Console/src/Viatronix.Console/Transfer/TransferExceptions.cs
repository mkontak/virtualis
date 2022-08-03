// $Id: TransferExceptions.cs,v 1.1 2005/12/13 20:45:59 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;

namespace Viatronix.Console
{

  #region TransferException
  /// <summary>
  /// Implements Transfer exception .
  /// </summary>
  [Serializable]
  public class TransferException : Exception
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public TransferException() : base() { 	}

    /// <summary>
    /// Transfer Exception with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public TransferException(string message) : base(message) { }

    /// <summary>
    /// Transfer Exception  with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public TransferException(string message, Exception ex) : base(message, ex) { }

    #endregion

  } // TransferException

  #endregion TransferException

  #region TransferAbortException
  /// <summary>
  /// Throw this if the user Aborts Transfer.
  /// </summary>
  [Serializable]
  public class TransferAbortException : Exception
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public TransferAbortException() : base() { 	}

    /// <summary>
    /// Transfer Abort Exception with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public TransferAbortException(string message) : base(message) { }

    /// <summary>
    /// Transfer Abort Exception  with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public TransferAbortException(string message, Exception ex) : base(message, ex) { }

    #endregion

  } // TransferAbortException

  #endregion TransferAbortException

  #region TransferAbortCancelException
  /// <summary>
  /// Throw this if the user Aborts Transfer.
  /// </summary>
  [Serializable]
  public class TransferAbortCancelException : Exception
  {

    #region constructors

    /// <summary>
    /// Base constructor
    /// </summary>
    public TransferAbortCancelException() : base() { 	}

    /// <summary>
    /// Transfer Abort Cancel Exception with an error message
    /// </summary>
    /// <param name="message">Error message</param>
    public TransferAbortCancelException(string message) : base(message) { }

    /// <summary>
    /// Transfer Abort Exception Cancel  with an error message and inner exception
    /// </summary>
    /// <param name="message">Error Message </param>
    /// <param name="ex">Inner exception</param>
    public TransferAbortCancelException(string message, Exception ex) : base(message, ex) { }

    #endregion

  } // TransferAbortCancelException

  #endregion TransferAbortCancelException
 
}//namespace Viatronix.Console

#region revision History

// $Log: TransferExceptions.cs,v $
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.3  2005/12/02 02:41:43  romy
// coding standards
//
// Revision 1.2  2005/09/30 17:13:09  romy
// Transfer cancel method added
//
// Revision 1.1  2005/09/22 16:15:49  romy
// transfer exception initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferExceptions.cs,v 1.1 2005/12/13 20:45:59 romy Exp $
// $Id: TransferExceptions.cs,v 1.1 2005/12/13 20:45:59 romy Exp $

#endregion revision History




