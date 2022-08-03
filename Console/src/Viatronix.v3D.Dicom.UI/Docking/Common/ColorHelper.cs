// $Id: ColorHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Win32;

namespace Viatronix.v3D.Dicom.UI.Docking.Common
{
  public class ColorHelper
  {
    public static Color TabBackgroundFromBaseColor(Color backColor)
    {
      Color backIDE;

      // Check for the 'Classic' control color
      if ((backColor.R == 212) &&
        (backColor.G == 208) &&
        (backColor.B == 200))
      {
        // Use the exact background for this color
        backIDE = Color.FromArgb(247, 243, 233);
      }
      else
      {
        // Check for the 'XP' control color
        if ((backColor.R == 236) &&
          (backColor.G == 233) &&
          (backColor.B == 216))
        {
          // Use the exact background for this color
          backIDE = Color.FromArgb(255, 251, 233);
        }
        else
        {
          // Calculate the IDE background color as only half as dark as the control color
          int red = 255 - ((255 - backColor.R) / 2);
          int green = 255 - ((255 - backColor.G) / 2);
          int blue = 255 - ((255 - backColor.B) / 2);
          backIDE = Color.FromArgb(red, green, blue);
        }
      }
                        
      return backIDE;
    }
  }
}



#region revision history

// $Log: ColorHelper.cs,v $
// Revision 1.1  2005/10/11 14:39:47  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Common/ColorHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $
// $Id: ColorHelper.cs,v 1.1 2005/10/11 14:39:47 mkontak Exp $

#endregion

