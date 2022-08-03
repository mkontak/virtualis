// $Id: Utils.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Text;

using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Defines utility functions that can assist in Reading and Verifing Cad Reports.
  /// </summary>
  public static class Utils
  {

    #region methods

    /// <summary>
    /// Reads the sphericity values desired by reading them from the App.Config.
    /// </summary>
    /// <returns>A list of the desired sphericity values.</returns>
    public static int[] ReadDesiredSphericityValues()
    {
      return (int[])ConfigurationManager.GetSection("sphericityValues");
    } // ReadDesiredSphericityValues()


    /// <summary>
    /// Gets the list of findings that have a sphericity that of sphericityValue.
    /// </summary>
    /// <param name="findings">The list of findings read from the report.</param>
    /// <param name="sphericityValue">The desired sphericity value.</param>
    /// <returns>A sorted list of the findings that contain the desired sphericity value.</returns>
    public static List<CadFinding> GetFindingsBySpericityValue(List<CadFinding> findings, int sphericityValue)
    {
      List<CadFinding> sphericityFindings = new List<CadFinding>();

      foreach (CadFinding finding in findings)
      {
        if (finding.SphericityValues.Contains(sphericityValue))
        {
          sphericityFindings.Add(finding);
        }
      }

      return sphericityFindings;
    } // GetFindingsBySpericityValue(List<CadFinding> findings, int sphericityValue)


    /// <summary>
    /// Generates a string of spacing from the value and the total count of spaces.
    /// </summary>
    /// <param name="value">The value to get the spacing for.</param>
    /// <param name="spaces">The total number of spaces.</param>
    /// <returns>Return a string of spacing from the value and the total count of spaces.</returns>
    public static string GenerateSpacing(int value, int spaces)
    {
      return GenerateSpacing((double)value, spaces);
    } // GenerateSpacing( value, spaces )


    /// <summary>
    /// Generates a string of spacing from the value and the total count of spaces.
    /// </summary>
    /// <param name="value">The value to get the spacing for.</param>
    /// <param name="spaces">The total number of spaces.</param>
    /// <returns>Return a string of spacing from the value and the total count of spaces.</returns>
    public static string GenerateSpacing(double value, int spaces)
    {
      int spacing = spaces - value.ToString().Length;

      if (spacing > 0)
      {
        char[] spacers = new char[spacing];
        for (int i = 0; i < spacing; ++i)
        {
          spacers[i] = ' ';
        }

        return new string(spacers);
      }

      return " ";
    } // GenerateSpacing( value, spaces )

    #endregion

  } // Utils
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/Utils.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: Utils.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
