// $Id: CadFinding.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $
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
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// Represents a Finding found in the Cad data file.
  /// </summary>
  public class CadFinding
  {

    #region fields

    /// <summary>
    /// The finding's GUID.
    /// </summary>
    private Guid m_findingGuid = new Guid();

    /// <summary>
    /// The centroid coordinates of this finding.
    /// </summary>
    Point3Df m_centroidCoordinates = new Point3Df();

    /// <summary>
    /// The centroid point converted to DICOM coordinate system.
    /// </summary>
    Point3Df m_convertedCentroid = new Point3Df();

    /// <summary>
    /// The list of sphericity values.
    /// </summary>
    List<int> m_sphericityList = new List<int>();

    #endregion

    #region properties

    /// <summary>
    /// The Guid associated with this finding.
    /// </summary>
    public Guid Guid
    {
      get { return m_findingGuid;  }
      set { m_findingGuid = value; }
    } // Guid


    /// <summary>
    /// The centroid coordinates.
    /// </summary>
    public Point3Df Centroid
    {
      get { return m_centroidCoordinates;  }
      set { m_centroidCoordinates = value; }
    } // Centroid


    /// <summary>
    /// Gets or sets the centroid point converted into DICOM coordinates.
    /// </summary>
    public Point3Df ConvertedCentroid
    {
      get { return m_convertedCentroid;  }
      set { m_convertedCentroid = value; }
    } // ConvertedCentroid


    /// <summary>
    /// The list of sphericity values.
    /// </summary>
    public List<int> SphericityValues
    {
      get { return m_sphericityList;  }
      set { m_sphericityList = value; }
    } // SphericityValues
    
    #endregion

    #region methods

    /// <summary>
    /// Compares two finding's converted centroid points.
    /// </summary>
    /// <param name="lhs">The left hand side.</param>
    /// <param name="rhs">The right hand side.</param>
    /// <returns>The comparison between the converted centroid of lhs and rhs.</returns>
    public static int CompareConvertedCentroidPoints(CadFinding lhs, CadFinding rhs)
    {
      IComparer<Point3Df> comparer = new Point3DfComparer();
      if (lhs != null && rhs != null)
      {
        return comparer.Compare(lhs.ConvertedCentroid, rhs.ConvertedCentroid);
      }
      else
      {
        if (lhs == null && rhs == null)
        {
          return 0;
        }
        else if (lhs != null && rhs == null)
        {
          return 1;
        }
        else
        {
          return -1;
        }
      }
    } // CompareFindingsConvertedCentroidPoints(CadFinding lhs, CadFinding rhs)

    #endregion

  } // CadFinding
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/CadFinding.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $
// $Id: CadFinding.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $

#endregion
