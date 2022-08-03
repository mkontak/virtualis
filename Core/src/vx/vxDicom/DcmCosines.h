// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmCosines.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "DcmUtility.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{

/**
 * Implements the cosines as an object
 */
class DCM_DLL DcmCosines
{
public:

  /// Constructor
  DcmCosines();
 
  /// Copy constructor
  DcmCosines(const DcmCosines & cosines );

  /// Constructor the vector into the cosines
  DcmCosines(std::vector<double> cosines);

  /// Constructor
  DcmCosines(const std::string & sDicomValue);
  
	/// to string
  std::string ToString();

  /// Sets the cosines
  void Assign(const std::string & sCosines);

  /// Overload to create the date from a stream
  friend std::istream & operator>>(std::istream & is, DcmCosines & cosines) 
  {
    std::string sCosines;
    is >> sCosines;
    
    cosines.Assign(sCosines);

    return is;
  } // operator>>
 
  /// Overload to create the cosines from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmCosines & cosines) 
  {
    os << cosines.ToString();
    
    return os;
  } // operator>>

private:

  /// Initializes
  void Initialize();


public:

  /// Sets the value by index
  void Assign(int index, double value);


  /// Gets the orientation defined by these unit cosines
  DcmOrientations GetOrientation();

  /// Gets the plane associated with these unit cosines
  DcmPlanes GetPlane();

  /// Gets the Row .. X
  double GetRowX()
  { return m_cosines[ROW_X]; }

  /// Sets the Row .. X
  void SetRowX(double rowX)
  { m_cosines[ROW_X] = rowX; }

  /// Gets the Row .. Y
  double GetRowY()
  { return m_cosines[ROW_Y]; }

  /// Sets the Row .. Y
  void SetRowY(double rowY)
  { m_cosines[ROW_Y] = rowY; }

  /// Gets the Row .. Z
  double GetRowZ()
  { return m_cosines[ROW_Z]; }

  /// Sets the Row .. Z
  void SetRowZ(double rowZ)
  { m_cosines[ROW_Z] = rowZ; }

  /// Gets the Column .. X
  double GetColumnX()
  { return m_cosines[COL_X]; }

  /// Sets the Column .. X
  void SetColumnX(double rowX)
  { m_cosines[COL_X] = rowX; }

  /// Gets the Column .. Y
  double GetColumnY()
  { return m_cosines[COL_Y]; }

  /// Sets the Column .. Y
  void SetColumnY(double rowY)
  { m_cosines[COL_Y] = rowY; }

  /// Gets the Column .. Z
  double GetColumnZ()
  { return m_cosines[COL_Z]; }

  /// Sets the Column .. Z
  void SetColumnZ(double rowZ)
  { m_cosines[COL_Z] = rowZ; }

private:

  enum DcmCosineIndexs
  {
    ROW_X = 0,
    ROW_Y,
    ROW_Z,
    COL_X,
    COL_Y,
    COL_Z
  };

  std::vector<double> m_cosines;

}; // class DcmCosines

}; // namespace dcm

#pragma warning(pop)