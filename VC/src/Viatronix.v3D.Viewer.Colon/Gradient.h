// $Id: Gradient.h,v 1.3.2.2 2007/03/16 23:37:34 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Gradient.h
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#if !defined(AFX_GRADIENT_H__B8518180_B289_11D4_A890_C3D6794EED5F__INCLUDED_)
#define AFX_GRADIENT_H__B8518180_B289_11D4_A890_C3D6794EED5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

namespace WinUtils
{
  /// a peg within a gradient control
  class Peg : CObject
  {
  public:
    /// Default constructor
    Peg() : m_uId(m_uUniqueID++), m_colour(0x00000000), m_colourPrevEnd(0x0), m_fPosition(0.0f) {}
    /// Constructor
    Peg(float fPos, COLORREF colour) : m_uId(m_uUniqueID++) { m_fPosition = fPos; m_colour = colour; m_colourPrevEnd = colour; }
    /// Copy constructor
    Peg(const Peg &src) : m_colour(src.m_colour), m_colourPrevEnd(src.m_colourPrevEnd), m_fPosition(src.m_fPosition), m_uId(m_uUniqueID++) {}
    /// Copy operator
    Peg& operator =(const Peg &src) { m_colour = src.m_colour, m_colourPrevEnd = src.m_colourPrevEnd, m_fPosition = src.m_fPosition, m_uId = src.m_uId; return *this;};
    /// File serializer
    void Serialize(CArchive &ar);
    /// Get the peg ID
    const uint4 GetID() const {return m_uId;};
    /// CObject class method
    DECLARE_SERIAL(Peg)

    /// Returns the current color
    COLORREF GetColor() const { return m_colour; }
    /// Sets the current color
    void SetColor(COLORREF col) { m_colour = col; }

    /// Returns the current previous end color
    COLORREF GetPrevEndColor() const { return m_colourPrevEnd; }
    /// Sets the current color
    void SetPrevEndColor(COLORREF col) { m_colourPrevEnd = col; }

    /// Returns the current position
    float4 GetPosition() const { return m_fPosition; }
    /// Sets the current position
    void SetPosition(float fPos) { m_fPosition = fPos; }

  public:
    /// Color of the peg
    COLORREF m_colour;
    /// Location of the peg
    float4 m_fPosition;
    /// For dual-color mode, the end-color of the previous range
    COLORREF m_colourPrevEnd;

  private:
    /// ID of this peg
    uint4 m_uId;
    /// Holder for next peg ID
    static uint4 m_uUniqueID;
  }; // class Peg

  /// interpolate colours in the gradient
  typedef COLORREF (__cdecl* InterpolateFn)(COLORREF first, COLORREF second, float position, float start, float end);

  /// list of gradient pegs
  class GradientList : public CObject
  {
  public:
    /// Default constructor
    GradientList();
    /// Copy constructor
    GradientList(const GradientList &src);
    /// Destructor
    virtual ~GradientList();
    /// copy operator
    GradientList& operator =(const GradientList &src);

    DECLARE_SERIAL(GradientList)

  //----- Attributes -----//
    /// adds a new peg
    int AddPeg(COLORREF crColour, float fPosition);
    /// adds a new peg
    int AddPeg(Peg peg);

    /// removes a peg
    void RemovePeg(int iIndex);

    /// get number of pegs
    int GetPegCount() const;

    /// gets peg by index
    const Peg GetPeg(int iIndex) const;

    /// sets parameters for a peg
    int SetPeg(int iIndex, COLORREF crColour, float fPosition);
    /// sets parameters for a peg
    int SetPeg(int iIndex, Peg peg);

    /// gets index from a peg location
    int IndexFromPos(float pos);
    /// gets index from a peg location
    int IndexFromPos(const float pos, const CArray <Peg, Peg&> & vPegs);

    /// first peg's colour
    void SetStartPegColour(const COLORREF crColour){m_pegStart.SetColor(crColour);};
    /// first peg's colour
    COLORREF GetStartPegColour() const {return m_pegStart.GetColor();};

    /// last peg's colour
    void SetEndPegColour(const COLORREF crColour) {m_pegEnd.SetColor(crColour);};
    /// last peg's colour
    COLORREF GetEndPegColour() const {return m_pegEnd.GetColor();};

    /// colour of control background
    void SetBackgroundColour(const COLORREF crColour) {m_pegBackground.SetColor(crColour);};
    /// colour of control background
    COLORREF GetBackgroundColour() const {return m_pegBackground.GetColor();};

    /// whether to use a background
    void SetUseBackground(const BOOL bUseBackground) {m_bUseBackground = bUseBackground;};
    /// whether to use a background
    BOOL GetUseBackground() const {return m_bUseBackground;};

    /// Returns true if order of pegs is fixed (i.e. pegs cannot move past one another
    bool IsPegOrderFixed() const { return m_bFixedOrder; }
    /// Allow/disallow the peg order to be rearranged
    void SetPegOrderFixed(const bool bFixed) { m_bFixedOrder = bFixed; }

    /// Returns true if each peg has a start and (previous range) end color defined
    bool IsDiscreteColorRangesMode() const { return m_bDiscreteRanges; }
    /// Turn dual color mode on or off
    void SetDiscreteColorRangesMode(const bool bMode) { m_bDiscreteRanges = bMode; }

    /// colour interpolation
    enum InterpolationMethodEnum
    {
      LINEAR,
      FLATSTART,
      FLATMID,
      FLATEND,
      COSINE,
      HSLREDBLUE,
      HSLBLUERED,
      HSLSHORTEST,
      HSLLONGEST,
      REVERSE
    }; // enum InterpolationMethodEnum
    /// peg types
    enum { BACKGROUND = -4, STARTPEG = -3, ENDPEG = -2, NONE = -1 };

    /// get interpolation method
    InterpolationMethodEnum GetInterpolationMethod() const;
    /// set interpolation method
    void SetInterpolationMethod(const InterpolationMethodEnum eMethod);

    /// get quantization
    int GetQuantization() const;
    /// set quantization
    void SetQuantization(const int entries);

  //----- Operations -----//
    /// make colour palette
    void MakePalette(CPalette *lpPalette);
    /// make colour palette
    void Make8BitPalette(RGBTRIPLE *lpPal);
    /// make colour entries
    void MakeEntries(RGBTRIPLE *lpPal, int iEntryCount);
    
    /// colour from a location
    COLORREF ColourFromPosition(float pos);
    
    /// gradient file I/O
    void Serialize(CArchive &ar);

  //----- Internals -----//
  protected:
    /// sorts pegs by location
    void SortPegs();
    /// colour interpolation method
    InterpolateFn GetInterpolationProc();
    /// return the next peg from that given
    POSITION GetNextPeg(POSITION current);
    
    /// interpolation routine
    static COLORREF InterpolateLinear(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateFlatStart(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateFlatMid(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateFlatEnd(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateCosine(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateHSLClockwise(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateHSLAntiClockwise(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateHSLLongest(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateHSLShortest(COLORREF first, COLORREF second,
      float position, float start, float end);
    /// interpolation routine
    static COLORREF InterpolateReverse(COLORREF first, COLORREF second,
      float position, float start, float end);

  private:  
    /// gets index of peg with the given ID
    int4 IndexFromId(uint4 id);
    /// insertion sort
    void InsertSort(int lb, int ub);
    /// partition (part of quick sort)
    int Partition(int lb, int ub);
    /// quick sort
    void QuickSort(int lb, int ub);

  protected:
    /// array of pegs
    CArray <Peg, Peg&> m_vPegs;
    /// special pegs
    Peg m_pegStart, m_pegEnd, m_pegBackground;
    /// whether to use background peg/colour
    BOOL m_bUseBackground;
    /// quantization method
    int m_iQuantization;
    /// interpolation method
    InterpolationMethodEnum m_eInterpolationMethod;

  private:
    /// True if you are not allowed to change the order of pegs (by moving) once added/inserted
    bool m_bFixedOrder;
    /// True if each peg has a start and (previous range) end color defined
    bool m_bDiscreteRanges;
  }; // class GradientList

} // namespace WinUtils

#endif // !defined(AFX_GRADIENT_H__B8518180_B289_11D4_A890_C3D6794EED5F__INCLUDED_)

// $Log: Gradient.h,v $
// Revision 1.3.2.2  2007/03/16 23:37:34  jmeade
// comments, class variable naming.
//
// Revision 1.3.2.1  2007/03/16 00:25:32  jmeade
// Issue 5494: rollback changes to fix bug introduced in code review.
//
// Revision 1.3  2007/03/14 23:32:33  jmeade
// code standards.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.2  2004/06/23 18:27:30  jmeade
// Allow for discrete color ranges between pegs (in addition to continuous).
//
// Revision 1.1.2.1  2004/04/21 03:44:19  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Gradient.h,v 1.3.2.2 2007/03/16 23:37:34 jmeade Exp $
// $Id: Gradient.h,v 1.3.2.2 2007/03/16 23:37:34 jmeade Exp $
