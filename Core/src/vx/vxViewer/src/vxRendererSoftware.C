// $Id: vxRendererSoftware.C,v 1.8 2006/02/02 20:01:52 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSoftware.h"
#include "vxEnvironment.h"
#include "SystemInfo.h"
#include "vxMathUtils.h"
#include "vxIntensityConverter.h"
#include "vxDataRendererSoftware.h"


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor
 * @param environment
 */
vxRendererSoftware::vxRendererSoftware(vxEnvironment & environment)
:vxRenderer(environment),
m_iShift8Bit(4),
m_pRGBA(NULL)
//*******************************************************************
{
  SetTextureId(0);
  SetRenderingNecessary(false);

  // set number of threads to use
  SetNumberOfThreads( SystemInfo::GetCpuInfo().GetTotalThreads() );
  SetRenderThreadsPtr(new vxRenderThread[GetNumberOfThreads()]);
  SetRenderThreadsProgressPtr(new float4[GetNumberOfThreads()]);
  SetMultiThreadRendering((GetNumberOfThreads() > 1) ? true : false);

  // start the thread(s)
  for (int i(0); i<GetNumberOfThreads(); i++)
  {
    GetRenderThreadsPtr()[i].SetId(i);
    GetRenderThreadsPtr()[i].Start();
  }
} // constructor


/**
 * Destructor
 */
vxRendererSoftware::~vxRendererSoftware()
//*******************************************************************
{
  if (GetRenderThreadsPtr() != NULL)
  {
    // let the threads gracefully terminate
    for (int i(0); i<GetNumberOfThreads(); i++)
    {
      GetRenderThreadsPtr()[i].ExitRendering();
      GetRenderThreadsPtr()[i].Wait();
    }

    delete[] GetRenderThreadsPtr();
    delete[] GetRenderThreadsProgressPtr();
    SetRenderThreadsPtr(NULL);
    SetRenderThreadsProgressPtr(NULL);
  }
} // destructor


/**
 * Initialize routine
 */
void vxRendererSoftware::Initialize()
//*******************************************************************
{
  // get appropriate bit shift and colormap based on volume header
  int4 iMax(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()));
  int4 iShift(log(static_cast<float4>(GetNextPowerOfTwo(iMax+1)))/log(2.0F) - 8.0F); // 32 bins and 8 bit for color channels
  SetNumberOfShiftBitsFor8bitAlpha(iShift);
  SetTablePointerRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

  // create texture Id in case it is not yet there
  if (GetTextureTile().GetDensityTextureId() == 0)
  {
    GLuint id;
    glGenTextures(1, &id);
    GetTextureTile().SetDensityTextureId(id);
  }
} // Initialize()


/**
 * Uninitialize routine
 */
void vxRendererSoftware::Uninitialize()
//*******************************************************************
{
  // create texture Id in case it is not yet there
  if (GetTextureTile().GetDensityTextureId() != 0)
  {
    glDeleteTextures(1, &(GetTextureTile().GetDensityTextureId()));
    GetTextureTile().SetDensityTextureId(0);
  }
} // Uninitialize()


/**
 * PreRender routine
 */
int4 vxRendererSoftware::PreRender()
//*******************************************************************
{
  // get appropriate bit shift and colormap based on volume header
  int4 iMax(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()));
  int4 iShift(log(static_cast<float4>(GetNextPowerOfTwo(iMax+1)))/log(2.0F) - 8.0F); // 32 bins and 8 bit for color channels
  SetNumberOfShiftBitsFor8bitAlpha(iShift);
  SetTablePointerRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

  return false;
}


/**
 * Generates the texture
 */
void vxRendererSoftware::CreateTexture()
//*******************************************************************
{    
  // textureSize is the texture image resolution. it must be power of 2
  Triple<uint4> textureSize(GetImage().GetDim());

  if (textureSize.X() != GetNextPowerOfTwo(textureSize.X()))
  {
    throw ex::VException(LogRec("image to be used as texture is not power of 2 in DimX: " + textureSize, "vxRendererSoftware", "CreateTexture"));
  }
  if (textureSize.Y() <  GetNextPowerOfTwo(textureSize.Y())) // this is something most renderers do not cause but raycaster uses this for padding
  {
    textureSize.m_y = GetNextPowerOfTwo(textureSize.Y()) / 2;
  }

  // create a texture for the renderer
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, GetTextureTile().GetDensityTextureId());
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.X(), textureSize.Y(), 
               0, GL_RGBA, GL_UNSIGNED_BYTE, GetImage().GetDataPtr());

  return;
} // CreateTexture()


/**
 * returns the image
 *
 * @return the image
 */
Image<PixelRGBA<uint1> > & vxRendererSoftware::GetImage()
{

  vxDataRendererSoftware * pDataRendererSoftware = dynamic_cast< vxDataRendererSoftware * >( GetEnvironment().GetDataRendererSoftware() );
  if ( pDataRendererSoftware == NULL )
  {
    throw ex::NullPointerException( LogRec( "missing data renderer software in environment", "vxRendererSoftware", "GetImage" ) );
  }
  return pDataRendererSoftware->GetImage();

} // GetImage()


// undefines
#undef FILE_REVISION


// $Log: vxRendererSoftware.C,v $
// Revision 1.8  2006/02/02 20:01:52  geconomos
// updated for changes to SystemInfo
//
// Revision 1.7  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.6  2005/10/25 17:33:26  geconomos
// added for up to four threads
//
// Revision 1.5  2005/01/07 20:29:39  michael
// using the actual number of physical processors and NOT the logical processors
// to trigger the number of threads. Hence, we are NOT making use of hyperthreading!!!
//
// Revision 1.4  2004/10/01 12:03:44  frank
// used image in environment element rather than in renderer
//
// Revision 1.3  2004/04/06 16:27:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2003/11/03 15:10:39  michael
// reenabled multi-threading!!!
//
// Revision 1.28  2003/09/22 16:00:05  michael
// renamed shift variable to reflect the precision it is shifting to (8bit)
//
// Revision 1.27  2003/07/24 17:53:52  michael
// Changed RenderWindow() to pass in thread ID so that the progress of a
// multi-threaded rendering can be monitored
//
// Revision 1.26  2003/06/13 12:13:39  michael
// fixed missing initialization of shift bits in Initialize()
//
// Revision 1.25  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.24  2003/05/20 20:23:23  michael
// fixed crash that occurred when havin multi threading on but only one thread.
// In this case, the array was NULL but should contain one thread.
//
// Revision 1.23  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.22  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.21  2003/05/16 11:45:26  michael
// code review
//
// Revision 1.20  2003/05/15 11:52:35  michael
// code review
//
// Revision 1.19  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.18  2003/03/26 19:28:29  michael
// formatting
//
// Revision 1.17  2002/12/24 00:00:41  ingmar
// AsString() -> ToString() + operator string
//
// Revision 1.16  2002/11/20 21:38:19  geconomos
// Made rendering threads persitent.
//
// Revision 1.15  2002/11/14 17:34:48  ingmar
// added explanation
//
// Revision 1.14  2002/11/12 21:26:34  ingmar
// moved default CreateTexture() implementation from vxRendererSoftwareMPR() to vxRendererSoftware()
//
// Revision 1.13  2002/09/18 22:14:44  frank
// Fixed flickering - now we can multi-thread again.
//
// Revision 1.12  2002/08/21 18:33:29  michael
// ensured that despite multi-threading being off, there is at least one valid thread pointer
//
// Revision 1.11  2002/08/14 22:45:55  wenli
// initial version - Vascular
//
// Revision 1.10  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.9  2002/07/22 18:24:12  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.8.2.1  2002/07/09 18:28:36  michael
// Re-enabled multi-threading on the software MPR renderers
//
// Revision 1.8  2002/06/12 17:01:51  frank
// Sped up performance by more than 30%.
// Moved to 16^2 tiles for rendering.
// Automatically determined if we need to include the label volume.
//
// Revision 1.7  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.6  2002/04/06 00:17:45  michael
// fixed sample intensity range
//
// Revision 1.5  2002/04/03 20:31:23  michael
// fixed doxygen description
//
// Revision 1.4  2002/04/03 13:35:30  michael
// added method to uninitialize the textures
//
// Revision 1.3  2002/03/29 23:48:25  ingmar
// first step to enableing multithreading in CPU raycaster
//
// Revision 1.2  2002/03/15 21:19:42  michael
// further progress on the 2D renderers
//
// Revision 1.1  2002/03/15 01:27:55  michael
// added initial version of software renderers (still buggy)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSoftware.C,v 1.8 2006/02/02 20:01:52 geconomos Exp $
// $Id: vxRendererSoftware.C,v 1.8 2006/02/02 20:01:52 geconomos Exp $
