// $Id: Tracker.cs,v 1.3 2007/03/06 19:49:57 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

namespace Viatronix.Console.ImageRedactor
{

  #region enums

  // keep track of which border of the box is to be resized.
  enum RESIZE_BORDER	
  {
    NONE,
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
  }

  #endregion



  #region class Tracker

  /// <summary>
  /// Internal class holds the Tracker Rectangle.
  /// </summary>
  internal class Tracker
  {
    #region fields

    /// <summary>
    /// Holds the Tracker Rectangle object.
    /// </summary>
    private System.Drawing.RectangleF m_tracker = System.Drawing.RectangleF.Empty;

    /// <summary>
    /// Represents the Resize Border.
    /// </summary>
    private RESIZE_BORDER m_resizing = RESIZE_BORDER.NONE;

    #endregion fields

    #region properties

    /// <summary>
    /// Gets/Sets the the track (Left,right,bottom,top)
    /// </summary>
    public RESIZE_BORDER Track
    {
      get{return m_resizing;}
      set{m_resizing = value;}
    }//Track


    #endregion properties


    #region constructors and finalizers

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="x">float</param>
    /// <param name="y">float</param>
    /// <param name="width">float</param>
    /// <param name="height">float</param>
    public Tracker(float x, float y, float width, float height)
    {
      m_tracker = new System.Drawing.RectangleF(x, y, width, height);
    }//Tracker( int x, int y, int width, int height )



    #endregion

    #region methods

    /// <summary>
    /// Check the point is inside Tracker
    /// </summary>
    /// <param name="point">Point</param>
    /// <returns>True if inside track false otherwise</returns>
    public bool IsInsideTracker( System.Drawing.PointF point )
    {
      if( m_tracker.Contains(point.X, point.Y ) )
        return true;
      return false;
    }//IsInsideTracker( System.Drawing.Point point )


    /// <summary>
    /// Draws the Tracker Object
    /// </summary>
    /// <param name="args">Paint event args</param>
    public void DrawTracker( System.Windows.Forms.PaintEventArgs args )
    {
      using ( System.Drawing.Pen trackerPen = new System.Drawing.Pen( RedactorColors.TrackerColor ))
      {
        args.Graphics.DrawRectangle( trackerPen, m_tracker.X, m_tracker.Y, m_tracker.Width, m_tracker.Height );
        args.Graphics.FillRectangle( RedactorBrushes.TrackerBrush, m_tracker );
      }
    }//DrawTracker( System.Windows.Forms.PaintEventArgs args )


    /// <summary>
    /// Checks whether the object is empty or not
    /// </summary>
    /// <returns>bool</returns>
    public bool IsEmpty()
    {
      if ( m_tracker.Width ==0 || m_tracker.Height == 0 )
        return true;
      return false;
    }//IsEmpty()


    #endregion methods

  }// internal class Tracker

  #endregion


  #region struct RedatorColors

  /// <summary>
  /// Class defines Colors for Redacting Pens.
  /// </summary>
  internal struct RedactorColors
  {

    #region properties 

    /// <summary>
    /// Gets the Fill Color
    /// </summary>
    static public System.Drawing.Color FillColor
    {
      get{return System.Drawing.Color.Black;}    
    }//FillColor

    /// <summary>
    /// Gets the Selected Item Drawing Color
    /// </summary>
    static public System.Drawing.Color SelItemDrawColor
    {
      get{return System.Drawing.Color.Red;}    
    }//SelItemDrawColor

    /// <summary>
    /// Gets the Drawing Color
    /// </summary>
    static public System.Drawing.Color DrawColor
    {
      get{return System.Drawing.Color.White;}    
    }//DrawColor


    /// <summary>
    /// Gets the Tracker Color
    /// </summary>
    static public System.Drawing.Color TrackerColor
    {
      get{return System.Drawing.Color.Yellow;}    
    }//TrackerColor

    #endregion

  }//internal struct RedactorColors

  #endregion


  #region struct RedactorBrushes

  /// <summary>
  /// Class Defines Brushes for Redactor
  /// </summary>
  internal struct RedactorBrushes
  {

    #region properties

    /// <summary>
    /// Gets the Fill Brush.
    /// </summary>
    static public System.Drawing.Brush FillBrush
    {
      get{return System.Drawing.Brushes.Black;}    
    }//FillBrush

    /// <summary>
    /// Gets the Drwaing Brush
    /// </summary>
    static public System.Drawing.Brush DrawingBrush
    {
      get
      {
        System.Drawing.Color red = System.Drawing.Color.FromArgb(100,0,0, 0);
        return new System.Drawing.SolidBrush(red);
      }    
    }//DrawingBrush

    /// <summary>
    /// Gets the Tracker Brush
    /// </summary>
    static public System.Drawing.Brush TrackerBrush
    {
      get{return System.Drawing.Brushes.Yellow;}    
    }//TrackerBrush

    #endregion


  }// internal struct RedactorBrushes

  #endregion

}//namespace ImageRedactor



#region revision history

// $Log: Tracker.cs,v $
// Revision 1.3  2007/03/06 19:49:57  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/Tracker.cs,v 1.3 2007/03/06 19:49:57 mkontak Exp $
// $Id: Tracker.cs,v 1.3 2007/03/06 19:49:57 mkontak Exp $

#endregion
