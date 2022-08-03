// $Id: Component.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
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

/**
 * Template class Component<T>
 */

#ifndef Component_h
#define Component_h


#include "PixelRGBA.h"
#include "PixelRGB.h"



// class declaration
template <class T> 
class Component
{

// Enumerations
public:

  /// The different types of 3D model components.
  enum SegmentEnum 
  { 
    NONE = 0,
    AIR = 1,
    BODY = 2,
    SOFT_PLAQUE = 3,
    BLOOD = 4 ,
    BONE = 5,
    CALCIFIED_PLAQUE = 6,
    NON_CALCIFIED_PLAQUE = 7,
    STENT = 8,
    METAL = 9,
    LEFT_MAIN_ARTERY = 10,
    LEFT_ANTERIOR_DESCENDING = 11,
    LEFT_CIRCUMFLEX_ARTERY = 12,
    RIGHT_CORONARY_ARTERY = 13,
    POSTERIOR_DESCENDING_ARTERY = 14,
    SUSPICIOUS_CALCIFICATION = 15,
    PLAQUE_131HU_TO_200HU = 16,
    PLAQUE_201HU_TO_300HU = 17,
    PLAQUE_301HU_TO_400HU = 18,
    PLAQUE_401HU_AND_ABOVE = 19,
    LEFT_ANTERIOR_DESCENDING_PROXIMAL = 20,
    LEFT_ANTERIOR_DESCENDING_MID = 21,
    LEFT_ANTERIOR_DESCENDING_DISTAL = 22,
    LEFT_ANTERIOR_DESCENDING_DIAG_1 = 23,
    LEFT_ANTERIOR_DESCENDING_DIAG_2 = 24,
    LEFT_CIRCUMFLEX_ARTERY_PROXIMAL = 25,
    LEFT_CIRCUMFLEX_ARTERY_MID = 26,
    LEFT_CIRCUMFLEX_ARTERY_DISTAL = 27,
    LEFT_CIRCUMFLEX_ARTERY_OM1 = 28,
    RIGHT_CORONARY_ARTERY_PROXIMAL = 29,
    RIGHT_CORONARY_ARTERY_MID = 30,
    RIGHT_CORONARY_ARTERY_DISTAL = 31,
    AORTIC_VALVE = 32,
    MITRAL_VALVE = 33,
    THORACIC_AORTA = 34,
    ABDOMINAL_AORTA = 35,
    OTHER_CALCIFICATION = 36,
    MAX_SEGMENTS = 37
  }; // enum SegmentEnum
  
  /// The events supported by this class.
  enum EventsEnum
  {
    EVENT_ADDED           = 0x00000001,
    EVENT_REMOVED         = 0x00000002,
    EVENT_NAME            = 0x00000004,
    EVENT_SELECTED        = 0x00000008,
    EVENT_LOCKED          = 0x00000010,
    EVENT_COLOR           = 0x00000020,
    EVENT_LABEL_VISIBLE   = 0x00000040,
    EVENT_DENSITY_VISIBLE = 0x00000080,
    EVENT_MODIFIED        = 0x00000100,
    EVENT_RESET           = 0x00000160,
  }; // enum EventsEnum
  
protected:
private:


// Member Functions
public:

  /// Default Constructor.
  Component<T>();

  /// Constructor.
  Component<T>( bool bSetDefaults );

  /// Destructor.
  ~Component<T>();

 
  /// Assignment operator from Component.
  inline Component<T> & operator = (const Component<T> & rhs);


  // Access Functions
  // ----------------

  /// Get the numer of components.
  inline uint4 GetComponentCount() const { return m_vName.size(); };

  /// Get the selected state of the component.
  inline bool GetSelected(const uint4 uLabelIndex);

  /// Set the selected state of the component.
  inline void SetSelected(const uint4 uLabelIndex, bool bSelected);

  /// Sets defaults for all components.
  inline void SetDefaults();

  /// Get the name of the component.
  inline const std::string & GetName(const uint4 uLabelIndex);

  /// Set the name of the component.
  inline void SetName(const uint4 uLabelIndex, const std::string & sName);

  /// Get the color of the component.
  inline PixelRGBA<T> & GetColor(const uint4 uLabelIndex); 
  
  /// Get the premultipled (by alpha) color of the component.
  inline PixelRGBA<T> & GetPremultipliedColor(const uint4 uLabelIndex);
  
  /// Set the color of the component.
  inline void SetColor(const uint4 uLabelIndex, PixelRGBA<T> & color);

  /// Get the active flag for the component.
  inline const bool GetLabelVisible(const uint4 uLabelIndex) const;

  /// Set the active flag for the component.
  inline void SetLabelVisible(const uint4 uLabelIndex, bool bLabelVisible);

  /// Get the active flag for the component.
  inline const bool GetDensityVisible(const uint4 uLabelIndex) const;
  
  /// Set the active flag for the component.
  inline void SetDensityVisible(const uint4 uLabelIndex, bool bDensityVisible);
 
  /// Get the lock flag for the component.
  inline bool GetLock(const uint4 uLabelIndex);

  /// Set the lock flag for the component.
  inline void SetLock(const uint4 uLabelIndex, bool bLock);

  /// Return the color of the component if it is active otyherwise return black.
  inline PixelRGBA<T>  GetActiveColor(const uint4 uLabelIndex);

  /// Add a new component.
  inline const uint2 AddComponent(const std::string & sName, PixelRGBA<T> & color);

  /// Removes a component.
  inline void RemoveComponent(const uint4 uLabelIndex);

  /// Clear components.
  inline void Clear();

  /// Returns if there are any visible labels.
  inline bool AreAnyLabelsVisible() const;

  /// Returns if there are any densities that are hidden.
  inline bool AreAnyDensitiesInvisible() const;

protected:
 
  /// Notify framework that this object has changed.
  virtual inline void FireOnChanged(uint4 uFlags, int4 iID) {};

  /// Helper function to calculate the premultiplied color.
  inline PixelRGBA<T> ComputePremultipliedColor(const PixelRGBA<T> & inColor) const;

  /// Helper function to calculate the premultiplied color.
  inline PixelRGBA<T> ComputePremultipliedColor(const PixelRGB<T> & inColor) const;
  

// Data Members
private:
  
  /// Names of each component.
  std::vector<std::string> m_vName;

  /// Colors of each component.
  std::vector< PixelRGBA<T> > m_vColor;
  
  /// Premultipled colors of each component.
  std::vector< PixelRGBA<T> > m_vPremultipliedColor;
  
  /// Active state of each component.
  std::vector<bool> m_vLabelVisible;

  /// Active state of each component.
  std::vector<bool> m_vDensityVisible;  

  /// Lock state of each component.
  std::vector<bool> m_vLock;

  /// Selected state of each component.
  std::vector<bool> m_vSelected;

}; // class Component



#include "Component.inl"

#endif // Component_h



// $Log: Component.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 3.22  2002/08/14 19:14:59  geconomos
// Added new component. Changed default components names.
//
// Revision 3.21  2002/08/13 16:02:36  frank
// Added more enumerations.
//
// Revision 3.20  2002/07/08 13:23:58  geconomos
// exported class
//
// Revision 3.19  2002/06/20 20:39:20  michael
// fixed bug for not showing component NONE
//
// Revision 3.18  2002/06/12 23:21:21  frank
// Added a test for invisible densities.
//
// Revision 3.17  2002/04/26 15:57:35  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.16  2002/04/15 15:33:51  jaddonisio
// New event: Modified.
//
// Revision 3.15  2002/03/11 17:39:19  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.14  2002/03/07 22:27:09  dmitry
// Modified to fit new architecture.
//
// Revision 3.13  2002/02/28 19:29:07  michael
// coding guidelines adjustments
//
// Revision 3.12.2.3  2002/03/30 00:10:06  frank
// Improved the speed of loading.  Improved the speed of plaque assignment.  Permitted more than 200 plaques.
//
// Revision 3.12.2.2  2002/02/28 19:45:25  frank
// Coding compliance.
//
// Revision 3.12.2.1  2002/02/21 20:18:39  frank
// Coding standards compliance.
//
// Revision 3.12  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.11  2001/11/16 03:56:50  frank
// Added pre-multiplied color.
//
// Revision 3.10  2001/11/13 19:03:09  dmitry
// Changed Active to LabelVisible and DensityVisible in Components class.
//
// Revision 3.9  2001/11/10 03:25:21  dmitry
// Events modified
//
// Revision 3.8  2001/11/09 19:20:27  dmitry
// Modified event throwing, TBD: Event Flag
//
// Revision 3.7  2001/11/08 22:23:49  geconomos
// Started new event processing (Have fun Dmitry!)
//
// Revision 3.6  2001/11/06 22:19:56  dmitry
// Count, Selected, and Added and Removed methods added and enums changed to uint1.
//
// Revision 3.5  2001/11/05 16:41:31  jaddonisio
// SetLock & GetLock take uint1.
//
// Revision 3.4  2001/10/29 15:05:06  liwei
// Modified to be able to create tag volumes for VolumePro.
//
// Revision 3.3  2001/10/25 20:58:05  jaddonisio
// Added new methods for locking. Also, created SetDefaults method and modified GetComponentCount to return actual size.
//
// Revision 3.2  2001/10/24 18:05:49  jenny
// new components added
//
// Revision 3.1  2001/10/19 18:36:09  jenny
// Added  AddComponent, RemoveComponent, GetNewComponent
//
// Revision 3.0  2001/10/14 23:02:27  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 11 2001 19:19:02   frank
// Added log at footer.
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Component.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// $Id: Component.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
