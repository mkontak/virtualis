// $Id: FilterFactory.cs,v 1.1.2.2 2009/06/04 18:56:23 kchalupa Exp $000000004
//
// Copyright@ 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: [ Kyle R Chalupa ] email: [ kchalupa@viatronix.com ]

using System;
using System.Collections.Generic;

using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Looks for an appropriate filter to parse the given directory.
  /// </summary>
  public class FilterFactory
  {

    #region public static methods

    /// <summary>
    /// Looks for an appropriate filter to parse the directory contained in the IOArgs parameter.
    /// </summary>
    /// <param name="ioArgs">Contains the desired source directory.</param>
    /// <returns>
    /// The appropriate filter if one is found that can parse the directory, otherwise a null.
    /// </returns>
    public static IFilter Create(ImporterArgs ioArgs)
    {
      VCFilter20 vcfilter20 = new VCFilter20();
      XPFilter20 xpFilter20 = new XPFilter20();
      VASFilter20 vasfilter20 = new VASFilter20();
      Filter30 filter30 = new Filter30();

      // If the VCFilter20 can parse it, return the instance.
      if(vcfilter20.CanParse(ioArgs))
      {
        return vcfilter20;
      } // if(vcfilter.CanParse(ioArgs))

      // If the XPFilter20 can parse it, return the instance.
      else if( xpFilter20.CanParse(ioArgs) )
      {
        return xpFilter20;
      } // else if( xpFilter20.CanParse(ioArgs) )

      // If the VASFilter20 can parse it, return the instance.
      else if( vasfilter20.CanParse(ioArgs) )
      {
        return vasfilter20;
      } // else if( vasfilter20.CanParse(ioArgs) )

      // If the Filter30
      else if(filter30.CanParse(ioArgs))
      {
        return filter30;
      } // else if(filter30.CanParse(ioArgs))

      // If no filters could parse it, return a null.
      return null;
    } // Create()

    #endregion

  } // FilterFactory
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: FilterFactory.cs,v $
// Revision 1.1.2.2  2009/06/04 18:56:23  kchalupa
// Coding standards.
//
// Revision 1.1.2.1  2008/09/24 14:31:19  mkontak
// Initial Check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/FilterFactory.cs,v 1.1.2.2 2009/06/04 18:56:23 kchalupa Exp $
// $Date: 2009/06/04 18:56:23 $25 July 2008
// $Id: FilterFactory.cs,v 1.1.2.2 2009/06/04 18:56:23 kchalupa Exp $000000004

#endregion
