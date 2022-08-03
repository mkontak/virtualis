// $Id: DrawOverView.h,v 1.6.2.1 2010/03/10 00:42:01 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (mailto:george@viatronix.com)

// includes
#include "OverView.h"
#include "Segment3DChildView.h"
#include "Dib.h"

namespace ColonViews
{
  
  namespace ChildViews
  {

    /// Instance for overview image, e.g. for quick localized drawing
    class DrawOverView : public OverView
    {
      // member functions
    public:

      /// constructor
      DrawOverView( CWnd * pWnd, const ReaderLib::Dataset& studyref, const Rect<int4>& viewRect = Rect<int4>( 0, 0, 512, 512 ) );

      /// destructor
      virtual ~DrawOverView();

      /// overriden to return the hdc associated with the dib
      virtual HDC GetHDC() const { return m_dib.GetHDC(); }

      // overidden to retrieve the image data from the contained dib rather than gl buffer
      virtual const void * GetUpdateBuffer( int4 & iWidth, 
                                              int4 & iHeight,
                                              int4 & iWidthAlignedDWORD,
                                              const GLenum ePixelFormat = GL_BGR_EXT,    
                                              const GLenum eDatatype = GL_UNSIGNED_BYTE, 
                                              const bool bOnlyGLRenderScene = false );
    protected:

      /// creates the rendering context
      virtual bool GLCreate();




    // member variables
    private:

      /// previous trackball for reset in destructor
      //ReaderLib::TrackBall m_prevTrackball;

      /// offscreen dib rendering context is created from
      WinUtils::Dib m_dib;
    };

    /// Instance for segment 3d image, e.g. for quick localized drawing
    class DrawSegmentView : public Segment3DChildView
    {
      // member functions
    public:

      /// constructor
      DrawSegmentView( CWnd * pWnd, const ReaderLib::Dataset& studyref, const Rect<int4>& viewRect = Rect<int4>( 0, 0, 1024, 512 ) );

      /// destructor
      virtual ~DrawSegmentView();

      /// overriden to return the hdc associated with the dib
      virtual HDC GetHDC() const { return m_dib.GetHDC(); }

      /// overriden to skip operations dealing with other views
      virtual void GLResize(GLsizei iWidth, GLsizei iHeight);

      // overidden to retrieve the image data from the contained dib rather than gl buffer
      virtual const void * GetUpdateBuffer( int4 & iWidth, 
                                              int4 & iHeight,
                                              int4 & iWidthAlignedDWORD,
                                              const GLenum ePixelFormat = GL_BGR_EXT,    
                                              const GLenum eDatatype = GL_UNSIGNED_BYTE, 
                                              const bool bOnlyGLRenderScene = false );
    protected:

      /// creates the rendering context
      virtual bool GLCreate();




    // member variables
    private:

      /// offscreen dib rendering context is created from
      WinUtils::Dib m_dib;
    };

  } // namespace ChildViews

} // namespace ColonViews

// $Log: DrawOverView.h,v $
// Revision 1.6.2.1  2010/03/10 00:42:01  dongqing
// localized version of segments 3d view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2007/03/01 21:40:08  geconomos
// code review preparation
//
// Revision 1.5  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.4.2.1  2005/11/17 16:30:42  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.4  2005/09/23 20:10:22  vxconfig
// Merged from 1-6-1 branch to the trunk after release
//
// Revision 1.3.2.1  2005/09/21 13:28:47  geconomos
// modified DrawImage to implicitly use an image size of 512
//
// Revision 1.3  2005/08/16 22:02:28  jmeade
// namespaces for WinUtils.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2005/06/21 15:12:53  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/DrawOverView.h,v 1.6.2.1 2010/03/10 00:42:01 dongqing Exp $
// $Id: DrawOverView.h,v 1.6.2.1 2010/03/10 00:42:01 dongqing Exp $
