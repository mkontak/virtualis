// $Id: vxSerializer.inl,v 1.10 2007/07/27 21:44:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)


/**
 * to differentiate parameter std::vector<T> and std::vector<T*>
 */
template <class T>
static std::vector<T> vxSerializer::CreateRealVectorFromPtr(const std::vector<T *> & vpObject)
{
  std::vector<T> vObject;
  std::vector<T *>::const_iterator iter;
  for (iter=vpObject.begin(); iter != vpObject.end(); ++iter)
  {
    vObject.push_back(**iter);
  }

  return vObject;
} // CreateRealVectorFromPtr()


/**
 * to differentiate parameter std::vector<T> and std::vector<T*>
 */
template <class T>
static std::vector<T> vxSerializer::CreateRealVectorFromPtr(const std::vector<const T *> & vpObject)
{
  std::vector<T> vObject;
  std::vector<const T *>::const_iterator iter;
  for (iter=vpObject.begin(); iter != vpObject.end(); ++iter)
  {
    vObject.push_back(**iter);
  }

  return vObject;
} // CreateRealVectorFromPtr()


/**
 * to differentiate parameter std::vector<T> and std::vector<T*>
 */
template <class T>
static std::vector<T*> vxSerializer::CreatePtrVectorFromReal(const std::vector<T> & vObject)
{
  T * pT;
  std::vector<T*> vpObjects;
  std::vector<T>::const_iterator iter;
  for (iter=vObject.begin(); iter != vObject.end(); ++iter)
  {
    pT = new T;
    *pT = *iter;
    vpObjects.push_back(pT);
  }

  return vpObjects;
} // CreatePtrVectorFromReal()


/**
 * to differentiate parameter std::vector<T> and std::vector<T*>
 */
template <class T>
static std::vector<const T *> vxSerializer::CreateConstPtrVectorFromReal(const std::vector<T> & vObject)
{
  T * pT;
  std::vector<const T *> vpObjects;
  std::vector<T>::const_iterator iter;

  for (iter=vObject.begin(); iter != vObject.end(); ++iter)
  {
    pT = new T;
    *pT = *iter;
    vpObjects.push_back(pT);
  }

  return vpObjects;
} // CreatePtrVectorFromReal()


/**
 * write a std::map<T1, T2>
 *
 * @param element the element
 * @param mp the map to write
 */
template <class T1, class T2>
static void vxSerializer::Write(vxDOMElement & element, const std::map<T1, T2> & map)
{
  std::pair<T1, T2> pair;
  std::map<T1, T2>::const_iterator iter;
  std::string sMember("");
  SetAttribute(element, "version", "2.0");
  int i(0);

  for (iter=map.begin(); iter!=map.end(); ++iter)
  {
    pair = *iter;
    sMember = ToAscii(i++);
    Write(AddElement(element, vxSerializer::GetElementName(pair), sMember), pair);
  }
} // Write( map )


/**
 * read a std::map<T1, T2>
 *
 * @param element the element
 * @param map the map to read
 *
 * @param success
 */
template <class T1, class T2>
static bool vxSerializer::Read(vxDOMElement & element, std::map<T1, T2> & map)
{
  if( element.IsNull() ) return false;

  std::pair<T1, T2> pair;
  map.clear();  
  
  vxDOMNodeList nodes( element.GetChildNodes() );
  for( int i( 0 ); i < nodes.GetLength(); ++i )
  {
    vxDOMElement child( nodes.Item( i ).ToElement() );
    Read( child, pair );
    map.insert(pair);
  }
  return true;
} // Read()


/**
 * write a std::list<T>
 *
 * @param element the element
 * @param list the list to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const std::list<T> & list)
{
  T t;
  std::list<T>::const_iterator iter;
  std::string sMember = "";

  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));

  int i(0);
  for (iter=list.begin(); iter != list.end(); iter++)
  {
    sMember = ToAscii(i++);
    Write(AddElement(element, vxSerializer::GetElementName(t), sMember), *iter);
  }
} // Write( list )


/**
 * read a std::list<T>
 *
 * @param element the element
 * @param list the list to read
 *
 * @param success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, std::list<T> & list)
{
  if( element.IsNull() ) return false;

  T t;
  list.clear();

  vxDOMNodeList nodes( element.GetChildNodes() );
  for( int i( 0 ); i < nodes.GetLength(); ++i )
  {
    vxDOMElement child( nodes.Item( i ).ToElement() );

    std::string sValue;
    GetAttribute( child, "member", sValue );
    if( sValue != ToAscii( i ) )
    {
      int4 use_safe_deserialize;
      return false;
    }
    
    Read( child, t );
    list.push_back( t );
  }
  return true;
} // Read( list )


/**
 * write a std::pair<T1, T2>
 *
 * @param element the element
 * @param pair the pair to read
 */
template <class T1, class T2>
static void vxSerializer::Write(vxDOMElement & element, const std::pair<T1, T2> & pair)
{
  T1 t1;
  T2 t2;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template1", vxSerializer::GetElementName(t1));
  SetAttribute(element, "template2", vxSerializer::GetElementName(t2));
  Write(AddElement(element, vxSerializer::GetElementName(t1), "first"),  pair.first);
  Write(AddElement(element, vxSerializer::GetElementName(t2), "second"), pair.second);
} // Write()


/**
 * read a std::pair<T1, T2>
 *
 * @param element the element
 * @param the pair to read
 *
 * @param success
 */
template <class T1, class T2>
static bool vxSerializer::Read(vxDOMElement & element, std::pair<T1, T2> & pair)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate1;
  std::string sTemplate2;
  bSuccess &= GetAttribute(element, "template1", sTemplate1);
  bSuccess &= GetAttribute(element, "template2", sTemplate2);
  T1 t1; bSuccess &= Read(FindElement(element, sTemplate1, "first"),  t1);
  T2 t2; bSuccess &= Read(FindElement(element, sTemplate2, "second"), t2);
  pair = std::pair<T1, T2>(t1, t2); 

  return bSuccess;
} // Read()


/**
 * write a PixelRGB<T>
 *
 * @param element the element
 * @param pixel the pixel to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const PixelRGB<T> & pixel)
{
  T t;
  SetAttribute(element, "version", "1.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "red"),   pixel.R());
  Write(AddElement(element, vxSerializer::GetElementName(t), "green"), pixel.G());
  Write(AddElement(element, vxSerializer::GetElementName(t), "blue"),  pixel.B());

  return;
} // Write()


/**
 * read a PixelRGB<T>
 *
 * @param element the element
 * @param pixel the pixel to read
 *
 * @param success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, PixelRGB<T> & pixel)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T r; bSuccess &= Read(FindElement(element, sTemplate, "red"),   r);
  T g; bSuccess &= Read(FindElement(element, sTemplate, "green"), g);
  T b; bSuccess &= Read(FindElement(element, sTemplate, "blue"),  b);
  pixel = PixelRGB<T>(r, g, b);

  return bSuccess;
} // Read()


/**
 * write a PixelRGBA<T>
 *
 * @param element the element
 * @param pixel the pixel to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const PixelRGBA<T> & pixel)
{
  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "red"),   pixel.R());
  Write(AddElement(element, vxSerializer::GetElementName(t), "green"), pixel.G());
  Write(AddElement(element, vxSerializer::GetElementName(t), "blue"),  pixel.B());
  Write(AddElement(element, vxSerializer::GetElementName(t), "alpha"), pixel.A());
} // Write()


/**
 * read a PixelRGBA<T>
 *
 * @param element the element
 * @param pixel the pixel to read
 *
 * @param success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, PixelRGBA<T> & pixel)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T r; bSuccess &= Read(FindElement(element, sTemplate, "red"),   r);
  T g; bSuccess &= Read(FindElement(element, sTemplate, "green"), g);
  T b; bSuccess &= Read(FindElement(element, sTemplate, "blue"),  b);
  T a; bSuccess &= Read(FindElement(element, sTemplate, "alpha"), a);
  pixel = PixelRGBA<T>(r, g, b, a);

  return bSuccess;
} // Read()


/**
 * std::vector<T*>
 *
 * @param element the element
 * @param vpObjects the object array to write
 */
template <class T>
static void vxSerializer::WriteISerialize(vxDOMElement & element, const std::vector<T*> & vpObjects)
{
  std::vector<T*>::const_iterator iter;
  std::string sMember = "";

  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", "polymorph");
  int i(0);
  for (iter=vpObjects.begin(); iter != vpObjects.end(); iter++)
  {
    sMember = ToAscii(i++);
    (*iter)->ToXml(AddElement(element, (*iter)->GetElementName(), sMember));
  }
} // WriteISerialize()


/**
 * std::vector<T*>
 *
 * @param element the element
 * @param vpObjects the object array to read
 *
 * @return the objects
 */
template <class T>
static bool vxSerializer::ReadISerialize(vxDOMElement & element, std::vector<T*> & vpObjects)
{
  if( element.IsNull() ) return false;

  vpObjects.clear();

  vxDOMNodeList nodes( element.GetChildNodes() );
  for( int i( 0 ); i < nodes.GetLength(); ++i )
  {
    vxDOMElement child( nodes.Item( i ).ToElement() );

    std::string sName = child.GetTagName();

    std::string sValue;
    GetAttribute( child, "member", sValue );
    if( sValue != ToAscii( i ) )
      return false;
    
    vxISerialize * pSerialize(vxISerializeFactoryManager::GetManager().Create(sName));
    pSerialize->FromXml( child );
    vpObjects.push_back( dynamic_cast<T*>(pSerialize) );
  }
  return true;
} // ReadISerialize()


/**
 * write object derived from vxISerialize
 *
 * @param element the element
 * @param vpObjects the object to read
 *
 * @return success
 */
template < class T >
static void vxSerializer::WriteISerialize( vxDOMElement & element, const T * pObject )
{

  SetAttribute( element, "version", "2.0" );
  SetAttribute( element, "template", "polymorph" );
  pObject->ToXml( AddElement( element, pObject->GetElementName(), "object" ) );

} // WriteISerialize()


/**
 * read object derived from vxISerialize
 *
 * @param element the element
 * @param vpObjects the object to read
 *
 * @return success
 */
template < class T >
static bool vxSerializer::ReadISerialize( vxDOMElement & element, T ** pObject )
{

  if( element.IsNull() ) return false;

  vxDOMElement elementObject = FindElementByMember( element, "object" );
  if ( elementObject.IsNull()) return false;

  bool bSuccess( true );

  std::string sName =  elementObject.GetTagName();
  vxISerialize * pSerialize( vxISerializeFactoryManager::GetManager().Create( sName ) );
  pSerialize->FromXml( elementObject );
  * pObject = dynamic_cast< T * > ( pSerialize );

  return true;

} // ReadISerialize()


/**
 * write a std::vector<T>
 *
 * @param element the element
 * @param vpObjects the object array to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const std::vector<T> & vObjects)
{
  T t;
  std::vector<T>::const_iterator iter;
  std::string sMember = "";

  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));

  int i(0);
  for (iter=vObjects.begin(); iter != vObjects.end(); iter++)
  {
    sMember = ToAscii(i++);
    Write(AddElement(element, vxSerializer::GetElementName(t), sMember), *iter);
  }
} // Write()


/**
 * read a std::vector<T>
 *
 * @param element the element
 * @param vpObjects the object array to write
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read( vxDOMElement & element, std::vector<T> & vObject )
{
  if( element.IsNull() ) return false;

  T t;
  bool bSuccess(true);
  vObject.clear();
  
  vxDOMNodeList nodes( element.GetChildNodes() );
  for( int i( 0 ); i < nodes.GetLength(); ++i )
  {
    vxDOMElement child( nodes.Item( i ).ToElement() );
		if ( child.IsNull() )
			continue;

		std::string sValue;
		GetAttribute( child, "member", sValue );
		/*if( sValue != ToAscii( i ) )
		{
			return false;
		}*/
	  Read( child, t );
		vObject.push_back( t );
  }
  return bSuccess;
} // Read()


/**
 * write a Box<T>
 *
 * @param element the element
 * @param box the box to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Box<T> & box)
{
  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x-min"), box.GetMinX());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y-min"), box.GetMinY());
  Write(AddElement(element, vxSerializer::GetElementName(t), "z-min"), box.GetMinZ());
  Write(AddElement(element, vxSerializer::GetElementName(t), "x-max"), box.GetMaxX());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y-max"), box.GetMaxY());
  Write(AddElement(element, vxSerializer::GetElementName(t), "z-max"), box.GetMaxZ());
} // Write()


/**
 * read a Box<T>
 *
 * @param element the element
 * @param box the box to read
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Box<T> & box)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T Xmin; bSuccess &= Read(FindElement(element, sTemplate, "x-min"), Xmin);
  T Ymin; bSuccess &= Read(FindElement(element, sTemplate, "y-min"), Ymin);
  T Zmin; bSuccess &= Read(FindElement(element, sTemplate, "z-min"), Zmin);
  T Xmax; bSuccess &= Read(FindElement(element, sTemplate, "x-max"), Xmax);
  T Ymax; bSuccess &= Read(FindElement(element, sTemplate, "y-max"), Ymax);
  T Zmax; bSuccess &= Read(FindElement(element, sTemplate, "z-max"), Zmax);
  box = Box<T>(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

  return bSuccess;
} // Read()


/**
 * write a Point2D<T>
 *
 * @param element the element
 * @param point2D the point to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Point2D<T> & point2D)
{
  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x"), point2D.X());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y"), point2D.Y());
} // Write()


/**
 * read a point2D
 *
 * @param element the element
 * @param point2D the point to read
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Point2D<T> & point2D)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T x; bSuccess &= Read(FindElement(element, sTemplate, "x"), x);
  T y; bSuccess &= Read(FindElement(element, sTemplate, "y"), y);
  point2D = Point2D<T>(x, y);

  return bSuccess;
} // Read()


/**
 * write a Point<T>
 *
 * @param element the element
 * @param point the point to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Point3D<T> & point)
{
  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x"), point.X());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y"), point.Y());
  Write(AddElement(element, vxSerializer::GetElementName(t), "z"), point.Z());
} // Write()


/**
 * read a Point<T>
 *
 * @param element the element
 * @param point the point to write
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Point3D<T> & point)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T x; bSuccess &= Read(FindElement(element, sTemplate, "x"), x);
  T y; bSuccess &= Read(FindElement(element, sTemplate, "y"), y);
  T z; bSuccess &= Read(FindElement(element, sTemplate, "z"), z);
  point = Point<T>(x, y, z);

  return bSuccess;
} // Read()


/**
 * write a Point<T>
 *
 * @param element the element
 * @param vector the vector to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Vector3D<T> & vector)
{
  T t;
  SetAttribute(element, "version", "1.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x"), vector.X());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y"), vector.Y());
  Write(AddElement(element, vxSerializer::GetElementName(t), "z"), vector.Z());
} // Write()


/**
 * read a Point<T>
 *
 * @param element the element
 * @param vector the vector to write
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Vector3D<T> & vector)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T x; bSuccess &= Read(FindElement(element, sTemplate, "x"), x);
  T y; bSuccess &= Read(FindElement(element, sTemplate, "y"), y);
  T z; bSuccess &= Read(FindElement(element, sTemplate, "z"), z);
  vector = Vector<T>(x, y, z);

  return bSuccess;
} // Read()


/**
 * write a Triple<T>
 *
 * @param element the element
 * @param triple the triple to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Triple<T> & triple)
{
  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x"), triple.m_x);
  Write(AddElement(element, vxSerializer::GetElementName(t), "y"), triple.m_y);
  Write(AddElement(element, vxSerializer::GetElementName(t), "z"), triple.m_z);
} // Write()


/**
 * read a Triple<T>
 *
 * @param element the element
 * @param triple the triple to write
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Triple<T> & triple)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T x; bSuccess &= Read(FindElement(element, sTemplate, "x"), x);
  T y; bSuccess &= Read(FindElement(element, sTemplate, "y"), y);
  T z; bSuccess &= Read(FindElement(element, sTemplate, "z"), z);
  triple = Triple<T>(x, y, z);

  return bSuccess;
} // Read()

/**
 * write a Normal<T>
 *
 * @param element the element
 * @param normal the normal to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Normal3D<T> & normal)
{

  T t;
  SetAttribute(element, "version", "2.0");
  SetAttribute(element, "template", vxSerializer::GetElementName(t));
  Write(AddElement(element, vxSerializer::GetElementName(t), "x"), normal.X());
  Write(AddElement(element, vxSerializer::GetElementName(t), "y"), normal.Y());
  Write(AddElement(element, vxSerializer::GetElementName(t), "z"), normal.Z());

} // Write()


/**
 * read a Normal<T>
 *
 * @param element the element
 * @param normal the normal to read
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Normal3D<T> & normal)
{
  if( element.IsNull() ) return false;

  bool bSuccess(true);
  std::string sTemplate;
  bSuccess &= GetAttribute(element, "template", sTemplate);
  T x; bSuccess &= Read(FindElement(element, sTemplate, "x"), x);
  T y; bSuccess &= Read(FindElement(element, sTemplate, "y"), y);
  T z; bSuccess &= Read(FindElement(element, sTemplate, "z"), z);
  normal = Normal<T>(x, y, z);

  return bSuccess;
} // Read()


/**
 * write a Vector4d<T>
 *
 * @param element the element
 * @param vector the vector to write
 */
template <class T>
static void vxSerializer::Write(vxDOMElement & element, const Vector4d<T> & vector)
{

  T t;
  SetAttribute( element, "version", "2.0" );
  SetAttribute( element, "template", vxSerializer::GetElementName( t ) );
  Write( AddElement( element, vxSerializer::GetElementName( t ), "x" ), vector.X() );
  Write( AddElement( element, vxSerializer::GetElementName( t ), "y" ), vector.Y() );
  Write( AddElement( element, vxSerializer::GetElementName( t ), "z" ), vector.Z() );
  Write( AddElement( element, vxSerializer::GetElementName( t ), "t" ), vector.T() );

} // Write( Vector4d<T> )


/**
 * read a Vector4d<T>
 *
 * @param element the element
 * @param normal the normal to read
 *
 * @return success
 */
template <class T>
static bool vxSerializer::Read(vxDOMElement & element, Vector4d<T> & vector)
{
  if( element.IsNull() ) return false;

  bool bSuccess( true );
  std::string sTemplate;
  bSuccess &= GetAttribute( element, "template", sTemplate );
  T x; bSuccess &= Read( FindElement( element, sTemplate, "x" ), x );
  T y; bSuccess &= Read( FindElement( element, sTemplate, "y" ), y );
  T z; bSuccess &= Read( FindElement( element, sTemplate, "z" ), z );
  T t; bSuccess &= Read( FindElement( element, sTemplate, "t" ), t );
  vector = Vector4d<T>( x, y, z, t );

  return bSuccess;

} // Read( Vector4d<T> )


// $Log: vxSerializer.inl,v $
// Revision 1.10  2007/07/27 21:44:04  gdavidson
// Added Read And Write for Vector
//
// Revision 1.9  2006/08/23 19:27:23  romy
// modified Read to eliminate null DOM character elements
//
// Revision 1.8  2006/01/18 20:37:46  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/22 21:16:48  geconomos
// added supoprt for serialization of class Triple
//
// Revision 1.6  2005/04/12 15:23:23  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.5  2004/12/15 12:55:00  frank
// added list serialization
//
// Revision 1.4  2004/07/12 18:03:52  frank
// added functions to read and write general serializable classes
//
// Revision 1.3  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.2  2004/03/24 17:10:33  frank
// Moved template functions into .inl file
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.6  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 1.5  2002/10/17 19:38:43  michael
// moved static templatized member function code into header file. This is
// necessary for the MS VC 7.0 compiler but should be fixed in 7.1
//
// Revision 1.4  2002/10/16 23:10:35  michael
// extended Serialize/Deserialize() and also fixed naming in Serialize() using
// vxSerialize::GetElementName() instead of hardcoded text. I also made the
// parameters of all GetElementName() const which I forgot in the first place.
//
// Revision 1.3  2002/10/16 15:53:36  michael
// added try/catch
//
// Revision 1.2  2002/10/15 20:44:50  michael
// got the serialization to work :-)
//
// Revision 1.1  2002/10/12 23:24:42  michael
// Initial version of the templatized methods (require MS VC 7.0)
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSerializer.inl,v 1.10 2007/07/27 21:44:04 gdavidson Exp $
// $Id: vxSerializer.inl,v 1.10 2007/07/27 21:44:04 gdavidson Exp $
