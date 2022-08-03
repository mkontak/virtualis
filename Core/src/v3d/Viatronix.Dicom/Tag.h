#pragma once


using namespace System;
using namespace Collections::Generic;


OPEN_DICOM_NS

/** 
 * Defines a DICOM tag consisting of a GroupId and ElementId id
 */
public ref class Tag : public IComparable
{

#pragma region constructors


public:

  /// Constructor
  Tag(unsigned short iGroup, unsigned short iElement) : m_iGroup(iGroup), m_iElement(iElement) { }


protected:

  // Default constructor
  Tag() : m_iGroup(0), m_iElement(0) {}

#pragma endregion

#pragma region methods

public:

  /// compare objects
  virtual int CompareTo(Object ^ mpObject);

  /// compare tags
  int CompareTo(Tag ^ mpTag);

  /// Determine equality 
  virtual bool Equals(Object ^ mpObject) override;

  /// Determines equality
  bool Equals(Tag ^ mpTag);

  /// Creates the tag from the string
  static Tag ^ Create(String ^ msTag);

  /// Returns the string representation of the object (Group,Element)
  virtual String ^ ToString() override
  {
    return String::Format("{0:X4},{1:X4}", m_iGroup, m_iElement)->ToLower();
  } // ToString()

private:

  ///
  static unsigned short ToUInt16(Char ch);

#pragma endregion

#pragma region properties

public:

  /// Gets the tag id 
  property int Id
  {

    int get() { return ((int)m_iGroup << 16) + (int)m_iElement;  }
  }

  /// Gets the Group
  property unsigned short Group
  {
    unsigned short get() { return m_iGroup; }
    //void set(unsigned short iValue) { m_iGroup = iValue; }
  } // Group


  /// Gets the Field
  property unsigned short Element
  {
    unsigned short get() { return m_iElement; }
    //void set(unsigned short iValue) { m_iElement = iValue; }
  } // Field

#pragma endregion

#pragma region fields


protected:

  /// Group identifier
  unsigned short m_iGroup;

  /// Element identifier
  unsigned short m_iElement;

#pragma endregion

  

};  // class tag




CLOSE_DICOM_NS