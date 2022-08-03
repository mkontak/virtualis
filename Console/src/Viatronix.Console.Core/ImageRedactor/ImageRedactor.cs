// $Id: ImageRedactor.cs,v 1.2.2.4 2008/01/03 21:00:38 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Drawing;



namespace Viatronix.Console.ImageRedactor
{
  /// <summary>
  /// Redacts Images based on associated redact Areas..
  /// </summary>
  public  class ImageRedactor
  {

    #region fields
    
    #endregion fields

    #region properties

    /// <summary>
    /// Gets the Redact Color
    /// </summary>

    #endregion properties

    #region Construction
    
    /// <summary>
    /// Redactor Constructor
    /// </summary>
    private ImageRedactor()
    {
      //
      // TODO: Add constructor logic here
      //
    }//Redactor()
   

    
    #endregion Construction

    #region methods
    /// <summary>
    /// Redacts the Images and put them in to the destination folder
    /// </summary>
    /// <param name="destination">string</param>
    public static void Redact( System.Collections.Hashtable redactTable )
    {
      try
      {
        //Iterate through the Redact Info hash table
        System.Collections.IDictionaryEnumerator redactInfoEnumerator =  redactTable.GetEnumerator();

        while ( redactInfoEnumerator.MoveNext() )
        {
          //Get each file and create Bitmap object from the table
          string fileToRedact  =  ( string ) (redactInfoEnumerator.Key);
          System.Drawing.Bitmap image = new System.Drawing.Bitmap( fileToRedact );

          //Get Redact Rectangle Values 
          using ( System.Drawing.Graphics gfx = System.Drawing.Graphics.FromImage( image ) )
          {
            using ( System.Drawing.Pen blackPen = new System.Drawing.Pen( System.Drawing.Color.Black ) )
            {
              //Draw Redacting Rectangles on all images
              RedactInformation redactInfo = ( RedactInformation ) ( redactInfoEnumerator.Value );
              System.Collections.ArrayList redactRects = ( System.Collections.ArrayList ) redactInfo.RedactionRects;

              foreach ( System.Drawing.RectangleF rect in redactRects )
              {
                System.Drawing.RectangleF redactArea = new RectangleF();

                redactArea.X = UI.Utilities.Interpolate( redactInfo.MinX, rect.X, redactInfo.MaxX, 0.0F, 1.0F ) * image.Width;
                redactArea.Y = UI.Utilities.Interpolate( redactInfo.MinY, rect.Y, redactInfo.MaxY, 0.0F, 1.0F ) * image.Height;

                redactArea.Width = rect.Width / redactInfo.Width * image.Width;
                redactArea.Height = rect.Height / redactInfo.Height * image.Height;

                gfx.FillRectangle( Brushes.Black, redactArea );
                gfx.DrawRectangle( blackPen, redactArea.X, redactArea.Y, redactArea.Width, redactArea.Height );
              }
              blackPen.Dispose();

              if ( redactInfo.Destination != string.Empty )
                image.Save( redactInfo.Destination );
            }
            //dispose resources.
            gfx.Dispose();
            image.Dispose();

          }
        }//while ( redactInfoEnumerator->MoveNext() )
      }
      catch ( Exception ex)
      {
        throw ex;
      }
    }// public void Redact( string destination )


    #endregion methods

  }// public class Redactor
}//namespace ImageRedactor


// $Log: ImageRedactor.cs,v $
// Revision 1.2.2.4  2008/01/03 21:00:38  mkontak
// no message
//
// Revision 1.2.2.3  2007/04/23 21:51:24  romy
// destination file path is part of RedactInformation.  fixed issue#5603
//
// Revision 1.2.2.2  2007/03/26 21:07:17  romy
// fixed issue #5519
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/ImageRedactor.cs,v 1.2.2.4 2008/01/03 21:00:38 mkontak Exp $
// $Id: ImageRedactor.cs,v 1.2.2.4 2008/01/03 21:00:38 mkontak Exp $