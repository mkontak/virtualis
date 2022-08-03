#ifndef vxWindowHandle_h
#define vxWindowHandle_h

#include "vxElement.h"

class vxWindowHandle : public vxElement
{
public:
  
  vxWindowHandle( )
  {
    m_hWnd = 0;
  }
  
  vxWindowHandle( HWND hwnd )
  {
    m_hWnd = hwnd;
  }

  HWND GetHandle() { return m_hWnd; }

  void SetHandle( HWND hwnd ) { m_hWnd = hwnd; }

private:
  
  HWND m_hWnd;
};

#endif