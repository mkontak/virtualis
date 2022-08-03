// $Id: glExtensionStr.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

#ifndef GL_EXTENSION_STR_H
#define GL_EXTENSION_STR_H

const unsigned int MaxExtensionStrLength = 50;
const char glExtensionStr[] ={
							"GL_ARB_multitexture "
							"GL_ARB_texture_border_clamp "
							"GL_ARB_texture_compression "
							"GL_ARB_texture_cube_map "
							"GL_ARB_transpose_matrix "
							"GL_EXT_abgr "
							"GL_EXT_bgra "
							"GL_EXT_blend_color "
							"GL_EXT_blend_minmax "
							"GL_EXT_blend_subtract "
							"GL_EXT_compiled_vertex_array "
							"GL_EXT_fog_coord "
							"GL_EXT_light_max_exponent "
							"GL_EXT_point_parameters "
							"GL_EXT_paletted_texture "
							"GL_EXT_rescale_normal "
							"GL_EXT_secondary_color "
							"GL_EXT_separate_specular_color "
							"GL_EXT_shared_texture_palette "
							"GL_EXT_stencil_wrap "
							"GL_EXT_texture_compression_s3tc "
							"GL_EXT_texture_cube_map "
							"GL_EXT_texture_edge_clamp "
							"GL_EXT_texture_env_add "
							"GL_EXT_texture_env_combine "
							"GL_EXT_texture_filter_anisotropic "
							"GL_EXT_texture_lod_bias "
							"GL_EXT_texture_object "
							"GL_EXT_vertex_array "
							"GL_EXT_vertex_weighting "
							"GL_NV_blend_square "
							"GL_NV_fence "
							"GL_NV_fog_distance "
							"GL_NV_packed_depth_stencil "
							"GL_NV_register_combiners "
							"GL_NV_register_combiners2 "
							"GL_NV_texgen_reflection "
							"GL_NV_texture_env_combine4 "
							"GL_NV_texture_rectangle "
							"GL_NV_texture_shader "
							"GL_NV_vertex_array_range "
							"GL_NV_vertex_program "
							"GL_SGIS_texture_lod "
							"GL_SGIX_depth_texture "
							"GL_SGIX_shadow "
							"GL_VERSION_1_2 " 
							"GL_WIN_swap_hint "
							"WGL_ARB_pbuffer "
							"WGL_ARB_pixel_format "
              "GL_EXT_texture3D ",
};


#endif // GL_EXTENSION_STR_H


// $Log: glExtensionStr.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/05/08 16:39:05  michael
// more code reviews (75 renderer Utils)
//
// Revision 3.1  2001/12/15 00:49:07  liwei
// Added 3D texture extension string.
//
// Revision 3.0  2001/10/14 23:02:32  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:56   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:32   ingmar
// Initial revision.
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/13 14:31:07  liwei
// Added to CVS
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/glExtensionStr.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: glExtensionStr.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $