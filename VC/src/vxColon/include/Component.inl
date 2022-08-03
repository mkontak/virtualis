// $Id: Component.inl,v 1.2 2006/06/01 20:25:16 frank Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Component.inl,v 1.2 2006/06/01 20:25:16 frank Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Huamin Qu (huamin@viatronix.net)
//
// Complete History at bottom of file.



#define FILE_REVISION "$Revision: 1.2 $"



/**
 * default constructor 
 */
template <class T> 
Component<T>::Component() 
{
  m_vName.reserve(256);
  m_vColor.reserve(256);
  m_vPremultipliedColor.reserve(256);
  m_vLabelVisible.reserve(256);
  m_vDensityVisible.reserve(256);
  m_vLock.reserve(256);
  m_vSelected.reserve(256);

  m_vName.resize(1);
  m_vColor.resize(1);
  m_vPremultipliedColor.resize(1);
  m_vLabelVisible.resize(1);
  m_vDensityVisible.resize(1);
  m_vLock.resize(1);
  m_vSelected.resize(1);
  m_vName[NONE] = "None";
  m_vColor[NONE].SetColor(0,0,0,0);
  m_vPremultipliedColor[NONE].SetColor(0,0,0,0);
  m_vLabelVisible[NONE] = true;
  m_vDensityVisible[NONE] = true;
  m_vLock[NONE] = false;
  m_vSelected[NONE] = false;
//  SetDefaults();
} // Component<T>()



/**
 * Constructor 
 * @param set default values 
 */
template <class T> 
Component<T>::Component(bool bSetDefaults) 
{
  if (bSetDefaults == true)
  {
    SetDefaults();
  }
  else
  {
    m_vName.reserve(256);
    m_vColor.reserve(256);
    m_vPremultipliedColor.reserve(256);
    m_vLabelVisible.reserve(256);
    m_vDensityVisible.reserve(256);
    m_vLock.reserve(256);
    m_vSelected.reserve(256);

    m_vName.resize(1);
    m_vColor.resize(1);
    m_vPremultipliedColor.resize(1);
    m_vLabelVisible.resize(1);
    m_vDensityVisible.resize(1);
    m_vLock.resize(1);
    m_vSelected.resize(1);
    m_vName[NONE] = "None";
    m_vColor[NONE].SetColor(0,0,0,0);
    m_vPremultipliedColor[NONE].SetColor(0,0,0,0);
    m_vLabelVisible[NONE] = true;
    m_vDensityVisible[NONE] = true;
    m_vLock[NONE] = false;
    m_vSelected[NONE] = false;
  }
} // Component<T>()



/**
 * Destructor.
 */
template <class T> Component<T>::~Component()
{
} // ~Component<T>()



/**
 * SetDefaults 
 */
template <class T> 
void Component<T>::SetDefaults() 
{
   /// Resize the name and color vectors
   m_vName.resize(MAX_SEGMENTS+1);                       //// there should not be anything in MAX_SEGMENTS
   m_vColor.resize(MAX_SEGMENTS+1);                      //// so we should resize to MAX_SEGMENTS after RSNA
   m_vPremultipliedColor.resize(MAX_SEGMENTS+1);
   m_vLabelVisible.resize(MAX_SEGMENTS+1);
   m_vDensityVisible.resize(MAX_SEGMENTS+1);
   m_vLock.resize(MAX_SEGMENTS+1);
   m_vSelected.resize(MAX_SEGMENTS+1);

   /// Initialize the name vector
   m_vName[NONE                             ] = "None";
   m_vName[AIR                              ] = "Air";
   m_vName[BODY                             ] = "Body";
   m_vName[SOFT_PLAQUE                      ] = "Soft Plaque";
   m_vName[BLOOD                            ] = "Blood";
   m_vName[BONE                             ] = "Bone";
   m_vName[CALCIFIED_PLAQUE                 ] = "Calcified Plaque";
   m_vName[NON_CALCIFIED_PLAQUE             ] = "Noncalcified Plaque";
   m_vName[STENT                            ] = "Stent";
   m_vName[METAL                            ] = "Metal";
   m_vName[LEFT_MAIN_ARTERY                 ] = "LMA";
   m_vName[LEFT_ANTERIOR_DESCENDING         ] = "LAD";
   m_vName[LEFT_CIRCUMFLEX_ARTERY           ] = "LC";
   m_vName[RIGHT_CORONARY_ARTERY            ] = "RCA";
   m_vName[POSTERIOR_DESCENDING_ARTERY      ] = "PD";
   m_vName[SUSPICIOUS_CALCIFICATION         ] = "Unreasonably sized calcification regions";
   m_vName[PLAQUE_131HU_TO_200HU            ] = "Plaque with peak/mean density in the range 131-200HU";
   m_vName[PLAQUE_201HU_TO_300HU            ] = "Plaque with peak/mean density in the range 201-300HU";
   m_vName[PLAQUE_301HU_TO_400HU            ] = "Plaque with peak/mean density in the range 301-400HU";
   m_vName[PLAQUE_401HU_AND_ABOVE           ] = "Plaque with peak/mean density above 401HU";
   m_vName[LEFT_ANTERIOR_DESCENDING_PROXIMAL] = "LAD Prox",
   m_vName[LEFT_ANTERIOR_DESCENDING_MID     ] = "LAD Mid",
   m_vName[LEFT_ANTERIOR_DESCENDING_DISTAL  ] = "LAD Dist",
   m_vName[LEFT_ANTERIOR_DESCENDING_DIAG_1  ] = "LAD Diag1",
   m_vName[LEFT_ANTERIOR_DESCENDING_DIAG_2  ] = "LAD Diag2",
   m_vName[LEFT_CIRCUMFLEX_ARTERY_PROXIMAL  ] = "LC Prox",
   m_vName[LEFT_CIRCUMFLEX_ARTERY_MID       ] = "LC Mid",
   m_vName[LEFT_CIRCUMFLEX_ARTERY_DISTAL    ] = "LC Dist",
   m_vName[LEFT_CIRCUMFLEX_ARTERY_OM1       ] = "LC OM1",
   m_vName[RIGHT_CORONARY_ARTERY_PROXIMAL   ] = "RCA Prox",
   m_vName[RIGHT_CORONARY_ARTERY_MID        ] = "RCA Mid",
   m_vName[RIGHT_CORONARY_ARTERY_DISTAL     ] = "RCA Dist",
   m_vName[AORTIC_VALVE                     ] = "AV",
   m_vName[MITRAL_VALVE                     ] = "MV",
   m_vName[THORACIC_AORTA                   ] = "TA",
   m_vName[ABDOMINAL_AORTA                  ] = "AA",
   m_vName[OTHER_CALCIFICATION              ] = "Other",
   m_vName[MAX_SEGMENTS] = "";

   /// Initialize the color vector. Treate pixel as uint1 type
   m_vColor[NONE].SetColor(0,0,0,0);
   m_vColor[AIR].SetColor(0,0,0,0);
   m_vColor[BODY].SetColor(0,0,0,0);
   m_vColor[SOFT_PLAQUE].SetColor(211,195,64,255);
   m_vColor[BLOOD].SetColor(255,0,0,255);
   m_vColor[BONE].SetColor(255,255,255,255);
   m_vColor[CALCIFIED_PLAQUE].SetColor(255,255,0,255);
   m_vColor[NON_CALCIFIED_PLAQUE].SetColor(0,0,255,255);
   m_vColor[STENT].SetColor(0,255,0,255);
   m_vColor[METAL].SetColor(125,125,125,255);
   m_vColor[LEFT_MAIN_ARTERY].SetColor(255,0,204,255);
   m_vColor[LEFT_ANTERIOR_DESCENDING].SetColor(185,43,251,255);
   m_vColor[LEFT_CIRCUMFLEX_ARTERY].SetColor(8,79,240,255);
   m_vColor[RIGHT_CORONARY_ARTERY].SetColor(8,175,33,255);
   m_vColor[POSTERIOR_DESCENDING_ARTERY].SetColor(75,219,207,255);
   m_vColor[SUSPICIOUS_CALCIFICATION].SetColor(236,202,129,255);
   m_vColor[PLAQUE_131HU_TO_200HU].SetColor(185,43,251,255);
   m_vColor[PLAQUE_201HU_TO_300HU].SetColor(255,0,204,255);
   m_vColor[PLAQUE_301HU_TO_400HU].SetColor(251,126,43,255);
   m_vColor[PLAQUE_401HU_AND_ABOVE].SetColor(255,0,51,255);
	 m_vColor[LEFT_ANTERIOR_DESCENDING_PROXIMAL].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_ANTERIOR_DESCENDING_MID     ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_ANTERIOR_DESCENDING_DISTAL  ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_ANTERIOR_DESCENDING_DIAG_1  ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_ANTERIOR_DESCENDING_DIAG_2  ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_CIRCUMFLEX_ARTERY_PROXIMAL  ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_CIRCUMFLEX_ARTERY_MID       ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_CIRCUMFLEX_ARTERY_DISTAL    ].SetColor(0, 0, 0, 255);
	 m_vColor[LEFT_CIRCUMFLEX_ARTERY_OM1       ].SetColor(0, 0, 0, 255);
	 m_vColor[RIGHT_CORONARY_ARTERY_PROXIMAL   ].SetColor(0, 0, 0, 255);
	 m_vColor[RIGHT_CORONARY_ARTERY_MID        ].SetColor(0, 0, 0, 255);
	 m_vColor[RIGHT_CORONARY_ARTERY_DISTAL     ].SetColor(0, 0, 0, 255);
	 m_vColor[AORTIC_VALVE                     ].SetColor(0,255,204,255);
	 m_vColor[MITRAL_VALVE                     ].SetColor(0,255,204,255);
	 m_vColor[THORACIC_AORTA                   ].SetColor(0,255,204,255);
	 m_vColor[ABDOMINAL_AORTA                  ].SetColor(0,255,204,255);
   m_vColor[OTHER_CALCIFICATION              ].SetColor(0,255,204,255);
	 m_vColor[MAX_SEGMENTS].SetColor(0, 0, 0, 255);                     //// there should not be anything in MAX_SEGMENTS

   { /// Initialize the LabelVisible vector.
   for( int4 i(0); i<=MAX_SEGMENTS; ++i )  m_vLabelVisible[i] = true;
   }

   { /// Initialize the DensityVisible vector. 
   for( int4 i(0); i<=MAX_SEGMENTS; ++i )  m_vDensityVisible[i] = true;
   }

   { /// Initialize the lock vector. 
   for( int4 i(0); i<=MAX_SEGMENTS; ++i )  m_vLock[i] = false;
   }

   {// Initialize the selected vector
   for( int4 i(0); i<=MAX_SEGMENTS; ++i )  m_vSelected[i] = false;
   }

} // SetDefaults()



/**
 * Assignment operator, copies a component's values into this
 * @param component
 * @return component
 */
template <class T> 
inline Component<T> & Component<T>::operator = (const Component<T> & rhs)
{
  m_vName  = rhs.m_vName;
  m_vColor = rhs.m_vColor;
  m_vPremultipliedColor = rhs.m_vPremultipliedColor;
  m_vLabelVisible   = rhs.m_vLabelVisible;
  m_vDensityVisible = rhs.m_vDensityVisible;
  m_vSelected = rhs.m_vSelected;
  return *this;
} // operator =



/**
 * Compute premultiplied colors
 * @param input color
 * @return output color
 */
template <class T> 
inline PixelRGBA<T> Component<T>::ComputePremultipliedColor(const PixelRGBA<T> & inColor) const
{
  // compute premultiplied color
  float4 fAlpha(.0F);
    
  if (typeid(T) == typeid(uint1))
  {
    fAlpha = inColor.GetAlpha()/255.0F;
  }
  else
  {
    fAlpha = inColor.GetAlpha();
  }

  return PixelRGBA<T>(float4(inColor.GetRed  ()) * fAlpha,
                      float4(inColor.GetGreen()) * fAlpha,
                      float4(inColor.GetBlue ()) * fAlpha,
                             inColor.GetAlpha());
} // ComputePremultipledColor()



/**
 * Compute premultiplied color (helper method)
 * @param input color
 * @return output color
 */
template <class T> 
inline PixelRGBA<T> Component<T>::ComputePremultipliedColor(const PixelRGB<T> & inColor) const
{
  return ComputePremultipliedColor(PixelRGBA<T>(inColor.GetRed(),
                                               inColor.GetGreen(), 
                                               inColor.GetBlue(), 
                                               static_cast<T>(255)));
} // ComputePremultipledColor()



/**
 * Is label selected?
 * @param label index
 * @return is label selected?
 */
template <class T>
bool Component<T>::GetSelected(const uint4 uLabelIndex)
{
  return m_vSelected[uLabelIndex];
} // GetSelected()



/**
 * Set/Unset label selected
 * @param label index
 * @param should it be selected
 */
template <class T>
void Component<T>::SetSelected(const uint4 uLabelIndex, bool bSelected)
{
  m_vSelected[uLabelIndex] = bSelected;  
} // SetSelected()



/**
 * Returns label name
 * @param label index
 * @return name
 */
template <class T> 
const std::string & Component<T>::GetName(const uint4 uLabelIndex)
{ 
  return m_vName[uLabelIndex]; 
} // GetName()



/**
 * Compute premultiplied colors
 * @param pixel color
 * @return pixel color
 */
template <class T> 
void Component<T>::SetName(const uint4 uLabelIndex, const std::string & sName)
{
  m_vName[uLabelIndex] = sName; 
} // SetName()



/**
 * Return label color
 * @param label index
 * @return label color
 */
template <class T> 
PixelRGBA<T> & Component<T>::GetColor(const uint4 uLabelIndex)
{ 
  return m_vColor[uLabelIndex]; 
} // GetColor()



/**
 * Returns pre-mulitplied colors of label
 * @param label index
 * @return label color
 */
template <class T> 
PixelRGBA<T> & Component<T>::GetPremultipliedColor(const uint4 uLabelIndex)
{ 
  return m_vPremultipliedColor[uLabelIndex]; 
} // GetPremultipliedColor()




/**
 * Set label color
 * @param label index
 * @param label color
 */
template <class T> 
void Component<T>::SetColor(const uint4 uLabelIndex, PixelRGBA<T> & color) 
{
  m_vColor[uLabelIndex] = color; 
  m_vPremultipliedColor[uLabelIndex] = ComputePremultipliedColor(color);
} // SetColor()



/**
 * Is the label visible
 * @param label index
 * @return is visible?
 */
template <class T> 
const bool Component<T>::GetLabelVisible(const uint4  uLabelIndex) const
{
  return m_vLabelVisible[uLabelIndex];
} // GetLabelVisible()



/**
 * Set/unset label visible
 * @param label index
 * @return is visible?
 */
template <class T> 
void Component<T>::SetLabelVisible(const uint4 uLabelIndex, bool bLabelVisible)
{
  m_vLabelVisible[uLabelIndex] = bLabelVisible;
} // SetLabelVisible()



/**
 * Is the density visible
 * @param label index
 * @return is visible?
 */
template <class T> 
const bool Component<T>::GetDensityVisible(const uint4  uLabelIndex) const
{
  return m_vDensityVisible[uLabelIndex];
} // GetDensityVisible()


/**
 * Set/unset label visible
 * @param label index
 * @param should it be active?
 */
template <class T> 
void Component<T>::SetDensityVisible(const uint4  uLabelIndex, bool bActive)
{
  m_vDensityVisible[uLabelIndex] = bActive;
} // SetDensityVisible()


/**
 * Returns whether a label is locked
 * @param label index
 * @return is locked?
 */
template <class T> 
bool Component<T>::GetLock(const uint4 uLabelIndex)
{
  return m_vLock[uLabelIndex];
} // GetLock()



/**
 * Lock/unlock a label
 * @param label index
 * @param should it be locked?
 */
template <class T> 
void Component<T>::SetLock(const uint4 uLabelIndex, bool bLock)
{
  m_vLock[uLabelIndex] = bLock;
} // SetLock()



/**
 * Get color of active component
 * @param label index
 * @return label color
 */
template <class T> 
PixelRGBA<T>  Component<T>::GetActiveColor(const uint4 uLabelIndex)
{
  if(m_vLabelVisible[uLabelIndex])
    return GetColor(uLabelIndex);

  return PixelRGBA<T>();
} // GetActiveColor()



/**
 * Add a component
 * @param component name 
 * @param component color
 * @return new component's index
 */
/// Add a new component.
template <class T>
const uint2 Component<T>::AddComponent(const std::string & sName, PixelRGBA<T> & color)
{
  m_vName.push_back(sName);
  m_vColor.push_back(color);
  m_vPremultipliedColor.push_back(ComputePremultipliedColor(color));
  m_vLabelVisible.push_back(true);
  m_vDensityVisible.push_back(true);
  m_vLock.push_back(false);

  return (m_vName.size() - 1);
} // AddComponent().



/**
 * Remove a component by number
 * @param number of component to be removed
 */
template <class T>
void Component<T>::RemoveComponent(const uint4 uNumber) 
{
  if (uNumber >= m_vName.size())
  {
    throw ex::IndexOutOfBoundsException(LogRec("Component<T>", "RemoveComponent", "invalid index"));
  }
	m_vName.erase(m_vName.begin() + uNumber);
	m_vColor.erase(m_vColor.begin() + uNumber);
  m_vPremultipliedColor.erase(m_vPremultipliedColor.begin() + uNumber);
	m_vLabelVisible.erase(m_vLabelVisible.begin() + uNumber);
  m_vDensityVisible.erase(m_vDensityVisible.begin() + uNumber);
  m_vLock.erase(m_vLock.begin() + uNumber);
} // RemoveComponent().



/**
 * Clears all components
 */
/// Clears components
template <class T>
void Component<T>::Clear()
{
  m_vName.resize(1);
  m_vColor.resize(1);
  m_vPremultipliedColor.resize(1);
  m_vLabelVisible.resize(1);
  m_vDensityVisible.resize(1);
  m_vLock.resize(1);
} // Clear().



/**
 * Returns if there are any visible labels
 * @return any visible labels?
 */
template <class T>
inline bool Component<T>::AreAnyLabelsVisible() const
{
  for (uint4 i(0); i<GetComponentCount(); ++i)
  {
    if (m_vLabelVisible[i] == true)
    {
      return true; // early return for performance
    }
  }
  return false;
} // AreAnyLabelsVisible()


/**
 * Returns if there are any densities that are hidden.
 * @return any hidden densities?
*/
template <class T>
inline bool Component<T>::AreAnyDensitiesInvisible() const
{
  for (uint4 i(0); i<GetComponentCount(); ++i)
  {
    if (m_vDensityVisible[i] == false)
    {
      return true; // early return for performance
    }
  }
  return false;
} // AreAnyDensitiesInvisible()


#undef FILE_REVISION

// $Log: Component.inl,v $
// Revision 1.2  2006/06/01 20:25:16  frank
// updated to visual studio 2005
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.33  2002/08/14 19:14:59  geconomos
// Added new component. Changed default components names.
//
// Revision 3.32  2002/08/14 16:26:26  ingmar
// added scoping brackets to allow compilation in VC++ 6.0
//
// Revision 3.31  2002/08/13 17:45:25  frank
// Added more enumerations.
//
// Revision 3.30  2002/08/02 13:35:04  michael
// Added reserve(256) to prevent constant memory copying and a fatal crash
// when loading studies with more than 40+ components. Needs further investigations
//
// Revision 3.29  2002/07/08 13:23:58  geconomos
// exported class
//
// Revision 3.28  2002/06/20 20:39:20  michael
// fixed bug for not showing component NONE
//
// Revision 3.27  2002/06/12 23:21:21  frank
// Added a test for invisible densities.
//
// Revision 3.26  2002/04/26 15:57:35  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.25  2002/04/09 15:27:35  geconomos
// Removed firing event on Components sets.
//
// Revision 3.24  2002/03/29 22:35:59  jaddonisio
// Changed default constructor so that it does not create the predifned
// components other than "None". Other v3D products will have to change
// accordingly when we merge(e.g., Calcium Scoing).
//
// Revision 3.23  2002/03/11 17:39:19  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.22  2002/03/07 23:29:54  dmitry
// fixed more typos
//
// Revision 3.21  2002/03/07 23:17:56  dmitry
// Fixed update bug.
//
// Revision 3.20  2002/03/07 23:13:57  dmitry
// Fixed update bug.
//
// Revision 3.19  2002/03/07 22:29:10  dmitry
// Modified to fit new architecture.
//
// Revision 3.18  2002/02/28 19:29:07  michael
// coding guidelines adjustments
//
// Revision 3.17.2.2  2002/03/30 00:10:06  frank
// Improved the speed of loading.  Improved the speed of plaque assignment.  Permitted more than 200 plaques.
//
// Revision 3.17.2.1  2002/02/28 19:43:22  frank
// Coding compliance.
//
// Revision 3.18  2002/02/28 19:29:07  michael
// coding guidelines adjustments
//
// Revision 3.17  2002/01/15 17:57:45  geconomos
// Changed color of non-calcified plaque component ( AAA demo)
//
// Revision 3.16  2001/11/21 04:10:58  frank
// Updated colors according to Marketing.
//
// Revision 3.15  2001/11/16 04:38:35  michael
// added missing implementation ....
//
// Revision 3.14  2001/11/16 03:56:50  frank
// Added pre-multiplied color.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Component.inl,v 1.2 2006/06/01 20:25:16 frank Exp $
// $Id: Component.inl,v 1.2 2006/06/01 20:25:16 frank Exp $
