// $Id: RedactInformation.cs,v 1.1.2.2 2007/04/23 21:50:42 romy Exp $ 
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
using System.Text;


namespace Viatronix.Console.ImageRedactor
{
  /// <summary>
  /// Class represents image data such as co-ordinates based on the control.
  /// </summary>
  public class RedactInformation
  {
    #region fields

    /// <summary>
    /// X Minimum
    /// </summary>
    private float m_xMin;

    /// <summary>
    /// X Maximum
    /// </summary>
    private float m_xMax;

    /// <summary>
    /// Y Minimum
    /// </summary>
    private float m_yMin;

    /// <summary>
    /// Y Maximum
    /// </summary>
    private float m_yMax;

    /// <summary>
    /// Width
    /// </summary>
    private float m_width;

    /// <summary>
    /// Height
    /// </summary>
    private float m_height;
    
    /// <summary>
    /// redact areas
    /// </summary>
    private System.Collections.ArrayList m_rectangles = new ArrayList();

    /// <summary>
    /// Destination File path 
    /// </summary>
    private string m_destination;

    #endregion

    #region properties

    /// <summary>
    /// Destination File path 
    /// </summary>
    public string Destination
    {
      get { return m_destination; }
      set { m_destination = value; }
    }//Destination

    /// <summary>
    /// Redact areas
    /// </summary>
    public System.Collections.ArrayList RedactionRects
    {
      get { return m_rectangles; }
      set { m_rectangles = value; }
    }//RedactionRects

    /// <summary>
    /// MinX
    /// </summary>
    public float MinX
    {
      get { return m_xMin; }
      set { m_xMin = value; }
    }//MinX

    /// <summary>
    /// MaxX
    /// </summary>
    public float MaxX
    {
      get { return m_xMax; }
      set { m_xMax = value; }
    }//MaxX

    /// <summary>
    /// MinY
    /// </summary>
    public float MinY
    {
      get { return m_yMin; }
      set { m_yMin = value; }
    }//MinY

    
    /// <summary>
    /// MaxY
    /// </summary>
    public float MaxY
    {
      get { return m_yMax; }
      set { m_yMax = value; }
    }//MaxY

    /// <summary>
    /// Width
    /// </summary>
    public float Width
    {
      get { return m_width; }
      set { m_width = value; }
    }//Width

    /// <summary>
    /// Height
    /// </summary>
    public float Height
    {
      get { return m_height; }
      set { m_height = value; }
    }//Height

    #endregion

    #region construction

    /// <summary>
    /// Construction
    /// </summary>
    /// <param name="destFile"></param>
    public RedactInformation( string destFile )
    {
      m_destination = destFile;
    }//RedactInformation( string destFile )

    #endregion


  }//  RedactInformation
}//namespace Viatronix.Console.ImageRedactor


// $Log: RedactInformation.cs,v $
// Revision 1.1.2.2  2007/04/23 21:50:42  romy
// added destination file path to fix issue#5603
//
// Revision 1.1.2.1  2007/03/26 21:07:56  romy
// fixed issue #5519
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/RedactInformation.cs,v 1.1.2.2 2007/04/23 21:50:42 romy Exp $
// $Id: RedactInformation.cs,v 1.1.2.2 2007/04/23 21:50:42 romy Exp $