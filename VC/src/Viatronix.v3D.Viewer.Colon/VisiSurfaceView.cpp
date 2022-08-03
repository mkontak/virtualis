// VisiSurfaceView.cpp: implementation of the CVisiSurfaceView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisiSurfaceView.h"

#include "ReaderGlobal.h"
#include "glFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisiSurfaceView::CVisiSurfaceView(HWND hWndParent) : GLChildView(eGVT_VISIBLE_SURFACE, hWndParent)
{
  m_zoomWidth = 512;
  m_zoomPos = m_zoomWidth/2;
  m_backTextureCreated = false;
}

CVisiSurfaceView::~CVisiSurfaceView()
{

}

void CVisiSurfaceView::GLRenderScene(void *data)
{
	if (!rdrGlobal.m_pStudy->IsVolumeLoaded())
  {
		GLChildView::GLRenderScene(data);
		return;
	}

  int csi = rdrGlobal.m_pStudy->m_fields.GetClosestSkeletonIndex();
  Skeleton * pSkeleton = rdrGlobal.m_pStudy->m_segmentList.GetCurrentSkeleton();
  if (pSkeleton == NULL) 
  {
    // Error::WarnLog("No current skeleton!");
    ex::NullPointerException(LogRecord(LogRecord::SL_EXCEPTION, Date::GetCurrentDate(),
      "VisiSurfaceView.cpp", "$Revision: 1.1 $", __LINE__, "CVisiSurfaceView", "GLRenderScene", 
      "No current skeleton!"));
    return;
  }
  m_zoomPos = float(csi)/float(pSkeleton->GetNumOfElements())*rdrGlobal.m_pMapHeight;
  float viewLinePos = 0.5;
  if(m_zoomPos<m_zoomWidth/2) viewLinePos = float(m_zoomPos)/float(m_zoomWidth);
  if(m_zoomPos>rdrGlobal.m_pMapHeight-m_zoomWidth/2) viewLinePos = m_zoomPos/(rdrGlobal.m_pMapHeight-m_zoomWidth);

  if(m_zoomPos<m_zoomWidth/2) m_zoomPos = m_zoomWidth/2;
  if(m_zoomPos>rdrGlobal.m_pMapHeight-m_zoomWidth/2) m_zoomPos = rdrGlobal.m_pMapHeight-m_zoomWidth/2;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // Draw the unwrapped colon
  if (rdrGlobal.m_pMap != NULL) {
    
    const int greenMapPixelSize = rdrGlobal.m_greenMapPixelSize;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); {
      glLoadIdentity();
      
      glMatrixMode(GL_PROJECTION);
      glPushMatrix(); {
        glLoadIdentity();
        gluOrtho2D(0, 1, 0, 1);
 
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, rdrGlobal.m_pMapWidth);

        void * pSubImage = (void *)(rdrGlobal.m_pMap +
                                    (m_zoomPos-m_zoomWidth/2)*rdrGlobal.m_pMapWidth*3);
        void * pMarkedSubImage = (void *)(rdrGlobal.m_pGreenMap +
                                    (m_zoomPos-m_zoomWidth/2)*rdrGlobal.m_pMapWidth*greenMapPixelSize);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        glEnable(GL_TEXTURE_2D);

       glTexImage2D (GL_TEXTURE_2D, 0, 3, rdrGlobal.m_pMapWidth, m_zoomWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, pSubImage);


			 glBegin(GL_QUADS); {
          glTexCoord2d(0,0); glVertex2i(0, 1);
          glTexCoord2d(1,0); glVertex2i(0, 0);
          glTexCoord2d(1,1); glVertex2i(1, 0);
          glTexCoord2d(0,1); glVertex2i(1, 1);
        } glEnd();

				glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
				if(greenMapPixelSize == 4){
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glTexImage2D (GL_TEXTURE_2D, 0, greenMapPixelSize, rdrGlobal.m_pMapWidth, m_zoomWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, pMarkedSubImage);
				}else if(greenMapPixelSize == 1){
					glBlendFunc(GL_ONE, GL_ONE);
					glTexImage2D (GL_TEXTURE_2D, 0, greenMapPixelSize, rdrGlobal.m_pMapWidth, m_zoomWidth, 0, GL_GREEN, GL_UNSIGNED_BYTE, pMarkedSubImage);
				}	
				glBegin(GL_QUADS); {
          glTexCoord2d(0,0); glVertex2i(0, 1);
          glTexCoord2d(1,0); glVertex2i(0, 0);
          glTexCoord2d(1,1); glVertex2i(1, 0);
          glTexCoord2d(0,1); glVertex2i(1, 1);
        } glEnd();
 
        glDisable(GL_TEXTURE_2D);

        glColor3f(0.0,0.0,0.0);
        glLineWidth(3.0);
        glBegin(GL_LINES); {
          glVertex2f(viewLinePos, 0.0);
          glVertex2f(viewLinePos, 1.0);
        } glEnd();
        glLineWidth(1.0);
        
      } glPopMatrix();
      
      glMatrixMode(GL_MODELVIEW);
    } glPopMatrix();
  } // endif draw unwrapped colon
    
}

void CVisiSurfaceView::GLRasterScene(HDC hDC)
{
	// TODO: Replace this line with the necessary code
  GLChildView::GLRasterScene(hDC);
}


void CVisiSurfaceView::GLSetupRC(void *data)
{
	// TODO: Replace this line with the necessary setup code
	GLChildView::GLSetupRC(data);
}

void CVisiSurfaceView::GLResize(GLsizei width, GLsizei height)
{
	// TODO: Replace this line with the necessary resizing code
	GLChildView::GLResize(width, height);
}

void CVisiSurfaceView::OnLButtonDown(UINT nFlags, CPoint point) 
{
  
  CRect rect;
  GetClientRect(&rect);
  
  Skeleton * pSkeleton = rdrGlobal.m_pStudy->m_segmentList.GetCurrentSkeleton();
  float percentAlongWindow = float(point.x) / float(rect.Width());
  float percentAlongSkel = percentAlongWindow * float(m_zoomWidth) / float(rdrGlobal.m_pMapHeight);
  unsigned int csi = percentAlongSkel * float(pSkeleton->GetNumOfElements());

  rdrGlobal.m_pStudy->m_submarine.SetPositionWorld(pSkeleton->GetSkelNode(csi).m_vWorldPos);
  rdrGlobal.m_pStudy->m_submarine.SetUpDirection(Vector<float>(0.0,1.0,0.0));

  float angle = (float(point.y)/float(rect.Height()) * 2 - 1) * 3.14159;
  Matrix<float> m;
  m.Rotate(pSkeleton->GetSkelNode(csi).m_vWorldTan, angle);
  Vector<float> vDir = m * pSkeleton->GetSkelNode(csi).m_vWorldUp;
  rdrGlobal.m_pStudy->m_submarine.SetOrientation(vDir);
  rdrGlobal.m_pStudy->m_volume.FlushAccumulationBuffer();

  rdrGlobal.m_pStudy->m_submarine.EnforceOrthogonality();
  rdrGlobal.m_pStudy->m_submarine.UpdatePosition();

}

void CVisiSurfaceView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  switch (nChar) {
    case VK_LEFT:
      if (GetAsyncKeyState(VK_CONTROL)&0x8000) {
        m_zoomPos-=10;
      } else {
        m_zoomPos--;
      }
      break;
    case VK_RIGHT:
      if (GetAsyncKeyState(VK_CONTROL)&0x8000) {
        m_zoomPos+=10;
      } else {
        m_zoomPos++;
      }
      break;
    default:
      break;
  }
  if(m_zoomPos<m_zoomWidth/2) m_zoomPos = m_zoomWidth/2;
  if(m_zoomPos>rdrGlobal.m_pMapHeight-m_zoomWidth/2) m_zoomPos = rdrGlobal.m_pMapHeight-m_zoomWidth/2;
  
  Invalidate(FALSE);
  GLChildView::OnKeyDown(nChar, nRepCnt, nFlags);
}

bool CVisiSurfaceView::GLIdle(void *data)
{
	return true;
}
