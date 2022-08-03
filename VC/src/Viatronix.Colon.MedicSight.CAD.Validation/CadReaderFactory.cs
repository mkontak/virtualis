// $Id: CadReaderFactory.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
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

namespace Viatronix.v3D.Utilities.CadParser
{
  /// <summary>
  /// Creates a CadReportReader/CadVerifier pair based on whether or not the Reader can read the report.
  /// </summary>
  public class CadReaderFactory
  {

    #region methods

    /// <summary>
    /// Creates a CadReportReader/CadVerifier pair based on whether or not the Reader can read the report.
    /// </summary>
    /// <param name="filePath">The Cad Report to read.</param>
    /// <returns>A CadReportReader/CadVerifier pair, or null if no appropriate reader was found.</returns>
    public static KeyValuePair<ICadReportReader, ICadVerifier> CreateCadReader(string filePath)
    {
      XmlNode readerSection = (XmlNode)(ConfigurationManager.GetSection("cadReaderPairs"));

      foreach (XmlNode readerPairNode in readerSection.SelectNodes("cadReaderPair"))
      {
        ICadReportReader reader = (ICadReportReader)(Activator.CreateInstance(Type.GetType(readerPairNode.Attributes["reader"].Value)));
        ICadVerifier verifier = (ICadVerifier)(Activator.CreateInstance(Type.GetType(readerPairNode.Attributes["verifier"].Value)));

        if(reader.CanReadReport(filePath))
        {
          return new KeyValuePair<ICadReportReader, ICadVerifier>(reader, verifier);
        }
      }

      return new KeyValuePair<ICadReportReader,ICadVerifier>(null, null);
    } // CreateCadReader()

    #endregion

  } // CadReaderFactory
} // namespace Viatronix.v3D.Utilities.CadParser

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/CadReaderFactory.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: CadReaderFactory.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
