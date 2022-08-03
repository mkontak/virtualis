// $Id: vxComponent.C,v 1.2 2007/01/25 21:53:09 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Romy Sreedharan (mailto:romy@viatronix.com)



#include "stdafx.h"
#include "vxComponentData.h"
#include "vxComponent.h"


/*
 * default constructor
 */
vxComponent::vxComponent():
m_bIntensityVisible(true),
m_bLabelVisible(true),
m_bLocked(true),
m_pComponentData(NULL),
m_bRemovable(true),
m_bGrowable(true),
m_bShrinkable(true),
m_bUnlockable(true),
m_bNameEditable(true)
{
  
  //default constructor

}//vxComponent()


/*
 *constructor
 */
vxComponent::vxComponent( uint2 uLabelId, std::string sName, PixelRGBA<uint1> labelColor,        
                         bool bIntensityVisible, bool bLabelVisible, bool bLocked, bool bSelected )
                         :
  m_uLabelId(uLabelId),
  m_sName(sName),
  m_labelColor(labelColor),
  m_bIntensityVisible(bIntensityVisible),
  m_bLabelVisible(bLabelVisible),
  m_bLocked(bLocked),
  m_pComponentData(NULL),
  m_bRemovable(true),
  m_bGrowable(true),
  m_bShrinkable(true),
  m_bUnlockable(true),
  m_bNameEditable(true)
{
  //Constructor

}//vxComponent( uint2 uLabelId, std::string sName, PixelRGBA<uint1> labelColor,        
     // bool bIntensityVisible, bool bLabelVisible, bool bLocked, bool bSelected )

/*
 * copy constructor
 */
vxComponent::vxComponent(const vxComponent & other) :
  m_uLabelId(other.m_uLabelId),
  m_sName(other.m_sName),
  m_labelColor(other.m_labelColor), 
  m_bIntensityVisible(other.m_bIntensityVisible),
  m_bLabelVisible(other.m_bLabelVisible),
  m_bLocked(other.m_bLocked),
  m_bRemovable(other.m_bRemovable),
  m_bGrowable(other.m_bGrowable),
  m_bShrinkable(other.m_bShrinkable),
  m_bUnlockable(other.m_bUnlockable),
  m_bNameEditable(other.m_bNameEditable)
{
  //Copy the component data
  if ( other.m_pComponentData != NULL )
    m_pComponentData = other.m_pComponentData->Clone() ;
  else
    m_pComponentData = NULL;

}//vxComponent(const vxComponent & other) 



/*
 *Assignement operator
 */
vxComponent& vxComponent::operator=( const vxComponent & rhs )
{
  if( &rhs != this )
  {
    m_uLabelId    = rhs.m_uLabelId;
    m_sName       = rhs.m_sName;
    m_labelColor  = rhs.m_labelColor; 
    m_bLocked     = rhs.m_bLocked;
    m_bRemovable  = rhs.m_bRemovable;
    m_bGrowable   = rhs.m_bGrowable;
    m_bShrinkable = rhs.m_bShrinkable;
    m_bUnlockable = rhs.m_bUnlockable;
    m_bNameEditable     = rhs.m_bNameEditable;
    m_bIntensityVisible = rhs.m_bIntensityVisible;
    m_bLabelVisible     = rhs.m_bLabelVisible;

    //Copy the component data
    if ( rhs.m_pComponentData != NULL )
      m_pComponentData = rhs.m_pComponentData->Clone() ;
    else
      m_pComponentData = NULL;
  }
  return *this;
}//operator=( const vxComponent & rhs )



/*
 *Copy to Method
 * @dest vxComponent Object
 * &bWithComponentData flag indicates whether to copy component data or not
 */
void vxComponent::CopyTo( vxComponent & dest, bool bWithComponentData )
{
  if( &dest != this )
  {
    dest.m_uLabelId    = m_uLabelId;
    dest.m_sName       = m_sName;
    dest.m_labelColor  = m_labelColor; 
    dest.m_bLocked     = m_bLocked;
    dest.m_bRemovable  = m_bRemovable;
    dest.m_bGrowable   = m_bGrowable;
    dest.m_bShrinkable = m_bShrinkable;
    dest.m_bUnlockable = m_bUnlockable;
    dest.m_bNameEditable     = m_bNameEditable;
    dest.m_bIntensityVisible = m_bIntensityVisible;
    dest.m_bLabelVisible     = m_bLabelVisible;

    //Copy the component data only if the flag is ON
    if ( bWithComponentData && m_pComponentData != NULL )
      dest.m_pComponentData = m_pComponentData->Clone() ;
    else
      dest.m_pComponentData = NULL;
  }
}//CopyTo( vxComponent & dest, bool bWithComponentData )

/*
 * Destructor
 */
vxComponent::~vxComponent() 
{
  delete m_pComponentData;
  m_pComponentData = NULL;
}//~vxComponent()





// $Log: vxComponent.C,v $
// Revision 1.2  2007/01/25 21:53:09  romy
// Fixed the issue of loosing the component data between Tab switching. Calcium scoring was suffering from this with black blocks on Plaques
//
// Revision 1.1  2006/12/14 16:44:34  romy
// vxComponent moved from header file and fixed its by value copying which was causing memory errors
//

// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxComponent.C,v 1.2 2007/01/25 21:53:09 romy Exp $
// $Id: vxComponent.C,v 1.2 2007/01/25 21:53:09 romy Exp $
