#pragma once

using namespace System;



OPEN_DICOM_IMAGING_NS


/** 
 * Implements a container for the window leveling parameters
 */
public ref class WindowLevel
{

#pragma region constructors

public:

  // Defaul constructor
  WindowLevel() :
  m_center(0),
  m_width(0)
  { }

  // Initialization constructor
  WindowLevel(double dCenter, double dWidth) : 
  m_center(dCenter),
  m_width(dWidth)
  { }

#pragma endregion


#pragma region properties

public:

  /// Sets/Gets the Window Center
  property double Center 
  {
    double get() { return m_center; }
    void set(double iValue) { m_center = iValue; }
  } // Center

  /// Sets/Gets the Window Width
  property double Width 
  {
    double get() { return m_width; }
    void set(double iValue) { m_width = iValue; }
  } // Width


#pragma endregion

#pragma region fields

private:

  /// Window Center
  double m_center;

  /// Window Width
  double m_width;

#pragma  endregion

};  //class  WindowLevel

CLOSE_DICOM_IMAGING_NS