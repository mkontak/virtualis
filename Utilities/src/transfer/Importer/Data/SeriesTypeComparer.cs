// $Id: SeriesTypeComparer.cs,v 1.1.2.3 2009/06/04 18:56:56 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Collections.Generic;
using System.Text;

using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// Provides sorting functionality for a sorted dictionary of keys of type strings with series names.
  /// </summary>
  public class SeriesTypeComparer : IComparer<SeriesTypes>, IComparer<Series>, IComparer<ParsedSeries>
  {

    #region methods

    /// <summary>
    /// Compares the two series types and returns the result of said comparison.
    /// </summary>
    /// <param name="a">lhs opperand</param>
    /// <param name="b">rhs opperand</param>
    /// <returns>lhs' relation to rhs</returns>
    int IComparer<SeriesTypes>.Compare(SeriesTypes a, SeriesTypes b)
    {
      return a.CompareTo(b);
    } // Compare(string a, string b)


    /// <summary>
    /// Compares the two series types and returns the result of said comparison.
    /// </summary>
    /// <param name="a">lhs opperand</param>
    /// <param name="b">rhs opperand</param>
    /// <returns>lhs' relation to rhs</returns>
    int IComparer<Series>.Compare(Series a, Series b)
    {
      return a.Type.CompareTo(b.Type);
    } // Compare(string a, string b)


    /// <summary>
    /// Compares the two series types and returns the result of said comparison.
    /// </summary>
    /// <param name="a">lhs opperand</param>
    /// <param name="b">rhs opperand</param>
    /// <returns>lhs' relation to rhs</returns>
    int IComparer<ParsedSeries>.Compare(ParsedSeries a, ParsedSeries b)
    {
      return a.Series.Type.CompareTo(b.Series.Type);
    } // Compare(string a, string b)

    #endregion

  } // class SeriesTypeComparer
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: SeriesTypeComparer.cs,v $
// Revision 1.1.2.3  2009/06/04 18:56:56  kchalupa
// Coding standards.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/SeriesTypeComparer.cs,v 1.1.2.3 2009/06/04 18:56:56 kchalupa Exp $
// $Date: 2009/06/04 18:56:56 $24 July 2008
// $Id: SeriesTypeComparer.cs,v 1.1.2.3 2009/06/04 18:56:56 kchalupa Exp $000000004

#endregion
