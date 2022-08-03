// $Id: vxColormap.h,v 1.4 2006/08/23 19:29:27 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  This class contains all data necessary to wrap a colormap
  which is a window/level, a opacity curve, and a color spectrum
  that is mapped onto the window/level range. Originally, we had
  a transfer function but that code became redundant but some 
  made it into this class ...
*/

// pragmas
#pragma once


// includes
#include "Point2D.h"
#include "vxWindowLevel.h"
#include "Enums.h"
#include "vxElement.h"


// forward declarations
class ZeroOne;
class vxWindowLevel;
class vxColorTable;
class vxColorTableCPU;
class vxVolumeHeader;
class vxDOMElement;


// class definition
class VX_VIEWER_DLL vxColormap : public vxElement
{
// functions
public:

  /// constructor
  vxColormap();

  /// copy constructor
  vxColormap(const vxColormap & other);

  /// destructor
  ~vxColormap();

  /// assignment operator
  const vxColormap & operator=(const vxColormap & other);

  /// get title
  const std::string & GetName() const { return m_sName; }

  /// set title
  void SetName(std::string & name) { m_sName = name; };

  /// get current window
  const vxWindowLevel & GetWindowLevel() const { return m_windowLevel; }

  /// set Window/Level
  void SetWindowLevel(const vxWindowLevel & windowLevel) { m_windowLevel = windowLevel; };

  /// get the RGBA Curves
  const std::vector<Point2D<ZeroOne> > & GetList(const nsCOLOR::COLOR_CHANNEL color) const;

  /// get the red curve
  const std::vector<Point2D<ZeroOne> > & GetRedList() const { return m_vRed; }

  /// get the green curve
  const std::vector<Point2D<ZeroOne> > & GetGreenList() const { return m_vGreen; }

  /// get the blue curve
  const std::vector<Point2D<ZeroOne> > & GetBlueList() const { return m_vBlue; }

  /// get the alpha curve
  const std::vector<Point2D<ZeroOne> > & GetAlphaList() const { return m_vAlpha; }

  /// get the red curve
  void SetRedList(const std::vector<Point2D<ZeroOne> > & list) { m_vRed.clear(); m_vRed = list; }

  /// get the green curve
  void SetGreenList(const std::vector<Point2D<ZeroOne> > & list) { m_vGreen.clear(); m_vGreen = list; }

  /// get the blue curve
  void SetBlueList(const std::vector<Point2D<ZeroOne> > & list) { m_vBlue.clear(); m_vBlue = list; }

  /// get the alpha curve
  void SetAlphaList(const std::vector<Point2D<ZeroOne> > & list) { m_vAlpha.clear(); m_vAlpha = list; }

  /// set dirty
  void SetDirty();

  /// set active range
  void SetActiveRange(const bool & bFlag) { m_bActiveRange = bFlag; }

  /// is active range enabled
  const bool & IsActiveRange() const { return m_bActiveRange; }

  /// get color table
  float4 * GetTablePointerRGBA(const uint2 & iMaxIndex);

  /// get color table based on what is appropriate for the given DICOM volume header
  float4 * GetTablePointerRGBA(const vxVolumeHeader & header);

	/// gets the colormap object from the File
	static vxColormap * FromFile( const std::string & file ) ;

  /// gets the colormap object from the File
	static vxColormap * FromXml( const std::string & xml );

// data
private:

  /// gets the colormap object from the File
	static vxColormap * FromElement( vxDOMElement & element );

// data
private:

  /// which range to map the color to: active or full?
  bool m_bActiveRange;

  // window/level
  vxWindowLevel m_windowLevel;

  /// red vector
  std::vector<Point2D<ZeroOne> > m_vRed;

  /// green vector
  std::vector<Point2D<ZeroOne> > m_vGreen;

  /// blue vector
  std::vector<Point2D<ZeroOne> > m_vBlue;

  /// alpha vector
  std::vector<Point2D<ZeroOne> > m_vAlpha;

  /// list discrete color tables including size and dirty flag
  mutable std::list<std::pair<int, std::pair<bool, vxColorTableCPU *> > > m_lColorTables;

	//name
  std::string m_sName;
}; // class vxColormap


// $Log: vxColormap.h,v $
// Revision 1.4  2006/08/23 19:29:27  romy
// added Colormap read for Colon Application
//
// Revision 1.3  2006/08/07 19:45:13  gdavidson
// Extends vxElement
//
// Revision 1.2  2005/05/24 11:32:30  frank
// serialized the active range
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.13  2003/06/07 16:18:33  michael
// added additional functionality to support on demand lookup tables
//
// Revision 1.12  2003/06/04 18:33:04  michael
// tookout namespace declaration, should not be in header file!!!
//
// Revision 1.11  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.10  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.9  2003/03/30 20:58:32  michael
// formatting and cosmetics
//
// Revision 1.8  2003/03/21 16:51:46  michael
// Removed ReadFrom() stub since there is no implementation anyway
//
// Revision 1.7  2002/10/14 15:30:49  michael
// maded Get-methods const and added Set-methods for RGBA vectors
//
// Revision 1.6  2002/10/03 17:39:13  michael
// documentation: public Sec_70_vxUtils
//
// Revision 1.5  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.4  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.3  2002/09/24 16:58:04  michael
// cleaned-up colortable and colormap ... hopefully we'll be able to remove
// member colortable from colormap.
//
// Revision 1.2  2002/09/23 14:53:50  michael
// added name
//
// Revision 1.1  2002/09/19 15:12:56  michael
// Added vxColormap and vxColormapEditorSharedData
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxColormap.h,v 1.4 2006/08/23 19:29:27 romy Exp $
// $Id: vxColormap.h,v 1.4 2006/08/23 19:29:27 romy Exp $
