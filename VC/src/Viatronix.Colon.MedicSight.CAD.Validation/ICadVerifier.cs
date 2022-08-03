// $Id: ICadVerifier.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@gmail.com]

using System;
using System.Collections.Generic;
using System.Text;

namespace Viatronix.v3D.Utilities.CadParser
{
  /// <summary>
  /// An interface defining a Cad Verifier.
  /// </summary>
  public interface ICadVerifier
  {

    #region methods

    /// <summary>
    /// Reads the truth file to build up a list of accepted points to verify a 
    /// </summary>
    /// <param name="filePath">The truth file to read.</param>
    void ReadTruthFile(string filePath);

    /// <summary>
    /// Intended purely to validate the ReadTruthFile method.
    /// </summary>
    /// <remarks>Should throw out DICOM information to assist in validation.</remarks>
    /// <param name="filePath">The filePath to write out to.</param>
    /// <param name="inputFilePath">The input file that generated this output.</param>
    void SaveTruthFile(string filePath, string inputFilePath);

    /// <summary>
    /// Validates the Cad Report against the accepted results provided from the Cad vendor.
    /// </summary>
    /// <remarks>In implementations it is desirable to record in some way the errors so they may be recorded to a file.</remarks>
    /// <param name="findings">List of findings found in the Cad Report.</param>
    /// <param name="tolerance">Tolerance on the Z axis.</param>
    void VerifyFindingsAgainstTruthFile(List<CadFinding> findings, float tolerance);

    /// <summary>
    /// Writes out the verbose results of the Verification.
    /// </summary>
    /// <param name="outputFilePath">The path to write the result file to.</param>
    /// <param name="truthPath">The vendor provided truth file path.</param>
    /// <param name="reportPath">The Cad Report file path.</param>
    void SaveValidationResults(string outputFilePath, string truthPath, string reportPath);

    #endregion

  } // ICadVerifier
} // namespace Viatronix.v3D.Utilities.CadParser

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/ICadVerifier.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: ICadVerifier.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
