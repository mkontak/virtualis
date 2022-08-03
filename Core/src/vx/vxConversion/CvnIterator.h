// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

// Pragmas
#pragma once

#include "CvnEnums.h"

// Forward
template <class VoxelType> class vxBlockCvnIterator;

// Namespaces
namespace cvn
{


#pragma region function definitions


/// Increment 
typedef void (*CVN_INCREMENT_FUNCTION)(LPVOID lpCallbackData);

/// Next 
typedef void (*CVN_NEXT_FUNCTION)(LPVOID lpCallbackData);

/// Set X,Y,Z position 
typedef void (*CVN_SETPOSITION_FUNCTION)(int x, int y, int z, LPVOID lpCallbackData);

/// Set position
typedef void (*CVN_SET_FUNCTION)(int pos, LPVOID lpCallbackData);

#pragma endregion




/**
 * Encapsulates the volume iterator in a managed class
 *
 * Allows the iterator to be orientation general using delegates to set up indexed arrays of function
 * pointers that are called based on the image orientation type (index).
 *
 */
class CVN_DLL CvnIterator 
{

#pragma region contructors/destructors

private:

  /// onstructor
  CvnIterator();

public:

  /// onstructor
  CvnIterator(std::shared_ptr<vxBlockVolume<uint2>> pVolume, dcm::DcmPlanes ePlane);

  /// Destructor
  virtual ~CvnIterator();


 

#pragma endregion


public:


  /// moves the iterator to the next voxel according to a ZYX traversal (AXIAL)
  void NextZYX(void);

  /// moves the iterator to the next voxel according to a XZY traversal (SAGITTAL)
  void NextXZY(void);

  /// moves the iterator to the next voxel according to a YXZ traversal (CORONAL)
  void NextYXZ(void);

  /// Performs the default next
  void Next() 
  { m_next[(int)m_ePlane](this); }

  /// Sets the position in the volume
  void SetPosition(int x, int y, int z) 
  { m_setPosition[(int)m_ePlane]( x, y, z, this); }

  /// Sets the voxel
  void SetVoxel(short iVoxel);

  /// Sets the voxel
  void SetVoxel(unsigned short iVoxel);

  /// Gets the voxel
  short GetVoxel();

  /// Decrment X
  void DecrementX();

  /// Increment Y
  void DecrementY();

  /// Increment Z
  void DecrementZ();

  /// Increment X
  void IncrementX();

  /// Increment Y
  void IncrementY();

  /// Increment Z
  void IncrementZ();

  /// Set X Position
  void SetPosX(int X);

  /// Set Y Position
  void SetPosY(int Y);

  /// Set Z Position
  void SetPosZ(int Z);

  /// Set the image position
  void SetDepthPosition(int iImage) 
  { m_setDepth[(int)m_ePlane](iImage, this); }

  /// Set the image position
  void SetRowPosition(int iImage) 
  { m_setRow[(int)m_ePlane](iImage, this); }

  /// Set the image position
  void SetColumnPosition(int iImage) 
  { m_setColumn[(int)m_ePlane](iImage, this); }

  /// Set the image position
  void IncrementDepth() 
  { m_depthIncrement[(int)m_ePlane](this); }

  /// Set the image position
  void IncrementRow() 
  { m_rowIncrement[(int)m_ePlane](this); }

  /// Set the image position
  void IncrementColumn() 
  { m_columnIncrement[(int)m_ePlane](this); }

  /// Next Block Z same offset
  void NextBlockZsameOffset();

  /// Next Block Y same offset
  void NextBlockYsameOffset();

  /// Next XZY poistion inside the block
  void NextXZYinsideBlock();

private:

  /// INitialzies teh objects state
  void Initialize();

  /// Sets the position in the volume
  void SetPos(int x, int y, int z);


#pragma region static methods

private:

  /// moves the iterator to the next voxel according to a ZYX traversal (AXIAL)
  static void _NextZYX(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->NextZYX(); } 

  /// moves the iterator to the next voxel according to a XZY traversal (SAGITTAL)
  static void _NextXZY(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->NextXZY(); } 

  /// moves the iterator to the next voxel according to a YXZ traversal (CORONAL)
  static void _NextYXZ(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->NextYXZ(); } 

  /// Set X Position
  static void _SetPositionX(int x, LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPosX(x); } 

  /// Set Y Position
  static void _SetPositionY(int y, LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPosY(y); } 

  /// Set Z Position
  static void _SetPositionZ(int z, LPVOID lpCallbackData )
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPosZ(z); } 

  /// Increment X
  static void _IncrementX(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->IncrementX(); } 

  /// Increment Y
  static void _IncrementY(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->IncrementY(); } 

  /// Increment Z
  static void _IncrementZ(LPVOID lpCallbackData)
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->IncrementZ(); } 

   /// Sets the axial position
  static void _SetAxialPos(int x, int y, int z, LPVOID lpCallbackData) 
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPos(x, y, z); }

  /// Sets the sagittal position
  static void _SetSagittalPos(int x, int y, int z,LPVOID lpCallbackData ) 
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPos(z, x, y); }

  /// Sets the coronal position
  static void _SetCoronalPos(int x, int y, int z, LPVOID lpCallbackData) 
  { reinterpret_cast<CvnIterator *>(lpCallbackData)->SetPos(y, z, x); }

#pragma endregion


#pragma region properties

public:

  /// Sets the X position
  void SetX(int x);

  /// Gets the X position
  int GetX();

  /// Gets the Y position
  int GetY(); 

  /// Sets the Y poisition
  void SetY(int y);
 
  /// Gets the Z position
  int GetZ(); 

  /// Sets the Z
  void SetZ(int z);

  /// Returns the delta Z
  int GetVoxelOffsetDeltaZ();


#pragma endregion

#pragma region fields


private:

  /// Block Volume Iterator
  std::shared_ptr<vxBlockVolumeIterator<unsigned short>> m_pIterator;

  std::vector<CVN_NEXT_FUNCTION> m_next;

  std::vector<CVN_SETPOSITION_FUNCTION> m_setPosition;

  std::vector<CVN_INCREMENT_FUNCTION> m_depthIncrement;

  std::vector<CVN_INCREMENT_FUNCTION> m_rowIncrement;

  std::vector<CVN_INCREMENT_FUNCTION> m_columnIncrement;

  std::vector<CVN_SET_FUNCTION> m_setDepth;

  std::vector<CVN_SET_FUNCTION> m_setRow;

  std::vector<CVN_SET_FUNCTION> m_setColumn;

  /// Delegate index 
  dcm::DcmPlanes m_ePlane;

#pragma endregion

};  // class CvnIterator




}; // namespace cvn

