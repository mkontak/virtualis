// $Id: vxDataColormapEditor.h,v 1.1 2005/05/23 20:24:31 michael Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Michael Meissner (michael@viatronix.com)

/**
 *  This class contains all  data that is needed 
 *  between the different viewers in the color map editor.
 */

#ifndef vxDataColormapEditor_h
#define vxDataColormapEditor_h


// includes
#include "vxElement.h"
#include "Point2D.h"
#include "PixelRGBA.h"


// forward declarations
template <class T> class Image;


// class definition
class VX_VIEWER_DLL vxDataColormapEditor : public vxElement
{
// methods
public:

  /// constructor
  vxDataColormapEditor();

  /// destructor
  virtual ~vxDataColormapEditor();

  /// get location of selected color in image
  inline const Point2D<float4> & GetColorLocation() { return m_colormapLocation; }

  /// set location of selected color in image
  inline void SetColorLocation(const Point2D<float4> & colormapLocation) { m_colormapLocation = colormapLocation; }

  /// get the currently selected color control point; -1 means no selection
  inline const int4 & GetSelectedColorPointIndex()  const { return m_iSelectedColorPointIndex; }

  /// set the currently selected color control point; -1 means no selection
  inline void SetSelectedColorPointIndex(const int4 iSelectedColorPointIndex) { m_iSelectedColorPointIndex = iSelectedColorPointIndex; }
  
  /// get the currently selected alpha control point; -1 means no selection
  inline const int4 & GetSelectedAlphaPointIndex()  const { return m_iSelectedAlphaPointIndex; }
  
  /// set the currently selected alpha control point; -1 means no selection
  inline void SetSelectedAlphaPointIndex(const int4 iSelectedAlphaPointIndex) { m_iSelectedAlphaPointIndex = iSelectedAlphaPointIndex; }
  
  /// get image pointer
  inline Image<PixelRGBA<uint1> > * GetImagePtr() { return m_pImage; };
  
// variables
private:
  
  /// The currently selected color control point; -1 means no selection.
  int4 m_iSelectedColorPointIndex;

  /// The currently selected alpha control point; -1 means no selection.
  int4 m_iSelectedAlphaPointIndex;

  /// The location of the color on the colormap image
  Point2D<float4> m_colormapLocation;

  /// pointer to the image
  static Image<PixelRGBA<uint1> > * m_pImage;
};


#endif // vxDataColormapEditor_h


// $Log: vxDataColormapEditor.h,v $
// Revision 1.1  2005/05/23 20:24:31  michael
// Introduced concept of data objects to start with vxData....
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.7.2.1  2004/02/24 19:52:16  michael
// maded m_pImage a static variable so that we are not initalizing it multiple times
//
// Revision 1.7  2003/08/13 12:43:42  michael
// added derivation for 70_vxUtils
//
// Revision 1.6  2003/06/19 20:20:41  michael
// moved modified stuff to vxUtils ...
//
// Revision 1.5  2003/06/04 19:02:35  jmeade
// code review.
//
// Revision 1.4  2003/05/16 11:45:00  michael
// code review
//
// Revision 1.3  2003/04/19 01:49:08  michael
// some more code review
//
// Revision 1.2  2003/03/30 20:58:32  michael
// formatting and cosmetics
//
// Revision 1.1  2002/10/04 18:10:49  michael
// renaming vxColormapEditorSharedData --> vxDataColormapEditor
//
// Revision 1.2  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.1  2002/09/19 15:12:56  michael
// Added vxColormap and vxDataColormapEditor
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataColormapEditor.h,v 1.1 2005/05/23 20:24:31 michael Exp $
// $Id: vxDataColormapEditor.h,v 1.1 2005/05/23 20:24:31 michael Exp $
