// $Id: vxManipulatorDebug.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)
//
// Complete history at bottom of file.



// include declarations
#include "vxManipulatorDebug.h"
#include "vxEnvironment.h"
#include "Timer.h"


// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"



/**
 * default constructor
 * @param environment
 */
vxManipulatorDebug::vxManipulatorDebug(vxEnvironment & environment):
vxManipulator(environment)
//*******************************************************************
{  
} // Constructor()



/**
 * Event handler if key is pressed
 * @param uKeyId
 * @param uKeyboardState
 */ 
bool vxManipulatorDebug::OnKeyPressed(const uint1 uKeyId, 
                                      const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);
  switch (uKeyId)
  {
  case 'L':
  case 'l':
    ComputeLaplacianWeightedHistogram();
    break;
  case 'O':
  case 'o':
    WriteOutBuffers();
  case 'T':
  case 't':
    TestGLTexCallTimings();
    break;
  default:
    // just log out a debug message giving the key and state
    LogDbg(ToAscii(uint4(uKeyId)) + string(" ") + ToAscii(uint4(uKeyboardState)),"vxManipulatorDebug", "OnKeyPressed");
    break;
  }
  return bIfHandled;
} // OnKeyPressed



/**
 * writes out the OpenGL buffer content
 * debug test of Michael Meissner
 */ 
void vxManipulatorDebug::WriteOutBuffers()
//*******************************************************************
{
  static int4 iCount(0);
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  GLdouble mvmatrix[16], projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

  uint4 uSizeX = viewport[2];
  uint4 uSizeY = viewport[3];

  // first image
  Image<PixelRGB<uint1> > iImage(Triple<uint4>(uSizeX, uSizeY, 0), 0);
  float4 * pRGBA  = new float4[4*uSizeX*uSizeY];
  float4 * pDepth = new float4[uSizeX*uSizeY];
  vxUtils::readFramebuffer(pRGBA, pDepth, true);
  
  for (int4 j=0 ; j<uSizeY; ++j )
  {
    for (int4 i=0 ; i<uSizeX; ++i )
    {
      uint1 iColor = Bound(static_cast<uint1>(0), 
                           static_cast<uint1>(255.0*pDepth[j*uSizeX + i]), 
                           static_cast<uint1>(255));
      iImage.SetPixel((Point<uint2>(i,j)),PixelRGB<uint1>(iColor,iColor,iColor));
      //iImage.SetPixel((Point<uint2>(i,j)),PixelRGB<uint1>(255*pRGBA[4*(j*uSizeX + i)],
      //                                                    255*pRGBA[4*(j*uSizeX + i)+1],
      //                                                    255*pRGBA[4*(j*uSizeX + i)+2]));
    }
  }
  char sText[256];
  sprintf(sText, "C:\\depthImage1_%d.ppm", iCount++);
  iImage.Write(sText);

  // second image
  float4 fR, fG, fB, fA, vfDepth[3];
  for (j=0 ; j<uSizeY; ++j )
  {
    for (int4 i=0 ; i<uSizeX; ++i )
    {
      vxUtils::readFramebuffer(Point2D<uint4>(i,j), fR, fG, fB, fA, vfDepth, true);
      uint1 iColor = Bound(static_cast<uint1>(0), 
                           static_cast<uint1>(255.0*(vfDepth[0])), 
                           static_cast<uint1>(255));
      iImage.SetPixel((Point<uint2>(i,j)),PixelRGB<uint1>(iColor,iColor,iColor));
      //iImage.SetPixel((Point<uint2>(i,j)),PixelRGB<uint1>(255*fR, 255*fG, 255*fB));
    }
  }
  sprintf(sText, "C:\\depthImage2_%d.ppm", iCount++);
  iImage.Write(sText);
}


      
/**
 * compute the laplacian weighted histogram.
 * test code of Uday Chebrolu
 */
void vxManipulatorDebug::ComputeLaplacianWeightedHistogram()
//*******************************************************************
{
  std::vector<int4> vHistogram(4096,0);
  vxBlockVolume<uint2>*  pVol = static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolumePtr());
  vxBlockVolumeIterator<uint2> volIter(*pVol);

  int4 iMinVal(1000);
  //compute the initial laplacian weighted histogram.
  for( volIter.SetPos(0,0,0) ; !volIter.IsAtEnd() ; volIter.NextZYX())
  {
    uint2 uDensity(volIter.GetVoxel());

    // Intensity 2nd derivative w.r.t. X and Y:
    int4 uIxx = (int4) ( volIter.Peek1px0py0pz() - 2*uDensity + volIter.Peek1nx0py0pz() );
    int4 uIyy = (int4) ( volIter.Peek0px1py0pz() - 2*uDensity + volIter.Peek0px1ny0pz() );
    int4 uIzz = (int4) ( volIter.Peek0px0py1pz() - 2*uDensity + volIter.Peek0px0py1nz() );

    // Laplacian:
    int4 laplacian = uIxx + uIyy + uIzz;
    if(uDensity > 0)
    {
      vHistogram[uDensity] += laplacian;
      if(iMinVal > vHistogram[uDensity])
      {
        // find min value for repositioning the histogram.
        iMinVal = vHistogram[uDensity];
      }
    }
  }

  // reposition the histogram.
  // and compute the max val.
  int4 iMaxVal(0);
  for( int2 i(0) ; i <4096 ; ++i )
  {
    vHistogram[i] -= (iMinVal-1);
    if(iMaxVal < vHistogram[i])
    {
      iMaxVal = vHistogram[i];
    }
  }

  float4 fScaleFactor(1999.0F/iMaxVal);
  //Write a png imageof the histogram.
  Image<PixelRGB<uint1> > iImage(Triple<uint4>(4096,2000,0), 0);
  for( i=0 ; i<4096 ; ++i )
  {
    int4 iScaledVal(fScaleFactor * vHistogram[i]);
    iImage.SetPixel((Point<uint2>(i,iScaledVal)),PixelRGB<uint1>(200,200,200));
    // Also scale the histogram values.
  }
  iImage.Write("histGraph.ppm");


  // compute the cumulative histogram.
  int4 iMaxCumVal(0);
  for( i=4094 ; i >= 0 ; --i )
  {
    vHistogram[i] += vHistogram[i+1];
    int4 temp(vHistogram[i]);
    if(iMaxCumVal < vHistogram[i])
    {
      iMaxCumVal = vHistogram[i];
    }
  }

  // Write a png image of the cumulative histogram.
  float4 fCumScaleFactor(1999.0F/iMaxCumVal);
  Image<PixelRGB<uint1> > iCumImage(Triple<uint4>(4096,2000,0), 0);
  for( i=0 ; i<4096 ; ++i )
  {
    int4 iScaledVal(fCumScaleFactor * vHistogram[i]);
    iScaledVal = Bound(0,iScaledVal,1999);
    iCumImage.SetPixel((Point<uint2>(i,iScaledVal)),PixelRGB<uint1>(200,200,200));
  }
  iCumImage.Write("cumHistGraph.ppm");

  return;
} // ComputeLaplacianWeightedHistogram()




/**
 * compute timings for glTex calls
 * for comparision.
 */
void vxManipulatorDebug::TestGLTexCallTimings()
//*******************************************************************
{
  const GLsizei iNumTextures(100);
  std::vector<GLuint *> pData;

  Timer watch;
  // glTexImage2D()
  GLuint * pId1 = new GLuint[iNumTextures];
  glGenTextures(iNumTextures, pId1);
  for(int4 i(0) ; i<iNumTextures ; ++i)
  {
    pData[i] = new GLuint[(1<<12)];
    glBindTexture(GL_TEXTURE_2D, pId1[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, (1<<6), (1<<6), 0, GL_ALPHA, GL_UNSIGNED_BYTE, pData[i]);
    delete [] pData[i];
  }
  LogDbg("glTexImage2D : " + ToAscii(watch.ElapsedTime()),"vxManipulatorDebug","TestGLTexCallTimings");

  watch.Reset();
  // glTexSubImage2D()
  GLuint * pId2 = new GLuint[iNumTextures];
  glGenTextures(iNumTextures, pId2);
  for(i=0 ; i<iNumTextures ; ++i)
  {
    pData[i] = new GLuint[(1<<12)];
    glBindTexture(GL_TEXTURE_2D, pId2[i]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (1<<6), (1<<6), GL_ALPHA, GL_UNSIGNED_BYTE, pData[i]);
    delete [] pData[i];
  }
  LogDbg("glTexSubImage2D : " + ToAscii(watch.ElapsedTime()),"vxManipulatorDebug","TestGLTexCallTimings");

  watch.Reset();
  // glTexImage3D()
  GLuint * pId3 = new GLuint[iNumTextures];
  glGenTextures(iNumTextures, pId3);
  for(i=0 ; i<iNumTextures ; ++i)
  {
    pData[i] = new GLuint[(1<<12)];
    glBindTexture(GL_TEXTURE_2D, pId3[i]);
    glTexImage3DEXT(GL_TEXTURE_3D_EXT, 0, GL_ALPHA8, (1<<5), (1<<5), (1<<5), 0, GL_ALPHA, GL_UNSIGNED_BYTE, pData[i]);
    delete [] pData[i];
  }
  LogDbg("glTexImage3DEXT : " + ToAscii(watch.ElapsedTime()),"vxManipulatorDebug","TestGLTexCallTimings");
/*
  watch.Reset();
  // glTexSubImage3D()
  GLuint * pId4 = new GLuint[iNumTextures];
  glGenTextures(iNumTextures, pId4);
  for(i=0 ; i<iNumTextures ; ++i)
  {
    pData[i] = new GLuint[(1<<12)];
    glBindTexture(GL_TEXTURE_2D, pId4[i]);
    glTexSubImage3DEXT(GL_TEXTURE_3D_EXT, 0, 0, 0, 0, (1<<5), (1<<5), (1<<5), GL_ALPHA, GL_UNSIGNED_BYTE, pData[i]);
    delete [] pData[i];
  }
  LogDbg("glTexSubImage3DEXT : " + ToAscii(watch.ElapsedTime()),"vxManipulatorDebug","TestGLTexCallTimings");
*/
}




#undef FILE_REVISION



// Revision History:
// $Log: vxManipulatorDebug.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.5  2002/08/20 22:32:37  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.4  2002/08/08 15:09:58  uday
// Added TestGLTexCallTimings().
//
// Revision 1.3  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.2  2002/07/22 18:24:11  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.1.4.1  2002/07/15 19:17:50  michael
// added debug manipulator
//
// Revision 1.1  2002/07/01 15:46:25  michael
// added manipulator for debugging. This one needs to put into the VB code so
// that it is available in the view(s). Put in whatever you want to debug/write out
// into files upon certain inputs.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorDebug.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxManipulatorDebug.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
