// $Id: Rect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeff Meade


/**
 * Constructor from left, top, right and bottom coordinates
 * @param l const T &
 * @param t const T &
 * @param r const T &
 * @param b const T &
 */
template <class T> 
Rect<T>::Rect(const T & l, const T & t, const T & r, const T & b)
{
  m_left = l;  m_top = t;  m_right = r;  m_bottom = b;
} // end of Rect<T>(l,t,r,b)


/**
 * Constructor from two Point2D
 * @param pt Point2D<T>
 * @param sz Point2D<T>
 */
template <class T> 
Rect<T>::Rect(const Point2D<T> & pt, const Point2D<T> & sz)
{
  m_left = pt.m_x;              m_top = pt.m_y;
  m_right = pt.m_x + sz.m_x;    m_bottom = pt.m_y + sz.m_y;
} // end of Rect<T>(const Point2D<T> &pt, const Point2D<T> &sz)


/**
 * Bounding Point Method
 * CenterPoint
 * @return Point2D
 */
template <class T> inline 
Point2D<T> Rect<T>::CenterPoint() const
{
  return Point2D<T>(m_left + (Width() / T(2)), m_top + (Height() / T(2)));
} // end of CenterPoint()


/**
 * Bounding Point Method
 * LeftTop
 * @return Point2D
 */
template <class T> inline 
Point2D<T> Rect<T>::LeftTop() const
{
  return Point2D<T>(m_left, m_top);
} // end of LeftTop()


/**
 * Bounding Point Method
 * RightTop
 * @return Point2D
 */
template <class T> inline 
Point2D<T> Rect<T>::RightTop() const
{
  return Point2D<T>(m_right, m_top);
} // end of RightTop()


/**
 * Bounding Point Method
 * LeftBottom
 * @return Point2D
 */
template <class T> inline 
Point2D<T> Rect<T>::LeftBottom() const
{
  return Point2D<T>(m_left, m_bottom);
} // end of LeftBottom()


/**
 * Bounding Point Method
 * RightBottom
 * @return Point2D
 */
template <class T> inline 
Point2D<T> Rect<T>::RightBottom() const
{
  return Point2D<T>(m_right, m_bottom);
} // end of RightBottom()


/**
 * Set
 * @param l const T &
 * @param t const T &
 * @param r const T &
 * @param b const T &
 */
template <class T> inline 
Rect<T> & Rect<T>::Set(const T & l, const T & t, const T & r, const T & b)
{
  m_left = l;  m_top = t;  m_right = r;  m_bottom = b;
  return *this;
} // end of Set<T>(l,t,r,b)


/**
 * Width
 * @return const T
 */
template <class T> inline 
const T Rect<T>::Width() const
{
  return m_right - m_left;
} // end of Width()


/**
 * Height
 * @return const T
 */
template <class T> inline 
const T Rect<T>::Height() const
{
  return m_bottom - m_top;
} // end of Height()


/**
 * WidthAbs
 * @return const T
 */
template <class T> inline 
const T Rect<T>::WidthAbs() const
{
  if (m_right >= m_left)
    return m_right - m_left;
  return m_left - m_right;
} // end of WidthAbs()


/**
 * HeightAbs
 * @return const T
 */
template <class T> inline 
const T Rect<T>::HeightAbs() const
{
  if (m_top >= m_bottom)
    return m_top - m_bottom;
  return m_bottom - m_top;
} // end of HeightAbs()


/** 
 * Offset 
 * @param x const T &
 * @param y const T &
 */
template <class T> inline 
Rect<T> & Rect<T>::Offset(const T & x, const T & y)
{
  m_left += x;    
  m_right += x;
  m_top += y;     
  m_bottom += y;
  return *this;
} // end of Offset(x,y)


/**
 * Offset
 * @param t const Point2D<T>
 * @return t
 */
template <class T> inline 
Rect<T> & Rect<T>::Offset(const Point2D<T> & t)
{
  return Offset(t.m_x,t.m_y);
} // end of Offset(t)


/**
 * GetOffset
 * @param x const T &
 * @param y const T &
 * @return result const Rect<T>
 */
template <class T> inline 
const Rect<T> Rect<T>::GetOffset(const T & x, const T & y) const
{
  Rect<T> result(*this);
  result.Offset(x,y);
  return result;
} // end of GetOffset(x,y)


/**
 * GetOffset
 * @param t const Point2D<T>
 * @return result const Rect<T>
 */
template <class T> inline 
const Rect<T> Rect<T>::GetOffset(const Point2D<T> & t) const
{
  Rect<T> result(*this);
  result.Offset(t);
  return result;
} // end of GetOffset(t)


/**
 * Inflate
 * @param l const T &
 * @param t const T &
 * @param r const T &
 * @param b const T &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Inflate(const T & l, const T & t, const T & r, const T & b)
{
  m_left -= l;    m_right += r;
  m_top -= t;     m_bottom += b;
  return *this;
} // end of Inflate(l,t,r,b)


/**
 * Inflate
 * @param w const T &
 * @param h const T &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Inflate(const T & w, const T & h)
{
  return Inflate(w,h, w,h);
} // end of Inflate(w,h)


/**
 * Inflate
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Inflate(const Point2D<T> & t)
{
  return Inflate(t.m_x,t.m_y);
} // end of Inflate(t)


/**
 * Inflate
 * @param r const Rect<T>
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Inflate(const Rect<T> & r)
{
  return Inflate(r.m_left,r.m_top,r.m_right,r.m_bottom);
} // end of Inflate(r)


/**
 * Deflate
 * @param l const T &
 * @param t const T &
 * @param r const T &
 * @param b const T &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Deflate(const T & l, const T & t, const T & r, const T & b)
{
  return Inflate(-l,-t,-r,-b);
}


/**
 * Deflate
 * @param w const T &
 * @param h const T &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Deflate(const T & w, const T & h)
{
  return Inflate(-w,-h);
} // end of Deflate(w,h)


/**
 * Deflate
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Deflate(const Point2D<T> & t)
{
  return Inflate(-t);
} // end of Deflate(t)


/**
 * Deflate
 * @param r const Rect<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Deflate(const Rect<T> & r)
{
  return Inflate(-r.m_left,-r.m_top,-r.m_right,-r.m_bottom);
} // end of Deflate(r)


/**
 * Unary - operator 
 * @return Rect<T>
 */
template <class T> inline
Rect<T> Rect<T>::operator-()
{
  return Rect<T>(-m_left,-m_top,-m_right,-m_bottom);
}


/**
 * Binary + operator
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> Rect<T>::operator+(const Point2D<T> & t) const
{
  return GetOffset(t);
} // end of operator+(const Point2D<T> &pt)


/**
 * Binary + operator
 * @param r const Rect<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> Rect<T>::operator+(const Rect<T> & r) const
{
  Rect<T> result(*this);
  return (result += r);
}


/**
 * Binary += operator
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::operator+=(const Point2D<T> & t)
{
  return Offset(t);
}


/**
 * Binary += operator
 * @param r const Rect<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::operator+=(const Rect<T> & r)
{
  return Inflate(r);
}


/**
 * Binary - operator
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> Rect<T>::operator-(const Point2D<T> & t) const
{
  return GetOffset(-t);
} // end of operator+(const Point2D<T> &pt)


/**
 * Binary - operator
 * @param r const Rect<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> Rect<T>::operator-(const Rect<T> & r) const
{
  Rect<T> result(*this);
  return (result -= r);
}


/**
 * Binary -= operator
 * @param t const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::operator-=(const Point2D<T> & t)
{
  return Offset(-t);
}


/**
 * Binary -= opertor
 * @param r const Rect<T> &
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::operator-=(const Rect<T> & r)
{
  return Deflate(r);
}


/**
 * Binary == operator: takes another Rect
 * @param rect the other rectangle
 * @return ==
 */
template <class T> inline 
bool Rect<T>::operator==(const Rect<T> & other) const
{
  return m_top    == other.m_top    &&
         m_bottom == other.m_bottom &&
         m_left   == other.m_left   &&
         m_right  == other.m_right;
} // operator ==()


/**
 * Binary != operator: takes another Rect
 * @param rect the other rectangle
 * @return !=
 */
template <class T> inline 
bool Rect<T>::operator !=(const Rect<T> & other) const
{
  return !(*this == other);
} // operator !=()



/**
 * Test method PtInRect
 * @param pt const Point2D<T> &
 * @return Rect<T>
 */
template <class T> inline
bool Rect<T>::PtInRect(const Point2D<T> & pt) const
{
  return PtInRect(pt.m_x, pt.m_y);
}


/**
 * Test method PtInRect
 * @param x const T &
 * @param y const T &
 * @return Rect<T>
 */
template <class T> inline
bool Rect<T>::PtInRect(const T &x, const T & y) const
{
  return ( (x >= m_left) && (x <= m_right) && 
           (y >= m_top) && (y <= m_bottom) );
} // end of PtInRect(x,y)


/**
 * Test method ConstrainPt
 * @param x T &
 * @param y T &
 * @return Rect<T>
 */
template <class T> inline
const Rect<T> & Rect<T>::ConstrainPt(T & x, T & y) const
{
  if (x < m_left)
  {
    x = m_left;   
  } // end if (x < m_left)
  else if (x > m_right)   
  {
    x = m_right;
  } // end   else if (x > m_right)    

  if (y < m_top)    
  {
    y = m_top;    
  } // end if (y < m_top)   
  else if (y > m_bottom)  
  {
    y = m_bottom;
  } // end else if (y > m_bottom) 

  return *this;
} // end of ConstrainPt(x,y)


/**
 * Normalize
 * @return Rect<T>
 */
template <class T> inline 
Rect<T> & Rect<T>::Normalize()
{
  if (m_left > m_right)   
  { 
    T tmpleft = m_left; 
    m_left = m_right; 
    m_right = tmpleft; 
  }
  if (m_top > m_bottom)   
  { 
    T tmptop = m_top; 
    m_top = m_bottom; 
    m_bottom = tmptop; 
  }
  return *this;
} // end of Normalize()


/**
 * GetNormalized
 * @return Rect<T>
 */
template <class T> inline 
const Rect<T> Rect<T>::GetNormalized() const
{
  Rect<T> result(*this);
  if (result.m_left > result.m_right)
  { 
    T tmpleft = result.m_left; 
    result.m_left = result.m_right; 
    result.m_right = tmpleft; 
  } // end  if (result.m_left > result.m_right)

  if (result.m_top > result.m_bottom)
  { 
    T tmptop = result.m_top; 
    result.m_top = result.m_bottom; 
    result.m_bottom = tmptop; 
  } // end  if (result.m_top > result.m_bottom)
  return result;
} // end of GetNormalized()


/** 
 * Compute the intersection
 */
template <class T>
const Rect<T> Rect<T>::Intersect(const Rect<T> & another) const
{
	//return Rect<T>(	max(m_left, another.m_left), max(m_top, another.m_top),
		//		  				min(m_right, another.m_right), min(m_bottom, another.m_bottom) );
  return Rect<T>(max(GetMinX(), another.GetMinX()), max(GetMinY(), another.GetMinY()),
                 min(GetMaxX(), another.GetMaxX()), min(GetMaxY(), another.GetMaxY()));
} // end of Intersect()


/**
 * OpenGL helper Draw3dEtched
 */
template <class T> inline 
void Rect<T>::Draw3dEtched() const
{
  const float4 vfWhite[3] = {1,1,1}, vfGrey[3] = {0.33f, 0.33f, 0.33f};
  Rect<float4> r(static_cast<float4>(m_left),
                 static_cast<float4>(m_top),
                 static_cast<float4>(m_right),
                 static_cast<float4>(m_bottom));

  glPushAttrib(GL_CURRENT_BIT);
  r.Inflate(1,1);
  glBegin(GL_LINE_STRIP);
  glColor3fv(vfGrey);
  glVertex2f(r.m_right, r.m_bottom);
  glVertex2f(r.m_left-1.0f, r.m_bottom);
  glVertex2f(r.m_left-1.0f, r.m_top);
  glColor3fv(vfWhite);
  glVertex2f(r.m_left-1.0f, r.m_top);
  glVertex2f(r.m_right, r.m_top);
  glVertex2f(r.m_right, r.m_bottom);
  glEnd();

  r.Inflate(2,2);
  glBegin(GL_LINE_STRIP);
  glColor3fv(vfWhite);
  glVertex2f(r.m_right, r.m_bottom);
  glVertex2f(r.m_left-1.0f, r.m_bottom);
  glVertex2f(r.m_left-1.0f, r.m_top);
  glColor3fv(vfGrey);
  glVertex2f(r.m_left-1.0f, r.m_top);
  glVertex2f(r.m_right, r.m_top);
  glVertex2f(r.m_right, r.m_bottom);
  glEnd();
  glPopAttrib();
} // end of Draw3dEtched()


/**
 * OpenGL helper Draw3dEdge
 * @param type const RectType
 */
template <class T> inline 
void Rect<T>::Draw3dEdge(const RectType::eEdgeType & type) const
{
  const float4 vfLight[3] = {1.0f,1.0f,1.0f}, vfDark[3] = {0.3f,0.3f,0.3f};

  glPushAttrib(GL_CURRENT_BIT);

  // Inner edge
  glBegin(GL_LINE_STRIP);
    switch (type) 
    {
      case RectType::RAISED:  
      case RectType::ETCHED:    
        glColor3fv(vfLight); 
        break;
      case RectType::SUNKEN:  
      case RectType::BUMPED:    
        glColor3fv(vfDark); 
        break;
      default:
        break;
    } // end of switch (type)
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_top));
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_bottom));
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_bottom));

    switch (type) 
    {
      case RectType::RAISED:  
      case RectType::ETCHED:    
        glColor3fv(vfDark); 
        break;
      case RectType::SUNKEN:  
      case RectType::BUMPED:    
        glColor3fv(vfLight); 
        break;
      default:
        break;
    } // end of switch (type)
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_bottom));
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_top));
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_top));
  glEnd();

  // Outer edge
  if ( (type == RectType::ETCHED) || (type == RectType::BUMPED) )   
  {
    Rect<T> r(*this);
    r.Inflate(1,1);

    glBegin(GL_LINE_STRIP);
      switch (type) 
      {
        case RectType::ETCHED:  
          glColor3fv(vfDark); 
          break;
        case RectType::BUMPED:  
          glColor3fv(vfLight); 
          break;
        default:
          break;
      } // end switch (type)
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_top));
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_bottom));
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_bottom));

      switch (type) 
      {
        case RectType::ETCHED:  
          glColor3fv(vfLight); 
          break;
        case RectType::BUMPED:  
          glColor3fv(vfDark); 
          break;
        default:
          break;
      } // end switch (type)
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_bottom));
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_top));
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_top));
    glEnd();
  } // end of if((type == RectType::ETCHED) || (type == RectType::BUMPED))  

  glPopAttrib();
} // end of Draw3dEdge()


/**
 * OpenGL helper Draw3dEdge
 * @param type const RectType
 * @param vfLight
 * @param vfDark
 */
template <class T> inline 
void Rect<T>::Draw3dEdge(const RectType::eEdgeType & type, const float4 vfLight[3], const float4 vfDark[3]) const
{
  glPushAttrib(GL_CURRENT_BIT);

  // Inner edge
  glBegin(GL_LINE_STRIP);
    switch (type) 
    {
      case RectType::RAISED:  
      case RectType::ETCHED:    
        glColor3fv(vfLight); 
        break;
      case RectType::SUNKEN:  
      case RectType::BUMPED:    
        glColor3fv(vfDark); 
        break;
      default:
        break;
    } // end of switch (type)
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_top));
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_bottom));
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_bottom));

    switch (type) 
    {
      case RectType::RAISED:  
      case RectType::ETCHED:    
        glColor3fv(vfDark); 
        break;
      case RectType::SUNKEN:  
      case RectType::BUMPED:    
        glColor3fv(vfLight); 
        break;
      default:
        break;
    } // end of switch (type)
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_bottom));
    glVertex2f(static_cast<float4>(m_right),static_cast<float4>(m_top));
    glVertex2f(static_cast<float4>(m_left),static_cast<float4>(m_top));
  glEnd();

  // Outer edge
  if ( (type == RectType::ETCHED) || (type == RectType::BUMPED) )   
  {
    Rect<T> r(*this);
    r.Inflate(1,1);

    glBegin(GL_LINE_STRIP);
      switch (type) 
      {
        case RectType::ETCHED:  
          glColor3fv(vfDark); 
          break;
        case RectType::BUMPED:  
          glColor3fv(vfLight); 
          break;
        default:
          break;
      } // end switch (type)
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_top));
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_bottom));
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_bottom));

      switch (type) 
      {
        case RectType::ETCHED:  
          glColor3fv(vfLight); 
          break;
        case RectType::BUMPED:  
          glColor3fv(vfDark); 
          break;
        default:
          break;
      } // end switch (type)
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_bottom));
      glVertex2f(static_cast<float4>(r.m_right),static_cast<float4>(r.m_top));
      glVertex2f(static_cast<float4>(r.m_left),static_cast<float4>(r.m_top));
    glEnd();
  } // end of if((type == RectType::ETCHED) || (type == RectType::BUMPED))  

  glPopAttrib();
} // end of Draw3dEdge()

  
/**
 * OpenGL helper Draw3dRect
 */
template <class T> inline 
void Rect<T>::Draw3dRect() const
// Note: Inside of rect drawn in current color
{
  const float4 vfTop[3] = {0.7f,0.7f,0.7f}, vfBottom[3] = {0.3f, 0.3f, 0.3f};
  Rect<float4> r(static_cast<float4>(m_left),
                 static_cast<float4>(m_top),
                 static_cast<float4>(m_right),
                 static_cast<float4>(m_bottom));

  glBegin(GL_QUADS);
  glVertex2f(r.m_left,r.m_top);       
  glVertex2f(r.m_left,r.m_bottom);
  glVertex2f(r.m_right,r.m_bottom);   
  glVertex2f(r.m_right,r.m_top);
  glEnd();

  glPushAttrib(GL_CURRENT_BIT);
  glBegin(GL_LINE_STRIP);
  glColor3fv(vfTop);
  glVertex2f(r.m_left,r.m_top);
  glVertex2f(r.m_left,r.m_bottom);
  glVertex2f(r.m_right,r.m_bottom);
  glColor3fv(vfBottom);
  glVertex2f(r.m_right,r.m_bottom);
  glVertex2f(r.m_right,r.m_top);
  glVertex2f(r.m_left,r.m_top);
  glEnd();
  glPopAttrib();
} // end of Draw3dRect()


/**
 * Overloaded << Operator.
 * Directs the two data members to the ostream, reference of which is passed
 * @param os ostream &
 * @param rect Rect<T> &
 * @return ostream &
 */  
template <class T> 
inline std::ostream & operator<<(std::ostream & os, const Rect<T> & rect)
{
  os << "Rect(" ;
  os << rect.m_left << "," ;
  os << rect.m_top << "," ;
  os << rect.m_right << "," ;
  os << rect.m_bottom ;
  os << ")"; 
  return os;
} // end of operator<<(ostream &os, const Rect<T> & rect)


/**
 * Test function for template class Rect<T>.
 * @param T type of the Rect
 * @param os output stream
 * @return bool bSuccess
 */
template <class T>
bool Rect<T>::TestByType(std::ostream & os) 
{
  static const int1 * const sBoolStr[] = {"    Oh No ! :-( ","   :-) "};
  bool bSuccess(true);
  bool bOk(true);
  T l(150.5), t(200.25), r(250.75), b(250.5); // left, top, right, bottom
  T x(170.125), y(220.125);
  T w(50.25), h(100.5);

  os << endl << "-Testing Rect class for type: " << typeid(T).name();
  os << endl <<"size of Rect<T>: " ;
  os << sizeof(Rect<T>) << " Bytes" << endl;
  os << "l= " << l << " t= " << t ;
  os << " r= " << r << " b= " << b << endl;
  os << "x= " << x << " y= " << y << endl;

  // tests default constructor
  os << "Rect<T> rect1= ";
  Rect<T> rect1;
  os << rect1 << endl;
  bOk = (rect1.m_left   == 0 &&
         rect1.m_top    == 0 &&
         rect1.m_right  == 0 &&
         rect1.m_bottom == 0);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;
    
  // tests default constructor with args
  os << "Rect<T> rect2(l,t,r,b)= ";
  Rect<T> rect2(l,t,r,b);
  os << rect2 << endl;
  bOk = (rect2.m_left   == l &&
         rect2.m_top    == t &&
         rect2.m_right  == r &&
         rect2.m_bottom == b);  
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests Point/Size constructor
  os << "Rect<T> rect3(point1,size1): rect3= ";
  Point2D<T> point1(8,8);
  Point2D<T> size1(40,40);
  Rect<T> rect3(point1,size1);
  os << rect3 << endl;
  bOk = ( (rect3.m_left   == point1.m_x) &&
          (rect3.m_top    == point1.m_y) &&
          (rect3.m_right  == (point1.m_x + size1.m_x)) &&
          (rect3.m_bottom == (point1.m_y + size1.m_y)) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests copy constructor from Rect
  os << "Rect<T> rect4(rect3): rect4= ";
  Rect<T> rect4(rect3);
  os << rect4 << endl; 
  bOk = (rect4.m_left   == rect3.m_left  && 
         rect4.m_top    == rect3.m_top   && 
         rect4.m_right  == rect3.m_right &&
         rect4.m_bottom == rect3.m_bottom);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  // tests Set operator
  os << "rect3.Set(l,t,r,b)= ";
  rect3.Set(l,t,r,b);
  os << rect3 << endl;
  bOk = (rect3.m_left   == l &&
         rect3.m_top    == t &&
         rect3.m_right  == r &&
         rect3.m_bottom == b);  
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


 // tests Assignment operator from Rect
  os << "rect5=Rect<T>(l,t,r,b): rect5= "; 
  Rect<T> rect5 = Rect<T>(l,t,r,b);
  os << rect5 << endl; 
  bOk = (rect5.m_left   == l && 
         rect5.m_top    == t && 
         rect5.m_right  == r &&
         rect5.m_bottom == b);
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests if {x,y} is inside of rectangle
  os << "rect5.PtInRect(x,y): ";
  if (bOk=rect5.PtInRect(x,y))
    os << "(x,y) is inside rectangle" << endl;
  else 
    os << "(x,y) is not inside rectangle" << endl;
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests if point is inside of rectangle
  os << "rect5.PtInRect(point2): ";
  Point2D<T> point2(180,250);
  if (bOk=rect5.PtInRect(point2))
    os << "point is inside rectangle" << endl;
  else 
    os << "point is not inside rectangle" << endl;
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;
  

  // tests width and height of a rectangle
  T width = rect5.Width();
  T height = rect5.Height();
  os << "rect5.Width()= " << width;
  os << " rect5.Height()= " << height << endl;
  bOk = (width  == (T)(rect5.m_right - rect5.m_left) &&
         height == (T)(rect5.m_bottom - rect5.m_top) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

 
  // tests bounding points
  // they divided into 3 sections to look better on the screen
  Point2D<T> centerPoint = rect5.CenterPoint();
  Point2D<T> leftTop = rect5.LeftTop();
  Point2D<T> rightTop = rect5.RightTop();
  Point2D<T> leftBottom = rect5.LeftBottom();
  Point2D<T> rightBottom = rect5.RightBottom();
  
  os << "rect5.CenterPoint()= " << centerPoint << endl;
  bOk = ( centerPoint == Point2D<T>( rect5.m_left + (width/(T)2),
                                     rect5.m_top + (height/(T)2) ) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  os << "rect5.LeftTop()= " << leftTop; 
  os << " rect5.RightTop()= " << rightTop << endl;
  bOk = ( leftTop == Point2D<T>(rect5.m_left, rect5.m_top) &&
          rightTop == Point2D<T>(rect5.m_right, rect5.m_top) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;
  
  os << "rect5.LeftBottom()= " << leftBottom;
  os << " rect5.RightBottom()= " << rightBottom << endl;
  bOk = ( leftBottom == Point2D<T>(rect5.m_left, rect5.m_bottom) &&
          rightBottom == Point2D<T>(rect5.m_right, rect5.m_bottom) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests offset method by (x,y)
  rect1 = rect3;
  os << "rect3.Offset(x,y)= " << rect3.Offset(x,y) << endl;
  bOk = ( rect3.m_left   == (T)(rect1.m_left + x)  &&
          rect3.m_right  == (T)(rect1.m_right + x) &&
          rect3.m_top    == (T)(rect1.m_top + y)   &&
          rect3.m_bottom == (T)(rect1.m_bottom + y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests offset method by Point2D
  rect2 = rect5;
  os << "rect5.Offset(point1)= " << rect5.Offset(point1) << endl;
  bOk = ( rect5.m_left   == (T)(rect2.m_left + point1.m_x)  &&
          rect5.m_right  == (T)(rect2.m_right + point1.m_x) &&
          rect5.m_top    == (T)(rect2.m_top + point1.m_y)   &&
          rect5.m_bottom == (T)(rect2.m_bottom + point1.m_y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetOffset by (x,y)
  rect5 = rect2;
  rect2 = rect2.GetOffset(x,y);
  os << "rect2.GetOffset(x,y)= " << rect2 << endl;
  bOk = ( rect2.m_left   == (T)(rect5.m_left + x)  &&
          rect2.m_right  == (T)(rect5.m_right + x) &&
          rect2.m_top    == (T)(rect5.m_top + y)   &&
          rect2.m_bottom == (T)(rect5.m_bottom + y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetOffset by Point2D
  rect1 = rect5;
  rect5 = rect5.GetOffset(point2);
  os << "rect5.GetOffset(point2)= " << rect5 << endl;
  bOk = ( rect5.m_left   == (T)(rect1.m_left + point2.m_x)  &&
          rect5.m_right  == (T)(rect1.m_right + point2.m_x) &&
          rect5.m_top    == (T)(rect1.m_top + point2.m_y)   &&
          rect5.m_bottom == (T)(rect1.m_bottom + point2.m_y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests unary - operator
  // this test is out of the unsigned char scope
  if ( (typeid(T).name() != typeid(unsigned char).name())) 
  {
    rect1 = rect2;
    os << "-rect2= " << (rect2=(-rect2)) << endl;
    bOk = ( rect2.m_left   == (T)(-1 * rect1.m_left)  &&
            rect2.m_right  == (T)(-1 * rect1.m_right) &&
            rect2.m_top    == (T)(-1 * rect1.m_top)   &&
            rect2.m_bottom == (T)(-1 * rect1.m_bottom) );
    os << sBoolStr[bOk];
    bSuccess = bSuccess && bOk;
  }
  
  // tests binary + operator: takes a Point2D
  os << "rect4=rect4+Point2D<T>(x,y): rect4=  ";
  rect1 = rect4;
  rect4 = rect4 + Point2D<T>(x,y);
  os << rect4 << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left + x)  &&
          rect4.m_right  == (T)(rect1.m_right + x) &&
          rect4.m_top    == (T)(rect1.m_top + y)   &&
          rect4.m_bottom == (T)(rect1.m_bottom + y) );  
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary + operator: takes another Rect
  os << "rect1=rect1+Rect<T>(l,t,r,b): rect1= ";
  rect2 = rect1;
  rect1 = rect1+Rect<T>(l,t,r,b);
  os << rect1 << endl;
  bOk = ( rect1.m_left   == (T)(rect2.m_left - l)  &&
          rect1.m_right  == (T)(rect2.m_right + r) &&
          rect1.m_top    == (T)(rect2.m_top - t)   &&
          rect1.m_bottom == (T)(rect2.m_bottom + b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  
  // tests binary - operator: takes a Point2D
  os << "rect3=rect3-Point2D<T>(x,y): rect3= ";
  rect1 = rect3;
  rect3 = rect3 - Point2D<T>(x,y);
  os << rect3 << endl;
  bOk = ( rect3.m_left   == (T)(rect1.m_left - x)  &&
          rect3.m_right  == (T)(rect1.m_right - x) &&
          rect3.m_top    == (T)(rect1.m_top - y)   &&
          rect3.m_bottom == (T)(rect1.m_bottom - y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary - operator: takes another Rect
  os << "rect4=rect4-Rect<T>(l,t,r,b): rect4= ";
  rect1 = rect4;
  rect4 = rect4 - Rect<T>(l,t,r,b);
  os << rect4 << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left + l)  &&
          rect4.m_right  == (T)(rect1.m_right - r) &&
          rect4.m_top    == (T)(rect1.m_top + t)   &&
          rect4.m_bottom == (T)(rect1.m_bottom - b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary += operator: takes a Point2D
  os << "rect3+=Point2D<T>(x,y): rect3= ";
  rect1 = rect3;
  rect3 += Point2D<T>(x,y);
  os << rect3 << endl;
  bOk = ( rect3.m_left   == (T)(rect1.m_left + x)  &&
          rect3.m_right  == (T)(rect1.m_right + x) &&
          rect3.m_top    == (T)(rect1.m_top + y)   &&
          rect3.m_bottom == (T)(rect1.m_bottom + y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary += operator: takes another Rect
  os << "rect3+=Rect<T>(l,t,r,b): rect3= ";
  rect1 = rect3;
  rect3 += Rect<T>(l,t,r,b);
  os << rect3 << endl;
  bOk = ( rect3.m_left   == (T)(rect1.m_left - l)  &&
          rect3.m_right  == (T)(rect1.m_right + r) &&
          rect3.m_top    == (T)(rect1.m_top - t)   &&
          rect3.m_bottom == (T)(rect1.m_bottom + b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary -= operator: takes a Point2D
  os << "rect5-=Point<T>(x,y): rect5= ";
  rect1 = rect5;
  rect5 -= Point2D<T>(x,y);
  os << rect5 << endl;
  bOk = ( rect5.m_left   == (T)(rect1.m_left - x)  &&
          rect5.m_right  == (T)(rect1.m_right - x) &&
          rect5.m_top    == (T)(rect1.m_top - y)   &&
          rect5.m_bottom == (T)(rect1.m_bottom - y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests binary -= operator: takes another Rect
  os << "rect5-=Rect<T>(l,t,r,b): rect5= ";
  rect1 = rect5;
  rect5 -= Rect<T>(l,t,r,b);
  os << rect5 << endl;
  bOk = ( rect5.m_left   == (T)(rect1.m_left + l)  &&
          rect5.m_right  == (T)(rect1.m_right - r) &&
          rect5.m_top    == (T)(rect1.m_top + t)   &&
          rect5.m_bottom == (T)(rect1.m_bottom - b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests Inflate by w,h
  os << "rect3.Inflate(w,h)= ";
  rect1 = rect3;
  os << rect3.Inflate(w,h) << endl;
  bOk = ( rect3.m_left   == (T)(rect1.m_left - w)  &&
          rect3.m_right  == (T)(rect1.m_right + h) &&
          rect3.m_top    == (T)(rect1.m_top - w)   &&
          rect3.m_bottom == (T)(rect1.m_bottom + h) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

 
  // tests Inflate by Point2D
  os << "rect4.Inflate(Point2D<T>(x,y))= ";
  rect1 = rect4;
  os << rect4.Inflate(Point2D<T>(x,y)) << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left - x)  &&
          rect4.m_right  == (T)(rect1.m_right + y) &&
          rect4.m_top    == (T)(rect1.m_top - x)   &&
          rect4.m_bottom == (T)(rect1.m_bottom + y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests Inflate by Rect
  os << "rect4.Inflate(Rect<T>(l,t,r,b))= ";
  rect1 = rect4;
  os << rect4.Inflate(Rect<T>(l,t,r,b)) << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left - l)  &&
          rect4.m_right  == (T)(rect1.m_right + r) &&
          rect4.m_top    == (T)(rect1.m_top - t)   &&
          rect4.m_bottom == (T)(rect1.m_bottom + b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // Inflate by coordinates is tested by the previous 3


  // tests Deflate by w,h
  os << "rect4.Deflate(w,h)= ";
  rect1 = rect4;
  os << rect4.Deflate(w,h) << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left + w)  &&
          rect4.m_right  == (T)(rect1.m_right - h) &&
          rect4.m_top    == (T)(rect1.m_top + w)   &&
          rect4.m_bottom == (T)(rect1.m_bottom - h) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests Deflate by Point2D
  os << "rect4.Deflate(Point2D<T>(x,y))= ";
  rect1 = rect4;
  os << rect4.Deflate(Point2D<T>(x,y)) << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left + x)  &&
          rect4.m_right  == (T)(rect1.m_right - y) &&
          rect4.m_top    == (T)(rect1.m_top + x)   &&
          rect4.m_bottom == (T)(rect1.m_bottom - y) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;  
 

  // tests Deflate by Rect
  os << "rect4.Delate(Rect<T>(l,t,r,b))= ";
  rect1 = rect4;
  os << rect4.Deflate(Rect<T>(l,t,r,b)) << endl;
  bOk = ( rect4.m_left   == (T)(rect1.m_left + l)  &&
          rect4.m_right  == (T)(rect1.m_right - r) &&
          rect4.m_top    == (T)(rect1.m_top + t)   &&
          rect4.m_bottom == (T)(rect1.m_bottom - b) );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // Deflate by coordinates is tested by the previous 3


  // tests Normalize
  os << "Rect<T> rect6(100,250,60,240).Normalize()= ";
  Rect<T>rect6(100,250,60,240);
  os << rect6.Normalize() << endl;
  bOk = ( rect6.m_left   == 60  &&
          rect6.m_top    == 240 &&
          rect6.m_right  == 100 &&
          rect6.m_bottom == 250 );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;


  // tests GetNormalized
  os << "rect1=rect7(60,120,40,100).GetNormalized(): rect1= ";
  Rect<T> rect7(60,120,40,100);
  rect1 = rect7.GetNormalized();
  os << rect1 << endl;
  bOk = ( rect1.m_left   == 40  &&
          rect1.m_top    == 100 &&
          rect1.m_right  == 60 &&
          rect1.m_bottom == 120 );
  os << sBoolStr[bOk];
  bSuccess = bSuccess && bOk;

  os << endl; 
  
  if (bSuccess) 
  {
    os << "Test of Rect class is a success!" << endl;
  } 
  else 
  {
    os << "Test of Rect class is NOT a success" << endl;
  } //endif (bSuccess)
  
  return bSuccess;

} // end of TestByType

// $Log: Rect.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.11  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 3.10  2004/01/13 14:52:55  frank
// Fixed problem with lack of update when toggling full screen mode of curved mpr view
//
// Revision 3.9  2003/05/22 16:34:57  dongqing
// code review
//
// Revision 3.8  2003/05/16 13:14:52  frank
// formatting
//
// Revision 3.7  2003/02/05 22:31:25  jmeade
// Override of Draw3dRect that allows setting of colors.
//
// Revision 3.6  2002/09/27 14:18:09  ingmar
// replaced Size2D with Point2D
//
// Revision 3.5  2002/09/25 20:41:36  ingmar
// replaced Size2D with Point2D
//
// Revision 3.4  2002/07/22 23:17:20  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 3.3  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 3.2  2002/07/08 17:34:03  binli
// function for cropbox
//
// Revision 3.1.10.1  2002/07/03 15:18:00  binli
// CropBox stuff
//
// Revision 3.1  2001/11/12 19:19:35  jenny
// implemented WidthAbs() and HeightAbs()
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:20   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:18   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.17  2001/08/22 23:31:12  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.16  2001/06/28 20:36:09  jmeade
// Rect::Inflate(T& w, T& h) incorrect expansion of w and h to base Inflate() override
//
// Revision 1.15  2001/05/31 21:32:22  ana
// changes regarding code review
//
// Revision 1.14  2001/05/30 21:02:08  ana
// handle unsigned char when testing unary minus
//
// Revision 1.13  2001/05/29 20:37:42  ana
// minor changes to unsigned char test
//
// Revision 1.12  2001/05/23 21:34:39  ana
// finished TestByType function
//
// Revision 1.11  2001/05/22 21:31:05  ana
// Created tests functions for Rect<T> class.
//
// Revision 1.10  2001/04/09 18:03:19  dmitry
// Removed Revision History from Top of File
//
// Revision 1.9  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.8  2001/02/26 21:50:11  jmeade
// bool Rect<T>::PtInRect(const Tuple<T> &) const
//
// Revision 1.7  2001/01/15 19:54:52  jeff
// comments
//
// Revision 1.6  2001/01/15 17:05:44  dave
// updated lipng108 for UNIX flavors, correct some syntax problems found by gcc
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Rect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Rect.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
