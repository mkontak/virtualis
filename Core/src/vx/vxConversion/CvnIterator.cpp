// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


// Includes
#include "stdafx.h"

// Includes
#include "vxBlockVolumeLibrary.h"
#include "VxBlockVolume.h"
#include "VxBlockVolumeIterator.h"

#include "CvnIterator.h"
#include "CvnExceptions.h"


using namespace cvn;

/**
* Constructor
*
* @param blockVolumePtr      Block volume pointer
* @param eType               Image layout type
*/
CvnIterator::CvnIterator() :
m_ePlane(dcm::DcmPlanes::P_NONE),
m_pIterator(__nullptr)
{
  Initialize();

} // CvnIterator()


/**
 * Constructor
 *
 * @param pVolume       Volume
 * @param ePlane        Plane of the images
 */
CvnIterator::CvnIterator(std::shared_ptr<vxBlockVolume<uint2>> pVolume, dcm::DcmPlanes ePlane) :
m_ePlane(ePlane),
m_pIterator(__nullptr)
{

  if ( (int)ePlane > (int)dcm::DcmPlanes::P_AXIAL )
    throw CvnInvalidDataException(LogRec(util::Strings::Format("Iterator does not support the plane (%d)", (int)ePlane), "CvnIterator", "CvnIterator"));

  Initialize();

  m_pIterator = std::make_shared<vxBlockVolumeIterator<unsigned short>>(*pVolume);

  if ( m_pIterator == __nullptr )
    throw CvnOutOfMemoryException(LogRec("Failed to allocation volume iterator","CvnIterator", "CvnIterator"));

} // CvnIterator(vxBlockVolume<uint2> * pVolume, dcm::DcmPlanes ePlane)

/**
 * Finalizer for volume iterator
 */
CvnIterator::~CvnIterator()
{
  m_pIterator = __nullptr; 
} // !Iterator


/**
 * Initializes the object
 */
void CvnIterator::Initialize()
{
  m_next.push_back(&CvnIterator::_NextXZY);                         // Sagittal
  m_next.push_back(&CvnIterator::_NextYXZ);                         // Coronal
  m_next.push_back(&CvnIterator::_NextZYX);                         // Axial
 
  m_depthIncrement.push_back(&CvnIterator::_IncrementX);       // Sagittal
  m_depthIncrement.push_back(&CvnIterator::_IncrementY);       // Coronal
  m_depthIncrement.push_back(&CvnIterator::_IncrementZ);       // Axial

  m_rowIncrement.push_back(&CvnIterator::_IncrementZ);         // Sagittal
  m_rowIncrement.push_back(&CvnIterator::_IncrementZ);         // Coronal
  m_rowIncrement.push_back(&CvnIterator::_IncrementY);         // Axial

  m_columnIncrement.push_back(&CvnIterator::_IncrementY);      // Sagittal
  m_columnIncrement.push_back(&CvnIterator::_IncrementX);      // Coronal
  m_columnIncrement.push_back(&CvnIterator::_IncrementX);      // Axial


  m_setPosition.push_back(&CvnIterator::_SetSagittalPos);
  m_setPosition.push_back(&CvnIterator::_SetCoronalPos);
  m_setPosition.push_back(&CvnIterator::_SetAxialPos);

  //////////////////////////////////////////////////////////////////////////
  // The Depth Position is the axis in which the images are stacked
  //////////////////////////////////////////////////////////////////////
  m_setDepth.push_back(&CvnIterator::_SetPositionX);           // Sagittal - X
  m_setDepth.push_back(&CvnIterator::_SetPositionY);           // Coronal  - Y
  m_setDepth.push_back(&CvnIterator::_SetPositionZ);           // Axial    - Z

  m_setRow.push_back(&CvnIterator::_SetPositionZ);             // Sagittal - Row = Z
  m_setRow.push_back(&CvnIterator::_SetPositionZ);             // Coronal  - Row = Z
  m_setRow.push_back(&CvnIterator::_SetPositionY);             // Axial    - Row = Y
 
  m_setColumn.push_back(&CvnIterator::_SetPositionY);          // Sagittal  - Column = Y
  m_setColumn.push_back(&CvnIterator::_SetPositionX);          // Coronal   - Column = X
  m_setColumn.push_back(&CvnIterator::_SetPositionX);          // Axial     - Column = x

} // Initialize()


#pragma region Next methods

/** 
 * moves the iterator to the next voxel according to a ZYX traversal (AXIAL)
 */
void CvnIterator::NextZYX()
{ 
  m_pIterator->NextZYX(); 
} // NextZYX()

/** 
 * moves the iterator to the next voxel according to a XZY traversal (SAGITTAL)
 */
void CvnIterator::NextXZY()
{ 
  m_pIterator->NextXZY(); 
} // NextXYZ()

/** 
 * moves the iterator to the next voxel according to a YXZ traversal (CORONAL)
 */
void CvnIterator::NextYXZ()
{  
  m_pIterator->NextYXZ(); 
} // NextYXZ()

/** 
 * Next Block Z same offset
 */
void CvnIterator::NextBlockZsameOffset() 
{ 
  m_pIterator->NextBlockZsameOffset(); 
} // NextBlockZsameOffset()

/** 
 * Next Block Y same offset
 */
void CvnIterator::NextBlockYsameOffset() 
{ 
  m_pIterator->NextBlockYsameOffset(); 
} // NextBlockYsameOffset()

/** 
 * Next XZY poistion inside the block
 */
void CvnIterator::NextXZYinsideBlock() 
{ 
  m_pIterator->NextXZYinsideBlock(); 
} // NextXZYinsideBlock()

#pragma endregion

#pragma region SetPosition methods

/** 
 * Sets the position in the volume
 */
void CvnIterator::SetPos(int x, int y, int z) 
{ 
  m_pIterator->SetPos(x,y,z); 
} // SetPos()

/** 
 * Sets the X position
 */
void   CvnIterator::SetPosX(int x) 
{ 
  m_pIterator->SetPosX(x); 
} // SetPositionX(int x) 

/** 
* Sets the Y position
*/
void CvnIterator::SetPosY(int y) 
{ 
  m_pIterator->SetPosY(y);
} // SetPositionY(int y) 

/** 
* Sets the Z position
*/
void CvnIterator::SetPosZ(int z) 
{ 
  m_pIterator->SetPosZ(z);
} // SetPositionZ(int z) 


#pragma endregion

#pragma region Decrement methods

/** 
 * Decrrement X
 */
void CvnIterator::DecrementX() 
{ 
  m_pIterator->DecX(); 
} // DecrementX()

/** 
 * Decrrement Y
 */
void CvnIterator::DecrementY() 
{ 
  m_pIterator->DecY(); 
} // DecrementY()

/** 
 * Decrrement Z
 */
void CvnIterator::DecrementZ() 
{ 
  m_pIterator->DecZ(); 
} // DecrementZ()

#pragma endregion

#pragma region Increment methods

/** 
 * Increment X
 */
void CvnIterator::IncrementX()
{ 
  m_pIterator->IncX(); 
} // IncrementX()

/** 
 * Increment Y
 */
void CvnIterator::IncrementY() 
{ 
  m_pIterator->IncY(); 
} // IncrementY()

/** 
 * Increment Z
 */
void CvnIterator::IncrementZ() 
{ 
  m_pIterator->IncZ(); 
} // IncrementZ()

#pragma endregion


#pragma region SetVoxel/GetVoxel methods

/** 
 * Sets the voxel
 */
void CvnIterator::SetVoxel(short iVoxel) 
{ 
  m_pIterator->SetVoxel(iVoxel); 
} // SetVoxel()

/** 
* Sets the voxel
*/
void CvnIterator::SetVoxel(unsigned short iVoxel) 
{ 
  m_pIterator->SetVoxel(iVoxel); 
} // SetVoxel()

/** 
 * Gets the voxel
 */
short CvnIterator::GetVoxel() 
{ 
  return m_pIterator->GetVoxel(); 
} // GetVoxel()

#pragma endregion

#pragma region properties

/** 
 * Returns the current X position
 */
int CvnIterator::GetX() 
{ 
  return m_pIterator->GetPosX(); 
} // GetX()

/** 
 * Sets the X position
 */
void CvnIterator::SetX(int X) 
{ 
  m_pIterator->SetPosX(X); 
} // SetX(int x)

/** 
 * Returns the current Y position
 */
int CvnIterator::GetY() 
{ 
  return m_pIterator->GetPosY(); 
} // GetY()

/** 
 * Sets the Y position
 */
void CvnIterator::SetY(int Y) 
{ 
  m_pIterator->SetPosY(Y);
} // SetY(int y)


/** 
 * Returns the current Z position
 */
int CvnIterator::GetZ() 
{ 
  return m_pIterator->GetPosZ(); 
} // GetZ()

/** 
 * Sets the Z position
 */
void CvnIterator::SetZ(int z) 
{ 
  m_pIterator->SetPosZ(z);
} // SetZ(int z)


/** 
 * Returns the delta Z
 */
int CvnIterator::GetVoxelOffsetDeltaZ() 
{ 
  return vxBlockVolumeIteratorConsts::eL0VoxOffsetDeltaZ; 
} // GetVoxelOffsetDeltaZ()

#pragma endregion

