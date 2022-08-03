// $Id: Shapes.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright (c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: George Economos (george@viatronix.com)
//         frank "trig" dachille( frank@viatronix.net )

using System;
using System.Drawing;
using System.Collections;

namespace Viatronix.UI
{
  #region edges
  
  /// <summary>
  /// Specifies which corners are to be rounded.
  /// </summary>
  [ Flags ]
  public enum Edges
  {      
    /// <summary>
    /// Top-Left corner of rectangle should be rounded.
    /// </summary>
    TopLeft     = 0x0001,
      
    /// <summary>
    /// Top-Right corner of rectangle should be rounded.
    /// </summary>
    TopRight    = 0x0002,
      
    /// <summary>
    /// Bottom-Right corner of rectangle should be rounded.
    /// </summary>
    BottomRight = 0x0004,
      
    /// <summary>
    /// Bottom-left corner of rectangle should be rounded.
    /// </summary>
    BottomLeft  = 0x0008,

    /// <summary>
    /// All corners of recangle should be rounded.
    /// </summary>
    All         = 0xFFFF,
  }; // enum Edges

  
  #endregion
  
  /// <summary>
	/// Utility class that create point arrays for common shapes.
	/// </summary>
	public class Shapes
	{
    #region constants

    /// <summary>
    /// cos( 3*PI / 8 )
    /// </summary>
    static private readonly float COS_3PI_OVER_8 = ( float )Math.Cos( 3*Math.PI/8 );
    
    /// <summary>
    /// sin( 3*PI / 8 )
    /// </summary>
    static private readonly float SIN_3PI_OVER_8 = ( float )Math.Sin( 3*Math.PI/8 );
    
    /// <summary>
    /// cos( PI / 4 )
    /// </summary>
    static private readonly float COS_PI_OVER_4  = ( float )Math.Cos( Math.PI/4 );
    
    /// <summary>
    /// sin( PI / 4 )
    /// </summary>
    static private readonly float SIN_PI_OVER_4  = ( float )Math.Sin( Math.PI/4 );
    
    /// <summary>
    /// cos( PI / 8 )
    /// </summary>
    static private readonly float COS_PI_OVER_8  = ( float )Math.Cos( Math.PI/8 );
    
    /// <summary>
    /// sin( PI / 8 )
    /// </summary>
    static private readonly float SIN_PI_OVER_8  = ( float )Math.Sin( Math.PI/8 ); 
    
    #endregion

    #region BeveledEdge

    /// <summary>
    /// Returns an array of points defining a rectangle with rounded corners.
    /// </summary>
    /// <param name="bounds">Bounds for shape in pixels.</param>
    /// <param name="edges">One of the Edges enumeration values indicating which corners to round.</param>
    /// <param name="bevel">Amount of rounding for corners.</param>
    /// <returns>Array of PointF structures describing shape.</returns>
    static public PointF[] BeveledEdge( Rectangle bounds, Edges edges, int bevel )
    {
      // create an array list to hold the points that defne the shape
      ArrayList points = new ArrayList();
      
      // are we beveling the top left corner?
      if( ( edges & Edges.TopLeft ) > 0 )
        points.Add( new PointF( bounds.Left + bevel,  bounds.Top  ) );
      else
        points.Add( new PointF( bounds.Left,  bounds.Top  ) );
     
      // are we beveling the top right corner?
      if( ( edges & Edges.TopRight ) > 0 )
      {
        float xCenter = bounds.Right - bevel;
        float yCenter = bounds.Top + bevel;  
        
        points.Add( new PointF( bounds.Right - bevel + 1, bounds.Top ) );    
        points.Add( new PointF( xCenter + bevel * COS_3PI_OVER_8, yCenter - bevel * SIN_3PI_OVER_8 ) );
        points.Add( new PointF( xCenter + bevel * COS_PI_OVER_4,  yCenter - bevel * SIN_PI_OVER_4 ) );
        points.Add( new PointF( xCenter + bevel * COS_PI_OVER_8,  yCenter - bevel * SIN_PI_OVER_8 ) );
      } // if( ( edges & Edges.TopRight ) > 0 )
      else
        points.Add( new PointF( bounds.Right, bounds.Top ) );

      
      // are we beveling the bottom right corner?
      if( ( edges & Edges.BottomRight ) > 0 )
      {
        points.Add( new PointF( bounds.Right, bounds.Bottom - bevel - 1 ) );
        
        float xCenter = bounds.Right  - bevel ;
        float yCenter = bounds.Bottom - bevel;
      
        points.Add( new PointF( xCenter + bevel * COS_PI_OVER_8,  yCenter + bevel * SIN_PI_OVER_8 ) );
        points.Add( new PointF( xCenter + bevel * COS_PI_OVER_4,  yCenter + bevel * SIN_PI_OVER_4 ) );
        points.Add( new PointF( xCenter + bevel * COS_3PI_OVER_8, yCenter + bevel * SIN_3PI_OVER_8 ) );
        points.Add( new PointF( bounds.Right - bevel, bounds.Bottom ) );    
      } // if( ( edges & Edges.BottomRight ) > 0 )
      else
        points.Add( new PointF( bounds.Right, bounds.Bottom ) );    
      
      
      // are we beveling the bottom left corner?
      if( ( edges & Edges.BottomLeft ) > 0 )
      {
        points.Add( new PointF( bounds.Left + bevel, bounds.Bottom ) );
      
        float xCenter = bounds.Left + bevel;
        float yCenter = bounds.Bottom - bevel;
      
        points.Add( new PointF( xCenter - bevel * COS_3PI_OVER_8, yCenter + bevel * SIN_3PI_OVER_8 ) );
        points.Add( new PointF( xCenter - bevel * COS_PI_OVER_4,  yCenter + bevel * SIN_PI_OVER_4 ) );
        points.Add( new PointF( xCenter - bevel * COS_PI_OVER_8,  yCenter + bevel * SIN_PI_OVER_8 ) );
      
        points.Add( new PointF( bounds.Left, bounds.Bottom - bevel ) );
      } // if( ( edges & Edges.BottomLeft ) > 0 )
      else
        points.Add( new PointF( bounds.Left, bounds.Bottom ) );
      
      // are we beveling the top left corner?
      if( ( edges & Edges.TopLeft ) > 0 )
      {
        points.Add( new PointF( bounds.Left, bounds.Top + bevel ) );

        float xCenter = bounds.Left + bevel;
        float yCenter = bounds.Top + bevel;
    
        points.Add( new PointF( xCenter - bevel * COS_PI_OVER_8,  yCenter - bevel * SIN_PI_OVER_8 ) );
        points.Add( new PointF( xCenter - bevel * COS_PI_OVER_4,  yCenter - bevel * SIN_PI_OVER_4 ) );
        points.Add( new PointF( xCenter - bevel * COS_3PI_OVER_8, yCenter - bevel * SIN_3PI_OVER_8 ) );
      } // if( ( edges & Edges.TopLeft ) > 0 )
      else
        points.Add( new PointF( bounds.Left, bounds.Top ) );

      // create a PointF array from the array list
      PointF[] shape = new PointF[ points.Count ];
      for( int index = 0; index < points.Count; ++index )
        shape[ index ] = ( PointF )points[ index ];
      
      // return the shape
      return shape;
    
    } // BeveledEdge( bounds, edges, bevel )
    
    
    #endregion
    
    #region Rectangle
    
    /// <summary>
    /// Returns an array of points defining a rectangle.
    /// </summary>
    /// <param name="bounds">Bounds for shape in pixels.</param>
    /// <returns>Array of PointF structures describing shape.</returns>
    static public PointF[] Rectangle( Rectangle bounds )
    {
      PointF[] shape = new PointF[ 4 ];      
      shape[0] = new PointF( bounds.Left,       bounds.Top );
      shape[1] = new PointF( bounds.Width - 1,  bounds.Top );
      shape[2] = new PointF( bounds.Width - 1,  bounds.Height - 1 );
      shape[3] = new PointF( bounds.Left,       bounds.Height - 1 );
      return shape;
    } // Rectangle( bounds )


    #endregion
  
  } // class Shapes
} // namespace Viatronix.UI

#region revision history

// $Log: Shapes.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2004/03/02 01:10:40  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.5.4.1  2004/02/12 21:12:36  geconomos
// Moved public nested classes from  to namespace level.
//
// Revision 1.5  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.4  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.3  2003/04/24 14:26:35  frank
// polygon -> shape
//
// Revision 1.2  2003/04/24 12:32:03  frank
// Code review
//
// Revision 1.1  2003/04/23 15:01:40  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Shapes.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: Shapes.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion
