#include "Image.h"
inline float4 Image::Interpolate(const float4 & a,
								const float4 & b,
								const float4 & c,
								const float4 & d,
								const float4 & f)
{
	return (d + (b-a) / (c-a) * (f-d));
}


/**
 *  gets RGB values for given pixel
 *
 * @param   position        pixel location
 * @return                  S_OK upon success
 */
inline Vector<float4> Image::GetPixelSafe(const Point2D<uint4> & position) const
{
  Point2D<uint4> pos(position);
  pos.m_x = Bound(uint4(0), pos.m_x, uint4(m_iWidth -1));
  pos.m_y = Bound(uint4(0), pos.m_y, uint4(m_iHeight-1));
  const unsigned char * pPixel = (unsigned char *)m_ppvBits + 3 * (m_iWidth * pos.m_y + pos.m_x);
  
  return Vector<float4>(*(pPixel+0), *(pPixel+1), *(pPixel+2));
}


/**
 *  gets RGB values for given pixel (bound)
 *
 * @param   position        pixel location
 * @return                  S_OK upon success
 */
inline Vector<float4> Image::GetPixelBound(const Point2D<uint4> & position) const
{
  Point2D<uint4> pos(position);
  pos.m_x = Bound(uint4(0), pos.m_x, uint4(m_iWidth -2));
  pos.m_y = Bound(uint4(0), pos.m_y, uint4(m_iHeight-2));
  
  const unsigned char * pPixel = (unsigned char *)m_ppvBits + 3 * (m_iWidth * pos.m_y + pos.m_x);
  
  return Vector<float4>(*(pPixel+0), *(pPixel+1), *(pPixel+2));
}


/**
 *  resample at given position
 *
 * @param   position        pixel location
 * @return                  S_OK upon success
 */
inline Vector<float4> Image::Resample(const Point2D<float4> & position) const
{
  // clamp to image
  Point2D<float4> pos(position);
  pos.m_x = Bound(0.0F, pos.m_x, float4(m_iWidth)-2.0F);
  pos.m_y = Bound(0.0F, pos.m_y, float4(m_iHeight)-2.0F);
  
  // create weights
  uint4 x = (uint4) pos.m_x;
  uint4 y = (uint4) pos.m_y;
  const float4 wx1 = pos.m_x - x;
  const float4 wy1 = pos.m_y - y;
  const float4 wx0 = 1.0F - wx1;
  const float4 wy0 = 1.0F - wy1;
  const float4 wll = wx0 * wy0;
  const float4 wlr = wx1 * wy0;
  const float4 wul = wx0 * wy1;
  const float4 wur = wx1 * wy1;
  
  // get the four corner pixels
  Vector<float4> ll(GetPixelSafe(Point2D<uint4>(x+0,y+0)));
  Vector<float4> lr(GetPixelSafe(Point2D<uint4>(x+1,y+0)));
  Vector<float4> ul(GetPixelSafe(Point2D<uint4>(x+0,y+1)));
  Vector<float4> ur(GetPixelSafe(Point2D<uint4>(x+1,y+1)));
  
  return ll * wll + lr * wlr + ul * wul + ur * wur;
}


/**
 *  aligns the bitmap width to a WORD boundary
 *
 * @param   iWidth          image width
 * @param   iBpp            bits per pixel
 * @return                  S_OK upon success
 */
inline int4 Image::BytesPerLine(int4 iWidth, int4 iBpp)
{
  int4 iBytesPerLine = iWidth * iBpp;
  iBytesPerLine = ( (iBytesPerLine + 31) & (~31) ) / 8;
  
  return iBytesPerLine;
}
