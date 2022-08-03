// $Id: TransferFunctionVC.C,v 1.3 2006/12/05 20:16:05 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File:  TransferFucntion.C 
// Description: A class to support transfer function creation and lookup 
// Owner: Frank Dachille (frank@viatronix.net)


// include declarations
#include "stdafx.h"
#include "TransferFunctionVC.h"
#include "HUconverter.h"



// static member initialization
const uint4 TransferFunctionVC::m_uSize(256);
const Triple<uint1> TransferFunctionVC::m_vSeenScheme1(0,255,0);
const Triple<uint1> TransferFunctionVC::m_vNotSeenScheme1(255,138,87);
const Triple<uint1> TransferFunctionVC::m_vSeenScheme2(87,87,255);
const Triple<uint1> TransferFunctionVC::m_vNotSeenScheme2(255,225,87);



// define declarations
#define FILE_REVISION "$Revision: 1.3 $"



/**
 * Constructor.
 */
TransferFunctionVC::TransferFunctionVC()
{
  m_pfRed     = new float4[m_uSize];
  m_pfGreen   = new float4[m_uSize];
  m_pfBlue    = new float4[m_uSize];
  m_pfOpacity = new float4[m_uSize];
  m_vfOpenGLTexture.resize(3*m_uSize);
  m_vfOpenGLPixelMap.resize(4*m_uSize);
  if (m_pfRed            == NULL ||
      m_pfGreen          == NULL ||
      m_pfBlue           == NULL ||
      m_pfOpacity        == NULL ||
      m_vfOpenGLTexture.size() == 0 ||
      m_vfOpenGLPixelMap.size() == 0)
  {
    LogWrn("Out of memory","TransferFunctionVC","TransferFunctionVC");
  }
  SetColonWall(30);
  m_fTranslucentScale = 1.0F;

  GetDefaultBiopsyThresholds(m_vfBiopsyThresholds);
} // constructor


/**
 * Destructor.
 */
TransferFunctionVC::~TransferFunctionVC()
{
  delete [] m_pfRed;
  delete [] m_pfGreen;
  delete [] m_pfBlue;
  delete [] m_pfOpacity;
  m_vfOpenGLTexture.clear();
  m_vfOpenGLPixelMap.clear();
} // destructor


/**
 * Set the colors in the entire range to a single color.
 */
void TransferFunctionVC::SetConstantColor(const Triple<float4>& vColor)
{
  for (unsigned int i=0; i<m_uSize; i++)
  {
    m_pfRed[i]   = vColor.m_x;
    m_pfGreen[i] = vColor.m_y;
    m_pfBlue[i]  = vColor.m_z;
  }

  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetConstantColor


/**
 * Ramp up the opacity linearly from zero to some maximum over the full range.
 */
void TransferFunctionVC::SetLinearOpacity(const float4 fMaxOpacity)
{
  for (uint4 i=0; i<m_uSize; i++)
  {
    m_pfOpacity[i] = float4(i) / static_cast<float4>(m_uSize-1) * fMaxOpacity;
  }

  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetLinearOpacity


/**
 * Sets the transfer function for viewing the colon wall given a threshold value.
 */
void TransferFunctionVC::SetColonWall(const float4 fColonWallThreshold)
{
  m_fColonWallThreshold = fColonWallThreshold;  
  float4 gray_max = fColonWallThreshold + 20;

  // MAKE SURFACE TRANSFER FUNCTION
  for (uint4 i=0; i<m_uSize; i++)
  {

    if (i <= fColonWallThreshold)
      m_pfOpacity[i] = 0.0;
    else if (i > fColonWallThreshold && i <= gray_max)
      m_pfOpacity[i] =  static_cast<float4>(i - fColonWallThreshold) / (gray_max-fColonWallThreshold);
    else if (i > gray_max)
      m_pfOpacity[i] = 1.0;

    m_pfRed[i]   = m_vNotSeenScheme1.m_x;
    m_pfGreen[i] = m_vNotSeenScheme1.m_y;
    m_pfBlue[i]  = m_vNotSeenScheme1.m_z;
  } // endfor all 256 surface values

  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetColonWall


/**
 * Sets default thresholds for the biopsy (pseudocolor) rendering
 */
void TransferFunctionVC::GetDefaultBiopsyThresholds(std::vector<LinearTFParams>& vParams)
{
  const float4 vfDefaultThresholds[] = { 0.0f, 10.f, 70.f, 110.f, 120.f, 125.f, 148.181f, 255.0f };

  int4 i(0);
  vParams.resize(8);
//vParams[i++] = LinearTFParams(   0.00f, (0.0f  ,  0.0f  ), (0.0f  ,   0.0f), (0.0f  ,   0.0f), (0.0f  , 140.0f));
  vParams[i++] = LinearTFParams(   0.00f, PixelRGBA<float4>(0.0f, 0.0f, 0.0f, 0.0f), PixelRGBA<float4>(0.0f, 0.0f, 140.0f, 0.0f));
//vParams[i++] = LinearTFParams(  28.00f, (0.013f,  0.091f), (0.0f  ,   0.0f), (0.0f  ,   0.0f), (140.0f, 200.0f));
  vParams[i++] = LinearTFParams(  28.00f, PixelRGBA<float4>(0.0f, 0.0f, 140.0f, 0.013f), PixelRGBA<float4>(0.0f, 0.0f, 200.0f, 0.091f));

//vParams[i++] = LinearTFParams(  66.00f, (0.091f,  0.143f), (0.0f  ,   0.0f), (130.0f, 170.0f), (200.0f, 240.0f));
  vParams[i++] = LinearTFParams(  66.00f, PixelRGBA<float4>(0.0f, 130.0f, 200.0f, 0.091f), PixelRGBA<float4>(0.0f, 170.0f, 240.0f, 0.143f));
//vParams[i++] = LinearTFParams( 104.00f, (0.143f,  0.156f), (0.0f  ,   0.0f), (170.0f, 180.0f), (0.0f  ,   0.0f));
  vParams[i++] = LinearTFParams( 104.00f, PixelRGBA<float4>(0.0f, 170.0f, 0.0f, 0.143f), PixelRGBA<float4>(0.0f, 180.0f, 0.0f, 0.156f));

//vParams[i++] = LinearTFParams( 120.00f, (0.156f,  0.162f), (170.0f, 175.0f), (180.0f, 185.0f), (0.0f  ,   0.0f));
  vParams[i++] = LinearTFParams( 120.00f, PixelRGBA<float4>(170.0f, 180.0f, 0.0f, 0.156f), PixelRGBA<float4>(175.0f, 185.0f, 0.0f, 0.162f));
//vParams[i++] = LinearTFParams( 124.00f, (0.305f,  0.378f), (225.0f, 255.0f), (0.0f  ,   0.0f), (0.0f  ,   0.0f));
  vParams[i++] = LinearTFParams( 124.00f, PixelRGBA<float4>(225.0f, 0.0f, 0.0f, 0.305f), PixelRGBA<float4>(255.0f, 0.0f, 0.0f, 0.378f));

//vParams[i++] = LinearTFParams( 148.18f,         (0.378f,  0.622f), (255.0f, 255.0f), (255.0f, 255.0f), (255.0f, 255.0f));
  vParams[i++] = LinearTFParams( 148.00f,         PixelRGBA<float4>(255.0f, 255.0f, 255.0f, 0.378f), PixelRGBA<float4>(255.0f, 255.0f, 255.0f, 0.622f));
//vParams[i++] = LinearTFParams( float4(m_uSize), (0.378f,  0.622f), (255.0f, 255.0f), (255.0f, 255.0f), (255.0f, 255.0f));
  vParams[i++] = LinearTFParams( float4(m_uSize), PixelRGBA<float4>(255.0f, 255.0f, 255.0f, 0.378f), PixelRGBA<float4>(255.0f, 255.0f, 255.0f, 0.622f));
} // GetDefaultBiopsyThresholds


float4 LinearTfunc(uint4 uLevel, uint4 uStart, uint4 uStop, float4 fStartVal, float4 fStopVal)
{
  float4 fWeight = float4(uLevel-uStart) / float4(uStop-uStart);
  return fStartVal*(1.0-fWeight) + fStopVal*(fWeight);
} // LinearTfunc


/**
 * Sets the transfer function for pseudocolored translucent rendering.
 */
void TransferFunctionVC::SetBiopsy()
{
  std::vector<LinearTFParams>& vbt = m_vfBiopsyThresholds;
  for (uint4 i(0), uThresh(0); i<m_uSize; i++)
  {
    if (i >= static_cast<int4>( vbt[uThresh+1].m_fThreshold * m_fTranslucentScale ) )
    {
      uThresh++;
    }

    m_pfOpacity[i] = LinearTfunc(i,   vbt[uThresh].m_fThreshold,  vbt[uThresh+1].m_fThreshold,   vbt[uThresh].m_prevColor.A(),  vbt[uThresh].m_nextColor.A() );
    m_pfRed[i]     = LinearTfunc(i,   vbt[uThresh].m_fThreshold,  vbt[uThresh+1].m_fThreshold,   vbt[uThresh].m_prevColor.R(),  vbt[uThresh].m_nextColor.R() );
    m_pfGreen[i]   = LinearTfunc(i,   vbt[uThresh].m_fThreshold,  vbt[uThresh+1].m_fThreshold,   vbt[uThresh].m_prevColor.G(),  vbt[uThresh].m_nextColor.G() );
    m_pfBlue[i]    = LinearTfunc(i,   vbt[uThresh].m_fThreshold,  vbt[uThresh+1].m_fThreshold,   vbt[uThresh].m_prevColor.B(),  vbt[uThresh].m_nextColor.B() );
  }

  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetBiopsy
//HUconverter::Uchar2HU(const uint1 uc) {return(uc*2000/255-1000);}


/**

 * Sets the transfer function for Agatston scoring colors.
 * /
void TransferFunctionVC::SetAgatston(Component<uint1> * pComponentInfo)
{
  for (int4 i=0; i<m_uSize; i++)
  {
    
    // default to opaque white
    const PixelRGBA<uint1> defaultColor(255, 255, 255, 255);
    const PixelRGBA<uint1> * pColor = NULL;

    // define ranges
    const float4 fMinimumRange(CardiacPlaques::GetHistogramRangeHU().first);
    const float4 fMaximumRange(CardiacPlaques::GetHistogramRangeHU().second);
    const float4 f130HU(Interpolate(fMinimumRange, 130.0F, fMaximumRange, 0.0F, float4(m_uSize)));
    const float4 f200HU(Interpolate(fMinimumRange, 200.0F, fMaximumRange, 0.0F, float4(m_uSize)));
    const float4 f300HU(Interpolate(fMinimumRange, 300.0F, fMaximumRange, 0.0F, float4(m_uSize)));
    const float4 f400HU(Interpolate(fMinimumRange, 400.0F, fMaximumRange, 0.0F, float4(m_uSize)));

    // define colors
    const PixelRGBA<uint1> colorA = pComponentInfo->GetColor(Component<uint1>::PLAQUE_131HU_TO_200HU);
    const PixelRGBA<uint1> colorB = pComponentInfo->GetColor(Component<uint1>::PLAQUE_201HU_TO_300HU);
    const PixelRGBA<uint1> colorC = pComponentInfo->GetColor(Component<uint1>::PLAQUE_301HU_TO_400HU);
    const PixelRGBA<uint1> colorD = pComponentInfo->GetColor(Component<uint1>::PLAQUE_401HU_AND_ABOVE); 
    
    if (i <= f130HU)
    {
      pColor = &defaultColor;
    }
    else if (i <= f200HU)
    {
      pColor = &colorA;
    }
    else if (i <= f300HU)
    {
      pColor = &colorB;
    }
    else if (i <= f400HU)
    {
      pColor = &colorC;
    }
    else
    {
       // > 400 HU
      pColor = &colorD;
    }
    
    m_pfRed    [i] = pColor->GetRed()   / 255.0F;
    m_pfGreen  [i] = pColor->GetGreen() / 255.0F;
    m_pfBlue   [i] = pColor->GetBlue()  / 255.0F;
    m_pfOpacity[i] = 1.0F;
    
  } // for all vlaues


  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetAgatston()
*/



/**
 * Sets the transfer function for TTP or barium enema visualization.
 */
void TransferFunctionVC::SetTransparentTissueProjection()
{
  int4 lWindow =  0.0F / 255.0F * m_uSize;
  int4 mWindow = 45.0F / 255.0F * m_uSize;
  int4 hWindow = 90.0F / 255.0F * m_uSize;
  
  float4 maxAlpha = 3.0F;
  int4 e;
  for (e=0; e<lWindow; e++)
  {
    m_pfOpacity[e] = 0;
  }
  
  float4 d1, d2;
  d1 = mWindow - lWindow;
  if (d1 > 0)
  {
    for(e=lWindow; e<mWindow; e++)
    {
      m_pfOpacity[e] = maxAlpha * (e - lWindow) /d1;
      m_pfOpacity[e] = m_pfOpacity[e] * (1+m_pfOpacity[e])* (1+m_pfOpacity[e]);
    }
  }
  d2 = hWindow - mWindow;;
  if (d2>0)
  {
    for(e=mWindow; e<hWindow; e++)
    {
      m_pfOpacity[e] = maxAlpha * (hWindow - e)/d2;
      m_pfOpacity[e] = m_pfOpacity[e] * (1+m_pfOpacity[e])* (1+m_pfOpacity[e]);
    }
  }
  for (e=hWindow; e<255; e++)
  {
    m_pfOpacity[e] = 0;
  }
  
  uint4 r = 255;
  uint4 g = 127;
  uint4 b = 83;
  
  for (e=0; e<m_uSize; e++)
  {
    m_pfRed[e] = r;
    m_pfGreen[e] = g;
    m_pfBlue[e] = b;
  }
  
  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetTransparentTissueProjection


/**
 * Sets the transfer function for an external view of the colon's surface.
 */
void TransferFunctionVC::SetExternalColonView()
{
  int4 lWindow =  0.0F / 255.0F * m_uSize;
  int4 mWindow = 45.0F / 255.0F * m_uSize;
  int4 hWindow = 90.0F / 255.0F * m_uSize;
  
  float4 maxAlpha = 1.0F;
  int4 e;
  for (e=0; e<lWindow; e++)
  {
    m_pfOpacity[e] = 0;
  }
  
  float4 d1, d2;
  d1 = mWindow - lWindow;
  if (d1 > 0)
  {
    for(e=lWindow; e<mWindow; e++)
    {
      m_pfOpacity[e] = maxAlpha * (e - lWindow) /d1;
    }
  }
  d2 = hWindow - mWindow;;
  if (d2>0)
  {
    for(e=mWindow; e<hWindow; e++)
    {
      m_pfOpacity[e] = maxAlpha * (hWindow - e)/d2;
    }
  }
  for (e=hWindow; e<255; e++)
  {
    m_pfOpacity[e] = 0;
  }
  
  uint4 r = m_vNotSeenScheme1.m_x;
  uint4 g = m_vNotSeenScheme1.m_y;
  uint4 b = m_vNotSeenScheme1.m_z;
  
  for (e=0; e<m_uSize; e++)
  {
    m_pfRed[e] = r;
    m_pfGreen[e] = g;
    m_pfBlue[e] = b;
  }
  
  PrepareOpenGLTexture();
  PrepareOpenGLPixelMap();
} // SetExternalColonView


/**
 * Sets the OpenGL texture environment to the current transfer function.
 */
void TransferFunctionVC::PrepareOpenGLTextureEnvironment() const
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage1D(GL_TEXTURE_1D, 0, 3, m_uSize, 0, GL_RGB, GL_FLOAT, (void*) &m_vfOpenGLTexture[ 0 ]);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
} // PrepareOpenGLTextureEnvironment


/**
 * Pre-computes a data structure for efficient OpenGL texture binding at a later time.
 */
void TransferFunctionVC::PrepareOpenGLTexture()
{
  for (uint4 i=0; i<m_uSize; i++)
  {
    m_vfOpenGLTexture[3*i+0] = m_pfRed    [i];
    m_vfOpenGLTexture[3*i+1] = m_pfGreen  [i];
    m_vfOpenGLTexture[3*i+2] = m_pfBlue   [i];
  }
} // PrepareOpenGLTexture


/**
 * Pre-computes a data structure for efficient OpenGL pixel map binding at a later time.
 */
void TransferFunctionVC::PrepareOpenGLPixelMap()
{
  for (uint4 i=0; i<m_uSize; i++)
  {
    m_vfOpenGLPixelMap[0 * m_uSize + i] = m_pfRed   [i] / 255.0F;
    m_vfOpenGLPixelMap[1 * m_uSize + i] = m_pfGreen [i] / 255.0F;
    m_vfOpenGLPixelMap[2 * m_uSize + i] = m_pfBlue  [i] / 255.0F;
    m_vfOpenGLPixelMap[3 * m_uSize + i] = 1.0F;
  }
} // PrepareOpenGLPixelMap


/**
 * Sets the OpenGL pixel map environment to the current transfer function.
 */
void TransferFunctionVC::PrepareOpenGLPixelMapEnvironment() const
{
  GLint iMaxMapSize(0);
  glGetIntegerv(GL_MAX_PIXEL_MAP_TABLE, &iMaxMapSize);
  if (iMaxMapSize < m_uSize)
  {
    LogWrn("Insufficient memory in video chip.","TransferFunctionVC","PrepareOpenGLPixelMapEnvironment");
    return;
  }

  glPixelMapfv(GL_PIXEL_MAP_R_TO_R, m_uSize, ((float4*)&m_vfOpenGLPixelMap[0]) + m_uSize*0);
  glPixelMapfv(GL_PIXEL_MAP_G_TO_G, m_uSize, ((float4*)&m_vfOpenGLPixelMap[0]) + m_uSize*1);
  glPixelMapfv(GL_PIXEL_MAP_B_TO_B, m_uSize, ((float4*)&m_vfOpenGLPixelMap[0]) + m_uSize*2);
  glPixelMapfv(GL_PIXEL_MAP_A_TO_A, m_uSize, ((float4*)&m_vfOpenGLPixelMap[0]) + m_uSize*3);
} // PrepareOpenGLPixelMapEnvironment

#undef FILE_REVISION 

// $Log: TransferFunctionVC.C,v $
// Revision 1.3  2006/12/05 20:16:05  jmeade
// Issue 5047: better organization of translucency settings structure.
//
// Revision 1.2  2005/11/16 04:52:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.1.4.1  2005/11/10 00:36:18  jmeade
// Default threshold of white colour set to 200HU.
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.9.2.6  2004/07/21 20:29:42  jmeade
// New default threshold position values from Dr. Perry Pickhardt.
//
// Revision 3.9.2.5  2004/05/15 00:34:41  jmeade
// Allow storage/adjustment of all biopsy/transluncency values.
//
// Revision 3.9.2.4  2004/05/14 17:18:50  jmeade
// Adjustable biopsy thresholds.
//
// Revision 3.9.2.3  2004/03/22 21:46:19  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.9.2.2  2003/07/25 20:17:14  jmeade
// Issue 3404: Translucency display changes.
//
// Revision 3.9.2.1  2003/04/22 18:19:57  jmeade
// Issue 3123: Use std::vector<> for color maps (maybe one day we'll do same for the color arrays?)
//
// Revision 3.9  2002/11/16 22:28:53  kevin
// Add White to translucency for > 200HU
//
// Revision 3.8  2002/11/11 14:23:55  kevin
// Store the value used for the colon wall threshold
//
// Revision 3.7  2002/04/30 23:58:44  michael
// Took Plaque and CardiacPlaque out of project file, only working in CS branch
//
// Revision 3.6  2002/03/11 17:39:20  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.5  2002/02/02 00:51:13  frank
// Synchronized color transfer function with the CardiacPlaques object.
//
// Revision 3.4.2.2  2002/02/12 23:20:42  frank
// obtained the histogram range from CardiacPlaques.
//
// Revision 3.4.2.1  2002/02/06 23:51:58  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.4  2002/01/03 02:21:57  kevin
// Adjusted the TTP transfer function since we create the small opacity
// volume from the ECV vol now that the ses,set,sec are gone
//
// Revision 3.3  2001/11/21 20:49:37  jenny
// fixed a bug for rendering histograms
//
// Revision 3.2  2001/11/13 04:00:09  frank
// Updated transfer function to include Agatston colors.
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:28  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 11 2001 11:40:20   binli
// issue 1122: adjust TransferFunction (maximum opaciy) for better effect
// 
//    Rev 2.2   Oct 09 2001 10:08:36   manju
// No change.
// 
//    Rev 2.1   Oct 01 2001 16:10:20   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.1  2001/07/30 15:55:05  liwei
// 6_dataStruct TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.10  2001/07/20 13:16:15  geconomos
// Added event handling to handle state changes
//
// Revision 1.9  2001/06/06 19:47:48  jmeade
// Coding compliance
//
// Revision 1.8  2001/05/15 17:49:39  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.7  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.6  2001/03/21 14:58:57  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.5  2001/03/21 13:00:37  frank
// Moved SEEN and NOTSEEN colors out of #define into coding compliant Triple<uint1>
//
// Revision 1.4  2001/02/14 19:54:31  liwei
// TTP transfer function tuned
//
// Revision 1.3  2001/02/12 21:42:10  liwei
// TTP transfer function changed
//
// Revision 1.2  2001/02/12 21:18:10  liwei
// TTP rendering quality improved, with a little speed sacrifice
//
// Revision 1.1  2001/01/31 19:02:30  kevin
// Moved transferfunction to datastruct so triangle rendering could get
// correct colors
//
// Revision 1.15  2001/01/30 02:07:50  jeff
// NO ABORTS!!!
//
// Revision 1.14  2001/01/26 14:21:07  frank
// Fixed biopsy slice rendering...how did this get lost?
//
// Revision 1.13  2001/01/26 13:22:56  frank
// Removed bad #define
//
// Revision 1.12  2001/01/25 15:59:40  liwei
// Jittering for volume rendering in the overview
//
// Revision 1.11  2001/01/24 22:36:44  liwei
// Transfer function for non-transparent external view tuned
//
// Revision 1.10  2001/01/24 19:42:10  frank
// Rename TTP for clarity.
//
// Revision 1.9  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.8  2001/01/19 14:07:14  frank
// Added pixel color map features.
//
// Revision 1.7  2001/01/17 18:53:56  frank
// Fixed typo which removed the black line from the biopsy xfer function.
//
// Revision 1.6  2001/01/12 17:58:27  frank
// Added OpenGL texture functions.
//
// Revision 1.5  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.4  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.3  2000/10/09 23:35:36  kevin
// Changes to Volume Render Color
//
// Revision 1.2  2000/09/19 12:51:58  frank
// Added memory allocation check
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 7     8/28/00 12:07p Antonio
// updated header and footer
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/21/00  Time:  5:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added scaleing (window/level-like functionality) of the translucent
//transfer function
//
//*****************  Version 5  *****************
//User: Kevin           Date:  8/06/00  Time:  9:58a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  moved m_colonWall Thershold ot readerGloabl (from fields) and moved
//the reading of the inf file
//
//*****************  Version 4  *****************
//User: Frank           Date:  7/25/00  Time:  8:53a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Re-wrote for new volume classes and file format.
//Added colon boundary constraints.
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/18/00  Time:  1:40p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created TransferFunctionVC.cpp
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/TransferFunctionVC.C,v 1.3 2006/12/05 20:16:05 jmeade Exp $
// $Id: TransferFunctionVC.C,v 1.3 2006/12/05 20:16:05 jmeade Exp $

