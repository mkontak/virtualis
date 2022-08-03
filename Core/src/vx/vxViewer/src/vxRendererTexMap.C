// $Id: vxRendererTexMap.C,v 1.11 2007/02/13 22:33:41 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.com)

/*
  Base class for Texture Mapping Renderers
*/


// includes
#include "stdafx.h"
#include "vxRendererTexMap.h"
#include "vxEnvironment.h"
#include "vxPlaneOrtho.h"
#include "GLFontGeorge.h"
#include "vxIntensityConverter.h"
#include "vxMathUtils.h"
#include "vxRenderingMode.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.11 $"


/**
 * Constructor
 * @param reference to an environment
 */
vxRendererTexMap::vxRendererTexMap(vxEnvironment & environment)
:vxRenderer(environment),
m_eRenderMode(DENSITY_TAG_POST_CLASSIFY),
m_iTFTextureID(0),
m_iTFPreIntegratedTextureID(0),
m_piTFTextureData(NULL),
m_piTFPreIntegratedTextureData(NULL),
m_iComponentTextureID(0),
m_piComponentTextureData(NULL),
m_iIntensityRangeTextureID(0),
m_piIntensityRangeTextureData(NULL),
m_iDummyTextureID(0),
m_piDummyTextureData(NULL),
m_iDummyTextureCombID(0),
m_piDummyTextureCombData(NULL),
m_bTexCombEnabled(false),
m_bTagsEnabled(false),
m_fPlaneDistance(1.0)
//*******************************************************************
{
}


/**
 * Destructor
 */
vxRendererTexMap::~vxRendererTexMap()
//*******************************************************************
{
  if (m_piComponentTextureData != NULL)       delete[] m_piComponentTextureData;
  if (m_piDummyTextureCombData != NULL)       delete[] m_piDummyTextureCombData;
  if (m_piDummyTextureData != NULL)           delete[] m_piDummyTextureData;
  if (m_piIntensityRangeTextureData != NULL)  delete[] m_piIntensityRangeTextureData;
  if (m_piTFPreIntegratedTextureData != NULL) delete[] m_piTFPreIntegratedTextureData;
  if (m_piTFTextureData != NULL)              delete[] m_piTFTextureData;
} // destructor


/**
 * Public method to get the rendering mode
 */
void vxRendererTexMap::Initialize()
//*******************************************************************
{
  GLuint id;
  glGenTextures(1, &id); SetTFTextureID(id);
  glGenTextures(1, &id); SetTFPreIntegratedTextureID(id);
  glGenTextures(1, &id); SetIntensityRangeTextureID(id);
  glGenTextures(1, &id); SetComponentTextureID(id);
  glGenTextures(1, &id); SetDummyTextureID(id);
  glGenTextures(1, &id); SetDummyTextureCombID(id);
} // Initialize()


/**
 * Public method to get the rendering mode.
 */
void vxRendererTexMap::Uninitialize()
//*******************************************************************
{
  GLuint id;
  if ((id = GetTFTextureID()) > 0)              glDeleteTextures(1, &id);
  if ((id = GetTFPreIntegratedTextureID()) > 0) glDeleteTextures(1, &id);
  if ((id = GetIntensityRangeTextureID()) > 0)  glDeleteTextures(1, &id);
  if ((id = GetComponentTextureID()) > 0)       glDeleteTextures(1, &id);
  if ((id = GetDummyTextureID()) > 0)           glDeleteTextures(1, &id);
  if ((id = GetDummyTextureCombID()) > 0)       glDeleteTextures(1, &id);

  SetupDummyTexture();
} // Uninitialize()


/**
 * Private method to setup the GL texture combiners.
 */
void vxRendererTexMap::SetupGLTextureCombiners()
//*******************************************************************
{
  glDisable(GL_REGISTER_COMBINERS_NV);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_SHADER_NV);

  if (GetRenderMode() == DENSITY_TAG_POST_CLASSIFY)
  {
    glEnable(GL_REGISTER_COMBINERS_NV);
    if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR))
    {
      glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 5);
    }
    else
    {
      glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
    }
  }
  if (GetRenderMode() == DENSITY_PRE_INTEGRATED)
  {
    glEnable(GL_REGISTER_COMBINERS_NV);
    glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);
  }

  // register combiner setup
  if (GetEnvironment().GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_COMPONENT_IN_MPR))
  {
    // (1-<A>label)<RGB>sample
    glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
    glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_PRIMARY_COLOR_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // (1-<A>label) * <A>sample
    glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_INVERT_NV, GL_ALPHA);
    glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);    
    glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_PRIMARY_COLOR_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // (<A>label)<RGB>label
    glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
    glCombinerInputNV(GL_COMBINER1_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glCombinerOutputNV(GL_COMBINER1_NV, GL_RGB, GL_SECONDARY_COLOR_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // above <A>acc + <A>label
    glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // <A>acc
    glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_B_NV,             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);   // 1.0
    glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_C_NV,             GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);   // 1.0
    glCombinerInputNV(GL_COMBINER1_NV, GL_ALPHA, GL_VARIABLE_D_NV,     GL_TEXTURE3_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // <A>label
    glCombinerOutputNV(GL_COMBINER1_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SECONDARY_COLOR_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
   
    /////////////////////////////////////////
    // if blue == 255, set alpha to zero!!!
    /////////////////////////////////////////
    GLfloat color1[4] = {0.99F, 0.99F, 0.99F, 0.99F};
    glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, color1);
    glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_A_NV,               GL_ZERO, GL_UNSIGNED_INVERT_NV,  GL_ALPHA);  // 1.0
    glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_CONSTANT_COLOR0_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // 0.99
    glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_C_NV,               GL_ZERO, GL_UNSIGNED_INVERT_NV,  GL_ALPHA);  // 1.0
    glCombinerInputNV(GL_COMBINER2_NV, GL_ALPHA, GL_VARIABLE_D_NV,       GL_TEXTURE3_ARB, GL_SIGNED_NEGATE_NV, GL_BLUE);      // -<B>label
    glCombinerOutputNV(GL_COMBINER2_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE1_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

    glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_C_NV,      GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);  // 1.0
    glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_D_NV,      GL_ZERO, GL_HALF_BIAS_NEGATE_NV, GL_ALPHA); // 0.5
    glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_A_NV,      GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);  // 1.0
    glCombinerInputNV(GL_COMBINER3_NV, GL_ALPHA, GL_VARIABLE_B_NV, GL_SPARE1_NV, GL_SIGNED_IDENTITY_NV, GL_ALPHA);  // spare1
    glCombinerOutputNV(GL_COMBINER3_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SPARE0_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

    glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_A_NV,               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // 0
    glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_B_NV,               GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // 0
    glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_C_NV,               GL_ZERO, GL_UNSIGNED_INVERT_NV, GL_ALPHA);   // 1
    glCombinerInputNV(GL_COMBINER4_NV, GL_ALPHA, GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA); // <A>
    glCombinerOutputNV(GL_COMBINER4_NV, GL_ALPHA, GL_DISCARD_NV, GL_DISCARD_NV, GL_SECONDARY_COLOR_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_TRUE);
    
    // add RGB and keep final <A>
    glFinalCombinerInputNV(GL_VARIABLE_A_NV,               GL_ZERO,   GL_UNSIGNED_INVERT_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_B_NV,   GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_C_NV,               GL_ZERO, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_SECONDARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
  }
  else
  {
    glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB);
    glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,         GL_ZERO,   GL_UNSIGNED_INVERT_NV, GL_RGB);    
    glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_PRIMARY_COLOR_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
    glCombinerInputNV(GL_COMBINER0_NV, GL_ALPHA, GL_VARIABLE_B_NV,         GL_ZERO,   GL_UNSIGNED_INVERT_NV, GL_ALPHA);
    glCombinerOutputNV(GL_COMBINER0_NV, GL_ALPHA, GL_PRIMARY_COLOR_NV, GL_DISCARD_NV, GL_DISCARD_NV, GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_B_NV,      GL_ZERO,   GL_UNSIGNED_INVERT_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_C_NV,      GL_ZERO, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_D_NV,      GL_ZERO, GL_UNSIGNED_IDENTITY_NV,   GL_RGB);
    glFinalCombinerInputNV(GL_VARIABLE_G_NV, GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV, GL_ALPHA);
  }

  m_bTexCombEnabled = true; // register texture combiners are now enabled.

} // SetupGLTextureCombiners()


/**
 * Private method to setup the GL texture combiners.
 */
void vxRendererTexMap::DisableGLTextureCombiners()
//*******************************************************************
{
  if (GetRenderMode() == DENSITY_TAG_POST_CLASSIFY)
  {
    glDisable(GL_REGISTER_COMBINERS_NV);
  }

  glDisable(GL_TEXTURE_SHADER_NV);

  m_bTexCombEnabled = false; // register texture combiners are now disabled.
} // DisableGLTextureCombiners()


/**
 * Set up textures for TF and components
 * @return Success value
 */
int4 vxRendererTexMap::PreRender()
//*******************************************************************
{
  vxEnvironment & environment(GetEnvironment());
  
  // has the win rect changed?
  if ( GetEnvironment().IsModified( vxEnvironment::WIN_RECT ) == true )
    SetRenderingNecessary( true );



  if (environment.GetIntensityVolume() != NULL)
  {
    if (environment.GetLabelVolume() != NULL)
    {
      SetRenderingNecessary(true);
    }
  }

  if (environment.IsModified(vxEnvironment::COLORMAP)
      || environment.IsModified(vxEnvironment::VOLUME_INTENSITY) || environment.IsModified(vxEnvironment::CAMERA))
  {
    UpdateTransferFunctionTexture();
    SetRenderingNecessary(true);
  }
  if (environment.IsModified(vxEnvironment::COMPONENT_VISUAL))
  {
    UpdateComponentTexture();
    SetRenderingNecessary(true);
  }
  if (GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE))
  {
    if (environment.GetRenderingMode()->IsEnabled(vxRenderingMode::SHOW_INTENSITY_RANGE))
    {
      UpdateIntensityRangeTexture();
      SetRenderingNecessary(true);
    }
  }
  
  // has the rendering mode changed?
  if (GetEnvironment().IsModified(vxEnvironment::RENDERING_MODE))
  {
    SetRenderingNecessary(true);
  }
  
  return 1;
} // PreRender()


/**
 * PostRender
 * @return Success value.
 */
int4 vxRendererTexMap::PostRender()
//*******************************************************************
{
	GLEnv::DisableTextureUnits();

  return 1;
} // PostRender()


/**
 * Setup for dummy texture.
 */
void vxRendererTexMap::SetupDummyTexture()
//*******************************************************************
{
  const uint2 uDim(2*4*2); // dimension of texture data.

  // bind dummy texture
  if (m_piDummyTextureData == NULL)
  {
    m_piDummyTextureData = new GLubyte[ uDim ];
  }

  for (int j(0) ; j<uDim; j++)
  {
    m_piDummyTextureData[j] = 0;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, m_iDummyTextureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, 2, 2, 0, GL_ALPHA, GL_UNSIGNED_BYTE, m_piDummyTextureData);

  // Bind Dummy Texture Combiner:
  
  uint1 iAlpha(0), iRed(0), iGreen(0), iBlue(0);
  if (m_piDummyTextureCombData == NULL)
  {
    m_piDummyTextureCombData = new GLubyte[uDim];
  }
  PixelRGBA<uint1> dummyColor(0, 0, 0, 0);
  for (int i(0); i<2; i++)
  {
    iRed   = dummyColor.R();
    iGreen = dummyColor.G();
    iBlue  = dummyColor.B();
    iAlpha = dummyColor.A();
    m_piDummyTextureCombData[4*i + 0] = iRed;
    m_piDummyTextureCombData[4*i + 1] = iGreen;
    m_piDummyTextureCombData[4*i + 2] = iBlue;
    m_piDummyTextureCombData[4*i + 3] = iAlpha;
    m_piDummyTextureCombData[8 + 4*i + 0] = iRed;
    m_piDummyTextureCombData[8 + 4*i + 1] = iGreen;
    m_piDummyTextureCombData[8 + 4*i + 2] = iBlue;
    m_piDummyTextureCombData[8 + 4*i + 3] = iAlpha;
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, m_iDummyTextureCombID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_piDummyTextureCombData);
} // SetupDummyTexture()


/**
 * Set the transfer function, triggers dependent texture generation.
 */
void vxRendererTexMap::UpdateTransferFunctionTexture()
//*******************************************************************
{
  float4 * pRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(255));
  uint1 iAlpha(0), iRed(0), iGreen(0), iBlue(0);
  uint2 iSizeX(0);
  uint2 iSizeY(0);

  if (m_eRenderMode == DENSITY_TAG_POST_CLASSIFY)
  {
    iSizeX = 256;
    iSizeY = 2;

    if (m_piTFTextureData == NULL)
    {
      m_piTFTextureData = new GLubyte[4*iSizeY*iSizeX];
    }

    float8 fAlpha(0.0);
    for (int i(0); i<256; i++)
    {
      fAlpha = pRGBA[4*i+3] / 255.0;
      fAlpha = 1.0 - pow(1-fAlpha, 1);
      iAlpha = 255 * fAlpha;
      iRed   = pRGBA[4*i + 0];
      iGreen = pRGBA[4*i + 1];
      iBlue  = pRGBA[4*i + 2];
      m_piTFTextureData[4*i + 0] = iRed; 
      m_piTFTextureData[4*i + 1] = iGreen; 
      m_piTFTextureData[4*i + 2] = iBlue; 
      m_piTFTextureData[4*i + 3] = 0; //iAlpha; 
      m_piTFTextureData[1024 + 4*i + 0] = iRed; 
      m_piTFTextureData[1024 + 4*i + 1] = iGreen; 
      m_piTFTextureData[1024 + 4*i + 2] = iBlue; 
      m_piTFTextureData[1024 + 4*i + 3] = 0; //iAlpha; 
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, m_iTFTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iSizeX, iSizeY,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_piTFTextureData);
  }

  if (m_eRenderMode == DENSITY_PRE_INTEGRATED)
  {
    iSizeX = 256;
    iSizeY = 256;

    if (m_piTFPreIntegratedTextureData == NULL)
    {
      m_piTFPreIntegratedTextureData = new GLubyte[4*iSizeX*iSizeY];
    }

    // call pre-integration to correctly compute the values
    PreIntegrateTF(m_piTFPreIntegratedTextureData);

    // generate the corresponding texture
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, m_iTFPreIntegratedTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iSizeX, iSizeY,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_piTFPreIntegratedTextureData);
  }

  return;
} // UpdateTransferFunctionTexture()


/**
 * Set the component, triggers dependent texture generation.
 */
void vxRendererTexMap::UpdateComponentTexture()
//*******************************************************************
{
  uint1 iAlpha(0), iRed(0), iGreen(0), iBlue(0);
  const std::vector<vxComponent *> & vpComponents(GetEnvironment().GetComponents()->ArrayByLabel());

  // component texture
  //////////////////////
  if (m_piComponentTextureData == NULL)
  {
    m_piComponentTextureData = new GLubyte[2*4*256];
  }

  // we do not allow full blue color since this is used in the 
  // register combiner to identify which fragments should be removed
  ////////////////////////////////////////////////////////////////////
  for (int i(0); i<256; i++)
  {
    PixelRGBA<uint1> componentColor(0, 0, 0, 0);
    if (i < vpComponents.size())
    {
      if (vpComponents[i]->IsIntensityVisible() == true)
      {
        if (vpComponents[i]->IsLabelVisible() == true)
        {
          componentColor = vpComponents[i]->GetColor();
          if (componentColor.B() > 250)
          {
            componentColor.B() = 250;
          }
        }
      }
      else
      {
        componentColor.B() = 255;
      }
    }

    iRed   = componentColor.R();
    iGreen = componentColor.G();
    iBlue  = componentColor.B();
    iAlpha = componentColor.A();

    m_piComponentTextureData[4*i + 0] = iRed; 
    m_piComponentTextureData[4*i + 1] = iGreen; 
    m_piComponentTextureData[4*i + 2] = iBlue; 
    m_piComponentTextureData[4*i + 3] = iAlpha; 
    m_piComponentTextureData[1024 + 4*i + 0] = iRed; 
    m_piComponentTextureData[1024 + 4*i + 1] = iGreen; 
    m_piComponentTextureData[1024 + 4*i + 2] = iBlue; 
    m_piComponentTextureData[1024 + 4*i + 3] = iAlpha; 
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, m_iComponentTextureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 2,
               0, GL_RGBA, GL_UNSIGNED_BYTE, m_piComponentTextureData);
} // UpdateComponentTexture()


/**
 * Set the intensity range, triggers dependent texture generation.
 */
void vxRendererTexMap::UpdateIntensityRangeTexture()
//*******************************************************************
{
  uint1 iAlpha(0), iRed(0), iGreen(0), iBlue(0);
  int4 iIntensityLow(  vxIntensityConverter::RawToModality(  GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityLow(),
    GetEnvironment().GetIntensityVolume()->GetHeader() ));

  int4 iIntensityHigh( vxIntensityConverter::RawToModality( GetEnvironment().GetInteractiveSegmentation()->GetRegionIntensityHigh(),
      GetEnvironment().GetIntensityVolume()->GetHeader() ));

  // get appropriate bit shift and colormap based on volume header
  int4 iMax(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()));
  int4 iShift(log(static_cast<float4>(GetNextPowerOfTwo(iMax+1)))/log(2.0F) - 8.0F); // 32 bins and 8 bit for color channels

  iIntensityLow  = (iIntensityLow) >> iShift;
  iIntensityHigh = (iIntensityHigh) >> iShift;
 
  // Allocate intensity range texture data:
  if (m_piIntensityRangeTextureData == NULL)
  {
    m_piIntensityRangeTextureData = new GLubyte[2*4*256];
  }

  // Set RGBA for intensity range texture:
  PixelRGBA<uint1> intensityRangeColor(0, 255, 0, 0);
  for (int i(0); i<256 ; i++)
  {
    if (iIntensityLow <= i && i <= iIntensityHigh)
    {
      iAlpha = 128; // if in range set opacity to max.
    }
    else
    {
      iAlpha = intensityRangeColor.A();
    }
    iRed   = intensityRangeColor.R();
    iGreen = intensityRangeColor.G();
    iBlue  = intensityRangeColor.B();
    m_piIntensityRangeTextureData[4*i + 0] = iRed; 
    m_piIntensityRangeTextureData[4*i + 1] = iGreen; 
    m_piIntensityRangeTextureData[4*i + 2] = iBlue; 
    m_piIntensityRangeTextureData[4*i + 3] = iAlpha; 
    m_piIntensityRangeTextureData[1024 + 4*i + 0] = iRed; 
    m_piIntensityRangeTextureData[1024 + 4*i + 1] = iGreen; 
    m_piIntensityRangeTextureData[1024 + 4*i + 2] = iBlue; 
    m_piIntensityRangeTextureData[1024 + 4*i + 3] = iAlpha; 
  }

  // Set GL texture parameters:

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glBindTexture(GL_TEXTURE_2D, GetIntensityRangeTextureID());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 2,
               0, GL_RGBA, GL_UNSIGNED_BYTE, m_piIntensityRangeTextureData);
} // UpdateIntensityRangeTexture()


/**
 * Pre-integrate transfer function
 * @param pointer to GLuByte data
 */
void vxRendererTexMap::PreIntegrateTF(GLubyte * pData)
//*******************************************************************
{
  int2 i, j, k;
  int1 iStep(0);
  uint1 iAlpha(0);
  float4 fAccumLength(0.0);
  float4 fRed(0.0), fGreen(0.0), fBlue(0.0), fTp(0.0), fAlpha(0.0);
  float4 fLocalAlpha(0.0);
 
  float4 * pRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(255));
 
  for (j=0; j<256; j++) 
  {
    for (i=0; i<256; i++) 
    {
      if (i==j)
      {
        fAlpha = pRGBA[4*i+3];
        fAlpha = 1.0F - pow(1.0F-fAlpha, (float4)( 1.0F/m_fPlaneDistance));
        
        pData[4*(256*j + i) + 0] = pRGBA[4*i + 0];
        pData[4*(256*j + i) + 1] = pRGBA[4*i + 1];
        pData[4*(256*j + i) + 2] = pRGBA[4*i + 2];
        pData[4*(256*j + i) + 3] = 255 * fAlpha;
      }
      else 
      {
        // initialize values for ray segment [j,i]
        ////////////////////////////////////////////
        fRed = fGreen = fBlue  = 0.0;
        fTp  = 1.0;
        fAccumLength = m_fPlaneDistance/fabs(static_cast<float4>(i-j));

        const int1 iTmpUp(1);
        const int1 iTmpDown(-1);
        iStep = (j<i) ? iTmpUp : iTmpDown;

        for (k=j; k!=i; k+=iStep) 
        {
          if (fTp >= 0.05)
          {
            // pre-integrate color and alpha for each interval
            ////////////////////////////////////////////
            fAlpha = 1.0 - pow(1.0F - pRGBA[4*k+3], fAccumLength);
            fLocalAlpha = fTp * fAlpha;
            fRed   += fLocalAlpha * pRGBA[4*k + 0];
            fGreen += fLocalAlpha * pRGBA[4*k + 1];
            fBlue  += fLocalAlpha * pRGBA[4*k + 2];
            fTp *= (1 - fAlpha);
          }
          else
            break;
        }
        if (fTp < 1.0) { // Divide the color by its opacity 
          fRed   /= (1.0 - fTp);
          fGreen /= (1.0 - fTp);
          fBlue  /= (1.0 - fTp);
        }

        pData[4*(256*j + i) + 0] = 255 * fRed;
        pData[4*(256*j + i) + 1] = 255 * fGreen;
        pData[4*(256*j + i) + 2] = 255 * fBlue;
        pData[4*(256*j + i) + 3] = 255 * (1.0 - fTp);
      }
    }
  }
  WritePreintegratedTables(pData);
} // PreIntegrateTF()


/**
 * Helper function - write out preintegrated tables
 * @param pointer to GL unsigned byte data
 */
void vxRendererTexMap::WritePreintegratedTables(GLubyte * pData)
//*******************************************************************
{
  char szFileNameHeader[255];
  strcpy(szFileNameHeader, "D:/CVS/PriTable");

  char szFileNameRGB[255];
  strcpy(szFileNameRGB, szFileNameHeader);
  strcat(szFileNameRGB, "RGB.ppm");

  char szFileNameA[255];
  strcpy(szFileNameA, szFileNameHeader);
  strcat(szFileNameA, "A.ppm");

  FILE *fpRGB, *fpA;
  fopen_s( &fpRGB, szFileNameRGB, "w");
  if ( !fpRGB ) 
  {
    cout << "Error opening file for preintegrated map!" << endl;
    return;
  }
  fopen_s ( &fpA, szFileNameA, "w");
  if (!fpA ) 
  {
    cout << "Error opening file for preintegrated map!" << endl;
    return;
  }
  fprintf_s(fpRGB, "P3\n");
  fprintf_s(fpRGB, "256 256\n");
  fprintf_s(fpRGB, "255\n");
  fprintf_s(fpA, "P3\n");
  fprintf_s(fpA, "256 256\n");
  fprintf_s(fpA, "255\n");

  for (uint2 j(0); j<256; j++) 
  {
    for (uint2 i(0); i<256; i++) 
    {
      fprintf_s(fpRGB, "%d, %d, %d\n", pData[4*(256*j+i) + 0], 
                                     pData[4*(256*j+i) + 1],
                                     pData[4*(256*j+i) + 2]);
      fprintf_s(fpA, "%d, %d, %d\n", pData[4*(256*j+i) + 3],
                                   pData[4*(256*j+i) + 3],
                                   pData[4*(256*j+i) + 3]);
    }
  }
  fclose(fpRGB);
  fclose(fpA);
} // WritePreintegratedTables()


/**
 * Private method to render the volume/all tiles
 * @param orthonormal plane of 3D cursor
 * @param index indicating which cursor slabs - 0, or 1
 */
GLuint vxRendererTexMap::GenCursor3dGLlist(vxPlaneOrtho<float8> & rCursor3dPlane, uint2 i)
//*******************************************************************
{
  if (i > 1)
  {
    i = 1;
  }

  if (m_cursorPlaneId[i] == 0)
  {
    m_cursorPlaneId[i] = glGenLists(1);
  }
  
  if (m_cursorPlaneId[i] != 0)
  {
    glNewList(m_cursorPlaneId[i], GL_COMPILE);
  }
  else
  {
    throw ex::RuntimeException(LogRec("no more OpenGL IDs available", "vxRendererTexMap2D", "GenCursor3dGLlist"));
  }

  const Vector3D<float4> size(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetWorldDim());
  float8 fMax(0.05 * Max(size.X() - 1, size.Y() - 1, size.Z() - 1));

  Point<float4> volSize(Point<float4>(size.X() - 1, size.Y() - 1, size.Z() - 1));
  bool bSlabEnabled(GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB);
  float8 fSlabThickness((bSlabEnabled && (i==1)) ? GetEnvironment().GetRenderingMode()->GetSlabThickness() : 0.0);

  float fDelta(0.01F); // delta to prevent aliasing due to z-buffer resolution problem, not needed if depth test is off!

  //glBegin(GL_QUADS);
  //{
  //  // Coordinate with non-negative value determines direction and position. This is by (old)definition.
  //  // (note, we need to define a plane for the 3D cursor!).
  //  vxSliceShadowDataOrtho * pOrthoData = dynamic_cast< vxSliceShadowDataOrtho * >( GetEnvironment().GetSliceShadow()->GetSliceShadowData() );
  //  if ( pOrthoData == NULL )
  //    throw ex::RuntimeException( LogRec( "Not an orthographic projection", "vxRendererTexMap2D", "GenCursor3dGLlist" ) );
  //  uint1 uIndex(pOrthoData->GetPlaneOrtho().GetNormal().GetMaxAbsComponentCoord());
  //  float8 fPlaneNormDist(pOrthoData->GetPlaneOrtho().GetDistance() + fSlabThickness);

  //  switch(uIndex)
  //  {
  //  case 0:
  //    {
  //      glColor4f(0,1,0,0.5);
  //      glVertex3f(fPlaneNormDist,            -fMax,            -fMax);
  //      glVertex3f(fPlaneNormDist, volSize.m_y+fMax,            -fMax);
  //      glVertex3f(fPlaneNormDist, volSize.m_y+fMax, volSize.m_z+fMax);
  //      glVertex3f(fPlaneNormDist,            -fMax, volSize.m_z+fMax);
  //      rCursor3dPlane.SetNormal(Normal<float8>(1,0,0));
  //      rCursor3dPlane.SetDistance(-fPlaneNormDist);
  //      break;
  //    }
  //  case 1:
  //    {
  //      glColor4f(0,0,1,0.5);
  //      glVertex3f(-fMax,            fPlaneNormDist,            -fMax);
  //      glVertex3f(volSize.m_x+fMax, fPlaneNormDist,            -fMax);
  //      glVertex3f(volSize.m_x+fMax, fPlaneNormDist, volSize.m_z+fMax);
  //      glVertex3f(-fMax,            fPlaneNormDist, volSize.m_z+fMax);
  //      rCursor3dPlane.SetNormal(Normal<float8>(0,1,0));
  //      rCursor3dPlane.SetDistance(-fPlaneNormDist);
  //      break;
  //    }
  //  case 2:
  //    {
  //      glColor4f(1,0,0,0.5);
  //      glVertex3f(-fMax,            -fMax,            fPlaneNormDist);
  //      glVertex3f(volSize.m_x+fMax, -fMax,            fPlaneNormDist);
  //      glVertex3f(volSize.m_x+fMax, volSize.m_y+fMax, fPlaneNormDist);
  //      glVertex3f(-fMax,            volSize.m_y+fMax, fPlaneNormDist);
  //      rCursor3dPlane.SetNormal(Normal<float8>(0,0,1));
  //      rCursor3dPlane.SetDistance(-fPlaneNormDist);
  //      break;
  //    }
  //  default:
  //    break;
  //  }    
  //}
  // 
  //glEnd();

  glEndList();

  return m_cursorPlaneId[i];
} // GenCursor3dGLlist()


/**
 * Public function to draw a single textured polygon
 * @param number of points
 * @param pointer to a 3D point
 * @param pointer to texture coordinates
 * @param pointer to texture coordinates
 */
void vxRendererTexMap::GLDrawTexPolygon(const uint1 & uNumberOfPoints,
                                        Point3D<float8> * pPoints,
                                        Point3D<float8> * pTexCoords,
                                        Point3D<float8> * pTexCoordsNext)
//*******************************************************************
{
  // actual rendering
  glBegin(GL_TRIANGLE_FAN);
  {
    for (int i(0); i<uNumberOfPoints ; i++)
    {
      glMultiTexCoord3fARB(GL_TEXTURE0_ARB, pTexCoordsNext[i][0], pTexCoordsNext[i][1], pTexCoordsNext[i][2]);
      glMultiTexCoord3fARB(GL_TEXTURE1_ARB, pTexCoords[i][0], pTexCoords[i][1], pTexCoords[i][2]);
      glMultiTexCoord3fARB(GL_TEXTURE2_ARB, 1.0, 0.0, 0.0);
      glMultiTexCoord3fARB(GL_TEXTURE3_ARB, 1.0, 0.0, 0.0);
      glVertex3f(pPoints[i][0], pPoints[i][1], pPoints[i][2]);
    }
  }
  glEnd();

  return;
} // GLDrawPolygon()


/**
 * Draw a single textured plane with tag option
 * @param uNumberOfPoints Number of corners of polygon
 * @param pPoints Pointer to corner points of polygon
 * @param pTexCoords Pointer to corner points of texture
 * @param bTags Flag indicating if tags are on
 */
void vxRendererTexMap::GLDrawTexPolygonTags(uint1 & uNumberOfPoints,
                                            Point3D<float8> * pPoints,
                                            Point3D<float8> * pTexCoords,
                                            bool bTags)
//*******************************************************************
{
  glBegin(GL_TRIANGLE_FAN);
  {
    for (int i(0); i<uNumberOfPoints; i++)
    {
      glMultiTexCoord3fARB(GL_TEXTURE0_ARB, pTexCoords[i][0], pTexCoords[i][1], pTexCoords[i][2]);
      if (bTags == true)
      {
        glMultiTexCoord3fARB(GL_TEXTURE2_ARB, pTexCoords[i][0], pTexCoords[i][1], pTexCoords[i][2]);
      }
      glVertex3f(pPoints[i][0], pPoints[i][1], pPoints[i][2]);
    }
  }
  glEnd();
} // GLDrawTexPolygonTags()



/**
 * Draw text showing that we are busy generating textures
 */
void vxRendererTexMap::RenderBusyText()
//*******************************************************************
{
  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  // new settings
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  Rect<float4> viewPortRect(0.0F, 0.0F, 
                            GetEnvironment().GetWinRect().GetViewportSize().first, 
                            GetEnvironment().GetWinRect().GetViewportSize().second);
  gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  Triple<float4> color(1.0F, 0.0F, 0.0F);
  GLFontGeorge * pFont(GetEnvironment().GetFont());
  pFont->PutFormattedString("Generating display ...", 
                            GLFontGeorge::ALIGN_LEFT,
                            Point2D<int4>(static_cast<uint2>(viewPortRect.m_left + 0.5 * viewPortRect.Width() - 50),
                                          static_cast<uint2>(viewPortRect.m_top  + 0.5 * viewPortRect.Height()) + 10),
                            color);
  pFont->PutFormattedString("... please be patient.", 
                            GLFontGeorge::ALIGN_LEFT,
                            Point2D<int4>(static_cast<uint2>(viewPortRect.m_left + 0.5 * viewPortRect.Width() - 50),
                                          static_cast<uint2>(viewPortRect.m_top + 0.5 * viewPortRect.Height() - 10)),
                            color);

  // restore the OpenGL state
  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();  

  return;
} // RenderBusyText()


#undef FILE_REVISION


// Revision History:
// $Log: vxRendererTexMap.C,v $
// Revision 1.11  2007/02/13 22:33:41  romy
// We need to convert all modality values to RAW before the segmentation. Issue #5382
//
// Revision 1.10  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.9  2006/07/06 13:29:08  romy
// fopen to fopen_s Migration
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.1  2005/07/19 11:50:33  frank
// missing include
//
// Revision 1.6  2005/05/18 15:06:49  frank
// cleaning up slice shadows
//
// Revision 1.5  2004/10/05 14:35:05  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.4  2004/09/01 12:59:50  frank
// decreased the granularity of component array modified bits
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
// Revision 1.64  2004/01/30 01:21:35  geconomos
// added a check for WIN_RECT when deciding if rendering is necessary.
//
// Revision 1.63  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.62  2003/11/05 01:18:49  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.61  2003/09/02 14:52:49  frank
// Changed the way slice shadows are determined - made it more flexible
//
// Revision 1.60  2003/06/18 15:19:59  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.59  2003/06/09 14:57:45  michael
// made shifting dependent on used bits instead if hardcoding 12 bits and shifting by 4
//
// Revision 1.58  2003/06/09 12:30:23  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.57  2003/06/04 18:23:18  michael
// added namespace declaration
//
// Revision 1.56  2003/05/21 12:12:31  frank
// Revised the slice shadow to more specific about the current slice shadow
//
// Revision 1.55  2003/05/17 15:44:51  michael
// reformatting
//
// Revision 1.54  2003/05/16 15:34:07  dongqing
// code review
//
// Revision 1.53  2003/05/16 13:58:18  michael
// code review
//
// Revision 1.52  2003/05/16 13:22:15  frank
// formatting
//
// Revision 1.51  2003/05/16 11:45:27  michael
// code review
//
// Revision 1.50  2003/05/15 20:10:41  frank
// Moving away from old point class
//
// Revision 1.49  2003/05/12 16:32:59  michael
// cosmetics
//
// Revision 1.48  2003/04/14 21:08:57  michael
// cosmetics
//
// Revision 1.47  2003/02/28 21:43:01  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.46  2003/01/22 22:03:31  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.45  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.44  2002/12/06 19:20:38  ingmar
// vxRenderingMode::SHOW_LABEL -> SHOW_COMPONENT_IN_MPR
//
// Revision 1.43  2002/11/25 22:33:52  michael
// Fixed 3x3 and 4x4 renderer to show up again and correctly display/belnd out components
//
// Revision 1.42  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.41  2002/11/11 19:31:39  michael
// more fixing of texture map server-less rendering
//
// Revision 1.40  2002/11/05 22:51:22  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.39  2002/09/25 20:36:42  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.38  2002/09/19 14:26:02  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.37  2002/09/09 22:48:18  michael
// fixed bug in indexing std::vector<vxComponent> where index was out of range
// and caused redraw error.
//
// Revision 1.36  2002/09/05 22:37:47  jaddonisio
// New vxComponent.
//
// Revision 1.35  2002/07/30 20:22:17  dmitry
// bunch of stuff changed to vx
//
// Revision 1.34  2002/06/21 21:20:45  michael
// fixed rendering of every second slice ... while moving camera
//
// Revision 1.33  2002/06/13 22:23:07  jaddonisio
// Changed name on Get/Set region thresholds.
//
// Revision 1.32  2002/06/05 15:17:11  michael
// added depth buffer test so that annotations are visible in 3D
//
// Revision 1.31  2002/05/23 14:48:56  michael
// final move to VC color: XYZ (SCA) --> GBR
//
// Revision 1.30  2002/05/17 14:57:48  manju
// Moved the call to GetSliceDistance out of the loop.
//
// Revision 1.29  2002/05/16 04:27:30  michael
// TexMapServer returns NULL if tile or tile vector is not (yet) available. Adjusted
// texture map renderer to display information when texture generation is in progress.
//
// Revision 1.28  2002/05/13 20:55:34  dmitry
// Enum naming changed in RenderingMode.
//
// Revision 1.27  2002/05/13 20:16:33  michael
// Display busyText while texture generation
//
// Revision 1.26  2002/05/06 15:34:01  jaddonisio
// Fixed constant type to fix compiler warning.
//
// Revision 1.25  2002/05/06 09:06:35  michael
// fixed removing of intensity values from 3D rendering
//
// Revision 1.24  2002/05/04 10:41:20  michael
// Disallow full blue color for components. Blue is used to filter associated intensity not to be displayed
//
// Revision 1.23  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.22  2002/05/01 19:42:30  jenny
// getting rid of volumeset
//
// Revision 1.21  2002/05/01 00:01:00  michael
// Took out crop box (temporarily) but will be revisited ...
//
// Revision 1.20  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.19  2002/04/25 19:53:58  jenny
// got rid of Cursor3d
//
// Revision 1.18  2002/04/25 01:39:48  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 1.17  2002/04/24 15:10:22  jenny
// vxBlockVolume stuff
//
// Revision 1.16  2002/04/23 20:12:22  michael
// replaced Density by Intensity
//
// Revision 1.15  2002/04/23 15:56:35  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.14  2002/04/14 00:20:23  michael
// moved GetSliceDistanceFirst to TexMap3D renderer and made GetSliceDistance()
// a virtual function such that it can be overloaded. E.g., 2DTexMap has view
// dependent slice distance ...
//
// Revision 1.13  2002/04/08 18:04:02  jaddonisio
// Corrected render mode enums to all caps.
//
// Revision 1.12  2002/04/08 17:43:02  michael
// corrected checking when intensity range texture needs to be recomputed
//
// Revision 1.11  2002/04/06 00:17:38  michael
// fixed sample intensity range
//
// Revision 1.10  2002/04/04 00:53:31  jaddonisio
// Prerender() fixes.
//
// Revision 1.9  2002/04/02 21:51:32  geconomos
// Updated to use vxServerTexMap2d in lieu of ServerTexMap2d.
//
// Revision 1.8  2002/04/01 21:24:11  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.7  2002/04/01 16:30:25  dmitry
// Changed to use vxRenderingSettings.
//
// Revision 1.6  2002/03/28 23:07:13  jaddonisio
// Changed "TAG" to "LABEL" in enum.
//
// Revision 1.5  2002/03/22 15:57:57  jaddonisio
// New functions for Intensity Range texture, and a dummy texture combiner.
//
// Revision 1.4  2002/03/20 16:45:33  michael
// Added PostRender to clean up used gl stuff
//
// Revision 1.3  2002/03/18 23:24:41  jaddonisio
// New setup and disable functions for GL texture combiners.
//
// Revision 1.2  2002/03/14 22:48:11  jaddonisio
// cleaned-up dead code
//
// Revision 1.1  2002/03/12 23:23:22  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererTexMap.C,v 1.11 2007/02/13 22:33:41 romy Exp $
// $Id: vxRendererTexMap.C,v 1.11 2007/02/13 22:33:41 romy Exp $
