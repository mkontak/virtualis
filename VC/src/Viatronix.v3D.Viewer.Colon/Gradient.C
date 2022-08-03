// $Id: Gradient.C,v 1.4.2.3 2007/03/16 23:37:33 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Gradient.C
// Description: 
// Owner: Jeff Meade [jmeade@viatronix.com]

#include "stdafx.h"
#include "Gradient.h"

#include <math.h>

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

using namespace WinUtils;

IMPLEMENT_SERIAL(GradientList, CObject, 0)
IMPLEMENT_SERIAL(Peg, CObject, 0)

uint4 Peg::m_uUniqueID = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * constructor
 */
GradientList::GradientList() : m_bFixedOrder(false), m_bDiscreteRanges(false), m_eInterpolationMethod(COSINE),
m_bUseBackground(false), m_iQuantization(-1)
{
  m_pegStart.SetColor(0x00000000);
  m_pegStart.SetPosition(0.0f);
  m_pegEnd.SetColor(0x00FFFFFF);
  m_pegEnd.SetPosition(1.0f);
  m_pegBackground.SetColor(0x00000000); //A default pal
  m_pegBackground.SetPosition(0.0f);
} // constructor


/**
 * copy constructor
 * @param src   the source
 */
GradientList::GradientList(const GradientList &src)
{
  m_pegStart.SetColor(src.m_pegStart.GetColor());;
  m_pegEnd.SetColor(src.m_pegEnd.GetColor());
  m_pegBackground.SetColor(src.m_pegBackground.GetColor());
  m_eInterpolationMethod = src.m_eInterpolationMethod;
  m_bUseBackground = src.m_bUseBackground;
  m_iQuantization = src.m_iQuantization;
  m_bFixedOrder = src.m_bFixedOrder;
  m_bDiscreteRanges = src.m_bDiscreteRanges;

  m_vPegs.RemoveAll();
  m_vPegs.Copy(src.m_vPegs);
} // copy constructor


/**
 * destructor
 */
GradientList::~GradientList()
{
  m_vPegs.RemoveAll();  
} // destructor


/**
 * copy operator
 * @param src   the source
 * @return      the destination (this)
 */
GradientList& GradientList::operator =(const GradientList &src)
{
  m_vPegs.RemoveAll();  
  m_vPegs.Copy(src.m_vPegs);

  m_pegStart.SetColor( src.m_pegStart.GetColor() );
  m_pegEnd.SetColor( src.m_pegEnd.GetColor() );
  m_pegBackground.SetColor( src.m_pegBackground.GetColor() );
  m_eInterpolationMethod = src.m_eInterpolationMethod;
  m_bUseBackground = src.m_bUseBackground;
  m_iQuantization = src.m_iQuantization;
  m_bDiscreteRanges = src.m_bDiscreteRanges;

  return *this;
} // operator =


/**
 * @param crColour   peg colour
 * @param fPosition  peg location
 * @return           index of added peg
 */
int GradientList::AddPeg(COLORREF crColour, float fPosition)
{
  return AddPeg(Peg(fPosition, crColour));
} // AddPeg()


/**
 * @param peg        peg to add
 * @return           index of added peg
 */
int GradientList::AddPeg(Peg peg)
{
  if (peg.GetPosition() < 0.0f)
    peg.SetPosition(0.0f);
  else if (peg.GetPosition() > 1.0f)
    peg.SetPosition(1.0f);

  m_vPegs.Add(peg);
  SortPegs();

  return IndexFromId(peg.GetID());
} // AddPeg()


//----- Assorted short functions -----//
/**
 * removes a peg
 * @param iIndex   index of peg to remove
 */
void GradientList::RemovePeg(int iIndex)
{
  m_vPegs.RemoveAt(iIndex);
} // RemovePeg()


/**
 * sort routine
 */
void GradientList::SortPegs()
{
  QuickSort(0, m_vPegs.GetUpperBound());
} // SortPegs()


/**
 * get number of pegs
 * @return     number of pegs
 */
int GradientList::GetPegCount() const
{
  return m_vPegs.GetSize();
} // GetPegCount()


/**
 * gets a peg
 * @param iIndex   index of peg
 * @return         the referenced peg
 */
const Peg GradientList::GetPeg(int iIndex) const
{
  ASSERT(iIndex > -4 && iIndex != -1 && iIndex < GetPegCount());
  //You must pass a valid peg index or STARTPEG, ENDPEG, or BACKGROUND!

  if(iIndex >= 0)
  {
    const Peg peg(m_vPegs[iIndex]);
    return peg;
  }
  else if(iIndex == STARTPEG)
    return m_pegStart;
  else if(iIndex == ENDPEG)
    return m_pegEnd;
  else return m_pegBackground;
} // GetPeg()


/**
 * sets peg info
 * @param iIndex    peg index
 * @param peg       peg info
 * @return          peg index
 */
int GradientList::SetPeg(int iIndex, Peg peg)
{
  ASSERT(iIndex > -4 && iIndex != -1 && iIndex < GetPegCount());
  //You must pass a valid peg index or STARTPEG, ENDPEG, or BACKGROUND!

  if (peg.GetPosition() < 0.0f)
    peg.SetPosition(0.0f);
  else if (peg.GetPosition() > 1.0f)
    peg.SetPosition(1.0f);

  if (iIndex == STARTPEG)
    m_pegStart.SetColor( peg.GetColor() );
  else if (iIndex == ENDPEG)
    m_pegEnd.SetColor( peg.GetColor() );
  else if (iIndex == BACKGROUND)
    m_pegBackground.SetColor( peg.GetColor() );
  else
  {
    if (IsPegOrderFixed())
    {
      if (iIndex > 0)
      {
        // ensure you don't move below the previous peg in fixed ordering
        const float fPrevPos = GetPeg(iIndex-1).GetPosition();
        if (peg.GetPosition() < fPrevPos)
        {
          peg.SetPosition(fPrevPos + 0.01F);
        }
      }
      if (iIndex < GetPegCount()-1)
      {
        // ensure you don't move above the next peg in fixed ordering
        const float fNextPos = GetPeg(iIndex+1).GetPosition();
        if (peg.GetPosition() > fNextPos)
        {
          peg.SetPosition(fNextPos - 0.01F);
        }
      }
    }

    m_vPegs[iIndex] = peg;
    uint4 iTempID = m_vPegs[iIndex].GetID();
    SortPegs();
    return IndexFromId(iTempID);
  }
  return -1;
} // SetPeg()


/**
 * sets peg info
 * @param iIndex      peg index
 * @param crColour    peg colour
 * @param fPosition   peg colour
 * @return            peg index
 */
int GradientList::SetPeg(int iIndex, COLORREF crColour, float fPosition)
{
  return SetPeg(iIndex, Peg(fPosition, crColour));
} // SetPeg()


/**
 * colour palette
 * @param lpPal     palette
 */
void GradientList::Make8BitPalette(RGBTRIPLE *lpPal)
{
  MakeEntries(lpPal, 256);
} // Make8BitPalette()


/**
 * colour palette
 * @param lpPal     source
 */
void GradientList::MakePalette(CPalette *lpPal)
{
  RGBTRIPLE *entries = new RGBTRIPLE[256];
  LOGPALETTE *logpal;
  logpal = (LOGPALETTE*)malloc(2*sizeof(WORD) + 256*sizeof(PALETTEENTRY));

  lpPal->DeleteObject();

  Make8BitPalette(entries);

  logpal->palVersion = 0x300;
  logpal->palNumEntries = 256;

  for(int i = 0; i < 256; i++)
  {
    logpal->palPalEntry[i].peRed = entries[i].rgbtRed;
    logpal->palPalEntry[i].peGreen = entries[i].rgbtGreen;
    logpal->palPalEntry[i].peBlue = entries[i].rgbtBlue;
    logpal->palPalEntry[i].peFlags = PC_RESERVED;
  }

  delete[] entries;

  lpPal->CreatePalette(logpal);
  free(logpal);
} // MakePalette()


/**
 * sets peg info
 * @param lpPal             source
 * @param iEntryCount       number of entries
 */
void GradientList::MakeEntries(RGBTRIPLE *lpPal, int iEntryCount)
{
  float pos;
  COLORREF colour;

  ASSERT(iEntryCount > 1);
  ASSERT(iEntryCount < 65535);

  InterpolateFn Interpolate = GetInterpolationProc();
  ASSERT(Interpolate != NULL);

  if (m_vPegs.GetSize() > 0)
  {
    //Some things are already constant and so can be found early
    float firstpegpos = m_vPegs[0].m_fPosition;
    float lastpegpos = m_vPegs[m_vPegs.GetUpperBound()].m_fPosition;
    COLORREF lastpegcolour = m_vPegs[m_vPegs.GetUpperBound()].m_colour;
    int curpeg;

    CArray<Peg,Peg&> vPegs;
    if (!IsDiscreteColorRangesMode())
    {
      vPegs.SetSize(m_vPegs.GetSize());
      for(int iPegCopy = 0; iPegCopy < m_vPegs.GetSize(); iPegCopy++)
        vPegs[iPegCopy] = m_vPegs[iPegCopy];
    }
    else
    {
      vPegs.SetSize(m_vPegs.GetSize()*2);
      for (int iPegEx(0); iPegEx < m_vPegs.GetSize(); iPegEx++)
      {
        Peg peg = m_vPegs[iPegEx];
        vPegs[iPegEx*2] = vPegs[iPegEx*2+1] = m_vPegs[iPegEx];

        vPegs[iPegEx*2].SetColor(peg.GetPrevEndColor());

        vPegs[iPegEx*2+1].SetPosition(peg.GetPosition()+(1.0/256.0));
        vPegs[iPegEx*2+1].SetColor(peg.GetColor());
      }
    }

    for (int i = 0; i < iEntryCount; i++)
    {
      if(m_iQuantization == -1)
        pos = (float)i/iEntryCount; 
      else
        pos = ((float)(int)(((float)i/iEntryCount)*m_iQuantization))/m_iQuantization + 0.5f / m_iQuantization;

      if(pos <= firstpegpos)
      {
        colour = Interpolate(m_pegStart.m_colour, vPegs[0].m_colour, pos, 0, firstpegpos);
        lpPal[i].rgbtRed = GetRValue(colour);
        lpPal[i].rgbtGreen = GetGValue(colour);
        lpPal[i].rgbtBlue = GetBValue(colour);
      }
      else if(pos > lastpegpos)
      {
        colour = Interpolate(lastpegcolour, m_pegEnd.m_colour, pos, lastpegpos, 1);
        lpPal[i].rgbtRed = GetRValue(colour);
        lpPal[i].rgbtGreen = GetGValue(colour);
        lpPal[i].rgbtBlue = GetBValue(colour);
      }
      else
      {
        curpeg = IndexFromPos(pos, vPegs);
        colour = Interpolate(vPegs[curpeg].m_colour, vPegs[curpeg+1].m_colour, pos, vPegs[curpeg].m_fPosition, vPegs[curpeg+1].m_fPosition);
        lpPal[i].rgbtRed = GetRValue(colour);
        lpPal[i].rgbtGreen = GetGValue(colour);
        lpPal[i].rgbtBlue = GetBValue(colour);
      }
    }
  }
  else
  {
    //When there are no extra peg we can just interpolate the start and end
    for(int i = 0; i < iEntryCount; i++)
    {
      if(m_iQuantization == -1)
        pos = (float)i/iEntryCount; 
      else
        pos = ((float)(int)(((float)i/iEntryCount)*m_iQuantization))/m_iQuantization + 0.5f / m_iQuantization;

      colour = Interpolate(m_pegStart.m_colour, m_pegEnd.m_colour, pos, 0, 1);
      lpPal[i].rgbtRed = GetRValue(colour);
      lpPal[i].rgbtGreen = GetGValue(colour);
      lpPal[i].rgbtBlue = GetBValue(colour);
    }
  }

  if(m_bUseBackground)
  {
    lpPal[0].rgbtRed = GetRValue(m_pegBackground.m_colour);
    lpPal[0].rgbtGreen = GetGValue(m_pegBackground.m_colour);
    lpPal[0].rgbtBlue = GetBValue(m_pegBackground.m_colour);
  }
} // MakeEntries()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateLinear(COLORREF first, COLORREF second, float position, float start, float end)
{
  if(start == end) return first;
  if(end - start == 0) return second;
  if(position == start) return first;
  if(position == end) return second;
  return RGB((BYTE)((GetRValue(second)*(position - start) + GetRValue(first)*(end-position))/(end-start)),
    (BYTE)((GetGValue(second)*(position - start) + GetGValue(first)*(end-position))/(end-start)),
    (BYTE)((GetBValue(second)*(position - start) + GetBValue(first)*(end-position))/(end-start)));
} // InterpolateLinear()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateReverse(COLORREF first, COLORREF second, float position, float start, float end)
{
  if(start == end) return first;
  if(end - start == 0) return second;
  if(position == start) return second;
  if(position == end) return first;
  return RGB((BYTE)((GetRValue(first)*(position - start) + GetRValue(second)*(end-position))/(end-start)),
    (BYTE)((GetGValue(first)*(position - start) + GetGValue(second)*(end-position))/(end-start)),
    (BYTE)((GetBValue(first)*(position - start) + GetBValue(second)*(end-position))/(end-start)));
} // InterpolateReverse()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     unused
 * @param fPosition     unused
 * @param fStart        unused
 * @param fEnd          unused
 * @return              first colour
 **/
COLORREF GradientList::InterpolateFlatStart(COLORREF first, COLORREF, float, float, float)
{return first;}


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     unused
 * @param fStart        unused
 * @param fEnd          unused
 * @return              middle colour
 **/
COLORREF GradientList::InterpolateFlatMid(COLORREF first, COLORREF second, float, float, float)
{
  unsigned short sr, sg, sb, er, eg, eb;
  sr = GetRValue(first);
  sg = GetGValue(first);
  sb = GetBValue(first);
  er = GetRValue(second);
  eg = GetGValue(second);
  eb = GetBValue(second);

  return RGB((sr+er)/2, (sg+eg)/2, (sb+eb)/2);
} // InterpolateFlatMid()


/**
 * colour interpolate function
 * @param clrFirst      unused
 * @param clrSecond     last colour
 * @param fPosition     unused
 * @param fStart        unused
 * @param fEnd          unused
 * @return              last colour
 **/
COLORREF GradientList::InterpolateFlatEnd(COLORREF, COLORREF second, float, float, float)
{return second;}


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateCosine(COLORREF first, COLORREF second, float position, float start, float end)
{
  float theta = (position-start)/(end-start) * 3.1415927f;
  float f = (1 - cosf(theta)) * .5f;

  return RGB((BYTE)(((float)GetRValue(first))*(1-f) + ((float)GetRValue(second))*f),
    (BYTE)(((float)GetGValue(first))*(1-f) + ((float)GetGValue(second))*f),
    (BYTE)(((float)GetBValue(first))*(1-f) + ((float)GetBValue(second))*f));
} // InterpolateCosine()


/**
 * convert rgb colour to hsl
 * @param r   
 * @param g   
 * @param b   
 * @param h   
 * @param s   
 * @param l   
 */
void RGB_to_HSL (float r, float g, float b, float *h, float *s, float *l)
{
  float v;
  float m;
  float vm;
  float r2, g2, b2;

  v = max(r,g);
  v = max(v,b);
  m = min(r,g);
  m = min(m,b);

  if ((*l = (m + v) / 2.0f) <= 0.0f) return;
  if ((*s = vm = v - m) > 0.0f) {
    *s /= (*l <= 0.5f) ? (v + m ) :
    (2.0f - v - m) ;
  } else
    return;


  r2 = (v - r) / vm;
  g2 = (v - g) / vm;
  b2 = (v - b) / vm;

  if (r == v)
    *h = (g == m ? 5.0f + b2 : 1.0f - g2);
  else if (g == v)
    *h = (b == m ? 1.0f + r2 : 3.0f - b2);
  else
    *h = (r == m ? 3.0f + g2 : 5.0f - r2);

  *h /= 6.0f;
} // RGB_to_HSL()


/**
 * convert hsl colour to rgb
 * @param h   
 * @param sl   
 * @param l   
 * @param r   
 * @param g   
 * @param b   
 */
void HSL_to_RGB(float h, float sl, float l, float *r, float *g, float *b)
{
  float v;

  v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);
  if (v <= 0) {
    *r = *g = *b = 0.0f;
  } else {
    float m;
    float sv;
    int sextant;
    float fract, vsf, mid1, mid2;

    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0f;
    sextant = (int)h; 
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant) {
      case 0: *r = v; *g = mid1; *b = m; break;
      case 1: *r = mid2; *g = v; *b = m; break;
      case 2: *r = m; *g = v; *b = mid1; break;
      case 3: *r = m; *g = mid2; *b = v; break;
      case 4: *r = mid1; *g = m; *b = v; break;
      case 5: *r = v; *g = m; *b = mid2; break;
    }
  }
} // HSL_to_RGB()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateHSLClockwise(COLORREF first, COLORREF second, float position, float start, float end)
{
  float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
  RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
    (float)GetBValue(first)/255.0f, &sh, &ss, &sl);
  RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
    (float)GetBValue(second)/255.0f, &eh, &es, &el);

  sh = sh - floorf(sh);
  eh = eh - floorf(eh);

  //Interpolate H clockwise
  if(eh >= sh) h = (eh*(position - start) + sh*(end-position))/(end-start);
  else h = ((eh + 1.0f)*(position - start) + sh*(end-position))/(end-start);
  h = (h>=1.0f)?h-1.0f:h;

  s = ((es*(position - start) + ss*(end-position))/(end-start));
  l = ((el*(position - start) + sl*(end-position))/(end-start));

  HSL_to_RGB(h, s, l, &r, &g, &b);
  return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
} // InterpolateHSLClockwise()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateHSLAntiClockwise(COLORREF first, COLORREF second, float position, float start, float end)
{
  float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
  RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
    (float)GetBValue(first)/255.0f, &sh, &ss, &sl);
  RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
    (float)GetBValue(second)/255.0f, &eh, &es, &el);

  sh = sh - floorf(sh);
  eh = eh - floorf(eh);

  //Interpolate H anticlockwise
  if(eh <= sh) h = (eh*(position - start) + sh*(end-position))/(end-start);
  else h = ((eh + 1.0f)*(position - start) + sh*(end-position))/(end-start);
  h = (h>=1.0f)?h-1.0f:h;

  s = ((es*(position - start) + ss*(end-position))/(end-start));
  l = ((el*(position - start) + sl*(end-position))/(end-start));

  HSL_to_RGB(h, s, l, &r, &g, &b);
  return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
} // InterpolateHSLAntiClockwise()


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateHSLLongest(COLORREF first, COLORREF second, float position, float start, float end)
{
  float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
  RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
    (float)GetBValue(first)/255.0f, &sh, &ss, &sl);
  RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
    (float)GetBValue(second)/255.0f, &eh, &es, &el);

  sh = sh - (float)floor(sh);
  eh = eh - (float)floor(eh);

  //Interpolate H short route
  if(((eh-sh)-floor(eh-sh) < 0.5f)?(eh < sh):(eh >= sh)) h = (eh*(position - start) + sh*(end-position))/(end-start);
  else h = ((eh+(sh>eh?1.0f:-1.0f))*(position - start) + sh*(end-position))/(end-start);
  //TRACE3("sh: %f eh: %f h: %f\n", sh, eh, h);

  h = h - floorf(h);

  s = ((es*(position - start) + ss*(end-position))/(end-start));
  l = ((el*(position - start) + sl*(end-position))/(end-start));

  HSL_to_RGB(h, s, l, &r, &g, &b);
  return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
}


/**
 * colour interpolate function
 * @param clrFirst      first colour
 * @param clrSecond     last colour
 * @param fPosition     position between the two
 * @param fStart        overall start colour
 * @param fEnd          overall end colour
 * @return              colour at fPosition
 **/
COLORREF GradientList::InterpolateHSLShortest(COLORREF first, COLORREF second, float position, float start, float end)
{
  float sh = 0, ss = 0, sl = 0, eh = 0, es = 0, el = 0, h = 0, s = 0, l = 0, r = 0, g = 0, b = 0;
  RGB_to_HSL((float)GetRValue(first)/255.0f, (float)GetGValue(first)/255.0f,
    (float)GetBValue(first)/255.0f, &sh, &ss, &sl);
  RGB_to_HSL((float)GetRValue(second)/255.0f, (float)GetGValue(second)/255.0f,
    (float)GetBValue(second)/255.0f, &eh, &es, &el);

  sh = sh - (float)floor(sh);
  eh = eh - (float)floor(eh);

  //Interpolate H short route
  if(((eh-sh)-floor(eh-sh) > 0.5f)?(eh < sh):(eh >= sh)) h = (eh*(position - start) + sh*(end-position))/(end-start);
  else h = ((eh+(sh>eh?1.0f:-1.0f))*(position - start) + sh*(end-position))/(end-start);
  //TRACE3("sh: %f eh: %f h: %f\n", sh, eh, h);

  h = h - floorf(h);

  s = ((es*(position - start) + ss*(end-position))/(end-start));
  l = ((el*(position - start) + sl*(end-position))/(end-start));

  HSL_to_RGB(h, s, l, &r, &g, &b);
  return RGB((BYTE)(r*255.0f), (BYTE)(g*255.0f), (BYTE)(b*255.0f));
} // InterpolateHSLShortest()


/**
 * index at pos
 * @param fPos          position between the two
 * @return              index at fPos
 **/
int GradientList::IndexFromPos(float pos)
{
  ASSERT(pos >= 0.0f && pos <= 1.0f);
  // positions betwen 0 and 1!

  if(pos < m_vPegs[0].m_fPosition)
    return STARTPEG;

  for(int i = 0; i < m_vPegs.GetSize()-1; i++)
    if(pos >= m_vPegs[i].m_fPosition && pos <= m_vPegs[i+1].m_fPosition)
      return i;

  return -1; // Eh? somethings wrong here
} // IndexFromPos


/**
 * index at pos
 * @param fPos          position between the two
 * @param vPegs         list of pegs
 * @return              index at fPos
 **/
int GradientList::IndexFromPos(const float pos, const CArray <Peg, Peg&> & vPegs)
{
  ASSERT(pos >= 0.0f && pos <= 1.0f);
  // positions betwen 0 and 1!

  if(pos < vPegs[0].m_fPosition)
    return STARTPEG;

  for(int i = 0; i < vPegs.GetSize()-1; i++)
    if(pos >= vPegs[i].m_fPosition && pos <= vPegs[i+1].m_fPosition)
      return i;

  return -1; // Eh? somethings wrong here
} // IndexFromPos()


/**
 * index for peg with id
 * @param uID          peg id
 * @return             index for the peg
 **/
int GradientList::IndexFromId(UINT id)
{
  for(int i = 0; i < m_vPegs.GetSize(); i++)
    if(id == m_vPegs[i].GetID())
      return i;
  return -1;
}


/**
 * quick sort helper routine
 * @param iLB   lower bound    
 * @param iUB   upper bound    
 */
//----- Fast sorting alogarithm functions -----//
int GradientList::Partition(int lb, int ub)
{
  Peg temppeg, pivot;
  int i, j, p;

  /* select pivot and exchange with 1st element */
  p = lb + ((ub - lb)>>1);
  pivot = m_vPegs[p];
  m_vPegs[p] = m_vPegs[lb];

  /* sort lb+1..ub based on pivot */
  i = lb+1;
  j = ub;
  while(1)
  {
    while(i < j && pivot.m_fPosition >= m_vPegs[i].m_fPosition) i++;
    while(j >= i && m_vPegs[j].m_fPosition >= pivot.m_fPosition) j--;
    if(i >= j) break;
    temppeg = m_vPegs[i];
    m_vPegs[i] = m_vPegs[j];
    m_vPegs[j] = temppeg;
    j--; i++;
  }

  /* pivot belongs in a[j] */
  m_vPegs[lb] = m_vPegs[j];
  m_vPegs[j] = pivot;

  return j;
} // Partition()


/**
 * quick sort routine
 * @param iLB   lower bound    
 * @param iUB   upper bound    
 */
void GradientList::QuickSort(int lb, int ub)
{
  int m;

  /**************************
  *  sort array m_vPegs[lb..ub]  *
  **************************/

  while (lb < ub)
  {
    /* quickly sort short lists */
    if (ub - lb <= 12)
    {
      InsertSort(lb, ub);
      return;
    }

    m = Partition(lb, ub);

    if (m - lb <= ub - m)
    {
      QuickSort(lb, m - 1);
      lb = m + 1;
    }
    else
    {
      QuickSort(m + 1, ub);
      ub = m - 1;
    }
  }
} // QuickSort()


/**
 * insertion sort
 * @param iLB   lower bound
 * @param iLB   upper bound
 */
void GradientList::InsertSort(int lb, int ub)
{
  Peg temppeg;

  for (int i = lb + 1; i <= ub; i++)
  {
    temppeg = m_vPegs[i];
    int j = i-1;
    for(; j >= lb && m_vPegs[j].m_fPosition > temppeg.m_fPosition; j--)
      m_vPegs[j+1] = m_vPegs[j];
    m_vPegs[j+1] = temppeg;
  }
} // InsertSort()


/**
 * colour at position
 * @param fPos      the position
 * @return          corresponding colour
 */
COLORREF GradientList::ColourFromPosition(float pos)
{
  if(pos < 0 || pos > 1)
  {
    ASSERT(0); // Position out of bounds!!
    return 0;
  }

  if(pos == 0 && m_bUseBackground)
    return m_pegBackground.m_colour;

  InterpolateFn Interpolate = GetInterpolationProc();
  ASSERT(Interpolate != NULL);

  if(m_iQuantization != -1)
    pos = ((float)(int)(pos*m_iQuantization))/m_iQuantization + 0.5f / m_iQuantization;

  if(m_vPegs.GetSize() > 0)
  {
    //Some thing are already constant and so can be found early
    float firstpegpos = m_vPegs[0].m_fPosition;
    float lastpegpos = m_vPegs[m_vPegs.GetUpperBound()].m_fPosition;

    if(pos <= firstpegpos)
      return Interpolate(m_pegStart.m_colour, m_vPegs[0].m_colour, pos, 0,
      firstpegpos+1);
    else if(pos > lastpegpos)
      return Interpolate(m_vPegs[m_vPegs.GetUpperBound()].m_colour, m_pegEnd.m_colour,
      pos, lastpegpos, 1);
    else
    {
      int curpeg;
      curpeg = IndexFromPos(pos);
      return Interpolate(m_vPegs[curpeg].m_colour, m_vPegs[curpeg+1].m_colour, pos,
        m_vPegs[curpeg].m_fPosition, m_vPegs[curpeg+1].m_fPosition);
    }
  }
  else
  {
    //When there are no extra m_vPegs we can just interpolate the start and end
    return Interpolate(m_pegStart.m_colour, m_pegEnd.m_colour, pos, 0, 1);
  }
} // ColourFromPosition()


/**
 * file i/o
 * @param ar    i/o
 */
void GradientList::Serialize(CArchive &ar)
{
  if(ar.IsLoading())
  {
    int pegcount, temp;

    ar >> m_pegBackground.m_colour;
    ar >> m_pegStart.m_colour;
    ar >> m_pegEnd.m_colour;
    ar >> m_bUseBackground;
    ar >> m_iQuantization;
    ar >> temp;
    m_eInterpolationMethod = (GradientList::InterpolationMethodEnum)temp;

    ar >> pegcount;

    m_vPegs.SetSize(pegcount);
    for(int i = 0; i < pegcount; i++)
      m_vPegs[i].Serialize(ar);
  }
  else
  {
    ar << m_pegBackground.m_colour;
    ar << m_pegStart.m_colour;
    ar << m_pegEnd.m_colour;
    ar << (unsigned int)m_bUseBackground;
    ar << m_iQuantization;
    ar << (unsigned int)m_eInterpolationMethod;

    ar << m_vPegs.GetSize();

    for(int i = 0; i < m_vPegs.GetSize(); i++)
      m_vPegs[i].Serialize(ar);
  }
} // Serialize()


/**
 * gets the current interpolation method
 * @return    current interpolation method
 */
GradientList::InterpolationMethodEnum GradientList::GetInterpolationMethod() const
{
  return m_eInterpolationMethod;
} // GetInterpolationMethod()


/**
 * gets the current interpolation method
 * @param eMethod      new interpolation method
 */
void GradientList::SetInterpolationMethod(const InterpolationMethodEnum eMethod)
{
  m_eInterpolationMethod = eMethod;
} // SetInterpolationMethod()


/**
 * get quantization
 * @return    quantization
 */
int GradientList::GetQuantization() const
{
  return m_iQuantization;
} // GetQuantization()


/**
 * 
 * @param iEntries   number of entries    
 */
void GradientList::SetQuantization(const int entries)
{
  m_iQuantization = entries;
} // SetQuantization()


/**
 * file i/o
 * @param ar    archive i/o
 */
void Peg::Serialize(CArchive &ar)
{
  if(ar.IsLoading())
  {
    ar >> m_colour;
    ar >> m_fPosition;
    m_uId = m_uUniqueID;
    m_uUniqueID++;
  }
  else
  {
    ar << m_colour;
    ar << m_fPosition;
  }
} // Serialize()


/**
 * gets the current interpolation procedure
 * @return    current interpolation procedure
 */
InterpolateFn GradientList::GetInterpolationProc()
{
  switch(m_eInterpolationMethod)
  {
  case LINEAR: return InterpolateLinear;
  case FLATSTART: return InterpolateFlatStart;
  case FLATMID: return InterpolateFlatMid;
  case FLATEND: return InterpolateFlatEnd;
  case COSINE: return InterpolateCosine;
  case HSLREDBLUE: return InterpolateHSLClockwise;
  case HSLBLUERED: return InterpolateHSLAntiClockwise;
  case HSLSHORTEST: return InterpolateHSLShortest;
  case HSLLONGEST: return InterpolateHSLLongest;
  case REVERSE: return InterpolateReverse;
  default: return 0;
  }
} // GetInterpolationProc()


// $Log: Gradient.C,v $
// Revision 1.4.2.3  2007/03/16 23:37:33  jmeade
// comments, class variable naming.
//
// Revision 1.4.2.2  2007/03/16 22:45:17  jmeade
// comments
//
// Revision 1.4.2.1  2007/03/16 00:25:32  jmeade
// Issue 5494: rollback changes to fix bug introduced in code review.
//
// Revision 1.3  2006/06/01 20:20:23  frank
// updated to visual studio 2005
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.3  2004/06/23 19:51:34  jmeade
// typo/compile error.
//
// Revision 1.1.2.2  2004/06/23 18:27:30  jmeade
// Allow for discrete color ranges between pegs (in addition to continuous).
//
// Revision 1.1.2.1  2004/04/21 03:44:19  jmeade
// Gradient color edit control.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Gradient.C,v 1.4.2.3 2007/03/16 23:37:33 jmeade Exp $
// $Id: Gradient.C,v 1.4.2.3 2007/03/16 23:37:33 jmeade Exp $
