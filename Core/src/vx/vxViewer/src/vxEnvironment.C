// $Id: vxEnvironment.C,v 1.39 2008/01/18 21:29:27 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren Grimm (soeren@viatronix.com), Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxIRenderingContext.h"
#include "vxDataRendererSoftware.h"
#include "vxVisualizationGraph.h"


// defines
#define FILE_REVISION "$Revision: 1.39 $"


// namespaces
using namespace std;


// static member initialization
vxInputFeedback vxEnvironment::m_inputFeedback;
const vxEnvironment * vxEnvironment::m_pActiveEnvironment = NULL;
const vxEnvironment * vxEnvironment::m_pSelectedEnvironment = NULL;
vxUndoActionStack vxEnvironment::m_undoActions;
LONG vxEnvironment::m_iInteractingCount = 0;


/**
 * Default constructor
 */
vxEnvironment::vxEnvironment():
//*******************************************************************
m_pColormap(NULL),
m_pComponents(NULL),
m_pPlaneOrtho(NULL),
m_pPlaneOblique(NULL),
m_pPlaneCurved(NULL),
m_pRenderingMode(NULL),
m_uOffset(0),
m_uMaxOffset(0),
m_uScrollFactor(1),
m_pVolumeIntensity(NULL),
m_pVolumeLabel(NULL),
m_pInteractiveSeg(NULL),
m_pSculptContour(NULL),
m_pZoomFactor(NULL),
m_pCine(NULL),
m_pCameraFlySettings(NULL),
m_pMomentum(NULL),
m_pHistogram(NULL),
m_pDataRendererSoftware(NULL),
m_pCursor3d(NULL),
m_pCropBox(NULL),
m_pContourList(NULL),
m_pCamera(NULL),
m_pBackgroundColor(NULL),
m_pViewerType(NULL),
m_pFontInfo(NULL),
m_pCadFindings(NULL),
m_pVisualizationGraph(NULL),
m_pDataset( NULL ),
m_bIgnoreModifiedEvents( false )
{
} // default constructor


/**
 * Destructor
 */
vxEnvironment::~vxEnvironment()
//*******************************************************************
{  
} // destructor


/**
 * Creates a fully populated instance of a vxEnvironment.
 * @return  new vxEnvironment instance
 */
vxEnvironment * vxEnvironment::Create()
//*******************************************************************
{
  vxEnvironment * pEnvironment = new vxShareableObject< vxEnvironment >();
  
  pEnvironment->SetAnnotations(new vxShareableObject< vxAnnotations >());
  pEnvironment->SetBackgroundColor(new vxShareableObject< PixelRGBA<uint1> >());
  pEnvironment->SetCamera(new vxShareableObject< vxCamera >());
  //pEnvironment->SetColormap(new vxShareableObject< vxColormap >());
  //pEnvironment->SetComponents(new vxShareableObject< vxComponentArray >());
  pEnvironment->SetCropBox(new vxShareableObject< vxCropBox >());
  pEnvironment->SetCursor3d(new vxShareableObject< vxCursor3d >());
  pEnvironment->SetFontInfo(new vxShareableObject< vxFontInfo >);
  pEnvironment->SetInteractiveSegmentation(new vxShareableObject< InteractiveSegmentation >());
  pEnvironment->SetPlaneOrtho(new vxShareableObject< vxPlaneOrtho<float4> >());
  pEnvironment->SetPlaneCurved(new vxShareableObject< vxPlaneCurved >());
  pEnvironment->SetRenderingMode(new vxShareableObject< vxRenderingMode >());
  pEnvironment->SetSculptContour(new vxShareableObject< vxContour >());
  pEnvironment->SetViewerType(new vxViewerTypeUndefined(*pEnvironment));
  pEnvironment->SetZoomFactor(new vxShareableObject< vxZoomFactor >());
  pEnvironment->SetDataRendererSoftware( new vxShareableObject<  vxDataRendererSoftware >() );
  pEnvironment->SetMomentum( new vxShareableObject< vxMomentum >() );
  pEnvironment->SetCadFindings( new vxShareableObject< vxCAD::vxCadFindings >() );
  
  pEnvironment->AddElement(vxIDs::OverlayStrings, reinterpret_cast< vxElement * >(new vxShareableObject< vxOverlayStrings >()));

  return pEnvironment;
} // Create()


/**
 * Frees all resources associated with the specified enviroment
 */
void vxEnvironment::Free(vxEnvironment * pEnvironment)
//*******************************************************************
{
  if(pEnvironment->m_pAnnotations      != NULL) { delete pEnvironment->m_pAnnotations;      pEnvironment->m_pAnnotations      = NULL; }
  if(pEnvironment->m_pBackgroundColor  != NULL) { delete pEnvironment->m_pBackgroundColor;  pEnvironment->m_pBackgroundColor  = NULL; }
  if(pEnvironment->m_pCamera           != NULL) { delete pEnvironment->m_pCamera;           pEnvironment->m_pCamera           = NULL; }
//  if(pEnvironment->m_pColormap         != NULL) { delete pEnvironment->m_pColormap;         pEnvironment->m_pColormap         = NULL; }
//  if(pEnvironment->m_pComponents       != NULL) { delete pEnvironment->m_pComponents;       pEnvironment->m_pComponents       = NULL; }
  if(pEnvironment->m_pCropBox          != NULL) { delete pEnvironment->m_pCropBox;          pEnvironment->m_pCropBox          = NULL; }
  if(pEnvironment->m_pCursor3d         != NULL) { delete pEnvironment->m_pCursor3d;         pEnvironment->m_pCursor3d         = NULL; }
  if(pEnvironment->m_pFontInfo         != NULL) { delete pEnvironment->m_pFontInfo;         pEnvironment->m_pFontInfo         = NULL; }
  if(pEnvironment->m_pInteractiveSeg   != NULL) { delete pEnvironment->m_pInteractiveSeg;   pEnvironment->m_pInteractiveSeg   = NULL; }
  if(pEnvironment->m_pPlaneCurved      != NULL) { delete pEnvironment->m_pPlaneCurved;      pEnvironment->m_pPlaneCurved      = NULL; }
  if(pEnvironment->m_pPlaneOrtho       != NULL) { delete pEnvironment->m_pPlaneOrtho;       pEnvironment->m_pPlaneOrtho       = NULL; }
  if(pEnvironment->m_pRenderingMode    != NULL) { delete pEnvironment->m_pRenderingMode;    pEnvironment->m_pRenderingMode    = NULL; }
  if(pEnvironment->m_pSculptContour    != NULL) { delete pEnvironment->m_pSculptContour;    pEnvironment->m_pSculptContour    = NULL; }
  if(pEnvironment->m_pViewerType       != NULL) { delete pEnvironment->m_pViewerType;       pEnvironment->m_pViewerType       = NULL; }
  if(pEnvironment->m_pZoomFactor       != NULL) { delete pEnvironment->m_pZoomFactor;       pEnvironment->m_pZoomFactor       = NULL; }
  if(pEnvironment->m_pMomentum         != NULL) { delete pEnvironment->m_pMomentum;         pEnvironment->m_pMomentum         = NULL; }
  if(pEnvironment->m_pCadFindings      != NULL) { delete pEnvironment->m_pCadFindings;      pEnvironment->m_pCadFindings      = NULL; }

  //if(pEnvironment->m_pDataRenderer     != NULL) { delete pEnvironment->m_pDataRenderer;     pEnvironment->m_pDataRenderer     = NULL; }

  
  for(vxElements::iterator iter = pEnvironment->m_elements.begin(); iter != pEnvironment->m_elements.end(); ++iter)
    delete (*iter).second;
  pEnvironment->m_elements.clear();
} // Free(vxEnvironment * pEnvironment)

/**
 * Returns true if feature is modified
 * @param environment element index
 * @return true if element is modified
 */ 
bool vxEnvironment::IsModified(const vxEnvironmentEnum eElement) const
//*******************************************************************
{
  try
  {
    if (eElement < ALL)
    {
      return m_modified.at(eElement);
    }
    else if (eElement == ALL)
    {
      return(m_modified.any() || !m_vModified.empty());
    }
  }
  catch (out_of_range &)
  {
    throw ex::IndexOutOfBoundsException(LogRec("Bit index out of bound.", "vxEnvironment", "IsModified"));
  }
  return false;
} // IsModified()


/**
 * Get the modified state of an element
 * @param environment element id
 * @return true if element is modified
 */ 
bool vxEnvironment::IsModified(const vxID id) const
//*******************************************************************
{
  if (std::find(m_vModified.begin(), m_vModified.end(), id) == m_vModified.end())
  {
    return false;
  }

  return true;
} // IsModified()


/**
 * Set part of environment as modified or unmodified
 * @param element index
 * @param modified status
 */ 
void vxEnvironment::SetModified(const vxEnvironmentEnum eElement,
                                const bool bModified)
//*******************************************************************
{
  try
  {
    if (eElement < ALL)
    {
      m_modified.set(eElement, bModified);
    }
    else if (eElement == ALL)
    {
      if (bModified == true)
      {
        m_modified.set();
        for (vxElements::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter)
        {
          SetModified(iter->first, true);
        }
      }
      else
      {
        m_modified.reset();
        m_vModified.clear();
      }
    }
  }
  catch (out_of_range &)
  {
    throw ex::IndexOutOfBoundsException(LogRec("Bit index out of bound.","vxEnvironment","SetModified"));
  }
} // SetModified()


/**
 * Set the modified state of an enlement
 * @param environment element
 * @param modified status
 */ 
void vxEnvironment::SetModified(const vxID id, 
                                const bool bModified)
//*******************************************************************
{
  std::list<vxID>::iterator iter(find(m_vModified.begin(), m_vModified.end(), id));

  if (bModified == true)
  {
    // want to add this element to the list if it is not there already
    if (iter == m_vModified.end())
    {
      m_vModified.push_back(id); // didn't find it --> add it
    }
  }
  else
  {
    // want it removed from the list
    if (iter != m_vModified.end())
    {
      m_vModified.erase(iter); // found it --> so remove it
    }
  }
} // SetModified()


/**
 * Updates the font based on the values in the vxFontInfo object.
 */
void vxEnvironment::UpdateFont()
//*******************************************************************
{
  UpdateFont( GetFont(), GetContext() );
} // UpdateFont()


/**
 * Updates the font based on the values in the vxFontInfo object.
 *
 * @param         pFont             the font instance
 * @param         pContext          the rendering context
 */
void vxEnvironment::UpdateFont( GLFontGeorge * pFont, vxIRenderingContext * pContext )
//*******************************************************************
{
  if( pFont == NULL || pContext == NULL )
    return;

  if (m_pFontInfo && pContext->IsValid())
  {
    if( pFont->GetFontHeight() != m_pFontInfo->GetSize())
    {      
      pFont->Create( pContext, m_pFontInfo->GetFontFamily(), m_pFontInfo->GetSize(), 0, 0 );
      pFont->SetStyle(GLFontGeorge::SHADOW_AROUND);
    }
  }
} // UpdateFont()


/**
 * Set the current offset value
 * @param offset value
 */
void vxEnvironment::SetOffset(uint4 uOffset)
//*******************************************************************
{
  m_uOffset = Bound(0 , (int)uOffset, (int)m_uMaxOffset); 
} // SetOffset()


/**
 * Sets the maximum MaxOffset value
 * @param maximum offset value
 */
void vxEnvironment::SetMaximumOffset(uint4 uMaxOffset)  
//*******************************************************************
{
  m_uMaxOffset = uMaxOffset; 
  SetOffset(m_uOffset); 
} // SetMaximumOffset()


/**
 * Adds an element to the environment
 * @param id ID of element within the environment. Must be unique
 * @param element to add.
 */
void vxEnvironment::AddElement(const vxID & id, vxElement * pElement)
//*******************************************************************
{
  // id must be unique
  for (vxElements::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter)
  {
    if ((*iter).first == id)
    {
      RemoveElement(id);
      LogWrn("Existing element removed. Index must be unique. [ID=" + id + "]", "vxEnvironment", "AddElement");
      break;
    }
  }

  m_elements.push_back(std::make_pair(id, pElement));
} // AddElement()


/**
 * Removes an element from the environment
 * @param id ID of element to remove
 */
void vxEnvironment::RemoveElement(const vxID & id)
//*******************************************************************
{
  for (vxElements::iterator iter = m_elements.begin(); iter != m_elements.end(); ++iter)
  {
    if ((*iter).first == id)
    {
      m_elements.erase(iter);
      return;
    }
  }
} // RemoveElement()


/**
 * Gets an element from the environment
 * @param id ID of the element to retrieve
 * @return Pointer to the element or NULL
 */
vxElement * vxEnvironment::GetElement(const vxID & id) const
//*******************************************************************
{
  for (vxElements::const_iterator iter(m_elements.begin()); iter != m_elements.end(); ++iter)
  {
    if ((*iter).first == id)
    {
      return(*iter).second;
    }
  }

  return NULL;
} // GetElement()


/**
 * Get a string with the list of all changed environment elements
 * @return name
 */
std::string vxEnvironment::GetModifiedElementsString() const
//*******************************************************************
{
  static bool bFirst(true);
  static vector<string> sEnvironmentElementStrings(0);
  if (bFirst == true)
  {
    bFirst = false;
    const int1 * const cstrEnvironmentElementsStr[vxEnvironment::ALL+1] = ENVIRONMENT_ELEMENT_STRINGS;
    sEnvironmentElementStrings.resize(vxEnvironment::ALL+1);
    for (int4 iCount(0); iCount < vxEnvironment::ALL+1; ++iCount)
      sEnvironmentElementStrings[iCount] = cstrEnvironmentElementsStr[iCount];
  }

  // now make the string of modified elements
  string sModifiedElements("");
  string sModifiedBits("");

  for (int4 eElement=vxEnvironment::NONE;  eElement <= vxEnvironment::ALL;  ++eElement)
  {
    if (IsModified((vxEnvironment::vxEnvironmentEnum)(eElement)))
    {
      sModifiedBits += "1";
      sModifiedElements += sEnvironmentElementStrings[eElement];
      sModifiedElements += ", ";
    }
    else
    {
      sModifiedBits += "0";
    }
  }
  return string(sModifiedBits+": "+sModifiedElements);
} // GetModifiedElementsString()


/**
 * Returns if the user is interacting with the environment
 *
 * @return true if interacting; false otherwise
 **/
bool vxEnvironment::GetUserInteracting()
{
  return m_iInteractingCount > 0;
} // GetUserInteracting()


/**
 * Sets if the user is interacting with the environment
 */
void vxEnvironment::SetUserInteracting( const bool & bUserInteracting )
{
  bUserInteracting? InterlockedIncrement( &m_iInteractingCount ) : InterlockedDecrement( &m_iInteractingCount );
} // SetUserInteracting( const bool & bUserInteracting )


/**
 * get rendering context pointer
 *
 * @return        rendering context
 */
vxIRenderingContext * vxEnvironment::GetContext() 
{
  return m_pVisualizationGraph->GetRenderingContext( this );
} // GetContext()


/**
 * get rendering context pointer
 *
 * @return        rendering context
 */
GLFontGeorge * vxEnvironment::GetFont() 
{
  return m_pVisualizationGraph->GetFont( this );
} // GetFont()


// $Log: vxEnvironment.C,v $
// Revision 1.39  2008/01/18 21:29:27  geconomos
// Issue# 5869: Images updating in background while making a movie
//
// Revision 1.38  2007/10/24 20:11:28  gdavidson
// Moved the rendering context and font to the visualization graph
//
// Revision 1.37  2007/10/03 20:35:31  gdavidson
// Added gets and sets for SelectedEnvironment
//
// Revision 1.36  2007/09/19 14:33:22  gdavidson
// Added get and set methods for the associated vxDataset
//
// Revision 1.35  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.34  2007/05/03 21:30:53  gdavidson
// Added vxVisualizationGraph
//
// Revision 1.33  2007/04/11 20:54:08  romy
// Added cadFindings object added
//
// Revision 1.32  2007/04/06 14:12:29  gdavidson
// Added vxCadFindings
//
// Revision 1.31  2007/04/02 21:17:32  gdavidson
// Removed GetRenderBufferModified
//
// Revision 1.30.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.30.2.1  2007/03/22 21:36:05  gdavidson
// Issue #5505: Added ResetUserInteracting method
//
// Revision 1.30  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.29  2007/01/16 17:05:23  gdavidson
// Added vxCine and vxCameraFlySettings and removed timer
//
// Revision 1.28  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.27  2006/09/26 15:59:30  geconomos
// cleanuo
//
// Revision 1.26  2006/09/21 19:20:27  gdavidson
// Added more information to log message
//
// Revision 1.25  2006/08/23 17:57:33  geconomos
// + made "render buffer" a concrete instance
// + added flag to indicate if "render buffer" has been modified
//
// Revision 1.24  2006/08/03 14:00:08  geconomos
// + removed rendering engine and rendering engine data
// + added render buffer
//
// Revision 1.23  2006/07/07 13:33:06  geconomos
// added static boolean property that indicates if a user is interacting with any environment
//
// Revision 1.22  2006/06/27 20:12:42  geconomos
// added rendering engine and associated data as first class citizens
//
// Revision 1.21  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.20  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.19  2005/12/08 16:40:24  frank
// added undo support
//
// Revision 1.18  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.17.2.1  2005/08/12 17:20:51  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.17  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.16  2005/05/20 12:13:18  frank
// cleaning up slice shadows in 3D
//
// Revision 1.15  2005/05/18 15:07:07  frank
// cleaning up slice shadows
//
// Revision 1.14  2001/01/03 19:10:39  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.13  2004/11/17 22:38:25  geconomos
// added vxDataRenderer to Create
//
// Revision 1.12  2004/08/30 23:59:08  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.11  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.10  2004/06/08 14:39:52  geconomos
// Removed initialization of colormap in the factory methods: Create and Free
//
// Revision 1.9  2004/05/18 15:35:09  frank
// fixed footer
//
// Revision 1.8  2004/04/21 18:34:07  geconomos
// Added static methods for creating and freeing a vxEnvionrment.
//
// Revision 1.7  2004/04/06 16:24:32  frank
// unified environment access naming
//
// Revision 1.6  2004/04/06 12:09:52  frank
// no use for the skeleton in the environment
//
// Revision 1.5  2004/04/02 14:03:29  frank
// working on unifying the various rendering contexts
//
// Revision 1.4  2004/03/12 19:49:51  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.3  2004/03/12 18:03:48  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.2  2004/03/05 19:34:55  geconomos
// Removed references to vxServerTexMap2d and vxServerTexMap3d
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2003/10/09 01:13:12  geconomos
// Added a check in UpdateFont to check if font is already the desired size before setting.
//
// Revision 1.31  2003/10/02 10:23:29  geconomos
// Added vxFontInfo to environment as well as a method to update the font based on the settings contained in vxFontInfo.
//
// Revision 1.30  2003/09/29 17:21:29  michael
// fixed typo in font (Ariel -> Arial)
//
// Revision 1.29  2003/07/23 16:56:57  michael
// took out redundant stuff (old 8bit and 12 bit CPU color tables)
//
// Revision 1.28  2003/06/18 15:19:52  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.27  2003/06/04 18:24:04  michael
// added namespace declaration
//
// Revision 1.26  2003/05/29 20:24:41  michael
// iterator initialization in constructor, not assignement
//
// Revision 1.25  2003/05/12 18:16:21  frank
// Added slice shadows object
//
// Revision 1.24  2003/05/06 18:25:50  michael
// initial code review
//
// Revision 1.23  2003/04/29 21:19:03  michael
// renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData
//
// Revision 1.22  2003/03/21 15:50:52  frank
// Clarified logic
//
// Revision 1.21  2003/03/20 13:48:01  geconomos
// Proper initialization of m_pViewerType.
//
// Revision 1.20  2003/03/11 18:44:11  geconomos
// Added static member "ActiveViewerType" and related accessors.
//
// Revision 1.19  2003/01/28 19:31:27  geconomos
// Implementation of BackgroundColor.
//
// Revision 1.18  2003/01/24 21:06:39  michael
// added background color to the environment
//
// Revision 1.17  2003/01/22 21:45:10  ingmar
// initialized m_pCamera in constructor
//
// Revision 1.16  2003/01/09 14:54:57  michael
// added new annotations class to the environment
//
// Revision 1.15  2002/11/27 21:58:13  ingmar
// added GetModifiedElementsString()
//
// Revision 1.14  2002/11/27 15:22:38  frank
// Increased the font size slightly
//
// Revision 1.13  2002/11/19 21:27:28  michael
// added initialization of plane oblique
//
// Revision 1.12  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.11  2002/11/13 22:08:45  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.10  2002/11/12 21:29:28  geconomos
// Made vxContour  a first class citizen of vxEnvironment (frank really did it).
//
// Revision 1.9  2002/11/05 22:49:51  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.8  2002/11/01 21:27:53  frank
// Added Crop Box to environment.
//
// Revision 1.7  2002/10/24 14:09:48  frank
// Initialized plane oblique
//
// Revision 1.6  2002/09/26 18:11:42  dmitry
// Colormp presets are now an instance instead of a pointer.
//
// Revision 1.5  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.4  2002/09/19 19:52:18  michael
// fixed missing initialization of two pointers and removed dangling member pointer
// to old transferfunction.
//
// Revision 1.3  2002/09/05 22:37:46  jaddonisio
// New vxComponent.
//
// Revision 1.2  2002/08/05 16:19:31  dmitry
// vxEventTimer added
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.39  2002/07/25 17:47:13  dmitry
// Nothing to fix, you should remove an element before adding another one.
//
// Revision 1.38  2002/07/10 11:44:30  geconomos
// exported classes.
//
// Revision 1.37  2002/06/21 16:26:38  frank
// Implemented PlaneCurved.
//
// Revision 1.36  2002/06/20 02:53:06  dmitry
// Renamed Historgam from Volume to Binary.
//
// Revision 1.35  2002/06/20 02:22:14  dmitry
// Added to the Environment: 3D Cursor, MouseCursor, Intensity Volume Histogram, and Label Volume Histogram.
//
// Revision 1.34  2002/06/18 21:08:36  dmitry
// Reimplemented retrieving, setting and removing elements.
//
// Revision 1.33  2002/06/13 23:22:35  michael
// added feedback information from the manipulators so that they can be removed
// from VB once operation is completed ...
//
// Revision 1.32  2002/06/13 19:24:23  michael
// added Cursor3d
//
// Revision 1.31  2002/06/07 20:15:03  dmitry
// modified mechanism based on vxID added
//
// Revision 1.30  2002/06/07 19:59:45  geconomos
// Changed extended elements fron shareable object to vxElement.
//
// Revision 1.29  2002/06/05 21:38:11  geconomos
// Initial extended element implementation.
//
// Revision 1.28  2002/06/04 16:11:52  dmitry
// slice scroll factor added
//
// Revision 1.27  2002/05/09 21:02:40  jenny
// set the histogram ptr to NULL in the constructor
//
// Revision 1.26  2002/05/08 21:38:27  jenny
// set font style to SHADOW_AROUND
//
// Revision 1.25.2.1  2002/05/07 23:10:45  frank
// initialized m_planeCurved.
//
// Revision 1.25  2002/05/06 21:45:29  dmitry
// Cine and Momentum added.
//
// Revision 1.24  2002/05/03 17:38:10  jenny
// zoom factor
//
// Revision 1.23  2002/05/01 23:13:07  michael
// removed remainder of VolumeSetExt
//
// Revision 1.22  2002/05/01 21:15:21  jenny
// no volumeset
//
// Revision 1.21  2002/05/01 13:48:33  michael
// Clean-up of the environment ...
//
// Revision 1.20  2002/05/01 00:04:56  michael
// Removed old stuff such as CROPBOX, CARDIACPLAQUES, IsEnabled(), SetEnabled(), etc.
//
// Revision 1.19  2002/04/29 20:29:46  uday
// sliceshadows is a sharable object.
//
// Revision 1.18  2002/04/24 16:12:06  uday
// static Slice Shadow Plane Ortho
//
// Revision 1.17  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.16  2002/04/23 15:08:06  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.15  2002/04/18 20:19:01  frank
// Added sculpting manipulator and renderer.
//
// Revision 1.14  2002/04/15 15:49:57  manju
// Initializing InteractiveSegmentation object.
//
// Revision 1.13  2002/04/10 20:34:06  manju
// Removed Get/Set slice number and GetNumberofSlices methods.
//
// Revision 1.12  2002/04/08 21:08:54  dmitry
// Offset information added.
//
// Revision 1.11  2002/04/02 21:46:42  geconomos
// Added vxServerTexMap2d to environment.
//
// Revision 1.10  2002/03/29 16:34:28  michael
// added initialization of rendermode
//
// Revision 1.9  2002/03/29 01:29:32  ingmar
// added RenderQualitySpeedTradeoff
//
// Revision 1.8  2002/03/28 23:08:12  jaddonisio
// Changed "TAG" to "LABEL" in enum.
//
// Revision 1.7  2002/03/26 21:01:00  uday
// initializing variables
//
// Revision 1.6  2002/03/26 18:42:06  dmitry
// Moved creation of GLContext to v3D_Viewer.
//
// Revision 1.5  2002/03/25 17:59:46  jaddonisio
// Properly deleting the GLContext in the destructor.
//
// Revision 1.4  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.3  2002/03/20 15:31:05  dmitry
// Split the Planes into Ortho, Oblique and Curved.
//
// Revision 1.2  2002/03/20 14:25:15  dmitry
// modified and enabled flags set and reset appropriately on ALL
//
// Revision 1.1  2002/03/19 17:14:43  dmitry
// Added initial version of 74_vxUtils
//
// Revision 1.1  2002/03/19 16:40:10  dmitry
// Moved to 75_vxUtils from 75_vxManipulators
//
// Revision 1.5  2002/03/18 22:47:09  dmitry
// Put GlFontGeorge into the env, and moved functionality into the .C file.
//
// Revision 1.4  2002/03/16 18:08:25  michael
// made PlaneSimple a PlaneOrtho<float4> but this will be generalized soon ...
//
// Revision 1.3  2002/03/15 17:32:05  jaddonisio
// Change name of TagVolume variables to LabelVolume.
//
// Revision 1.2  2002/03/15 01:29:00  michael
// using box (math) instead of cropBox (renderutils)
//
// Revision 1.1  2002/03/11 19:42:44  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.3  2002/03/07 22:31:09  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.4  2002/01/12 22:47:49  uday
// curved and oblique stuff
//
// Revision 1.3  2001/12/19 17:01:09  soeren
// renamed get functions to GetPtr functions
//
// Revision 1.2  2001/12/19 15:40:14  soeren
// no message
//
// Revision 1.1  2001/12/18 17:00:18  soeren
// Rewrote Environement class and moved it from 76_rendererViews to 75_renderUtils
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxEnvironment.C,v 1.39 2008/01/18 21:29:27 geconomos Exp $
// $Id: vxEnvironment.C,v 1.39 2008/01/18 21:29:27 geconomos Exp $
