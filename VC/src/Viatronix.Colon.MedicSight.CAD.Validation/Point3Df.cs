// $Id: Point3Df.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
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
using System.Windows.Forms;

using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{

  #region Point3Df Class

  /// <summary>
  /// Represents a point in 3 space that can be read in from the CAD file.
  /// </summary>
  public class Point3Df
  {

    #region constants

    /// <summary>
    /// The spaces between coordinates.
    /// </summary>
    private readonly int SPACES = 13;

    #endregion

    #region fields

    /// <summary>
    /// The X Coordinate.
    /// </summary>
    private double m_xCoord = 0;

    /// <summary>
    /// The Y Coordinate.
    /// </summary>
    private double m_yCoord = 0;

    /// <summary>
    /// The Z Coordinate.
    /// </summary>
    private double m_zCoord = 0;

    /// <summary>
    /// The units the coordinates are in.
    /// </summary>
    private string m_units = string.Empty;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the X coordinate.
    /// </summary>
    public double X
    {
      get { return m_xCoord;  }
      set { m_xCoord = value; }
    } // X


    /// <summary>
    /// Gets or sets the Y coordinate.
    /// </summary>
    public double Y
    {
      get { return m_yCoord; }
      set { m_yCoord = value; }
    } // Y


    /// <summary>
    /// Gets or sets the Z coordinate.
    /// </summary>
    public double Z
    {
      get { return m_zCoord; }
      set { m_zCoord = value; }
    } // Z


    /// <summary>
    /// The units the coordinates are in.
    /// </summary>
    public string Units
    {
      get { return m_units;  }
      set { m_units = value; }
    } // Units

    #endregion

    #region methods

    /// <summary>
    /// Checks to see if this point and the other point refer to the same location in 3 space.
    /// If X and Y in both points are equal and | m_zCoord - other.m_zCoord | is less than threshold returns true.
    /// </summary>
    /// <param name="other">The point to check for equality.</param>
    /// <param name="threshold">The threshold to allow for.</param>
    /// <returns>True if the two points refer to the same location, false otherwise.</returns>
    public bool IsApproxSameLocation(Point3Df other, double threshold)
    {
      // Ensure that the distance between points are less than the threshold.
      if (this.GetEuclideanDistance(other) < threshold)
      {
        return true;
      }

      return false;
    } // IsApproxSameLocation(Point3Df other)


    /// <summary>
    /// Calculate the difference between the two points.
    /// </summary>
    /// <param name="other">The other point.</param>
    /// <returns>The difference between the two points.</returns>
    public double CalculateDifference(Point3Df other)
    {
      return this.GetEuclideanDistance(other);
    } // CalculateDifference( other )


    /// <summary>
    /// Finds the Euclidean distance between two points.
    /// </summary>
    /// <param name="other">The other point in the equation.</param>
    /// <returns>The distance between this and the other point.</returns>
    public double GetEuclideanDistance(Point3Df other)
    {
      return Math.Sqrt(Math.Pow((m_xCoord - other.m_xCoord), 2) + Math.Pow((m_yCoord - other.m_yCoord), 2) +
        Math.Pow((m_zCoord - other.m_zCoord), 2));
    } // GetEuclideanDistance(Point3Df other)


    /// <summary>
    /// Returns a string representation of this object.
    /// </summary>
    /// <returns>A string representation of this object.</returns>
    public override string ToString()
    {
      return m_xCoord + Utils.GenerateSpacing(m_xCoord, SPACES) + m_yCoord + Utils.GenerateSpacing(m_yCoord, SPACES) + m_zCoord + Utils.GenerateSpacing(m_zCoord, SPACES) + m_units;
    } // ToString()


    /// <summary>
    /// Returns a string representation of this object.
    /// </summary>
    /// <param name="decimals">The number of decimals to round to for all three coordinates.</param>
    /// <returns>A string representation of this object.</returns>
    public string ToString(int decimals)
    {
      return Math.Round(m_xCoord, decimals) + Utils.GenerateSpacing(Math.Round(m_xCoord, decimals), SPACES) + Math.Round(m_yCoord, decimals) + Utils.GenerateSpacing(Math.Round(m_yCoord, decimals), SPACES) + Math.Round(m_zCoord, decimals) + Utils.GenerateSpacing(Math.Round(m_zCoord, decimals), SPACES) + m_units;
    } // ToString()


    /// <summary>
    /// Reads an XmlNode and returns a Point3D object based on it.
    /// </summary>
    /// <param name="node">A node that contains information about a Point3D.</param>
    /// <returns>A Point3D object based on the XmlNode.</returns>
    public static Point3Df FromXml(XmlNode node)
    {
      XmlAttribute units = node.Attributes["units"];
      XmlAttribute x = node.Attributes["x"];
      XmlAttribute y = node.Attributes["y"];
      XmlAttribute z = node.Attributes["z"];

      if (x != null && y != null && z != null)
      {
        Point3Df point = new Point3Df();

        point.m_xCoord = XmlConvert.ToDouble(x.Value);
        point.m_yCoord = XmlConvert.ToDouble(y.Value);
        point.m_zCoord = XmlConvert.ToDouble(z.Value);

        if(units != null)
        {
          point.Units = units.Value;
        }

        return point;
      }

      Log.Error("Point could not be read in.  X, y, z or units attributes were missing from node.", "Point3Df", "FromXml");
      throw new XmlException("Point could not be read in.  X, y, z or units attributes were missing from node.");
    } // FromXml(XmlNode node)

    #endregion

  } // Point3Df

  #endregion

  #region Point3DfComparer

  /// <summary>
  /// Compares two Point3Df objects and sorts on the Z axis.
  /// </summary>
  public class Point3DfComparer : IComparer<Point3Df>
  {

    #region methods

    int IComparer<Point3Df>.Compare(Point3Df lhs, Point3Df rhs)
    {
      if (lhs != null && rhs != null)
      {
        //=============================================
        // Should be exactly equal to qualify.
        //=============================================

        if (lhs.IsApproxSameLocation(rhs, 0))
        {
          return 0;
        }
        else
        {
          //===========================================================================================================
          // If Z is not equal, compare based on it.  If Z is equal compare based on Y.  If Y is equal then sort on X.
          //============================================================================================================

          if (lhs.Z == rhs.Z)
          {
            if (lhs.Y == rhs.Y)
            {
              return lhs.X.CompareTo(rhs.X);
            }
            else
            {
              return lhs.Y.CompareTo(rhs.Y);
            }
          }
          else
          {
            return lhs.Z.CompareTo(rhs.Z);
          }
        }
      }
      else
      {
        //==================================================================
        // Check null states.
        //==================================================================
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
    } // Compare(Point3Df lhs, Point3Df rhs)

    #endregion

  } // Point3DfComparer

  #endregion

} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/Point3Df.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: Point3Df.cs,v 1.1.4.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
