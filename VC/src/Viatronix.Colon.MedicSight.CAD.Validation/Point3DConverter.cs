// $Id: Point3DConverter.cs,v 1.2.2.1 2009-06-29 13:22:25 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@gmail.com]

using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

using Viatronix.Dicom;
using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{

  /// <summary>
  /// Converts the points in the CAD Report from our coordinate system into the DICOM coordinate system.
  /// </summary>
  public class Point3DConverter
  {

    #region fields

    /// <summary>
    /// The patient's position.
    /// </summary>
    private string m_sPatientOrientation = string.Empty;

    /// <summary>
    /// The DICOM reference point.
    /// </summary>
    private Point3Df m_dicomReferencePoint = new Point3Df();

    /// <summary>
    /// The pixel spacing for the X and Y dimensions.
    /// </summary>
    private Point3Df m_dicomPixelSpacing = new Point3Df();

    /// <summary>
    /// Dimensions of the DICOM volume.
    /// </summary>
    private Point3Df m_dicomDimension = new Point3Df();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public Point3DConverter(string dicomPath)
    {
      // Fills out the position, DICOM reference point and spacing as well.
      RetrieveDicomInformation(dicomPath);
    } // Point3DConverter()

    #endregion

    #region properties

    /// <summary>
    /// The DICOM reference point in the upper left hand corner.
    /// </summary>
    public Point3Df ReferencePoint
    {
      get { return m_dicomReferencePoint; }
    } // ReferencePoint


    /// <summary>
    /// The Pixel spacing in each of the three axes.
    /// </summary>
    public Point3Df Units
    {
      get { return m_dicomPixelSpacing; }
    } // Units


    /// <summary>
    /// The dimensions of the DICOM 3D model.
    /// </summary>
    public Point3Df Dimensions
    {
      get { return m_dicomDimension; }
    } // Dimensions


    /// <summary>
    /// The position the patient was scanned in.
    /// </summary>
    public string BodyPosition
    {
      get { return m_sPatientOrientation; }
    } // BodyPosition

    #endregion

    #region methods

    /// <summary>
    /// Creates an element list by looking through the dicomPath for the last file.
    /// </summary>
    /// <param name="dicomPath">The path where the dicom files are located.</param>
    /// <returns>An element list from the last dicom file.</returns>
    public void RetrieveDicomInformation(string dicomPath)
    {
      ImageCollection coll = new ImageCollection();
      coll.AddDirectory(dicomPath, "*.dcm");

      //==========================================================================================
      // Read in the necessary DICOM header information.
      //==========================================================================================

      //------------------------------------------------------------------------------------------------------------------
      // Get the first image based on position (last image based on image number) to retrive the necessary informatation.
      //------------------------------------------------------------------------------------------------------------------

      m_dicomPixelSpacing.X = coll.PixelSpacing[0];
      m_dicomPixelSpacing.Y = coll.PixelSpacing[1];
      m_dicomPixelSpacing.Z = coll.PixelSpacing[2];

      m_dicomDimension.X = coll.Dimensions[0];
      m_dicomDimension.Y = coll.Dimensions[1];
      m_dicomDimension.Z = coll.Dimensions[2];

      m_dicomReferencePoint.X = coll.ReferencePoint[0];
      m_dicomReferencePoint.Y = coll.ReferencePoint[1];
      m_dicomReferencePoint.Z = coll.ReferencePoint[2];

      m_sPatientOrientation = AnalyzePosition(coll.ImageOrientation);
    } // RetrieveDicomInformation(string dicomPath)


    /// <summary>
    /// Analyzes the orientation.
    /// </summary>
    /// <remarks>Grabbed this from Fields_ext3 LoadVolumeFields3</remarks>
    /// <param name="consines">The cosines to draw from.</param>
    /// <returns>The position to use.</returns>
    public string AnalyzePosition(Cosines cosines)
    {
      /// Cosine
      double fcy = cosines.ColumnY;
      double frx = cosines.RowX;

      double fcx = cosines.ColumnX;
      double fry = cosines.RowY;

      if( fcy == 1.0F && frx == 1.0F )
      {
        return "HFS";
      }
      else if( fcy == 1.0F && frx == -1.0F )
      {
        return "FFS";
      }
      else if( fcy == -1.0F && frx == -1.0F )
      {
        return "HFP";
      }
      else if( fcy == -1.0F && frx == 1.0F )
      {
        return "FFP";
      }
      else if( fry == 1.0F && fcx == -1.0F )
      {
        return "HFDR";
      }
      else if( fry == -1.0F && fcx == -1.0F )
      {
        return "FFDR";
      }
      else if( fry == -1.0F && fcx == 1.0F )
      {
        return "HFDL";
      } // else
      else if( fry == 1.0F && fcx == 1.0F )
      {
        return "FFDL";
      } // else
      else
      {
        Log.Warning("Body position could not be identified.", "Point3DConverter", "AnalyzePosition");
        return "unsupport body position";
      }
    } // AnalyzePosition( cosines )


    /// <summary>
    /// Converts Volume World Coordinates to DICOM World Coordinates.
    /// </summary>
    /// <remarks>Copied from the SliceChildView.Region.C ConvertVol2DicomWorldCoordinate.</remarks>
    /// <param name="volWorld">The volume world coordinates.</param>
    /// <returns>The converted DICOM world coordinates.</returns>
    public Point3Df ConvertVolToDICOMCoordinates(Point3Df viVolCoordinate)
    {
      Point3Df viDim = m_dicomDimension;
      Point3Df vfUnits = m_dicomPixelSpacing;
      Point3Df vfWorldRCSdicom = m_dicomReferencePoint;
      string sPatientOrientation = m_sPatientOrientation;

      // The result.
      Point3Df vfWorldCoordinate = new Point3Df();

      vfWorldCoordinate.Z = vfWorldRCSdicom.Z + viVolCoordinate.Z * vfUnits.Z;

      if (sPatientOrientation == "HFS")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X + viVolCoordinate.X * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y + viVolCoordinate.Y * vfUnits.Y;
      }
      else if (sPatientOrientation == "FFS")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X - (viDim.X - viVolCoordinate.X) * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y + viVolCoordinate.Y * vfUnits.Y;
      }
      else if (sPatientOrientation == "HFP")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X - (viDim.X - viVolCoordinate.X) * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y - (viDim.Y - viVolCoordinate.Y) * vfUnits.Y;
      }
      else if (sPatientOrientation == "FFP")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X + viVolCoordinate.X * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y - (viDim.Y - viVolCoordinate.Y) * vfUnits.Y;
      }
      else if (sPatientOrientation == "HFDR")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X - (viDim.X - viVolCoordinate.X) * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y + viVolCoordinate.Y * vfUnits.Y;
      }
      else if (sPatientOrientation == "FFDR")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X - (viDim.X - viVolCoordinate.X) * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y - (viDim.Y - viVolCoordinate.Y) * vfUnits.Y;
      }
      else if (sPatientOrientation == "HFDL")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X + viVolCoordinate.X * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y - (viDim.Y - viVolCoordinate.Y) * vfUnits.Y;
      }
      else if (sPatientOrientation == "FFDL")
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X + viVolCoordinate.X * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y + viVolCoordinate.Y * vfUnits.Y;
      }
      else
      {
        vfWorldCoordinate.X = vfWorldRCSdicom.X + viVolCoordinate.X * vfUnits.X;
        vfWorldCoordinate.Y = vfWorldRCSdicom.Y + viVolCoordinate.Y * vfUnits.Y;
      }

      return vfWorldCoordinate;
    } // ConvertVolToDICOMCoordinates( viVolCoordinate )

    #endregion

  } // Point3DConverter
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.2  2009/06/26 13:04:01  kchalupa
// Convert to .NET 3.5 Framework.
//
// Revision 1.1  2009/05/19 13:39:33  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/Point3DConverter.cs,v 1.2.2.1 2009-06-29 13:22:25 kchalupa Exp $
// $Id: Point3DConverter.cs,v 1.2.2.1 2009-06-29 13:22:25 kchalupa Exp $

#endregion
