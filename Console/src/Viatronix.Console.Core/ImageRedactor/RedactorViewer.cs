// $Id: RedactorViewer.cs,v 1.2.2.5 2011/05/18 01:57:01 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;


namespace Viatronix.Console.ImageRedactor
{
  #region Enums
  //Indicates the user actions
  enum ACTION_MODE
  {
    NONE,
    DRAWING,
    RESIZING,
    MOVING
  }

  #endregion Enums

  /// <summary>
  /// Summary description for RedactorViewer.
  /// </summary>
  public class RedactorViewer : System.Windows.Forms.UserControl
  {
    #region fields
    /// <summary> 
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    /// <summary>
    /// List of Redacting Rectangles.
    /// </summary>
    private System.Collections.ArrayList m_rectangles = new ArrayList();

    /// <summary>
    /// Keeps the Mouse down point where the actaul drawing started
    /// </summary>
    private System.Drawing.PointF m_mouseDownPoint = System.Drawing.PointF.Empty;

    /// <summary>
    /// Rectangle holds the temporary RECT shape before adding to the main list
    /// </summary>
    private System.Drawing.RectangleF m_currentRect = System.Drawing.RectangleF.Empty;

    /// <summary>
    /// Representss the user action mode
    /// </summary>
    private ACTION_MODE m_userAction = ACTION_MODE.NONE;

    /// <summary>
    /// Holds the last mouse point.
    /// </summary>
    private System.Drawing.PointF m_lastMousePoint = System.Drawing.Point.Empty;

    /// <summary>
    /// Keeps the selected Rectangles index
    /// </summary>
    private int m_selectedRectIndex = -1;

    /// <summary>
    /// Selected Tracker Index
    /// </summary>
    private int m_selectedTrackerIndex = -1;

    /// <summary>
    /// Holds the tracker list
    /// </summary>
    private System.Collections.ArrayList m_trackers = new ArrayList( 4 );

    /// <summary>
    /// Holds the Image 
    /// </summary>
    private System.Drawing.Bitmap m_image;

    /// <summary>
    /// variable which indicates that there is a bitmap.
    /// </summary>
    private bool m_imageExists = false;

    /// <summary>
    /// Keeps the aspect ratio of the Image.
    /// </summary>
    private System.Drawing.SizeF m_imageBounds = System.Drawing.SizeF.Empty;

    /// <summary>
    /// Image Layout width.
    /// </summary>
    float m_width = 0;

    /// <summary>
    /// Image layout height.
    /// </summary>
    float m_height = 0;

    /// <summary>
    /// Minimum x value.
    /// </summary>
    float m_xMin = 0;

    /// <summary>
    /// Max x Value.
    /// </summary>
    float m_xMax = 0;

    /// <summary>
    /// Minimum y value.
    /// </summary>
    float m_yMin = 0;

    /// <summary>
    /// Max y value.
    /// </summary>
    float m_yMax = 0;

    float m_widthOffset = 0.0F;
    float m_heightOffset = 0.0F;



    #endregion fields

    #region properties

    /// <summary>
    /// Gets or Sets the Redacting area rectangles
    /// </summary>
    public ArrayList RedactAreaRectangles
    {
      get { return m_rectangles; }
      set { m_rectangles = value; }
    }

    /// <summary>
    /// Sets the BackGround Image of the control
    /// </summary>
    public System.Drawing.Bitmap Image
    {
      set
      {
        m_image = value;
        m_imageExists = true;
        m_selectedRectIndex = -1;
        Invalidate();
      }
    }//Image

    #endregion properties

    #region Construction
    /// <summary>
    /// Constructor
    /// </summary>
    public RedactorViewer()
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      //Double buffering eliminates flickering.
      SetStyle( ( ControlStyles ) ( ControlStyles.DoubleBuffer | ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint ), true );
      UpdateStyles();

    }//RedactorViewer()

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if ( disposing )
      {
        if ( components != null )
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }//Dispose( bool disposing )

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      // 
      // RedactorViewer
      // 
      this.Name = "RedactorViewer";
      this.Size = new System.Drawing.Size( 306, 283 );
      this.MouseUp += new System.Windows.Forms.MouseEventHandler( this.OnMouseUpClickHandler );
      this.Paint += new System.Windows.Forms.PaintEventHandler( this.OnPaint );
      this.KeyUp += new System.Windows.Forms.KeyEventHandler( this.OnKeyUp );
      this.MouseMove += new System.Windows.Forms.MouseEventHandler( this.OnMouseMoveEventHandler );
      this.MouseDown += new System.Windows.Forms.MouseEventHandler( this.OnMouseDownClickHandler );

    }
    #endregion

    #endregion Construction

    #region EventHandlers

    /// <summary>
    /// OnPaint 
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {

      //Draw the Image only if the IMage is valid.
      if ( m_imageExists )
        DrawImage( args.Graphics, m_image, ClientRectangle );

      System.Drawing.RectangleF activeRect = System.Drawing.Rectangle.Empty;

      //Draw all Rectangles from the List
      for ( int index = 0; index < m_rectangles.Count; index++ )
      {
        System.Drawing.RectangleF rect = ( System.Drawing.RectangleF ) m_rectangles [ index ];

        //draw the non active rectangles.
        if ( index != m_selectedRectIndex )
        {
          using ( System.Drawing.Pen pen = new Pen( RedactorColors.DrawColor ) )
          {
            pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dot;
            args.Graphics.FillRectangle( RedactorBrushes.FillBrush, rect.X, rect.Y, rect.Width, rect.Height );
            args.Graphics.DrawRectangle( pen, rect.X, rect.Y, rect.Width, rect.Height );
          }
        }
        else
          activeRect = rect;
      }// for ( int index = 0; index < m_rectangles.Count ; index++ )

      //Draw the Active Rect in a different color.
      if ( m_selectedRectIndex != -1 )
      {
        using ( System.Drawing.Pen pen = new Pen( RedactorColors.SelItemDrawColor ) )
        {
          args.Graphics.FillRectangle( RedactorBrushes.DrawingBrush, ( System.Drawing.RectangleF ) activeRect );
          args.Graphics.DrawRectangle( pen, activeRect.X, activeRect.Y, activeRect.Width, activeRect.Height );
          BuildTrackers( ( System.Drawing.RectangleF ) activeRect );
          DrawTrackers( args );
        }
      }

      //Draw the new Rectangle upon mouse move
      if ( m_currentRect != System.Drawing.RectangleF.Empty )
      {
        RectangleF tmp = m_currentRect;
        using ( System.Drawing.Pen pen = new Pen( RedactorColors.SelItemDrawColor ) )
        {
          args.Graphics.FillRectangle( RedactorBrushes.DrawingBrush, tmp );
          args.Graphics.DrawRectangle( pen, tmp.X, tmp.Y, tmp.Width, tmp.Height );
        }
      }
    }// OnPaint( object sender, System.Windows.Forms.PaintEventArgs args )


    /// <summary>
    /// Handles the mouse down click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">MouseEventArgs</param>
    private void OnMouseDownClickHandler(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      m_mouseDownPoint = new System.Drawing.PointF( args.X, args.Y );
      // Viatronix.Logging.Logger.CreateDefaultLogger("c:\\mytext",LogLevel.Highest );

    }//OnMouseDownClickHandler(object sender, System.Windows.Forms.MouseEventArgs e)


    /// <summary>
    /// Handles the mouse Up click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">MouseEventArgs</param>
    private void OnMouseUpClickHandler(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      //Adds the new rectangle to the list

      if ( m_userAction == ACTION_MODE.DRAWING && m_currentRect != System.Drawing.RectangleF.Empty )
        m_rectangles.Add( m_currentRect );

      //reset all areas, points and variables for the next mouse down/move operation.
      m_currentRect = System.Drawing.RectangleF.Empty;
      m_lastMousePoint = System.Drawing.Point.Empty;
      m_mouseDownPoint = System.Drawing.Point.Empty;

      m_selectedRectIndex = -1;
      m_selectedTrackerIndex = -1;

      m_userAction = ACTION_MODE.NONE;
      Invalidate();

      if ( args.Button == MouseButtons.Left )
        Capture = false;


    }//OnMouseUpClickHandler(object sender, System.Windows.Forms.MouseEventArgs e)


    /// <summary>
    /// Handles the mouse move  event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">MouseEventArgs</param>
    private void OnMouseMoveEventHandler(object sender, System.Windows.Forms.MouseEventArgs args)
    {
      if ( args.Button == MouseButtons.Left )
        Capture = true;

      System.Drawing.PointF point = new System.Drawing.PointF( args.X, args.Y );

      //set the rectangle as current if it contains this Point
      if ( m_userAction != ACTION_MODE.RESIZING && m_userAction != ACTION_MODE.MOVING )
        SelectContainedRectangleAsCurrent( point );

      if ( m_userAction == ACTION_MODE.RESIZING || IsInsideTracker( new System.Drawing.PointF( args.X, args.Y ) ) )
      {
        Cursor = GetSelectedTrackerCursor(); // gets the selected Tracker cursor from the selected index
        if ( args.Button == System.Windows.Forms.MouseButtons.Left )
        {
          m_userAction = ACTION_MODE.RESIZING;
          ReSizeRectangle( args.X, args.Y );
          Refresh();
        }
        return;
      }// if ( m_userAction == ACTION_MODE.RESIZING || IsInsideTracker( point ))
      else
        Cursor = System.Windows.Forms.Cursors.Default;


      //If the LEFT mouse button is down, create a New RECT or MOVE selected RECT.
      if ( args.Button == System.Windows.Forms.MouseButtons.Left && m_userAction != ACTION_MODE.RESIZING )
      {
        if ( ( m_userAction != ACTION_MODE.DRAWING ) && ( m_userAction == ACTION_MODE.MOVING || IsRectangleContains( point ) ) )
        {
          Cursor = System.Windows.Forms.Cursors.Cross;
          m_userAction = ACTION_MODE.MOVING;
          MoveRectangle( args.X, args.Y );
        }
        else
        {
          //Creating a new rectangle
          m_userAction = ACTION_MODE.DRAWING;
          CreateRectangle( args );
          //Assume the current selection is none because the new one is not in the list
          m_selectedRectIndex = -1;
        }
        Invalidate();
      }

    }//OnMouseMoveEventHandler(object sender, System.Windows.Forms.MouseEventArgs e)


    /// <summary>
    /// Handles the On Key UP event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">KeyEventArgs</param>
    private void OnKeyUp(object sender, System.Windows.Forms.KeyEventArgs args)
    {
      //On cancel remove the last item from the list
      if ( args.KeyValue == 27 && m_rectangles.Count > 0 )
        m_rectangles.RemoveAt( m_rectangles.Count - 1 );

      //On Del key press remove the selected item from the list
      if ( args.KeyValue == 46 && m_selectedRectIndex != -1 )
        m_rectangles.RemoveAt( m_selectedRectIndex );

      m_selectedRectIndex = -1;
      m_selectedTrackerIndex = -1;
      Cursor = System.Windows.Forms.Cursors.Default;
      Invalidate();

    }//OnKeyUp( object sender, System.Windows.Forms.KeyEventArgs args )


    #endregion EventHandlers

    #region methods

    //Gets the redacting area information
    public void GetRedactAreaInformation(RedactInformation redactInfo)
    {
      redactInfo.MinX = m_xMin;
      redactInfo.MaxX = m_xMax;

      redactInfo.MinY = m_yMin;
      redactInfo.MaxY = m_yMax;

      redactInfo.Width = m_width;
      redactInfo.Height = m_height;

      redactInfo.RedactionRects = ( System.Collections.ArrayList ) ( m_rectangles.Clone() );

    }//GetRedactAreaInformation(RedactInformation redactInfo)

    /// <summary>
    /// Clears the Redact Areas
    /// </summary>
    public void ClearRedactAreas()
    {
      m_rectangles.Clear();
      m_trackers.Clear();
      m_selectedRectIndex = -1;
      m_selectedTrackerIndex = -1;

    }//ClearRedactAreas


    /// <summary>
    /// Draws an image into the specified bounds. The aspect ratio is perserved.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    /// <param name="image">Image object to draw.</param>
    /// <param name="bounds">Bounds to draw image into</param>
    public void DrawImage(Graphics gfx, Image image, Rectangle bounds)
    {
      // size of source image
      SizeF size = new SizeF( image.Width / image.HorizontalResolution, image.Height / image.VerticalResolution );

      // scale and keep aspect ratio of source image
      float scale = Math.Min( bounds.Width / size.Width, bounds.Height / size.Height );
      size.Width *= scale;
      size.Height *= scale;

      // image layout in pixels
      RectangleF layout = new RectangleF( bounds.X + ( bounds.Width - size.Width ) / 2.0F,
        bounds.Y + ( bounds.Height - size.Height ) / 2.0F,
        size.Width, size.Height );

      // draw the image into the layout
      gfx.DrawImage( image, layout.X, layout.Y, layout.Width, layout.Height );


      m_width = layout.Width;
      m_height = layout.Height;

      //Keep the minimum and maximum X values.
      m_xMin = ( bounds.Width - size.Width ) / 2.0F;//layout.X / m_width ;
      m_xMax = this.Width - m_xMin;// ( layout.X + layout.Width ) / m_width;

      m_yMin = ( bounds.Height - size.Height ) / 2.0F;//layout.Y / m_height ;
      m_yMax = this.Height - m_yMin;// ( layout.Y + layout.Height ) / m_height;

      m_widthOffset = ( this.Width - m_width ) / 2.0F;
      m_heightOffset = ( this.Height - m_height ) / 2.0F;

    } // DrawImage( gfx, image, bounds, selected )


    /// <summary>
    /// Moves the rec
    /// </summary>
    /// <param name="X"></param>
    /// <param name="Y"></param>
    private void MoveRectangle(float X, float Y)
    {
      //set the last mouse point 
      if ( m_lastMousePoint == Point.Empty )
        m_lastMousePoint = new System.Drawing.PointF( X, Y );

      //alter the selected rectangles position.
      System.Drawing.RectangleF rect = ( ( System.Drawing.RectangleF ) m_rectangles [ m_selectedRectIndex ] );
      rect.Offset( ( X - m_lastMousePoint.X ), ( Y - m_lastMousePoint.Y ) );

      //Adjust x, y coordinates on move
      if ( rect.X < m_xMin )
        rect.X = m_xMin;

      if ( rect.Y < m_yMin )
        rect.Y = m_yMin;

      if ( ( rect.X + rect.Width > m_xMax ) )
        rect.X = m_xMax - rect.Width;

      if ( ( rect.Y + rect.Height > m_yMax ) )
        rect.Y = m_yMax - rect.Height;

      if ( m_selectedRectIndex != -1 )
        m_rectangles [ m_selectedRectIndex ] = rect;

      //Set the last mouse point
      m_lastMousePoint = new System.Drawing.PointF( X, Y );
      Invalidate();

    }//MoveRectangle( int X, int Y )

    /// <summary>
    /// Resizes the rectangle
    /// </summary>
    /// <param name="X">float</param>
    /// <param name="Y">float</param>
    private void ReSizeRectangle(float X, float Y)
    {
      if ( X < m_xMin ) X = m_xMin;
      if ( Y < m_yMin ) Y = m_yMin;
      if ( X > m_xMax ) X = m_xMax;
      if ( Y > m_yMax ) Y = m_yMax;

      try
      {
        System.Drawing.RectangleF rect = ( System.Drawing.RectangleF ) ( m_rectangles [ m_selectedRectIndex ] );

        if ( m_selectedTrackerIndex == -1 ) return;
        Tracker tracker = ( Tracker ) m_trackers [ m_selectedTrackerIndex ];

        switch ( tracker.Track )
        {
          case RESIZE_BORDER.TOP:
            {
              rect.Height -= ( Y - rect.Y );
              rect.Y = Y;
              break;
            }
          case RESIZE_BORDER.RIGHT:
            {
              rect.Width += ( X - rect.Right );
              break;
            }
          case RESIZE_BORDER.BOTTOM:
            {
              rect.Height += ( Y - rect.Bottom );
              break;
            }
          case RESIZE_BORDER.LEFT:
            {
              rect.Width -= ( X - rect.X );
              rect.X = X;
              break;
            }
          default:
            break;
        }


        if ( rect.Width < 0 || rect.Height < 0 ) return;


        m_rectangles [ m_selectedRectIndex ] = rect;

      }
      catch ( Exception )
      {
      }

    }//ReSizeRectangle( int X, int Y )



    /// <summary>
    /// Creates the rectangle
    /// </summary>
    /// <param name="args">MouseEventArgs</param>
    private void CreateRectangle(System.Windows.Forms.MouseEventArgs args)
    {

      //On the Right mouse down move redraw rectangle
      float minX = System.Math.Min( m_mouseDownPoint.X, args.X );// -m_widthOffset;
      //minX /= m_width;

      float minY = System.Math.Min( m_mouseDownPoint.Y, args.Y );// -m_heightOffset;
      // minY /= m_height;

      //Calculate the next corodinates
      float maxX = System.Math.Max( m_mouseDownPoint.X, args.X );// -m_widthOffset;
      //maxX /= m_width;

      float maxY = System.Math.Max( m_mouseDownPoint.Y, args.Y );// -m_heightOffset;
      //m/axY /= m_height;


      //If outside the bounds adjust the edges to start /end
      if ( minX < m_xMin ) minX = m_xMin;
      if ( maxX > m_xMax ) maxX = m_xMax;
      if ( minY < m_yMin ) minY = m_yMin;
      if ( maxY > m_yMax ) maxY = m_yMax;

      //If not Zero m_width create the Rectangle
      if ( ( maxX - minX != 0 ) && ( maxY - minY != 0 ) )
        m_currentRect = new System.Drawing.RectangleF( minX, minY, maxX - minX, maxY - minY );

    }//CreateNewRectangle( System.Windows.Forms.MouseEventArgs e )


    /// <summary>
    /// Check whether the point is inside a rectangle also set that as the current selected Rectangle
    /// </summary>
    /// <param name="point">Point</param>
    /// <returns></returns>
    private bool IsRectangleContains(System.Drawing.PointF point)
    {
      //Here take the rectangle which is the smallest in area of selection.
      float smallestArea = 0;
      bool bFound = false;

      for ( int index = 0; index < m_rectangles.Count; index++ )
      {
        System.Drawing.RectangleF rect = ( System.Drawing.RectangleF ) m_rectangles [ index ];
        if ( rect.Contains( point.X, point.Y ) )
        {
          bFound = true;
          //Pick the rectangle which have the smallest area
          if ( smallestArea == 0 || ( smallestArea > ( rect.Width * rect.Height ) ) )
          {
            smallestArea = rect.Width * rect.Height;
            m_selectedRectIndex = index;
          }
        }
      }
      return bFound;

    }//IsRectangleContains( System.Drawing.Point point )

    /// <summary>
    /// Sets the current Selected Rectangle which contains this point
    /// </summary>
    /// <param name="point">Point</param>
    /// <returns></returns>
    private void SelectContainedRectangleAsCurrent(System.Drawing.PointF point)
    {
      //Here take the rectangle which is the smallest in area of selection.
      float smallestArea = 0;
      m_selectedRectIndex = -1;
      for ( int index = 0; index < m_rectangles.Count; index++ )
      {
        System.Drawing.RectangleF rect = ( System.Drawing.RectangleF ) m_rectangles [ index ];
        if ( rect.Contains( point.X, point.Y ) )
        {
          //Pick the rectangle which have the smallest area
          if ( smallestArea == 0 || ( smallestArea > ( rect.Width * rect.Height ) ) )
          {
            smallestArea = rect.Width * rect.Height;
            m_selectedRectIndex = index;
          }
        }
      }
      return;

    }//IsRectangleContains( System.Drawing.Point point )


    #region Tracker Methods

    /// <summary>
    /// Build TOP,MID,LEFT,RIGHT trackers
    /// </summary>
    /// <param name="selectedRect"></param>
    void BuildTrackers(System.Drawing.RectangleF selectedRect)
    {
      m_trackers.Clear();

      //Here we calculate the width and height of the Tracker Rectangle
      float trackerWidth = 7.0F;
      float trackerHeight = 7.0F;

      //add Top Tracker
      Tracker tracker = new Tracker( ( selectedRect.Location.X + selectedRect.Width / 2 ), selectedRect.Top, trackerWidth, trackerHeight );
      tracker.Track = RESIZE_BORDER.TOP;
      m_trackers.Add( tracker );

      //add Left Tracker
      tracker = new Tracker( selectedRect.Location.X, ( selectedRect.Top + selectedRect.Height / 2 ) - 2.0F, trackerWidth, trackerHeight );
      tracker.Track = RESIZE_BORDER.LEFT;
      m_trackers.Add( tracker );


      //add bottom Tracker
      tracker = new Tracker( ( selectedRect.Location.X + selectedRect.Width / 2 ), selectedRect.Bottom - 6.0F, trackerWidth, trackerHeight );
      tracker.Track = RESIZE_BORDER.BOTTOM;
      m_trackers.Add( tracker );

      //add Right Tracker
      tracker = new Tracker( selectedRect.Right - 6.0F, ( selectedRect.Top + selectedRect.Height / 2 ) - 2.0F, trackerWidth, trackerHeight );
      tracker.Track = RESIZE_BORDER.RIGHT;
      m_trackers.Add( tracker );

    }//BuildTrackers( System.Drawing.RectangleF selectedRect )


    /// <summary>
    /// Draw the trackers on top of the selected Rectangle
    /// </summary>
    void DrawTrackers(System.Windows.Forms.PaintEventArgs args)
    {
      for ( int index = 0; index < m_trackers.Count; index++ )
      {
        Tracker tracker = ( Tracker ) m_trackers [ index ];
        tracker.DrawTracker( args );
      }
    }//DrawTrackers( System.Windows.Forms.PaintEventArgs args )


    /// <summary>
    /// Checcks whether the point is inside Tracker and sets the SelectedTracker Index as the index of this Tracker object
    /// </summary>
    /// <returns></returns>
    private bool IsInsideTracker(System.Drawing.PointF point)
    {
      for ( int index = 0; index < m_trackers.Count; index++ )
      {
        Tracker tracker = ( Tracker ) m_trackers [ index ];
        if ( tracker.IsInsideTracker( point ) )
        {
          m_selectedTrackerIndex = index;
          return true;
        }
      }
      return false;
    }//IsInsideTracker( System.Drawing.Point point )


    /// <summary>
    /// Determines the Tracker cursor upon point selection
    /// </summary>
    /// <param name="point">Point</param>
    /// <returns>Cursor</returns>
    System.Windows.Forms.Cursor GetSelectedTrackerCursor()
    {
      Tracker tracker = ( Tracker ) m_trackers [ m_selectedTrackerIndex ];

      if ( tracker.Track == RESIZE_BORDER.TOP || tracker.Track == RESIZE_BORDER.BOTTOM )
        return Cursors.SizeNS;

      if ( tracker.Track == RESIZE_BORDER.LEFT || tracker.Track == RESIZE_BORDER.RIGHT )
        return Cursors.SizeWE;

      return Cursors.Default;

    }//GetSelectedTrackerCursor(  )


    #endregion Tracker Methods

    #endregion methods

  }//public class RedactorViewer : System.Windows.Forms.UserControl
}//namespace ImageRedactor




// $Log: RedactorViewer.cs,v $
// Revision 1.2.2.5  2011/05/18 01:57:01  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2.2.4  2007/04/23 21:58:57  romy
// retrieval of RedactInformation modified.  fixed issue#5603
//
// Revision 1.2.2.3  2007/03/27 01:07:23  romy
// Issue# 5521
//
// Revision 1.2.2.2  2007/03/26 21:07:17  romy
// fixed issue #5519
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/RedactorViewer.cs,v 1.2.2.5 2011/05/18 01:57:01 mkontak Exp $
// $Id: RedactorViewer.cs,v 1.2.2.5 2011/05/18 01:57:01 mkontak Exp $