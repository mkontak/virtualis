// $Id: GLEnv.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

// includes
#include "stdafx.h"
#define GLH_EXT_SINGLE_FILE
#include "GLEnv.h"
#include "Enums.h"




// static member initializations
uint4 GLEnv::m_uColorBits = 8;
bool GLEnv::m_bExtMultiTexture = false;
uint4 GLEnv::m_uNumTextureUnits = 1;
bool GLEnv::m_bExtTextureCompression = false;
bool GLEnv::m_bExtTextureCompressionS3tc = false;
bool GLEnv::m_bExtSharedTexturePalette = false;
bool GLEnv::m_bExtRegisterCombiners = false;
bool GLEnv::m_bExtRegisterCombiners2 = false;
bool GLEnv::m_bInitializedGlobally = false;
bool GLEnv::m_bExtTextureBorderClamp = false;
bool GLEnv::m_bExtTexture3D = false;
bool GLEnv::m_bExtTextureShader = false;
bool GLEnv::m_bExtVertexShader = false;
uint4 GLEnv::m_uMaxGeneralCombiners = 0;
GLfloat GLEnv::m_white[] = {1.0,1.0,1.0,1.0};
GLfloat GLEnv::m_black[] = {0.0,0.0,0.0,0.0};
std::vector<float> GLEnv::m_vfInvTable;
float GLEnv::m_fScaleInvTable;


/**
 * Retrieve error status from OpenGL
 * context is valid
 */
bool GLEnv::Error(int& code)
//*******************************************************************
{
  code = glGetError();
  return(code != GL_NO_ERROR);
}


/**
 * Initialization function. Should be called when the current OpenGL
 * context is valid
 */
void GLEnv::Init()
//*******************************************************************
{
	if (m_bInitializedGlobally == false)
  {
		InitExtensions();
		GetGLConstants();
		m_bInitializedGlobally = true;
	}

	//Enable multitexture if it is available
	m_bOptMultiTexture = m_bExtMultiTexture;

} // Init()


/**
 * Set OpenGL to a set of default states
 */
void GLEnv::SetDefaultState()
//*******************************************************************
{
  glDisable(GL_LIGHTING);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_1D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
} // SetDefaultState()


/**
 * Set OpenGL to a set of default states
 */
void GLEnv::SetSize(uint4 uWidth, uint4 uHeight)
//*******************************************************************
{
  m_uWidth  = uWidth;
  m_uHeight = uHeight;
}


/**
 * Obtain several OpenGL hardware constants
 */
void GLEnv::GetGLConstants()
//*******************************************************************
{
  int4 iValue;

  // multi texture
  if (ExtensionSupported("GL_ARB_multitexture"))
  {
    m_bExtMultiTexture = true;

    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iValue);
    m_uNumTextureUnits = iValue;
  }

  if (ExtensionSupported("GL_EXT_shared_texture_palette"))
  {
    m_bExtSharedTexturePalette = true;
	}						

	if (ExtensionSupported("GL_NV_register_combiners"))
  {
		m_bExtRegisterCombiners = true;

    glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV, &iValue);
    m_uMaxGeneralCombiners = iValue;
	}

	if (ExtensionSupported("GL_NV_register_combiners2"))
  {
		m_bExtRegisterCombiners2 = true;
	}
	
	if (ExtensionSupported("GL_ARB_texture_compression"))
  {
		m_bExtTextureCompression = true;
	}

	if (ExtensionSupported("GL_EXT_texture_compression_s3tc"))
  {
		m_bExtTextureCompressionS3tc = true;
	}
	
  if (ExtensionSupported("GL_ARB_texture_border_clamp"))
  {
    m_bExtTextureBorderClamp = true;
  }

  if (ExtensionSupported("GL_EXT_texture3D"))
  {
    m_bExtTexture3D = true;
  }

	if (ExtensionSupported("GL_NV_texture_shader"))
  {
		m_bExtTextureShader = true;
	}

	if (ExtensionSupported("GL_NV_vertex_program"))
  {
		m_bExtVertexShader = true;
	}

  glGetIntegerv(GL_GREEN_BITS, &iValue);
  m_uColorBits = iValue;
} // GetGLConstants


/**
 * Init OpenGL extension specified by str
 */
bool GLEnv::InitExtensions(const char* str)
//*******************************************************************
{
  if (glh_init_extensions(str) == false)
		return false;
	return true;
}


/**
 * Disable all texture units
 */
void GLEnv::DisableTextureUnits()
//*******************************************************************
{
	for (int j(1); j<GLEnv::m_uNumTextureUnits; j++)
  {
	  glActiveTextureARB(GL_TEXTURE0_ARB+j);
	  glBindTexture(GL_TEXTURE_2D,0);
	  glDisable(GL_TEXTURE_2D);
	}

  glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D,0);
	glDisable(GL_TEXTURE_2D);
} // DisableTextureUnits()


/**
 * Enable all texture units
 */
void GLEnv::EnableTextureUnits()
//*******************************************************************
{
  for (int j(1); j<GLEnv::m_uNumTextureUnits; j++)
  {
		glActiveTextureARB(GL_TEXTURE0_ARB+j);
		glEnable(GL_TEXTURE_2D);
	}

  glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
} // EnableTextureUnits()


/**
 * Test whether an extension is supported
 * @param extension
 */
bool GLEnv::ExtensionSupported(const char *extension)
//*******************************************************************
{
  static const GLubyte * pExtensions(NULL);
  const GLubyte * pStart;
  GLubyte * pWhere, * pTerminator;

  /* Extension names should not have spaces. */
  pWhere = (GLubyte *) strchr(extension, ' ');
  if (pWhere || *extension == '\0')
    return false;

  if (!pExtensions)
  {
    pExtensions = glGetString(GL_EXTENSIONS);
  }
  // It takes a bit of care to be fool-proof about parsing the
  // OpenGL pExtensions string.  Don't be fooled by sub-strings, etc.
  pStart = pExtensions;
  while (true)
  {
    pWhere = (GLubyte *) strstr((const char *) pStart, extension);
    if (!pWhere)
      break;
    pTerminator = pWhere + strlen(extension);
    if (pWhere == pStart || *(pWhere - 1) == ' ') {
      if (*pTerminator == ' ' || *pTerminator == '\0') {
        return true;
      }
    }
    pStart = pTerminator;
  }
  return false;
} // ExtensionSupported()

// $Log: GLEnv.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/14 13:42:25  geconomos
// Coding standards.
//
// Revision 3.8  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.7  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.6  2002/01/03 19:58:25  liwei
// Added flags for Texture Shader and Vertex Shader.
//
// Revision 3.5  2001/12/13 16:27:18  liwei
// Inclusion to extension header file revisited.
//
// Revision 3.4  2001/12/13 01:14:38  liwei
// Code for initialize the OpenGL extensions cleaned.
//
// Revision 3.3  2001/12/03 22:31:14  liwei
// Code cleaned.
//
// Revision 3.2  2001/12/03 20:09:59  liwei
// Added variable for the number of general combiners.
//
// Revision 3.1  2001/11/19 22:56:18  liwei
// Minor changes.
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 11 2001 09:35:14   liwei
// Added a boolean variable for the availability of 3D texture.
// 
//    Rev 2.1   Sep 20 2001 13:09:06   liwei
// Added boolean flags for texture_clamp_to_border.
// 
//    Rev 2.0   Sep 16 2001 23:49:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:30   ingmar
// Initial revision.
// Revision 1.4  2001/07/25 19:06:01  liwei
// Creating a texture now uses default parameters of INTERP_LINEAR, compression = true.
//
// Revision 1.3  2001/07/24 18:22:35  liwei
// glut removed from the project.
//
// Revision 1.2  2001/07/02 20:34:40  liwei
// Revised for the compliance of the coding standard
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/25 17:23:49  liwei
// *.cpp files renamed to *.C files
//
// Revision 1.3  2001/06/13 16:26:02  liwei
// no message
//
// Revision 1.2  2001/06/12 22:48:11  liwei
// First time commited to CVS
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/GLEnv.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: GLEnv.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
