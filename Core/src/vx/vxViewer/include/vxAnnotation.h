// $Id: vxAnnotation.h,v 1.17 2006/11/23 06:24:14 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)

/*
  Base class of all vxAnnotations
*/

#ifndef vxAnnotation_h
#define vxAnnotation_h


// includes
#include "vxHandlesObj.h"
#include "Image.h"
#include "Box.h"
#include "PixelRGBA.h"


// forward declarations
class vxIntensityStatistic;


// class definition
class VX_VIEWER_DLL vxAnnotation : public vxHandlesObj
{
// enums
public:

  // StatusEnum
  enum StatusEnum
  {
    CREATE = 0,
    MORPH  = 1,
    DONE   = 2,
  }; // StatusEnum

 //Abstract method

// functions
public:
  
  /// constructor
  vxAnnotation();
  
  /// destructor
	virtual ~vxAnnotation();

	/// copy constructor
  vxAnnotation( const vxAnnotation & other );           
  
  /// assignment operator
  vxAnnotation & operator=( const vxAnnotation & other ); 

  /////////////////////////////////////////////////////////////////////
  // actual vxHandlesObj functions

  /// reset the annotation
  virtual void Reset();

  /// adds another handle
  virtual void AddHandle(const vxHandle & handle);

  /// set the location of a given handle
  virtual void SetHandle(const uint2 iHandleIndex, const vxHandle & handle);

  /// render the annotation
  virtual void Render( vxEnvironment & environment );

	///Clone method
	virtual vxAnnotation * Clone() const;
	

  /////////////////////////////////////////////////////////////////////
  // actual vxISerialize functions

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return "vxAnnotation"; }

  /////////////////////////////////////////////////////////////////////
  // actual annotation functions

  /// get the number of handles
  virtual uint2 GetNumEditableHandles() const { return 1; }

  /// get the number of handles required during creation of the object
  virtual uint2 GetNumCreationHandles() const { return 1; }

  /// get handle index where the label should appear
  virtual uint2 GetLabelHandleIndex() { return 0; }

  /// get handle index where the text should appear
  virtual uint2 GetTextHandleIndex() { return 1; }

	///Returns the Bounding rectangle
	virtual Box<float4> GetBoundingBox() ;

	/// Is label editable
	bool IsLabelEditable() const { return m_bLabelEditable; }

  /// get status
  StatusEnum GetStatus() const { return m_eStatus; };

  /// set status
  void SetStatus(StatusEnum eStatus) { m_eStatus = eStatus; };

  /// get selected handle index
  int4 GetSelectedHandleIndex() const { return m_iSelectedHandleIndex; }

  /// set selected handle index
  void SetSelectedHandleIndex(int iIndex);

  /// get the color
  PixelRGBA<uint1> GetColor() const { return m_color; }

  /// set the color
  void SetColor(const PixelRGBA<uint1> & color) { m_color = color; }

  /// get the selected color
  PixelRGBA<uint1> GetColorSelected() const { return m_colorSelected; }

  /// set the selected color
  void SetColorSelected(const PixelRGBA<uint1> & color) { m_colorSelected = color; }

  /// get the color of the label
  PixelRGBA<uint1> GetColorLabel() const { return m_colorLabel; }

  /// set the color of the label
  void SetColorLabel(const PixelRGBA<uint1> & color) { m_colorLabel = color; }

  /// get the color
  PixelRGBA<uint1> GetColorText() const { return m_colorText; }

  /// set the color
  void SetColorText(const PixelRGBA<uint1> & color) { m_colorText = color; }

  /// get the position of the label
  Point<float4> GetLabelPosition() const { return m_positionLabel; }

  /// set the position of the label
  void SetLabelPosition(const Point<float4> & position) { m_positionLabel = position; }

  /// get the position of the text
  Point<float4> GetTextPosition() const { return m_positionText; }

  /// set the position of the text
  void SetTextPosition(const Point<float4> & position) { m_positionText = position; }

  /// label selected?
  bool IsLabelSelected() const { return m_bLabelSelected; }

	/// set label selected
  void SetLabelSelected(bool bFlag) { m_bLabelSelected = bFlag; }

	/// Set the draw handle flag
  void SetDrawHandles(bool bFlag) { m_bDrawHandles = bFlag; }

  /// text selected?
  bool IsTextSelected() const { return m_bTextSelected; }

  /// set text selected
  void SetTextSelected(bool bFlag) { m_bTextSelected = bFlag; }

  /// get the label
  std::string GetLabel() const { return m_sLabel; }

  /// set the label
  void SetLabel(const std::string & sLabel) { m_sLabel = sLabel; }

  /// get the text
  std::string GetText() const;

  /// set the text (one string)
  void SetText(const std::string & sText);

  /// set the text (multiple strings)
  void SetText(const std::vector<std::string> & vsTextLines) { m_vsTextLines = vsTextLines; }

  /// get the individual text lines
  std::vector<std::string> GetTextLines() const { return m_vsTextLines; }

  /// get the text width
  int4 GetTextLinesWidth() const;

  /// get the text height
  int4 GetTextLinesHeight() const;

  /// returns the module instance handle for resources
  HMODULE GetModuleHandle() const { return m_hModule; }
  
  /// set the module instance handle for resources
  void SetModuleHandle(HMODULE hModule) { m_hModule = hModule; }

  /// set the mouse cursor string
  std::string GetMouseCursor() const { return m_sMouseCursor; }
  
  /// set the mouse cursor string
  void SetMouseCursor(const std::string & sMouseCursor) { m_sMouseCursor = sMouseCursor; }

 	/// get the image
  const Image<PixelRGBA<uint1> > * GetImage() const { return m_pImage; }

  /// set the image (class assumes ownership)
  void SetImage(Image< PixelRGBA<uint1> >  * pImage) { m_pImage = pImage; }

  
  /// sets the mouse mouse cursor
  void SetCursor();

  /// morph from creating the object with few handles to full annotation with possibly more handles
  virtual void Morph(vxEnvironment & environment);

  /// update the annotation
  virtual void Update(vxEnvironment & environment) {}

  /// method to render the annotation text
  virtual void RenderText(vxEnvironment & environment, GLint * piViewport,
                          GLdouble * pfModelviewMatrix, GLdouble * pfProjectionMatrix) const;

  /// method to determine whether to update while dragging
  virtual bool PerformUpdateWhileDragging() { return true; }

protected:

  /// returns relative line spacing relative tot he used font size
  float4 GetLineSpacingFontPercentage() const { return m_fLineSpacingFontPercentage; }

  // format and set the text
  void FormatAndSetStatisticText(int4 iMinimum, int4 iMaximum, float4 fAverage, float4 fStdDeviation,
                                 std::string & sAreaOrVolumeLabel, float4 fAreaOrVolumeValue,
                                 std::string & sAreaOrVolumeUnits);

  // format and set the text
  void FormatAndSetStatisticText(const vxIntensityStatistic & statistic, std::string & sAreaOrVolumeLabel, 
                                 float4 fAreaOrVolumeValue, std::string & sAreaOrVolumeUnits);
private:

   
  /// common initialization code
  void CommonInit();

// data

protected:
	//draw handles flag
	bool m_bDrawHandles;

	///Is the label modifiable
	bool m_bLabelEditable;

	//Line width property
	static const float m_fLineWidth;

	//Line width property
	static const float m_fLineShadowWidth;

private:

  /// position of the text
  Point<float4> m_positionText;

  /// position of the label
  Point<float4> m_positionLabel;

  /// associated color
  PixelRGBA<uint1> m_color;

  /// associated selected color
  PixelRGBA<uint1> m_colorSelected;

  /// associated color of text
  PixelRGBA<uint1> m_colorText;

  /// associated color of label
  PixelRGBA<uint1> m_colorLabel;

  /// label selected?
  bool m_bLabelSelected;

  /// text selected?
  bool m_bTextSelected;

	/// associated label
  std::string m_sLabel;

  /// test broken into lines
  mutable std::vector<std::string> m_vsTextLines;

  /// status of the annotation
  StatusEnum m_eStatus;

  /// is it selected/highlighted? handle index
  int4 m_iSelectedHandleIndex;

  /// percentage of label/text line spacing relative to the used font
  float4 m_fLineSpacingFontPercentage;

  /// associated instance handle needed to set mouse cursor
  HMODULE m_hModule;

  /// associated mouse cursor
  mutable std::string m_sMouseCursor;

  /// associated image
  Image<PixelRGBA<uint1> > * m_pImage;
}; // vxAnnotation

 
#endif // vxAnnotation_h


// Revision History:
// $Log: vxAnnotation.h,v $
// Revision 1.17  2006/11/23 06:24:14  romy
// cloning modified
//
// Revision 1.16  2006/09/27 21:31:17  romy
// put the resource pointer back
//
// Revision 1.14  2006/09/26 21:24:11  romy
// Modified the image object to static
//
// Revision 1.13  2006/09/25 20:47:06  romy
// added label editable flag for Volume annotations
//
// Revision 1.12  2006/08/08 13:50:39  romy
// Annotation length and Shadow length are now static consts
//
// Revision 1.11  2006/08/07 20:26:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.10  2006/08/07 18:47:48  romy
// added CCtrs
//
// Revision 1.9  2006/08/03 20:28:09  romy
// added GetBounding Box method
//
// Revision 1.8  2006/08/02 21:22:32  romy
// asjusted the line shadow width
//
// Revision 1.7  2006/08/02 20:45:10  romy
// added line shadow width
//
// Revision 1.6  2006/08/02 18:45:33  romy
// add renderer method and line width property
//
// Revision 1.5  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.4  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/16 18:25:32  geconomos
// Issue# 4356: (George &Frank) Reworked computation of volume annotation.
//
// Revision 1.3  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.2  2005/04/12 15:24:14  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.28  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.27  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.26  2004/01/05 18:58:30  michael
// added "mailto:"
//
// Revision 1.25  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.24  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.23  2003/04/08 19:52:41  geconomos
// Updated for resource management.
//
// Revision 1.22  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.21  2003/04/01 00:35:59  michael
// fixed issue #2862
//
// Revision 1.20  2003/03/04 16:51:22  geconomos
// Removed unecessary semi-colons from clas definitions.
//
// Revision 1.19  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.18  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.17  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.16  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.15  2003/02/18 15:43:57  michael
// made member mouse cursor mutable
//
// Revision 1.14  2003/02/13 16:56:47  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.13  2003/02/07 01:03:53  michael
// added annotation for components
//
// Revision 1.12  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.11.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.11  2003/01/30 00:36:29  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.10  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 1.9  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.8  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.7  2003/01/19 21:07:09  michael
// added box annotation
//
// Revision 1.6  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.5  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.4  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.3  2003/01/13 14:42:30  geconomos
// Did the following:
//   (1) Added mouse cursor handling.
//   (2) Set the active annotation in AddAnnotation().
//
// Revision 1.2  2003/01/09 14:55:44  michael
// some add-ons to the new annotation structure
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxAnnotation.h,v 1.17 2006/11/23 06:24:14 romy Exp $
// $Id: vxAnnotation.h,v 1.17 2006/11/23 06:24:14 romy Exp $
