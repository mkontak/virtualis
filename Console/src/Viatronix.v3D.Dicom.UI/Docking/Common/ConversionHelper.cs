// $Id: ConversionHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Text;
using System.Drawing;

namespace Viatronix.v3D.Dicom.UI.Docking.Common
{
  public class ConversionHelper
  {
    // Faster performance to cache the converters and type objects, rather
    // than keep recreating them each time a conversion is required
    protected static SizeConverter m_sc = new SizeConverter();
    protected static PointConverter m_pc = new PointConverter();
    protected static Type m_stringType = Type.GetType("System.String");

    public static string SizeToString(Size size)
    {
      return (string)m_sc.ConvertTo(size, m_stringType);
    }

    public static Size StringToSize(string str)
    {
      return (Size)m_sc.ConvertFrom(str);
    }

    public static string PointToString(Point point)
    {
      return (string)m_pc.ConvertTo(point, m_stringType);
    }

    public static Point StringToPoint(string str)
    {
      return (Point)m_pc.ConvertFrom(str);
    }
  }
}

#region revision history

// $Log: ConversionHelper.cs,v $
// Revision 1.1  2005/10/11 14:39:47  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Common/ConversionHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
// $Id: ConversionHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $

#endregion

