// $Id: GLEnv.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

#ifndef GLEnv_h
#define GLEnv_h


// includes
#include <glh_extensions.h>
#include "glExtensionStr.h"


// class definition
class VX_VIEWER_DLL GLEnv 
{
// functions
public:

	/// default constructor
  GLEnv() {};

	/// destructor
  virtual ~GLEnv() {};

	/// Initialization function
	void Init();

	/// set to default state of OpenGL
	virtual void SetDefaultState();

	/// set window size
	void SetSize(uint4 w, uint4 h);

  /// initialize OpenGL for orthogonal 2D operations
  void InitOpenGL2DOrtho();

  /// init inverse table
  static void InitInverseTable();

	/// retrieve the OpenGL error code 
  static bool Error(int& code);

	/// Disable all texture units
	static void DisableTextureUnits();

	/// enable all texture units
	static void EnableTextureUnits();

	/// returns values of some OpenGL constants
	static void GetGLConstants();

	/// initialize OpenGL extensions specified by str
	static bool InitExtensions(const char* str = (const char*)glExtensionStr);

private:

	/// test whether an extension is supported
	static bool GLEnv::ExtensionSupported(const char *extension);

// data
public:

  /// multi texture used
	bool m_bOptMultiTexture;

  /// multi texture available
	static bool m_bExtMultiTexture;

	/// texture compression available
	static bool m_bExtTextureCompression;

	/// S3's texture compression available
	static bool m_bExtTextureCompressionS3tc;

	/// shaded texture paletter available
	static bool m_bExtSharedTexturePalette;

	/// register combiners available
	static bool m_bExtRegisterCombiners;

	/// register combiners 2 available
  static bool m_bExtRegisterCombiners2;

  /// clamp to border available
  static bool m_bExtTextureBorderClamp;
  
	/// 3D texture extension available
  static bool m_bExtTexture3D;

	/// texture shader extension available
	static bool m_bExtTextureShader;

	/// vertex shader extension available
	static bool m_bExtVertexShader;
  
	/// maximum bits of each color channel
  static uint4 m_uColorBits;

	/// number of texture units available
  static uint4 m_uNumTextureUnits;

  /// max number of general combiner stages
  static uint4 m_uMaxGeneralCombiners;

  /// white color
	static GLfloat m_white[];

	/// black color
	static GLfloat m_black[];

private:

	/// whether the static members and extension function pointers have been initialized
	static bool m_bInitializedGlobally;

public:

  /// window width
  uint4 m_uWidth;

  /// window height
  uint4 m_uHeight;

  /// inverse table for computing division
  static std::vector<float> m_vfInvTable;

	/// the scale factor of the inverse table
  static float m_fScaleInvTable;

	/// OpenGL error code
	int m_iOpenGLErrorCode;
}; // GLEnv


#endif // GLEnv_h


// $Log: GLEnv.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.11  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.10  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 3.9  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 3.8  2002/01/03 19:58:25  liwei
// Added flags for Texture Shader and Vertex Shader.
//
// Revision 3.7  2001/12/28 21:57:52  ingmar
// added Sec_EnvElements to structure doxygen documentation
//
// Revision 3.6  2001/12/18 17:16:46  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.5  2001/12/17 16:50:08  soeren
// Removed some warnings
//
// Revision 3.4  2001/12/13 16:27:18  liwei
// Inclusion to extension header file revisited.
//
// Revision 3.3  2001/12/13 01:14:38  liwei
// Code for initialize the OpenGL extensions cleaned.
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
// Revision 1.2  2001/06/13 16:26:03  liwei
// no message
//
// Revision 1.1  2001/06/12 22:51:52  liwei
// Added to CVS
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/GLEnv.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: GLEnv.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $

/*
	History		:	$History: GLEnv.h $ 
 * 
 * *****************  Version 7  *****************
 * User: Liwei        Date: 5/31/01    Time: 1:31p
 * Updated in $/Vol/dataStruct
 * The bug in computing the interp in Ren2DTexView::RenderTriInterp fixed.
 * 
 * *****************  Version 6  *****************
 * User: Liwei        Date: 5/30/01    Time: 10:25a
 * Updated in $/AAA/dataStruct
 * A bug in computing the texture coordinates of bilinear interpolation
 * fixed
 * 
 * *****************  Version 5  *****************
 * User: Liwei        Date: 5/24/01    Time: 7:24p
 * Updated in $/AAA/dataStruct
 * Window Level added to VolumeRenderView
 * 
 * *****************  Version 4  *****************
 * User: Liwei        Date: 5/16/01    Time: 4:59p
 * Updated in $/AAA/dataStruct
 * 
 * *****************  Version 3  *****************
 * User: Liwei        Date: 5/14/01    Time: 6:23p
 * Updated in $/AAA/dataStruct
 * 
 * *****************  Version 2  *****************
 * User: Liwei        Date: 5/14/01    Time: 11:08a
 * Updated in $/AAA/dataStruct
 * Made compilable for lbm_hardware
 * 
 * *****************  Version 1  *****************
 * User: Liwei        Date: 5/11/01    Time: 5:11p
 * Created in $/AAA/dataStruct
 * 
 * *****************  Version 1  *****************
 * User: Liwei        Date: 4/04/01    Time: 9:30a
 * Created in $/AAA/dataStruct
*/