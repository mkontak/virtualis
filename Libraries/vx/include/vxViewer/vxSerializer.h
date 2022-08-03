// $Id: vxSerializer.h,v 1.15 2007/08/21 14:34:24 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)

#ifndef vxSerializerXerces_h
#define vxSerializerXerces_h


// includes
#include "Point2D.h"
#include "Triple.h"
#include "Point.h"
#include "Vector.h"
#include "Vector4D.h"
#include "Normal.h"
#include "vxISerializeFactoryManager.h"
#include "utilities.h"
#include "vxDOM.h"
#include "vxComponentData.h"


// forward declarations
class ZeroOne;
class vxWindowLevel;
class vxColormap;
class vxCamera;
class vxCameraPreset;
class vxVisualizationPreset;
class vxComponent;
class vxComponentArray;
class vxRenderingMode;
class vxHandle;
template <class T> class Box;
template <class T> class PixelRGB;
template <class T> class PixelRGBA;
class vxPreset;
class vxVessel;
class vxVesselSection;
class RaySegment;


// class declaration
class VX_VIEWER_DLL vxSerializer
{ 
// functions
public:

  /// constructor, taking the name of the root element as argument
  vxSerializer(const std::string & sTagName);

  /// destructor
  virtual ~vxSerializer() {};

  /// returns if the serializer is valid
  bool IsValid() { return true; }

  /// initializes from a string representation of the serialized data
  void FromString(std::string sXml);

  /// returns a string representation of the serialized data
  std::string ToString();

  /// writes an attribute to the element
  void WriteAttribute(const std::string & sField, const std::string & sValue);

  /// reads an attribute from the element
  bool ReadAttribute(const std::string & sField, std::string & sValue);

  /////////////////////////////////////////////////////////////
  //
  //                  STATIC HELPER FUNCTIONS
  //
  /////////////////////////////////////////////////////////////
  
  /// set an attribute of an element
  static void SetAttribute(vxDOMElement & element, const std::string & sField, const std::string & sValue);

  /// get an attribute of an element
  static bool GetAttribute(vxDOMElement & element, const std::string & sField, std::string & sValue);

  /// set the value of an element
  static void SetValue(vxDOMElement & element, const std::string & sValue);

  /// get the value of an element
  static bool GetValue(vxDOMElement & element, std::string & sValue);

  /// write a vxHandle
  static void Write(vxDOMElement & element, const vxHandle & handle);

  /// read a vxHandle
  static bool Read(vxDOMElement & element, vxHandle & handle);

  /// write a vxComponentArray
  static void Write(vxDOMElement & element, const vxComponentArray & componentArray);

  /// read a vxComponentArray
  static bool Read(vxDOMElement & element, vxComponentArray & componentArray);

  /// write a vxComponent
  static void Write(vxDOMElement & element, const vxComponent & component);

  /// read a vxComponent
  static bool Read(vxDOMElement & element, vxComponent & component);

  /// write a vxRenderingMode
  static void Write(vxDOMElement & element, const vxRenderingMode & renderingMode);

  /// read a vxRenderingMode
  static bool Read(vxDOMElement & element, vxRenderingMode & renderingMode);

  /// write a vxCamera
  static void Write(vxDOMElement & element, const vxCamera & camera);

  /// read a vxCamera
  static bool Read(vxDOMElement & element, vxCamera & camera);

  /// write a vxCameraPreset
  static void Write(vxDOMElement & element, const vxCameraPreset & cameraPreset);

  /// read a vxCameraPreset
  static bool Read(vxDOMElement & element, vxCameraPreset & cameraPreset);

  /// write a vxVisualizationPreset
  static void Write(vxDOMElement & element, const vxVisualizationPreset & visPreset);

  /// read a vxVisualizationPreset
  static bool Read(vxDOMElement & element, vxVisualizationPreset & visPreset);

  /// write a vxColormap
  static void Write(vxDOMElement & element, const vxColormap & colormap);

  /// read a vxColormap
  static bool Read(vxDOMElement & element, vxColormap & colormap);

  /// write a vxWindowLevel
  static void Write(vxDOMElement & element, const vxWindowLevel & windowLevel);

  /// read a vxWindowLevel
  static bool Read(vxDOMElement & element, vxWindowLevel & windowLevel);

  /// write a vxPreset
  static void Write(vxDOMElement & element, const vxPreset & preset);

  /// read a vxPreset
  static bool Read(vxDOMElement & element, vxPreset & preset);

  /// write a vxVesselSection
  static void Write(vxDOMElement & element, const vxVesselSection & section);

  /// read a vxVesselSection
  static bool Read(vxDOMElement & element, vxVesselSection & section);

  /// write a RaySegment
  static void Write(vxDOMElement & element, const RaySegment & raySegment);

  /// read a RaySegment
  static bool Read(vxDOMElement & element, RaySegment & raySegment);
  
  /// to differentiate parameter std::vector<T> and std::vector<T*>
  template <class T> inline static std::vector<T> CreateRealVectorFromPtr(const std::vector<T *> & vpObject);

  /// to differentiate parameter std::vector<T> and std::vector<T*>
  template <class T> inline static std::vector<T> CreateRealVectorFromPtr(const std::vector<const T *> & vpObject);

  /// to differentiate parameter std::vector<T> and std::vector<T*>
  template <class T> inline static std::vector<T *> CreatePtrVectorFromReal(const std::vector<T> & vObject);

  /// to differentiate parameter std::vector<T> and std::vector<T*>
  template <class T> inline static std::vector<const T *> CreateConstPtrVectorFromReal(const std::vector<T> & vObject);

  /// write a std::map<T1, T2>
  template <class T1, class T2> inline static void Write(vxDOMElement & element, const std::map<T1, T2> & map);

  /// read a std::map<T1, T2>
  template <class T1, class T2> inline static bool Read(vxDOMElement & element, std::map<T1, T2> & map);

  /// write a std::list<T>
  template <class T> inline static void Write(vxDOMElement & element, const std::list<T> & list);

  /// read a std::list<T>
  template <class T> inline static bool Read(vxDOMElement & element, std::list<T> & list);

  /// write a std::pair<T1, T2>
  template <class T1, class T2> inline static void Write(vxDOMElement & element, const std::pair<T1, T2> & pair);

  /// read a std::pair<T1, T2>
  template <class T1, class T2> inline static bool Read(vxDOMElement & element, std::pair<T1, T2> & pair);
 
  /// write a PixelRGB<T>
  template <class T> static void Write(vxDOMElement & element, const PixelRGB<T> & pixel);

  /// read a PixelRGB<T>
  template <class T> static bool Read(vxDOMElement & element, PixelRGB<T> & pixel);

  /// write a PixelRGBA<T>
  template <class T> inline static void Write(vxDOMElement & element, const PixelRGBA<T> & pixel);

  /// read a PixelRGBA<T>
  template <class T> inline static bool Read(vxDOMElement & element, PixelRGBA<T> & pixel);

  /// std::vector<T*>
  template <class T> inline static void WriteISerialize(vxDOMElement & element, const std::vector<T*> & vpObjects);

  /// std::vector<T*>
  template <class T> inline static bool ReadISerialize(vxDOMElement & element, std::vector<T*> & vpObjects);

  /// write object derived from vxISerialize
  template <class T> inline static void WriteISerialize(vxDOMElement & element, const T * pObject);

  /// read object derived from vxISerialize
  template <class T> inline static bool ReadISerialize(vxDOMElement & element, T ** pObject );

  /// write a std::vector<T>
  template <class T> inline static void Write(vxDOMElement & element, const std::vector<T> & vObjects);

  /// read a std::vector<T>
  template <class T> inline static bool Read(vxDOMElement & element, std::vector<T> & vObject);

  /// write a Box<T>
  template <class T> inline static void Write(vxDOMElement & element, const Box<T> & box);

  /// read a Box<T>
  template <class T> inline static bool Read(vxDOMElement & element, Box<T> & box);

  /// write a Point2D<T>
  template <class T> inline static void Write(vxDOMElement & element, const Point2D<T> & point2D);

  /// read a point2D
  template <class T> inline static bool Read(vxDOMElement & element, Point2D<T> & point2D);

  /// write a Point<T>
  template <class T> inline static void Write(vxDOMElement & element, const Point3D<T> & point);

  /// read a Point<T>
  template <class T> inline static bool Read(vxDOMElement & element, Point3D<T> & point);

    /// write a vector<T>
  template <class T> inline static void Write(vxDOMElement & element, const Vector3D<T> & vector);

  /// read a vector<T>
  template <class T> inline static bool Read(vxDOMElement & element, Vector3D<T> & vector);

  /// write a Triple<T>
  template <class T> inline static void Write(vxDOMElement & element, const Triple<T> & triple);

  /// read a Triple<T>
  template <class T> inline static bool Read(vxDOMElement & element, Triple<T> & triple);

  /// write a Normal<T>
  template <class T> inline static void Write(vxDOMElement & element, const Normal3D<T> & normal);

  /// read a Normal<T>
  template <class T> inline static bool Read(vxDOMElement & element, Normal3D<T> & normal);

  /// write a Vector4d<T>
  template <class T> inline static void Write(vxDOMElement & element, const Vector4d<T> & vector);

  /// read a Vector4d<T>
  template <class T> inline static bool Read(vxDOMElement & element, Vector4d<T> & vector);

  /// write a std::string
  static void Write(vxDOMElement & element, const std::string & sValue);
  
  /// read a std::string
  static bool Read(vxDOMElement & element, std::string & sValue);

  /// write a ZeroOne
  static void Write(vxDOMElement & element, const ZeroOne & sValue);
  
  /// read a ZeroOne
  static bool Read(vxDOMElement & element, ZeroOne & sValue);

  /// write a float
  static void Write(vxDOMElement & element, const float & fValue);
  
  /// read a float
  static bool Read(vxDOMElement & element, float & fValue);

  /// write an int
  static void Write(vxDOMElement & element, const int & iValue);
  
  /// read an int
  static bool Read(vxDOMElement & element, int & iValue);

  /// write a uint2
  static void Write(vxDOMElement & element, const uint2 & uValue);
  
  /// read a uint2
  static bool Read(vxDOMElement & element, uint2 & uValue);

  /// write a uint2
  static void Write(vxDOMElement & element, const uint1 & uValue);
  
  /// read a uint2
  static bool Read(vxDOMElement & element, uint1 & uValue);

  /// write a int2
  static void Write(vxDOMElement & element, const int2 & iValue);
  
  /// read a int2
  static bool Read(vxDOMElement & element, int2 & iValue);

  /// write a bool
  static void Write(vxDOMElement & element, const bool & bValue);
  
  /// read a bool
  static bool Read(vxDOMElement & element, bool & bValue);

  /// GetName identifiers for different types
  static std::string GetElementName(const bool &)                             { return "Bool"; };
  static std::string GetElementName(const int1 &)                             { return "Char"; };
  static std::string GetElementName(const int2 &)                             { return "Short"; };
  static std::string GetElementName(const int4 &)                             { return "Int"; };
  static std::string GetElementName(const uint1 &)                            { return "UnsignedChar"; };
  static std::string GetElementName(const uint2 &)                            { return "UnsignedShort"; };
  static std::string GetElementName(const uint4 &)                            { return "UnsignedInt"; };
  static std::string GetElementName(const float4 &)                           { return "Float"; };
  static std::string GetElementName(const float8 &)                           { return "Double"; };
  static std::string GetElementName(const std::string &)                      { return "StdString"; };
  static std::string GetElementName(const ZeroOne &)                          { return "ZeroOne"; };
  static std::string GetElementName(const vxCamera &)                         { return "vxCamera"; };
  static std::string GetElementName(const vxCameraPreset &)                   { return "vxCameraPreset"; };
  static std::string GetElementName(const vxVisualizationPreset &)            { return "vxVisualizationPreset"; };
  static std::string GetElementName(const vxWindowLevel &)                    { return "vxWindowLevel"; };
  static std::string GetElementName(const vxColormap &)                       { return "vxColormap"; };
  static std::string GetElementName(const vxRenderingMode &)                  { return "vxRenderingMode"; };
  static std::string GetElementName(const vxComponent &)                      { return "vxComponent"; };
  static std::string GetElementName(const vxComponentArray &)                 { return "vxComponentArray"; };
  static std::string GetElementName(const vxPreset &)                         { return "vxPreset"; };
  static std::string GetElementName(const vxHandle &)                         { return "vxHandle"; }
  static std::string GetElementName(const vxVessel &)                         { return "vxVessel"; }
  static std::string GetElementName(const vxVesselSection &)                  { return "vxVesselSection"; }
  static std::string GetElementName(const RaySegment &)                       { return "RaySegment"; }
  template <class T> static std::string GetElementName(const Point2D<T> &)    { return "Point2D"; }
  template <class T> static std::string GetElementName(const Point3D<T> &)    { return "Point"; }
  template <class T> static std::string GetElementName(const Triple<T> &)    { return "Triple"; }
  template <class T> static std::string GetElementName(const Normal3D<T> &)   { return "Normal"; }
  template <class T> static std::string GetElementName(const Vector4d<T> &)   { return "Vector4d"; }
  template <class T> static std::string GetElementName(const Box<T> &)        { return "Box"; }
  template <class T> static std::string GetElementName(const std::vector<T> &){ return "StdVector"; }
  template <class T> static std::string GetElementName(const std::list<T> &)  { return "StdList"; }
  template <class T> static std::string GetElementName(std::vector<const T *> &){ return "StdVector"; }
  template <class T> static std::string GetElementName(const PixelRGB<T> &)   { return "PixelRGB"; }
  template <class T> static std::string GetElementName(const PixelRGBA<T> &)  { return "PixelRGBA"; }
  template <class T1, class T2> static std::string GetElementName(const std::map<T1, T2> &)  { return "StdMap"; }
  template <class T1, class T2> static std::string GetElementName(const std::pair<T1, T2> &) { return "StdPair"; }

  /// accessor to the root element
  vxDOMDocument GetDocument() { return m_document; }

  /// accessor to the current element
  vxDOMElement & GetCurrent() { return m_current; }

  /// finds a child element with the specified member name
  static vxDOMElement FindElementByMember(vxDOMElement & spDomElement, const std::string & sMember);

  /// finds a child element with the specified tag name and member name
  static vxDOMElement FindElement(vxDOMElement & spDomElement, const std::string & sTagName, const std::string & sMember = "");

  /// finds a child element with the specified tag name
  static vxDOMElement AddElement(vxDOMElement & spDomElement, const std::string & sTagName, const std::string & sMember = "");

// data
private:

  /// maintains the entire serialized tree
  vxDOMDocument m_document;

  /// points to the current element
  vxDOMElement  m_current;
  
}; // class vxSerializer


#include "vxSerializer.inl"


#endif // vxSerializer_h


// $Log: vxSerializer.h,v $
// Revision 1.15  2007/08/21 14:34:24  dongqing
// add int2 type for serilizer
//
// Revision 1.14  2007/07/27 21:44:04  gdavidson
// Added Read And Write for Vector
//
// Revision 1.13  2006/12/14 22:05:27  romy
// component data changes incorporated
//
// Revision 1.12  2005/11/22 21:16:48  geconomos
// added supoprt for serialization of class Triple
//
// Revision 1.11  2005/06/07 18:00:10  vxconfig
// moved vessel serialization to vessel class
//
// Revision 1.10  2005/04/12 15:23:23  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.9  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.8.2.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.8  2004/12/15 12:54:50  frank
// added list serialization
//
// Revision 1.7  2004/07/12 18:03:24  frank
// added functions to read and write general serializable classes
//
// Revision 1.6  2004/05/17 18:48:35  frank
// moved visualization preset serialization to its own class
//
// Revision 1.5  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.4  2004/04/05 15:59:07  frank
// working on expanding the presets to include rendering modes, etc.
//
// Revision 1.3  2004/03/24 17:10:32  frank
// Moved template functions into .inl file
//
// Revision 1.2  2004/03/23 21:13:42  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/23 03:52:36  michael
// added serialization of vxHandle
//
// Revision 1.34  2004/01/05 18:57:23  michael
// added "mailto:"
//
// Revision 1.33  2003/08/05 13:36:24  frank
// Added visualization preset and PixelRGB serialization
//
// Revision 1.32  2003/06/03 20:31:29  michael
// added necessary include for ToAscii()
//
// Revision 1.31  2003/05/06 15:11:40  michael
// initial code review
//
// Revision 1.30  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.29  2003/04/01 00:35:59  michael
// fixed issue #2862
//
// Revision 1.28  2003/02/25 22:16:25  michael
// fixed crash (member call of unitialized pointer)
//
// Revision 1.27  2003/02/22 01:34:32  michael
// removed redundant code in (de-)serialization
//
// Revision 1.26  2003/02/22 00:22:55  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.25  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.24  2003/02/18 21:06:24  michael
// added the routines to serialize annotations
//
// Revision 1.23  2003/01/22 21:45:56  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.22  2003/01/13 19:29:15  ingmar
// Point2D is no longer derived from the 3D Point
// serialization and 2D rendering are updated accordingly
//
// Revision 1.21  2002/12/20 15:57:44  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.20  2002/11/19 16:21:20  michael
// added a FromString()
//
// Revision 1.19  2002/11/14 01:20:29  michael
// added more objects :-) (vxComponentArray, std::map, std::pair)
//
// Revision 1.18  2002/11/13 02:21:08  michael
// added some more serialization (Components)
//
// Revision 1.17  2002/11/12 23:49:06  michael
// added more objects to serialization
//
// Revision 1.16  2002/11/11 19:31:04  michael
// fixed warnings on FindElement. Any while loop would finally produce such a
// warning.Thus, warning is omitted, return false should be enough ...
//
// Revision 1.15  2002/10/24 01:22:55  michael
// added serialize/deserialize for int
//
// Revision 1.14  2002/10/21 15:45:47  michael
// took out serialize/deserialize methods, except one which can serve as
// template once we need a real wrapper class to access this from C# code.
//
// Revision 1.13  2002/10/18 22:59:56  michael
// fixed bug in writing camera: took eye position as normal vector :-(
//
// Revision 1.12  2002/10/18 18:51:51  michael
// fixed issue when reading from bad node (NULL): Check in all Read() first!!!
//
// Revision 1.11  2002/10/17 19:38:43  michael
// moved static templatized member function code into header file. This is
// necessary for the MS VC 7.0 compiler but should be fixed in 7.1
//
// Revision 1.10  2002/10/16 23:10:35  michael
// extended Serialize/Deserialize() and also fixed naming in Serialize() using
// vxSerialize::GetElementName() instead of hardcoded text. I also made the
// parameters of all GetElementName() const which I forgot in the first place.
//
// Revision 1.9  2002/10/16 22:20:43  michael
// added Serialize/Deserialize for vxCamera ...
//
// Revision 1.8  2002/10/15 22:37:09  michael
// moved writing/reading of std::string from attributes to elements
// (std::string containing ' " ' would not be possible with attributes)
//
// Revision 1.7  2002/10/15 20:44:50  michael
// got the serialization to work :-)
//
// Revision 1.6  2002/10/14 22:05:15  michael
// goes slightly better, dies off later. :-)
//
// Revision 1.5  2002/10/14 16:04:01  michael
// added further serialization functionality
//
// Revision 1.4  2002/10/13 20:19:21  michael
// added method to serialize/deserialize colormap
//
// Revision 1.3  2002/10/13 19:50:00  michael
// comment for constructor
//
// Revision 1.2  2002/10/12 23:25:16  michael
// Added many methods to serialize/deserialize vxBase elements
//
// Revision 1.1  2002/10/10 18:31:20  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSerializer.h,v 1.15 2007/08/21 14:34:24 dongqing Exp $
// $Id: vxSerializer.h,v 1.15 2007/08/21 14:34:24 dongqing Exp $
