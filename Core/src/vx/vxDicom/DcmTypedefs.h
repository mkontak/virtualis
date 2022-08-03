#pragma once


namespace dcm
{


// Forward declarations
class DCM_DLL IDcmImage;
class DCM_DLL DcmImageCollection;
class DCM_DLL DcmIndex;
class DCM_DLL DcmElement;
class DCM_DLL DcmElementCollection;

/// DcmImageCollection - Internal Image list
typedef std::vector<std::shared_ptr<IDcmImage>> DcmImageList;


/// DcmImageCollection  - GetStacks()
typedef   std::pair<int, std::shared_ptr<dcm::DcmImageCollection>> DcmStackMapItem;
typedef   std::map<int, std::shared_ptr<dcm::DcmImageCollection>> DcmStackMap;




/// DcmElementCollection 
typedef std::map<int, std::shared_ptr<dcm::DcmElement>> DcmElementMap;
typedef DcmElementMap::iterator DcmElementMapIterator;
typedef std::pair<int,std::shared_ptr<dcm::DcmElement>> DcmElementMapItem;


/// DcmImageCOllection
typedef std::vector<std::shared_ptr<dcm::DcmImageCollection>> DcmImageCollectionList;


/// DcmElementCollection
typedef std::vector<std::shared_ptr<dcm::DcmElementCollection>> DcmElementCollectionList;


}  // namespace dcm