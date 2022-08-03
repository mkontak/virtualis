// $Id: vxColorTable.C,v 1.4 2007/02/15 00:08:23 jmeade Exp $
//
// Copyright © 2002 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com))


// includes
#include "stdafx.h"
#include "vxColorTable.h"
#include "vxColormap.h"
#include "vxWindowLevel.h"
#include "Point2D.h"
#include "utilities.h"
#include "PixelRGBA.h"
#include "Timer.h"
#include "Image.h"


// namespaces
using namespace nsCOLOR;
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Copy constructor
 * @param color table
 */
vxColorTable::vxColorTable(const vxColorTable & other)
//*******************************************************************
{
  *this = other;
}


/**
 * assignment operator
 * @param other color table
 * @return const reference to this
 */
const vxColorTable & vxColorTable::operator=(const vxColorTable & other)
//*******************************************************************
{
  if (this != &other)
  {
    m_uMaxAlphaIndex = other.m_uMaxAlphaIndex;
    m_uMaxAlphaValue = other.m_uMaxAlphaValue;
    m_uMaxColorIndex = other.m_uMaxColorIndex;
    m_uMaxColorValue = other.m_uMaxColorValue;
    m_bActiveRange   = other.m_bActiveRange;
  }

  return *this;
} // operator=()


/**
 * Return the number of entries of the specified color channel
 * @param color index
 * @return color or alpha index
 */
uint2 vxColorTable::GetMaxIndex(COLOR_CHANNEL colorIndex)
//*******************************************************************
{
  if (colorIndex == ALPHA)
    return m_uMaxAlphaIndex;
  else
    return m_uMaxColorIndex;
} // GetMaxIndex()


/**
 * Return the maximal possible value of the specified color channel
 * @param color index
 * @return color or alpha index
 */
uint2 vxColorTable::GetMaxValue(COLOR_CHANNEL colorIndex)
//*******************************************************************
{
  if (colorIndex == ALPHA)
    return m_uMaxAlphaIndex;
  else
    return m_uMaxColorIndex;
} // GetMaxValue()


/**
 * Update lookup table according to the colormap
 * @param colormap
 */
void vxColorTable::UpdateLookupTable(vxColormap & colormap)
//*******************************************************************
{
  // this is actually a HACK since the new anti-alias stuff would
  // need to be extended to do alpha first, and then use the discrete
  // alpha values for the color computation for opacity weighting.
  // No hurry for this one, revisit sometime after RSNA2004
  if (IsActiveRange() == true)
  {
    BuildTables(colormap);
  }
  else
  {
    BuildTable(colormap, RED);   // Red
    BuildTable(colormap, GREEN); // Green
    BuildTable(colormap, BLUE);  // Blue
    BuildTable(colormap, ALPHA); // Alpha
  }

  return;
/*
  // purely debugging code to compare the anti-aliased 
  // color table with the old one that suffered from aliasing
  /////////////////////////////////////////////////////////////
  static uint1 uCount(0);
  const uint1 uCountMax(10);
  Timer timer;

  if (uCount++ != uCountMax)
  {
    BuildTable(colormap, RED);   // Red
    BuildTable(colormap, GREEN); // Green
    BuildTable(colormap, BLUE);  // Blue
    BuildTable(colormap, ALPHA); // Alpha

    return;
  }

  else
  {
    uCount = 0;

    uint2 uMaxAlphaIndex = m_uMaxAlphaIndex;
    uint2 uMaxAlphaValue = m_uMaxAlphaValue;
    uint2 uMaxColorIndex = m_uMaxColorIndex;
    uint2 uMaxColorValue = m_uMaxColorValue;
    m_uMaxAlphaIndex = 255;
    m_uMaxAlphaValue = 255;
    m_uMaxColorIndex = 255;
    m_uMaxColorValue = 255;

    const int4 iMax(GetMaxIndex(RED));
    const float4 fScale(255.0/(float4)iMax);
    Image<PixelRGB<uint1> > image(Triple<uint4>(iMax+20, 90, 0), PixelRGB<uint1>(0, 100, 0));

    BuildTables(colormap);

    // write 1D table
    for (int4 j(0); j<10; j++)
    {
      for (int4 i(0); i<=iMax; i++)
      {
        image.SetPixel(Point2D<uint2>(10+i, 10+j), PixelRGB<uint1>(GetColor(RED, i)*fScale, GetColor(GREEN, i)*fScale, GetColor(BLUE, i)*fScale));
        image.SetPixel(Point2D<uint2>(10+i, 50+j), PixelRGB<uint1>(GetColor(ALPHA, i)*fScale, GetColor(ALPHA, i)*fScale, GetColor(ALPHA, i)*fScale));
      }
    }

    //LogDbg("Time (1) is: " + ToAscii(timer.ElapsedTime()), "", "");

    //return;

    // old way of doing it but being buggy for narrow windows with color schemes (aliasing)

    //timer.Reset();
    BuildTable(colormap, RED);   // Red
    BuildTable(colormap, GREEN); // Green
    BuildTable(colormap, BLUE);  // Blue
    BuildTable(colormap, ALPHA); // Alpha

    //LogDbg("Time (2) is: " + ToAscii(timer.ElapsedTime()), "", "");
    // write 1D table
    for (int4 j(0); j<10; j++)
    {
      for (int4 i(0); i<=iMax; i++)
      {
        image.SetPixel(Point2D<uint2>(10+i, 30+j), PixelRGB<uint1>(GetColor(RED, i)*fScale, GetColor(GREEN, i)*fScale, GetColor(BLUE, i)*fScale));
        image.SetPixel(Point2D<uint2>(10+i, 70+j), PixelRGB<uint1>(GetColor(ALPHA, i)*fScale, GetColor(ALPHA, i)*fScale, GetColor(ALPHA, i)*fScale));
      }
    }

    LogDbg("writing image", "", "");
    image.Write("D:/Colormaps.ppm");

    m_uMaxAlphaIndex = uMaxAlphaIndex;
    m_uMaxAlphaValue = uMaxAlphaValue;
    m_uMaxColorIndex = uMaxColorIndex;
    m_uMaxColorValue = uMaxColorValue;

    BuildTable(colormap, RED);   // Red
    BuildTable(colormap, GREEN); // Green
    BuildTable(colormap, BLUE);  // Blue
    BuildTable(colormap, ALPHA); // Alpha

    return;
  }
*/
} // UpdateLookupTable()


/** 
 * Compute the Y value for a given X value
 * @param vPoints vector of points
 * @param fValue  given X value
 * @return computed Y value
 */
float4 ComputeValue(const vector<Point2D<ZeroOne> > & vPoints,
                    float4 fValue)
//*******************************************************************
{
  const int4 iSize(vPoints.size());

  if (iSize > 1)
  {
    if (fValue <= (float4)vPoints[0].X())
    {
      return vPoints[0].Y();
    }

    if (fValue > (float4)vPoints[iSize-1].X())
    {
      return vPoints[iSize-1].Y();
    }

    for (int4 i(1); i<iSize; i++)
    {
      if (fValue <= vPoints[i].X())
      {
        return Interpolate((float4)vPoints[i-1].X(), 
                           fValue, 
                           (float4)vPoints[i].X(), 
                           (float4)vPoints[i-1].Y(), 
                           (float4)vPoints[i].Y());
      }
    }
  }
  else if (iSize == 1)
  {
    return vPoints[0].Y();
  }

  return 0.0F; // else if (iSize == 0)
} // ComputeValue()


/**
 * Convert the TransferFunction to discrete color values for a single channel
 * @param colormap
 * @param color index
 */
void vxColorTable::BuildTables(vxColormap & colormap)
//*******************************************************************
{
  Timer timer;

  // consolidate color and alpha curves
  // (1) get all X values
  // (2) sort all X values
  // (3) compute colors for all sorted X values and scale and bias X values
  // (4) remove everything that is a negative position (wide window)
  // (5) compute colors for scaled and bias 0.0 or 1.0 if needed
  // (6) scanconvert functions into bins
  ///////////////////////////////////////////////////////////////////////////

  // (1) get all X values
  vector<float4> vAllXs;

	const float8 fSmallestGap( 0.1/65535.0);

  for (int4 j(3); j>=0; j--) // IMPORTANT THAT ALPHA IS DONE FIRST!!!
  {
    const vector<Point2D<ZeroOne> > * pvPoints(&(colormap.GetRedList()));
    switch (j)
    {
    case RED  : pvPoints = &(colormap.GetRedList  ()); break;
    case GREEN: pvPoints = &(colormap.GetGreenList()); break;
    case BLUE : pvPoints = &(colormap.GetBlueList ()); break;
    case ALPHA: pvPoints = &(colormap.GetAlphaList()); break;
    }
    const vector<Point2D<ZeroOne> > & vPoints(*pvPoints);
    
	  float8 fValue(0.0F);
    for (int4 i(0); i<pvPoints->size(); i++)
    {
      const float8 fValuePrevious(fValue);
      fValue = vPoints[i].X();

      // ensure that duplicate points end up with different positions
      if (i != 0 && fValue == fValuePrevious)
      {
        vAllXs.push_back(fValue + fSmallestGap);
      }
      else if (find(vAllXs.begin(), vAllXs.end(), fValue) == vAllXs.end())
      {
        vAllXs.push_back(fValue);
      }
    }
  } // for all curves

  // (2) sort all X values
  sort(vAllXs.begin(), vAllXs.end(), less<float4>());

  // (3) compute colors for all sorted X values and scale and bias X values
  deque<float4> vAllPoints;
  deque<PixelRGBA<uint2> > vAllColors;
  const vxWindowLevel & windowlevel(colormap.GetWindowLevel());
  float4 fBias((IsActiveRange() == false) ? 0.0F : (float4)windowlevel.GetLevel() - (float4)windowlevel.GetWindow() / 2.0F);
  float4 fScale(windowlevel.GetWindow());

  for (int4 i(0); i<vAllXs.size(); i++)
  {
    const float4 fValue(fBias + fScale * (float4)vAllXs[i]);
    {
      uint2 uR(m_uMaxAlphaValue * ComputeValue(colormap.GetRedList(),   vAllXs[i]));
      uint2 uG(m_uMaxAlphaValue * ComputeValue(colormap.GetGreenList(), vAllXs[i]));
      uint2 uB(m_uMaxAlphaValue * ComputeValue(colormap.GetBlueList(),  vAllXs[i]));
      uint2 uA(m_uMaxAlphaValue * ComputeValue(colormap.GetAlphaList(), vAllXs[i]));
      vAllPoints.push_back(fValue);
      vAllColors.push_back(PixelRGBA<uint2>(uR, uG, uB, uA));
    }
  }

  // (4) remove everything that is a negative position (wide window)
  while (vAllPoints.size() > 0 && vAllPoints[0] < 0.0F)
  {
    vAllPoints.pop_front();
    vAllColors.pop_front();
  }

  // (5) compute colors for scaled and bias 0.0 or 1.0 if needed
  if (vAllPoints[0] > 0.0F)
  {
    const float4 fValue((fScale == 0.0F) ? 0.0F : -fBias/fScale);
    uint2 uR(m_uMaxAlphaValue * ComputeValue(colormap.GetRedList(),   fValue));
    uint2 uG(m_uMaxAlphaValue * ComputeValue(colormap.GetGreenList(), fValue));
    uint2 uB(m_uMaxAlphaValue * ComputeValue(colormap.GetBlueList(),  fValue));
    uint2 uA(m_uMaxAlphaValue * ComputeValue(colormap.GetAlphaList(), fValue));
    //LogDbg("footprint 0.0F is: " + ToAscii(fValue), "", "");
    vAllPoints.push_front(0.0F);
    vAllColors.push_front(PixelRGBA<uint2>(uR, uG, uB, uA));
  }

  if (vAllPoints[vAllPoints.size()-1] < 1.0F)
  {
    const float4 fValue((fScale == 0.0F) ? 1.0F : (1.0F-fBias)/fScale);
    uint2 uR(m_uMaxAlphaValue * ComputeValue(colormap.GetRedList(),   fValue));
    uint2 uG(m_uMaxAlphaValue * ComputeValue(colormap.GetGreenList(), fValue));
    uint2 uB(m_uMaxAlphaValue * ComputeValue(colormap.GetBlueList(),  fValue));
    uint2 uA(m_uMaxAlphaValue * ComputeValue(colormap.GetAlphaList(), fValue));
    //LogDbg("footprint 1.0F is: " + ToAscii(fValue), "", "");
    vAllPoints.push_back(1.0F);
    vAllColors.push_back(PixelRGBA<uint2>(uR, uG, uB, uA));
    vAllPoints.push_back(1.1F);
    vAllColors.push_back(PixelRGBA<uint2>(uR, uG, uB, uA));
    vAllPoints.push_back(1.2F);
    vAllColors.push_back(PixelRGBA<uint2>(uR, uG, uB, uA));
  }

  for (int i(0); i<vAllPoints.size(); i++)
  {
    //LogDbg("before: X=" + ToAscii(vAllPoints[i]) + ", A=" + ToAscii(vAllColors[i].A()) + ", " + " RGB=" + ToAscii(vAllColors[i].R()) + ", " + ToAscii(vAllColors[i].G()) + ", " + ToAscii(vAllColors[i].B()), "", "");
  }

  // (6) scanconvert functions into bins
  float4 fLower(0.0F);
  float4 fWidth(1.0F/(float4)(m_uMaxAlphaIndex));
  int4 iIndex(0);
  float4 fR, fG, fB, fA;

  for (int4 i(0); i<vAllPoints.size() && iIndex<=m_uMaxAlphaIndex; )
  {
    const float4 fX1(vAllPoints[i]);
    const float4 fX2(vAllPoints[i+1]);
    const int4 iNumberOfBinsLower(vAllPoints[i]/fWidth);
    const int4 iNumberOfBinsUpper(vAllPoints[i+1]/fWidth);
    const int4 iNumberOfBins(iNumberOfBinsUpper - iNumberOfBinsLower);
    float4 fUpper(iNumberOfBinsUpper * fWidth);

    // compute values for bins containing one linear segment only
    if (iNumberOfBins > 0)
    {
      fR = (Bound(0.0F, Interpolate(vAllPoints[i], fLower, vAllPoints[i+1], vAllColors[i].R(), vAllColors[i+1].R()), (float4)m_uMaxAlphaValue));
      fG = (Bound(0.0F, Interpolate(vAllPoints[i], fLower, vAllPoints[i+1], vAllColors[i].G(), vAllColors[i+1].G()), (float4)m_uMaxAlphaValue));
      fB = (Bound(0.0F, Interpolate(vAllPoints[i], fLower, vAllPoints[i+1], vAllColors[i].B(), vAllColors[i+1].B()), (float4)m_uMaxAlphaValue));
      fA = (Bound(0.0F, Interpolate(vAllPoints[i], fLower, vAllPoints[i+1], vAllColors[i].A(), vAllColors[i+1].A()), (float4)m_uMaxAlphaValue));

      const float4 fRnext(Bound(0.0F, Interpolate(vAllPoints[i], fUpper, vAllPoints[i+1], vAllColors[i].R(), vAllColors[i+1].R()), (float4)m_uMaxAlphaValue));
      const float4 fGnext(Bound(0.0F, Interpolate(vAllPoints[i], fUpper, vAllPoints[i+1], vAllColors[i].G(), vAllColors[i+1].G()), (float4)m_uMaxAlphaValue));
      const float4 fBnext(Bound(0.0F, Interpolate(vAllPoints[i], fUpper, vAllPoints[i+1], vAllColors[i].B(), vAllColors[i+1].B()), (float4)m_uMaxAlphaValue));
      const float4 fAnext(Bound(0.0F, Interpolate(vAllPoints[i], fUpper, vAllPoints[i+1], vAllColors[i].A(), vAllColors[i+1].A()), (float4)m_uMaxAlphaValue));

      const float4 fDeltaR((fRnext-fR) / (float4)iNumberOfBins);
      const float4 fDeltaG((fGnext-fG) / (float4)iNumberOfBins);
      const float4 fDeltaB((fBnext-fB) / (float4)iNumberOfBins);
      const float4 fDeltaA((fAnext-fA) / (float4)iNumberOfBins);

      for (int4 j(0); j<iNumberOfBins && iIndex<=m_uMaxAlphaIndex; j++, iIndex++)
      {
        SetColor(RED,   iIndex, fR);
        SetColor(GREEN, iIndex, fG);
        SetColor(BLUE,  iIndex, fB);
        SetColor(ALPHA, iIndex, fA);
        fR += fDeltaR;
        fG += fDeltaG;
        fB += fDeltaB;
        fA += fDeltaA;
      }
    }

    // still more bins to deal with?
    if (iIndex <= m_uMaxAlphaIndex)
    {
      // not yet fully done with this bin?
      if (fUpper != vAllPoints[i+1])
      {
        const float4 fMaxAlphaValue(m_uMaxAlphaValue);
        float4 fR(0.0F), fG(0.0F), fB(0.0F), fA(0.0F);
        float4 fLowerLocal(fUpper);
        float4 fUpperLocal = vAllPoints[i+1];
        fUpper = Min(1.0F, fUpper + fWidth);

        while (fLowerLocal < fUpper)
        {
          const float4 fTmpA(Bound(0.0F, Interpolate(vAllPoints[i], fLowerLocal, vAllPoints[i+1], vAllColors[i].A(), vAllColors[i+1].A()), (float4)m_uMaxAlphaValue));
          if (fTmpA > 0)
          {
            const float4 fWeight((Min(fUpper, fUpperLocal)-fLowerLocal)/fWidth);
            const float4 fTmpR(Min(Interpolate(vAllPoints[i], fLowerLocal, vAllPoints[i+1], vAllColors[i].R(), vAllColors[i+1].R()), (float4)m_uMaxAlphaValue));
            const float4 fTmpG(Min(Interpolate(vAllPoints[i], fLowerLocal, vAllPoints[i+1], vAllColors[i].G(), vAllColors[i+1].G()), (float4)m_uMaxAlphaValue));
            const float4 fTmpB(Min(Interpolate(vAllPoints[i], fLowerLocal, vAllPoints[i+1], vAllColors[i].B(), vAllColors[i+1].B()), (float4)m_uMaxAlphaValue));
            const float4 fFactor(fWeight * fTmpA/fMaxAlphaValue);
            fR += fFactor * fTmpR;
            fG += fFactor * fTmpG;
            fB += fFactor * fTmpB;
            fA += fWeight * fTmpA;
          }

          if (fUpperLocal <= fUpper)
          {
            i++;
            fLowerLocal = vAllPoints[i];
            fUpperLocal = vAllPoints[i+1];
          }
          else
          {
            break;
          }
        } 

        if (fA > 0.0F)
        {
          const float4 fFactor(fA/fMaxAlphaValue);
          fR = Min(fR/fFactor, (float4)m_uMaxAlphaValue);
          fG = Min(fG/fFactor, (float4)m_uMaxAlphaValue);
          fB = Min(fB/fFactor, (float4)m_uMaxAlphaValue);
          fA = Min(fA,         (float4)m_uMaxAlphaValue);
        }

        SetColor(RED,   iIndex, fR);
        SetColor(GREEN, iIndex, fG);
        SetColor(BLUE,  iIndex, fB);
        SetColor(ALPHA, iIndex, fA);

        iIndex++;
      }
      else
      {
        i++;
      }

      fLower = fUpper;
    }
    else
    {
      break;
    }
  }
} // BuildTables()


/**
 * Convert the TransferFunction to discrete color values for a single channel
 * @param colormap
 * @param color index
 */
void vxColorTable::BuildTable(vxColormap & colormap, 
                              COLOR_CHANNEL colorIndex)
//*******************************************************************
{
  // grab the curve that we will discretize
  const vector<Point2D<ZeroOne> > * pvPoints(&(colormap.GetRedList()));
  switch (colorIndex)
  {
  case RED  : pvPoints = &(colormap.GetRedList  ()); break;
  case GREEN: pvPoints = &(colormap.GetGreenList()); break;
  case BLUE : pvPoints = &(colormap.GetBlueList ()); break;
  case ALPHA: pvPoints = &(colormap.GetAlphaList()); break;
  }
  const vector<Point2D<ZeroOne> > & vPoints(*pvPoints);

  // get the size of the discrete array
  const uint2 uMaxIndex(GetMaxIndex(colorIndex));

  // get the largest value
  const uint2 uMaxValue(GetMaxValue(colorIndex));

  // get the limits of the window/level
  const vxWindowLevel & windowlevel(colormap.GetWindowLevel());
  float4 fLow ((windowlevel.GetLevel() - windowlevel.GetWindow() / 2.0F) * uMaxIndex);
  float4 fHigh((windowlevel.GetLevel() + windowlevel.GetWindow() / 2.0F) * uMaxIndex);
  int4 iWidth(windowlevel.GetWindow() * uMaxIndex);

  if (IsActiveRange() == false && colorIndex != ALPHA)
  {
    fLow   = 0;
    fHigh  = uMaxIndex;
    iWidth = uMaxIndex;
  }
  const int4 iLow (Bound(int4(0), int4(fLow  + 0.5F), int4(uMaxIndex)));
  const int4 iHigh(Bound(int4(0), int4(fHigh + 0.5F), int4(uMaxIndex)));
  
  // if there is nothing to rasterize set to zero
  if (vPoints.size() < 2)
  {
    for (int4 i(0); i<GetMaxIndex(colorIndex); i++)
    {
      SetColor(colorIndex, i, 0);
    }
  }
  else // Rasterize the vPoints assuming there are >= 2 points
  {
    // Fill in the lower range
    const int4 iLowValue(vPoints.front().m_y * uMaxValue);

    for (int i(0); i<iLow; ++i)
      SetColor(colorIndex, i, iLowValue);

    // Fill in the window/level range
    for (int j(0); j<vPoints.size()-1; ++j)
    {
      // map the intensity to window/level range
      float4 fStartX(fLow + vPoints[j  ].m_x * iWidth);
      float4 fEndX  (fLow + vPoints[j+1].m_x * iWidth);
      int4 iStartX(fStartX + 0.5F);
      int4 iEndX  (fEndX   + 0.5F);

      // map the value
      const float4 fStartY(vPoints[j  ].m_y * uMaxValue);
      const float4 fEndY  (vPoints[j+1].m_y * uMaxValue);

      // interpolate over all points in segment
      if (iStartX != min(iEndX, uMaxIndex))
      {
        for (int4 k(max(0, iStartX)); k <= min(iEndX, uMaxIndex); ++k)
        {
          if (fEndX > fStartX)
          {
            const float4 fY(Bound(0.0F, Interpolate(iStartX, float4(k), iEndX, fStartY, fEndY), float4(uMaxValue)));
            SetColor(colorIndex, k, fY);
          }
          else
          {
            SetColor(colorIndex, k, fStartY);
          }
        } // loop over points in segment
      }
      else
      {
        SetColor(colorIndex, fStartX, fStartY);
      }

    } // loop over segments

    // Fill in the upper range
    const int4 iHighValue(vPoints.back().m_y * uMaxValue);
    for (int4 k(iHigh); k<=uMaxIndex; ++k)
      SetColor(colorIndex, k, iHighValue);
  } // if < 2 points

  return;
} // BuildTable()


#undef FILE_REVISION


// Revision History:
// $Log: vxColorTable.C,v $
// Revision 1.4  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.3  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.2.18.1  2006/05/24 14:02:28  geconomos
// corrected uninitialized variable warning
//
// Revision 1.2  2004/05/18 14:57:14  vxconfig
// make sure that a vertical ramp is considered (multiple points with same x-coord)
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.25  2003/11/20 05:55:43  michael
// Needs to be revisited after RSNA2004 since anti-aliased colormap only works
// for active range scenario. In full range, opacity needs to be treated differently
// than color, either in a two step way or by mixing the curves ...
//
// Revision 1.24  2003/11/20 05:28:29  michael
// fixed anti-aliased version of the color tables, now works properly.
//
// Revision 1.23  2003/11/17 15:47:35  michael
// using old code since new one currently creates artifacts
//
// Revision 1.22  2003/11/16 23:45:52  michael
// Added alternative method of scan converting the piece-wise linear colormap
// functions. For each bin that is fully "covered" by one linear segment, the
// values are incrementally computed. For all the others, the color is averaged
// and using opacity weighted colors. This solves the issue of aliasing artifacts
// as created by the other approach.
//
// Revision 1.21  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.20  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.19  2003/04/25 19:16:09  dongqing
// code review
//
// Revision 1.18  2003/04/19 01:49:08  michael
// some more code review
//
// Revision 1.17  2003/04/16 21:29:30  michael
// fixed default value for active/full range
//
// Revision 1.16  2003/04/16 17:28:18  michael
// active and full range are now working
//
// Revision 1.15  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.14  2003/03/30 20:58:31  michael
// formatting and cosmetics
//
// Revision 1.13  2002/12/09 23:47:06  michael
// cosmetics
//
// Revision 1.12  2002/12/09 22:51:25  michael
// Corrected error in discrete table computation: window=0, level=4095 generated table of '0' only
//
// Revision 1.11  2002/10/14 15:31:12  michael
// accounting for const methods in vxColormap
//
// Revision 1.10  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.9  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.7  2002/09/24 14:01:00  michael
// Fixed the wrong interval values by min/max-ing the loop values
//
// Revision 1.6  2002/09/19 14:25:24  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.5  2002/09/18 20:51:53  frank
// Fixed speckles in alpha transfer function.
//
// Revision 1.4  2002/09/17 13:00:22  michael
// adustments for changes in transfer function
//
// Revision 1.3  2002/09/10 22:58:30  frank
// Cleaned up conversion from color&opacity to window/level range.
//
// Revision 1.2  2002/08/19 21:07:02  ingmar
// seperated alpha computation from color computation
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.14  2002/07/24 15:28:19  ingmar
// code beautify
//
// Revision 3.13  2002/07/24 15:24:15  ingmar
// removed alpha pre-multiplication
//
// Revision 3.12  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 3.11  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 3.10.4.1  2002/07/10 13:59:34  michael
// fixed clamping of Window/Level when outside visible area and set CT lung
// window/level to default 2000,-300
//
// Revision 3.10  2002/05/03 20:28:55  soeren
// Cleaned up UpdateLookupTable and everything called from it.
//
// Revision 3.9  2002/04/25 21:30:26  ingmar
// code beautify
//
// Revision 3.8  2002/04/25 01:39:57  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 3.7  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 3.6  2001/12/21 00:08:10  liwei
// Alpha values are adjusted by the interpolation factor.
//
// Revision 3.5  2001/12/20 17:40:54  liwei
// Hacked the conversion problem that cause the preloaded transfer function
// of Calcium Scoring does not work properly.
//
// Revision 3.4  2001/11/19 18:19:10  michael
// changed the alpha modulation when generating the color channels since
// it was not showing the true window level.
//
// Revision 3.3  2001/11/06 02:56:33  liwei
// Modified for rendering tag volume.
//
// Revision 3.2  2001/10/26 16:14:21  dmitry
// Beautify
//
// Revision 3.1  2001/10/15 20:25:59  liwei
// The bug that makes volumePro rendering to be too dark in ColorTable fixed.
// I remember I ever fixed this bug before.
//
// Revision 3.0  2001/10/14 23:02:30  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.4   Oct 12 2001 17:59:44   liwei
// Added control point list to the transfer function class according to the requirement of the transfer function editor.
// 
//    Rev 2.3   Oct 01 2001 11:30:20   liwei
// Bugs in the interpolation factor of trilinear interpolation fixed.
// The alpha values are scaled according to the sampling distance and the interpolation factor.
// 
//    Rev 2.2   Sep 27 2001 18:15:26   liwei
// RendererTexMap uses its own colorTable, instead of one inside the transferFunction. Therefore we could compensate for different sampling distance owing to different viewing angle and different interpolation factor byscale the lookup table.
// 
//    Rev 2.1   Sep 27 2001 14:35:02   liwei
// Added bool flag for gradient opacity modulation.Bugs cause RenderVolumePro to render incorrectly fixed.
// 
//    Rev 2.0   Sep 16 2001 23:49:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:22   ingmar
// Initial revision.
// Revision 1.16  2001/09/13 16:30:29  liwei
// Bugs in non-alpha-from-window fixed.
//
// Revision 1.15  2001/09/06 19:00:33  frank
// This is still broken if you don't use alpha from window level!
//
// Revision 1.14  2001/08/06 16:03:10  liwei
// GLColorTable renamed to ColorTableGL.
// VLIColorTable renamed to ColorTableVLI.
//
// Revision 1.13  2001/08/06 15:49:00  liwei
// ColorTables now are members of transfer function rather than part of the renderers.
//
// Revision 1.12  2001/08/03 23:42:51  liwei
// TransferFunction now contains several lookup tables as its data members.
//
// Revision 1.11  2001/08/03 22:40:59  liwei
// Added virtual function AdjustRange().
//
// Revision 1.10  2001/07/30 15:55:42  liwei
// ColorCurves renamed to TransferFunction.
//
// Revision 1.9  2001/07/19 22:47:09  liwei
// Probably because the incorrect clock on the CVS server.
//
// Revision 1.8  2001/07/12 22:05:15  liwei
// Moved inline functions of ColorTable ColorTable.inl.
//
// Revision 1.7  2001/07/12 20:55:26  liwei
// ApplyColorTable() need to be called from outside, so it is now public.
//
// Revision 1.6  2001/07/11 22:27:30  liwei
// All color curves now using Point2D rather Point.
//
// Revision 1.5  2001/07/10 22:32:56  liwei
// ColorTable separated to ColorTable ColorTableGL
//
// Revision 1.4  2001/07/10 20:16:28  liwei
// Added to ZeroOne to handle variables of the value in [0, 1]
//
// Revision 1.3  2001/07/10 00:07:10  liwei
// Coordinates of the points in TransferFunction are scaled to the range of [0, 1].
//
// Revision 1.2  2001/07/03 23:03:20  liwei
// Inclusion of stdafx.h removed.
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/26 20:27:14  liwei
// Code restructured.
//
// Revision 1.1  2001/06/25 17:23:49  liwei
// *.cpp files renamed to *.C files
//
// Revision 1.4  2001/06/20 20:00:47  liwei
// Bugs in ColorTable SetRed() through SetAlpha() fixed
//
// Revision 1.3  2001/06/20 18:32:50  liwei
// A bug in BuildTable fixed
//
// Revision 1.2  2001/06/13 16:26:02  liwei
// no message
//
// Revision 1.1  2001/06/13 14:28:04  liwei
// Added to CVS
